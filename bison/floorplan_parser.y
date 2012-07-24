/******************************************************************************
 * This file is part of 3D-ICE, version 2.2 .                                 *
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
    #include "floorplan_element.h"
    #include "ic_element.h"
    #include "powers_queue.h"
}

%union
{
    Power_t             power_value ;
    String_t            identifier ;
    ICElement_t        *p_icelement ;
    FloorplanElement_t *p_floorplan_element ;
    PowersQueue_t      *p_powers_queue ;
}

%code
{
    #include "ic_element_list.h"
    #include "dimensions.h"
    #include "floorplan.h"
    #include "macros.h"

    #include "../flex/floorplan_scanner.h"

    void floorplan_parser_error

        (Floorplan_t *floorplan, Dimensions_t *dimensions,
         yyscan_t yyscanner, const char *msg) ;

    static char error_message [250] ;

    static bool local_abort ;

    static ICElementList_t ic_element_list ;
}

%type <p_floorplan_element> floorplan_element ;
%type <p_powers_queue>      optional_power_values_list ;
%type <p_powers_queue>      power_values_list ;
%type <p_icelement>         ic_element ;

%destructor { free              ($$) ; } <identifier>
%destructor { powers_queue_free ($$) ; } <p_powers_queue>

%token DIMENSION  "keyword dimension"
%token POSITION   "keyword position"
%token POWER      "keyword power"
%token RECTANGLE  "keywork rectangle"
%token VALUES     "keyword values"

%token <power_value> DVALUE     "double value"
%token <identifier>  IDENTIFIER "identifier"

%name-prefix "floorplan_parser_"
%output      "floorplan_parser.c"

%pure-parser

%error-verbose

%parse-param { Floorplan_t  *floorplan  }
%parse-param { Dimensions_t *dimensions }
%parse-param { yyscan_t      scanner    }

%lex-param   { yyscan_t scanner       }

%initial-action
{
    local_abort    = false ;

    ic_element_list_init (&ic_element_list) ;
} ;

%start floorplan_file

%%

floorplan_file

  : floorplan_element_list
    {
        floorplan->NElements = floorplan->ElementsList.Size ;

        if (local_abort == true)
        {
            floorplan_free (floorplan) ;

            YYABORT ;
        }
    }
  ;

/******************************************************************************/
/************************* List of floorplan elements *************************/
/******************************************************************************/

floorplan_element_list

  : floorplan_element
    {
        floorplan_element_list_insert_end (&floorplan->ElementsList, $1) ;

        floorplan_element_free ($1) ;
    }
  | floorplan_element_list floorplan_element
    {
        if (floorplan_element_list_find(&floorplan->ElementsList, $2) != NULL)
        {
            sprintf (error_message, "Floorplan element id %s already declared", $2->Id) ;

            floorplan_parser_error (floorplan, dimensions, scanner, error_message) ;

            local_abort = true ;
        }

        floorplan_element_list_insert_end (&floorplan->ElementsList, $2) ;

        floorplan_element_free ($2) ;
    }
  ;

/******************************************************************************/
/************************* Floorplan element **********************************/
/******************************************************************************/

floorplan_element

  : IDENTIFIER ':'                        // $1
      ic_elements                         // $3
      optional_power_values_list          // $4
    {
        FloorplanElement_t *floorplan_element = $$ = floorplan_element_calloc ( ) ;

        if (floorplan_element == NULL)
        {
            free ($1) ;

            ic_element_list_destroy (&ic_element_list) ;

            floorplan_parser_error (floorplan, dimensions, scanner, "Malloc floorplan element failed") ;

            YYABORT ;
        }

        floorplan_element->Id           = $1 ;
        floorplan_element->NICElements  = ic_element_list.Size ;
        floorplan_element->PowerValues  = $4 ;

        ic_element_list_copy (&floorplan_element->ICElements, &ic_element_list) ;

        ic_element_list_destroy (&ic_element_list) ;
        ic_element_list_init    (&ic_element_list) ;

        ICElementListNode_t *iceln1 ;

        for (iceln1  = ic_element_list_begin (&floorplan_element->ICElements) ;
             iceln1 != NULL ;
             iceln1  = ic_element_list_next (iceln1))
        {
            ICElement_t *icel1 = ic_element_list_data (iceln1) ;

            floorplan_element->Area += icel1->Length * icel1->Width ;

            ICElementListNode_t *iceln2 ;

            for (iceln2  = ic_element_list_begin (&floorplan_element->ICElements) ;
                 iceln2 != NULL ;
                 iceln2  = ic_element_list_next (iceln2))
            {
                ICElement_t *icel2 = ic_element_list_data (iceln2) ;

                if (check_intersection (icel1, icel2) == true)
                {
                    sprintf (error_message,
                        "Intersection between %s (%.1f, %.1f, %.1f, %.1f)" \
                                        " and %s (%.1f, %.1f, %.1f, %.1f)\n",
                        $1,
                        icel1->SW_X, icel1->SW_Y, icel1->Length, icel1->Width,
                        $1,
                        icel2->SW_X, icel2->SW_Y, icel2->Length, icel2->Width) ;

                    floorplan_parser_error (floorplan, dimensions, scanner, error_message) ;

                    local_abort = true ;
                }
            }


            FloorplanElementListNode_t *flpeln ;

            for (flpeln  = floorplan_element_list_begin (&floorplan->ElementsList) ;
                 flpeln != NULL ;
                 flpeln  = floorplan_element_list_next (flpeln))
            {
                FloorplanElement_t *flpel = floorplan_element_list_data (flpeln) ;

                ICElementListNode_t *iceln3 ;

                for (iceln3  = ic_element_list_begin (&flpel->ICElements) ;
                     iceln3 != NULL ;
                     iceln3  = ic_element_list_next (iceln3))
                {
                    ICElement_t *icel3 = ic_element_list_data (iceln3) ; ;

                    if (check_intersection (icel1, icel3) == true)
                    {
                        sprintf (error_message,
                            "Intersection between %s (%.1f, %.1f, %.1f, %.1f)" \
                                            " and %s (%.1f, %.1f, %.1f, %.1f)\n",
                            $1,
                            icel1->SW_X, icel1->SW_Y, icel1->Length, icel1->Width,
                            flpel->Id,
                            icel3->SW_X, icel3->SW_Y, icel3->Length, icel3->Width) ;

                        floorplan_parser_error (floorplan, dimensions, scanner, error_message) ;

                        local_abort = true ;
                    }
                }
            }
        }
    }
  ;

