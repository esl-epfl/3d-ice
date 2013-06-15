/******************************************************************************
 * This file is part of 3D-ICE, version 2.2.3 .                               *
 *                                                                            *
 * 3D-ICE is free software: you can  redistribute it and/or  modify it  under *
 * the terms of the  GNU General  Public  License as  published by  the  Free *
 * Software  Foundation, either  version  3  of  the License,  or  any  later *
 * version.                                                                   *
 *                                                                            *
 * 3D-ICE is  distributed  in the hope  that it will  be useful, but  WITHOUT *
 * ANY  WARRANTY; without  even the  implied warranty  of MERCHANTABILITY  or *
 * FITNESS  FOR A PARTICULAR  PURPOSE. See the GNU General Public License for *
 * more details.                                                              *
 *                                                                            *
 * You should have  received a copy of  the GNU General  Public License along *
 * with 3D-ICE. If not, see <http://www.gnu.org/licenses/>.                   *
 *                                                                            *
 *                             Copyright (C) 2010                             *
 *   Embedded Systems Laboratory - Ecole Polytechnique Federale de Lausanne   *
 *                            All Rights Reserved.                            *
 *                                                                            *
 * Authors: Arvind Sridhar                                                    *
 *          Alessandro Vincenzi                                               *
 *          Giseong Bak                                                       *
 *          Martino Ruggiero                                                  *
 *          Thomas Brunschwiler                                               *
 *          David Atienza                                                     *
 *                                                                            *
 * For any comment, suggestion or request  about 3D-ICE, please  register and *
 * write to the mailing list (see http://listes.epfl.ch/doc.cgi?liste=3d-ice) *
 * Any usage  of 3D-ICE  for research,  commercial or other  purposes must be *
 * properly acknowledged in the resulting products or publications.           *
 *                                                                            *
 * EPFL-STI-IEL-ESL                                                           *
 * Batiment ELG, ELG 130                Mail : 3d-ice@listes.epfl.ch          *
 * Station 11                                  (SUBSCRIPTION IS NECESSARY)    *
 * 1015 Lausanne, Switzerland           Url  : http://esl.epfl.ch/3d-ice.html *
 ******************************************************************************/

%code requires
{
    #include "types.h"
    #include "string_t.h"

    #include "material.h"
    #include "ic_element.h"
    #include "material_element.h"
}

%union
{
    String_t            identifier ;
    CellDimension_t     location ;
    Material_t         *material_p ;
    ICElement_t        *layout_p ;
    MaterialElement_t  *material_element_p ;
}

%code
{
    #include "material_list.h"
    #include "ic_element_list.h"
    #include "dimensions.h"
    #include "layer.h"

    #include "../flex/layout_scanner.h"

    void layout_parser_error

        (Layer_t *layer, Dimensions_t *dimensions, MaterialList_t *materials,
         yyscan_t yyscanner, const char *msg) ;

    #define LAYOUTERROR(m) layout_parser_error (layer, dimensions, materials, scanner, m)

    static char error_message [250] ;

    static bool local_abort ;

    static MaterialList_t  local_materials ;
    static ICElementList_t local_layout_elements ;
}

%type <material_p>         material
%type <layout_p>           layout_element
%type <material_element_p> layout ;

%destructor { string_destroy (&$$) ; } <identifier>

%token CAPACITY       "keyword capacity"
%token CONDUCTIVITY   "keyword conductivity"
%token DIMENSION      "keyword dimension"
%token HEAT           "keyword heat"
%token MATERIAL       "keyword material"
%token POSITION       "keyword position"
%token RECTANGLE      "keywork rectangle"
%token THERMAL        "keyword thermal"
%token VOLUMETRIC     "keywork volumetric"

%token <location>    DVALUE     "double value"
%token <identifier>  IDENTIFIER "identifier"

%name-prefix "layout_parser_"
%output      "layout_parser.c"

%pure-parser

%error-verbose

%parse-param { Layer_t        *layer      }
%parse-param { Dimensions_t   *dimensions }
%parse-param { MaterialList_t *materials  }
%parse-param { yyscan_t        scanner    }

%lex-param   { yyscan_t scanner }

%initial-action
{
    local_abort = false ;

    material_list_init   (&local_materials) ;
    ic_element_list_init (&local_layout_elements) ;
} ;

%start layout_file

%%

/******************************************************************************/
/******************************* File content *********************************/
/******************************************************************************/

layout_file

  : materials_list_opt
    layouts_list
    {
        material_list_destroy (&local_materials) ;

        if (local_abort == true)
        {
            ic_element_list_destroy (&local_layout_elements) ;

            YYABORT ;
        }
    }
  ;

/******************************************************************************/
/******************************* Local Materials ******************************/
/******************************************************************************/

materials_list_opt

  : /* empty */

  | materials_list

  ;

materials_list

  : material                // $1 : pointer to the first material found
    {
        material_list_insert_end (&local_materials, $1) ;

        material_free ($1) ;
    }
  | materials_list material // $1 : pointer to the last material in the list
                            // $2 : pointer to the material to add in the list
    {
        if (material_list_find (&local_materials, $2) != NULL)
        {
            sprintf (error_message, "Material %s already declared", $2->Id) ;

            LAYOUTERROR (error_message) ;

            material_free ($2) ;

            YYABORT ;
        }

        material_list_insert_end (&local_materials, $2) ;

        material_free ($2) ;
    }
  ;

material

  : MATERIAL IDENTIFIER ':'                     // $2
        THERMAL CONDUCTIVITY     DVALUE ';'     // $6
        VOLUMETRIC HEAT CAPACITY DVALUE ';'     // $11
    {
        Material_t *material = $$ = material_calloc () ;

        if (material == NULL)
        {
            LAYOUTERROR ("Malloc material failed") ;

            string_destroy (&$2) ;

            YYABORT ;
        }

        string_copy (&material->Id, &$2) ;

        material->ThermalConductivity    = (SolidTC_t) $6 ;
        material->VolumetricHeatCapacity = (SolidVHC_t) $11 ;

        string_destroy (&$2) ;
    }
  ;

/******************************************************************************/
/*********************** Layout ***********************************************/
/******************************************************************************/

layouts_list

  : layout
    {
        material_element_list_insert_end (&layer->MaterialLayout, $1) ;

        material_element_free ($1) ;
    }
  | layouts_list layout
    {
        if (material_element_list_find(&layer->MaterialLayout, $2) != NULL)
        {
            sprintf (error_message, "Layout element %s already declared", $2->Material.Id) ;

            LAYOUTERROR (error_message) ;

            material_element_free ($2) ;

            YYABORT ;
        }

        material_element_list_insert_end (&layer->MaterialLayout, $2) ;

        material_element_free ($2) ;
    }
  ;