ic_elements

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
            sprintf (error_message, "Floorplan element is outside of the IC") ;

            floorplan_parser_error (floorplan, dimensions, scanner, error_message) ;

            local_abort = true ;
        }

        ic_element_list_insert_end (&ic_element_list, &icelement) ;
    }

  | ic_elements_list

  ;

ic_elements_list

  :  ic_element
     {
        ic_element_list_insert_end (&ic_element_list, $1) ;

        ic_element_free ($1) ;
     }
  |  ic_elements_list ic_element
     {
        ic_element_list_insert_end (&ic_element_list, $2) ;

        ic_element_free ($2) ;
     }
  ;

ic_element

  : RECTANGLE '(' DVALUE ',' DVALUE ',' DVALUE ',' DVALUE ')' ';'  // $3 $5 $7 $9
    {
        ICElement_t *icelement = $$ = ic_element_calloc () ;

        if (icelement == NULL)
        {
            floorplan_parser_error (floorplan, dimensions, scanner, "Malloc ic element failed") ;

            ic_element_list_destroy (&ic_element_list) ;

            YYABORT ;
        }

        icelement->SW_X   = $3 ;
        icelement->SW_Y   = $5 ;
        icelement->Length = $7 ;
        icelement->Width  = $9 ;

        align_to_grid (icelement, dimensions) ;

        if (check_location (icelement, dimensions) == true)
        {
            sprintf (error_message, "Floorplan element is outside of the IC") ;

            floorplan_parser_error (floorplan, dimensions, scanner, error_message) ;

            local_abort = true ;
        }
    }
  ;

/******************************************************************************/
/************************* List of power values *******************************/
/******************************************************************************/

optional_power_values_list

  : // Declaring the entire subsection of power values is not mandatory

    {
        PowersQueue_t* powers_list = $$ = powers_queue_calloc () ;

        if (powers_list == NULL)
        {
            floorplan_parser_error (floorplan, dimensions, scanner, "Malloc power list failed") ;

            ic_element_list_destroy (&ic_element_list) ;

            YYABORT ;
        }

        powers_queue_build (powers_list, 10) ;
    }

  | POWER VALUES power_values_list ';' // $3

    {
        $$ = $3 ;
    }
  ;

power_values_list

  : DVALUE              // $1
                        // Here at least one power value is mandatory
    {
        PowersQueue_t* powers_list = $$ = powers_queue_calloc () ;

        if (powers_list == NULL)
        {
            floorplan_parser_error (floorplan, dimensions, scanner, "Malloc power list failed") ;

            ic_element_list_destroy (&ic_element_list) ;

            YYABORT ;
        }

        powers_queue_build (powers_list, 10) ;

        put_into_powers_queue (powers_list, $1) ;
    }

  | power_values_list ',' DVALUE         // $1 the power list so far ...
                                         // $3 the poer value to add
    {
        put_into_powers_queue ($1, $3) ;

        $$ = $1 ;
    }
  ;

%%

/******************************************************************************/

void floorplan_parser_error
(
    Floorplan_t  *floorplan,
    Dimensions_t *__attribute__ ((unused)) dimensions,
    yyscan_t      yyscanner,
    const char   *msg
)
{
    fprintf (stderr, "%s:%d: %s\n",
        floorplan->FileName, floorplan_parser_get_lineno(yyscanner), msg) ;
}

/******************************************************************************/