layout

  : IDENTIFIER ':'        // $1 the id of a material
      layout_elements     // the lis of locations
    {
        MaterialElement_t *melement = $$ = material_element_calloc ( ) ;

        if (melement == NULL)
        {
            LAYOUTERROR ("Malloc material element failed") ;

            ic_element_list_destroy (&local_layout_elements) ;
            string_destroy (&$1) ;

            YYABORT ;
        }

        // control if the material is named in the local list of materials
        // or in the list of maerials in the stack file

        Material_t material ;

        material_init (&material) ;

        string_copy (&material.Id, &$1) ;

        Material_t *tmp = material_list_find (&local_materials, &material) ;

        if (tmp == NULL)
        {
            tmp = material_list_find (materials, &material) ;

            if (tmp == NULL)
            {
                sprintf (error_message, "Unknown material %s", $1) ;

                LAYOUTERROR (error_message) ;

                ic_element_list_destroy (&local_layout_elements) ;
                material_destroy (&material) ;
                string_destroy   (&$1) ;

                YYABORT ;
            }
        }

        // Saves the material

        material_copy    (&melement->Material, tmp) ;

        material_destroy (&material) ;
        string_destroy (&$1) ;

        // Saves the list of ic elements

        melement->NMElements = local_layout_elements.Size ;

        ic_element_list_copy (&melement->MElements, &local_layout_elements) ;

        ic_element_list_destroy (&local_layout_elements) ;
        ic_element_list_init    (&local_layout_elements) ;

        // Cotrol for intersections between the layout elements

        ICElementListNode_t *meln1 ;

        for (meln1  = ic_element_list_begin (&melement->MElements) ;
             meln1 != NULL ;
             meln1  = ic_element_list_next (meln1))
        {
            ICElement_t *mel1 = ic_element_list_data (meln1) ;

            ICElementListNode_t *meln2 ;

            for (meln2  = ic_element_list_begin (&melement->MElements) ;
                 meln2 != NULL ;
                 meln2  = ic_element_list_next (meln2))
            {
                ICElement_t *mel2 = ic_element_list_data (meln2) ;

                if (check_intersection (mel1, mel2) == true)
                {
                    sprintf (error_message,
                        "Intersection between %s (%.1f, %.1f, %.1f, %.1f)" \
                                        " and %s (%.1f, %.1f, %.1f, %.1f)\n",
                        melement->Material.Id,
                        mel1->SW_X, mel1->SW_Y, mel1->Length, mel1->Width,
                        melement->Material.Id,
                        mel2->SW_X, mel2->SW_Y, mel2->Length, mel2->Width) ;

                    LAYOUTERROR (error_message) ;

                    local_abort = true ;
                }
            }

            MaterialElementListNode_t *mateln ;

            for (mateln  = material_element_list_begin (&layer->MaterialLayout) ;
                 mateln != NULL ;
                 mateln  = material_element_list_next (mateln))
            {
                MaterialElement_t *matel = material_element_list_data (mateln) ;

                ICElementListNode_t *meln3 ;

                for (meln3  = ic_element_list_begin (&matel->MElements) ;
                     meln3 != NULL ;
                     meln3  = ic_element_list_next (meln3))
                {
                    ICElement_t *mel3 = ic_element_list_data (meln3) ; ;

                    if (check_intersection (mel1, mel3) == true)
                    {
                        sprintf (error_message,
                            "Intersection between %s (%.1f, %.1f, %.1f, %.1f)" \
                                            " and %s (%.1f, %.1f, %.1f, %.1f)\n",
                            melement->Material.Id,
                            mel1->SW_X, mel1->SW_Y, mel1->Length, mel1->Width,
                            matel->Material.Id,
                            mel3->SW_X, mel3->SW_Y, mel3->Length, mel3->Width) ;

                        LAYOUTERROR (error_message) ;

                        local_abort = true ;
                    }
                }
            }
        }
    }
  ;

layout_elements

  : POSITION  DVALUE ',' DVALUE ';'  // $2 $4
    DIMENSION DVALUE ',' DVALUE ';'  // $7 $9
    {
        ICElement_t icelement ;

        ic_element_init (&icelement) ;

        icelement.SW_X   = $2 ;
        icelement.SW_Y   = $4 ;
        icelement.Length = $7 ;
        icelement.Width  = $9 ;

        align_to_grid (&icelement, dimensions) ;

        if (check_location (&icelement, dimensions) == true)
        {
            LAYOUTERROR ("Layout element is outside of the IC") ;

            local_abort = true ;
        }

        ic_element_list_insert_end (&local_layout_elements, &icelement) ;
    }

  | layout_elements_list

  ;

layout_elements_list

  : layout_element
    {
        ic_element_list_insert_end (&local_layout_elements, $1) ;

        ic_element_free ($1) ;
    }
  | layout_elements_list layout_element
    {
        ic_element_list_insert_end (&local_layout_elements, $2) ;

        ic_element_free ($2) ;
    }
  ;

layout_element

  : RECTANGLE '(' DVALUE ',' DVALUE ',' DVALUE ',' DVALUE ')' ';'  // $3 $5 $7 $9
    {
        ICElement_t *icelement = $$ = ic_element_calloc () ;

        if (icelement == NULL)
        {
            LAYOUTERROR ("Malloc layout element failed") ;

            ic_element_list_destroy (&local_layout_elements) ;

            YYABORT ;
        }

        icelement->SW_X   = $3 ;
        icelement->SW_Y   = $5 ;
        icelement->Length = $7 ;
        icelement->Width  = $9 ;

        align_to_grid (icelement, dimensions) ;

        if (check_location (icelement, dimensions) == true)
        {
            LAYOUTERROR ("Layout element is outside of the IC") ;

            local_abort = true ;
        }
    }
  ;

%%

/******************************************************************************/

void layout_parser_error
(
    Layer_t        *layer,
    Dimensions_t   *__attribute__ ((unused)) dimensions,
    MaterialList_t *__attribute__ ((unused)) materials,
    yyscan_t        yyscanner,
    const char     *msg
)
{
    material_list_destroy   (&local_materials) ;

    fprintf (stderr, "%s:%d: %s\n",
        layer->LayoutFileName, floorplan_parser_get_lineno(yyscanner), msg) ;
}

/******************************************************************************/
