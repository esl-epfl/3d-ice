/******************************************************************************
 * This file is part of 3D-ICE, version 1.0.1 .                               *
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
 *          Martino Ruggiero                                                  *
 *          Thomas Brunschwiler                                               *
 *          David Atienza                                                     *
 *                                                                            *
 * For any comment, suggestion or request  about 3D-ICE, please  register and *
 * write to the mailing list (see http://listes.epfl.ch/doc.cgi?liste=3d-ice) *
 *                                                                            *
 * EPFL-STI-IEL-ESL                                                           *
 * Batiment ELG, ELG 130                Mail : 3d-ice@listes.epfl.ch          *
 * Station 11                                  (SUBSCRIPTION IS NECESSARY)    *
 * 1015 Lausanne, Switzerland           Url  : http://esl.epfl.ch/3d-ice.html *
 ******************************************************************************/

%{
#include "types.h"
#include "macros.h"
#include "dimensions.h"
#include "material.h"
#include "die.h"
#include "layer.h"
#include "stack_element.h"
#include "stack_description.h"
%}

// The YYSTYPE union used to collect the types of tokens and rules

%union
{
    double         double_v ;
    String_t       char_p ;
    Material*      material_p ;
    Coolant_t      coolant_v ;
    ChannelModel_t channel_model_v ;
    Die*           die_p ;
    Layer*         layer_p ;
    StackElement*  stack_element_p ;
}

%type <double_v>        first_wall_length
%type <double_v>        last_wall_length
%type <channel_model_v> distribution
%type <coolant_v>       coolant_heat_transfer_coefficients_rm4
%type <coolant_v>       coolant_heat_transfer_coefficients_rm2
%type <material_p>      material
%type <material_p>      materials_list
%type <die_p>           die
%type <die_p>           dies_list
%type <layer_p>         layer
%type <layer_p>         layers_list
%type <layer_p>         source_layer
%type <layer_p>         layer_content
%type <stack_element_p> stack_element
%type <stack_element_p> stack_elements

%token AMBIENT               "keyword ambient"
%token BOTTOM                "keyword bottom"
%token CAPACITY              "keyword capacity"
%token CELL                  "keyword cell"
%token CHANNEL               "keyword channel"
%token CHIP                  "keyword chip"
%token COEFFICIENT           "keyword coefficient"
%token CONDUCTIVITY          "keyword conductivity"
%token CONVENTIONAL          "keyword conventional"
%token COOLANT               "keyword coolant"
%token DARCY                 "keyword darcy"
%token DIAMETER              "keyword diameter"
%token DIE                   "keyword die"
%token DIMENSIONS            "keyword dimensions"
%token DISTRIBUTION          "keyword distribution"
%token FIRST                 "keyword first"
%token FLOORPLAN             "keyword floorplan"
%token FLOW                  "keyword flow"
%token HEAT                  "keyword heat"
%token HEIGHT                "keyword height"
%token INCOMING              "keyword incoming"
%token INLINE                "keyword inline"
%token LAST                  "keyword last"
%token LAYER                 "keyword layer"
%token LENGTH                "keyword length"
%token MATERIAL              "keyword material"
%token MICROCHANNEL          "keyword microchannel"
%token PIN                   "keyword pin"
%token PINFIN                "keyword pinfin"
%token PITCH                 "keyword pitch"
%token RATE                  "keyword rate"
%token RM2                   "keyword rm2"
%token RM4                   "keyword rm4"
%token SIDE                  "keyword side"
%token SINK                  "keywork sink"
%token SOURCE                "keyword source"
%token STACK                 "keyword stack"
%token STAGGERED             "keyword staggered"
%token TEMPERATURE           "keyword temperature"
%token THERMAL               "keyword thermal"
%token TOP                   "keyword top"
%token TRANSFER              "keyword transfer"
%token VELOCITY              "keyword velocity"
%token VOLUMETRIC            "keywork volumetric"
%token WALL                  "keyword wall"
%token WIDTH                 "keyword width"

%token <double_v> DVALUE     "double value"
%token <char_p>   IDENTIFIER "identifier"
%token <char_p>   PATH       "path to file"

%destructor { FREE_POINTER (free,                     $$) ; } <char_p>
%destructor { FREE_POINTER (free_layers_list,         $$) ; } <layer_p>

%{
#include "../flex/stack_description_scanner.h"

void stack_description_error
(
    StackDescription* stack,
    yyscan_t          scanner,
    String_t          message
) ;

static char error_message [100] ;

static CellDimension_t first_wall_length = CELLDIMENSION_I ;
static CellDimension_t last_wall_length  = CELLDIMENSION_I ;
static CellDimension_t wall_length       = CELLDIMENSION_I ;
static CellDimension_t channel_length    = CELLDIMENSION_I ;
static Quantity_t      num_channels      = QUANTITY_I ;
static Quantity_t      num_dies          = QUANTITY_I ;
%}

%name-prefix "stack_description_"
%output      "stack_description_parser.c"

%pure-parser
%error-verbose
%parse-param { StackDescription *stkd }
%parse-param { yyscan_t scanner }
%lex-param   { yyscan_t scanner }

%initial-action
{
    first_wall_length = CELLDIMENSION_I ;
    last_wall_length  = CELLDIMENSION_I ;
    wall_length       = CELLDIMENSION_I ;
    channel_length    = CELLDIMENSION_I ;
    num_channels      = QUANTITY_I ;
    num_dies          = QUANTITY_I ;
} ;

%start stack_description_file

%%

/******************************************************************************/
/******************************* File content *********************************/
/******************************************************************************/

stack_description_file

  : materials_list
    conventional_heat_sink
    microchannel
    dies_list
    stack
    dimensions
  ;

/******************************************************************************/
/******************************* Materials ************************************/
/******************************************************************************/

materials_list

  : material                // $1 : pointer to the first material found
    {
        stkd->MaterialsList = $1 ;

        $$ = $1 ;           // $1 will be the new last element in the list
    }
  | materials_list material // $1 : pointer to the last material in the list
                            // $2 : pointer to the material to add in the list
    {

        if (find_material_in_list (stkd->MaterialsList, $2->Id) != NULL)
        {
            sprintf (error_message, "Material %s already declared", $2->Id) ;

            FREE_POINTER (free_material, $2) ;

            stack_description_error (stkd, scanner, error_message) ;

            YYABORT ;
        }

        $1->Next = $2 ;     // insert $2 at the end of the list
        $$ = $2 ;           // $2 will be the new last element in the list
    }
  ;

material

  : MATERIAL IDENTIFIER ':'                     // $2
        THERMAL CONDUCTIVITY     DVALUE ';'     // $6
        VOLUMETRIC HEAT CAPACITY DVALUE ';'     // $11
    {
        Material *material = $$ = alloc_and_init_material () ;

        if (material == NULL)
        {
            FREE_POINTER (free, $2) ;

            stack_description_error (stkd, scanner, "Malloc material failed") ;

            YYABORT ;
        }

        material->Id                     = $2 ;
        material->ThermalConductivity    = $6 ;
        material->VolumetricHeatCapacity = $11 ;
    }
  ;

/******************************************************************************/
/******************************* Heatsink *************************************/
/******************************************************************************/

conventional_heat_sink

  : // Declaring the heat sink section is not mandatory

  | CONVENTIONAL HEAT SINK ':'
        HEAT TRANSFER COEFFICIENT DVALUE ';'  // $8
        AMBIENT TEMPERATURE       DVALUE ';'  // $12
    {
        stkd->ConventionalHeatSink = alloc_and_init_conventional_heat_sink () ;

        if (stkd->ConventionalHeatSink == NULL)
        {
            stack_description_error (stkd, scanner, "Malloc conventional heat sink failed") ;

            YYABORT ;
        }

        stkd->ConventionalHeatSink->AmbientHTC         = $8 ;
        stkd->ConventionalHeatSink->AmbientTemperature = $12 ;
    }
  ;

/******************************************************************************/
/******************************* MicroChannel *********************************/
/******************************************************************************/

microchannel

  : // Declaring the channel section is not mandatory

  |  MICROCHANNEL RM4 ':'
        HEIGHT DVALUE ';'                             //  $5
        CHANNEL LENGTH DVALUE ';'                     //  $9
        WALL    LENGTH DVALUE ';'                     //  $13
        first_wall_length                             //  $15
        last_wall_length                              //  $16
        WALL MATERIAL IDENTIFIER ';'                  //  $19
        COOLANT FLOW RATE DVALUE ';'                  //  $24
        coolant_heat_transfer_coefficients_rm4        //  $26
        COOLANT VOLUMETRIC HEAT CAPACITY DVALUE ';'   //  $31
        COOLANT INCOMING TEMPERATURE DVALUE ';'       //  $36
    {
        stkd->Channel = alloc_and_init_channel () ;

        if (stkd->Channel == NULL)
        {
            FREE_POINTER (free, $19) ;

            stack_description_error (stkd, scanner, "Malloc channel failed") ;

            YYABORT ;
        }

        channel_length    = $9 ;
        wall_length       = $13 ;
        first_wall_length = ($15 != 0.0) ? $15 : $13 ;
        last_wall_length  = ($16 != 0.0) ? $16 : $13 ;

        stkd->Channel->ChannelModel      = TDICE_CHANNEL_MODEL_MC_RM4 ;
        stkd->Channel->Height            = $5 ;
        stkd->Channel->CoolantFR         = CONVERT_COOLANT_FLOW_RATE($24) ;
        stkd->Channel->Coolant.HTCSide   = $26.HTCSide ;
        stkd->Channel->Coolant.HTCTop    = $26.HTCTop ;
        stkd->Channel->Coolant.HTCBottom = $26.HTCBottom ;
        stkd->Channel->Coolant.VHC       = $31 ;
        stkd->Channel->Coolant.TIn       = $36 ;
        stkd->Channel->WallMaterial      = find_material_in_list (stkd->MaterialsList, $19) ;

        if (stkd->Channel->WallMaterial == NULL)
        {
            sprintf (error_message, "Unknown material %s", $19) ;

            FREE_POINTER (free, $19) ;

            stack_description_error (stkd, scanner, error_message) ;

            YYABORT ;
        }

        stkd->Channel->WallMaterial->Used++ ;

        FREE_POINTER (free, $19) ;
    }

  |  MICROCHANNEL RM2 ':'
        HEIGHT DVALUE ';'                            //  $5
        CHANNEL LENGTH DVALUE ';'                    //  $9
        WALL    LENGTH DVALUE ';'                    //  $13
        WALL MATERIAL IDENTIFIER ';'                 //  $17
        COOLANT FLOW RATE DVALUE ';'                 //  $22
        coolant_heat_transfer_coefficients_rm2       //  $24
        COOLANT VOLUMETRIC HEAT CAPACITY DVALUE ';'  //  $29
        COOLANT INCOMING TEMPERATURE DVALUE ';'      //  $34
    {
        stkd->Channel = alloc_and_init_channel () ;

        if (stkd->Channel == NULL)
        {
            FREE_POINTER (free, $17) ;

            stack_description_error (stkd, scanner, "Malloc channel failed") ;

            YYABORT ;
        }

        stkd->Channel->ChannelModel      = TDICE_CHANNEL_MODEL_MC_RM2 ;
        stkd->Channel->Height            = $5 ;
        stkd->Channel->Length            = $9 ;
        stkd->Channel->Pitch             = $13 + $9 ;
        stkd->Channel->Porosity          = stkd->Channel->Length / stkd->Channel->Pitch ;
        stkd->Channel->CoolantFR         = CONVERT_COOLANT_FLOW_RATE($22) ;
        stkd->Channel->Coolant.HTCSide   = $24.HTCSide ;
        stkd->Channel->Coolant.HTCTop    = $24.HTCTop ;
        stkd->Channel->Coolant.HTCBottom = $24.HTCBottom ;
        stkd->Channel->Coolant.VHC       = $29 ;
        stkd->Channel->Coolant.TIn       = $34 ;
        stkd->Channel->WallMaterial      = find_material_in_list (stkd->MaterialsList, $17) ;

        if (stkd->Channel->WallMaterial == NULL)
        {
            sprintf (error_message, "Unknown material %s", $17) ;

            FREE_POINTER (free, $17) ;

            stack_description_error (stkd, scanner, error_message) ;

            YYABORT ;
        }

        stkd->Channel->WallMaterial->Used++ ;

        FREE_POINTER (free, $17) ;
    }

  |  PINFIN ':'
        HEIGHT DVALUE ';'                              //  $4
        PIN DIAMETER DVALUE ';'                        //  $8
        PIN PITCH  DVALUE ';'                          //  $12
        PIN DISTRIBUTION distribution ';'              //  $16
        PIN MATERIAL IDENTIFIER ';'                    //  $20
        DARCY VELOCITY DVALUE ';'                      //  $24
        COOLANT VOLUMETRIC HEAT CAPACITY DVALUE ';'    //  $30
        COOLANT INCOMING TEMPERATURE DVALUE ';'        //  $35
    {
        stkd->Channel = alloc_and_init_channel () ;

        if (stkd->Channel == NULL)
        {
            FREE_POINTER (free, $20) ;

            stack_description_error (stkd, scanner, "Malloc channel failed") ;

            YYABORT ;
        }

        stkd->Channel->Height            = $4 ;
        stkd->Channel->Porosity          = 1.0 - (PI * $8 * $8 / 4.0) / ($12 * $12) ;
        stkd->Channel->ChannelModel      = $16 ;
        stkd->Channel->DarcyVelocity     = $24 ;
        stkd->Channel->Coolant.HTCSide   = COOLANTHTC_I ;
        stkd->Channel->Coolant.HTCTop    = COOLANTHTC_I ;
        stkd->Channel->Coolant.HTCBottom = COOLANTHTC_I ;
        stkd->Channel->Coolant.VHC       = $30 ;
        stkd->Channel->Coolant.TIn       = $35 ;
        stkd->Channel->WallMaterial      = find_material_in_list (stkd->MaterialsList, $20) ;

        if (stkd->Channel->WallMaterial == NULL)
        {
            sprintf (error_message, "Unknown material %s", $20) ;

            FREE_POINTER (free, $20) ;

            stack_description_error (stkd, scanner, error_message) ;

            YYABORT ;
        }

        stkd->Channel->WallMaterial->Used++ ;

        FREE_POINTER (free, $20) ;
    }
  ;

coolant_heat_transfer_coefficients_rm4

  : COOLANT HEAT TRANSFER COEFFICIENT DVALUE ';'
    {
        $$.HTCSide   = $5 ;
        $$.HTCTop    = $5 ;
        $$.HTCBottom = $5 ;
    }
  | COOLANT HEAT TRANSFER COEFFICIENT SIDE   DVALUE ','
                                      TOP    DVALUE ','
                                      BOTTOM DVALUE ';'
    {
        $$.HTCSide   = $6 ;
        $$.HTCTop    = $9 ;
        $$.HTCBottom = $12 ;
    }
  ;

coolant_heat_transfer_coefficients_rm2

  : COOLANT HEAT TRANSFER COEFFICIENT DVALUE ';'
    {
        $$.HTCSide   = COOLANTHTC_I ;
        $$.HTCTop    = $5 ;
        $$.HTCBottom = $5 ;
    }
  | COOLANT HEAT TRANSFER COEFFICIENT TOP    DVALUE ','
                                      BOTTOM DVALUE ';'
    {
        $$.HTCSide   = COOLANTHTC_I ;
        $$.HTCTop    = $6 ;
        $$.HTCBottom = $9 ;
    }
  ;

first_wall_length
  :                              { $$ = CELLDIMENSION_I ; } // Not mandatory
  | FIRST WALL LENGTH DVALUE ';' { $$ = $4 ;              }
  ;

last_wall_length
  :                             { $$ = CELLDIMENSION_I ; } // Not mandatory
  | LAST WALL LENGTH DVALUE ';' { $$ = $4 ;              }
  ;

distribution
  : INLINE                      { $$ = TDICE_CHANNEL_MODEL_PF_INLINE ;    }
  | STAGGERED                   { $$ = TDICE_CHANNEL_MODEL_PF_STAGGERED ; }
  ;

/******************************************************************************/
/******************************* Layers ***************************************/
/******************************************************************************/

layers_list

  :                    // Declaring a "non active" layer in a die is not mandatory
    {
        $$ = NULL ;    // The first layer in the list will be null
    }
  | layers_list layer  // $1 : pointer to the last layer in the list
                       // $2 : pointer to the layer to add in the list
    {
        if ($1 != NULL)
            JOIN_ELEMENTS($2, $1) ;  // this reverse the order !
        $$ = $2 ;                    // $2 will be the new reference to the list
    }
  ;

layer         : LAYER  layer_content { $$ = $2 ; } ;

source_layer  : SOURCE layer_content { $$ = $2 ; } ;

layer_content :

    DVALUE IDENTIFIER ';' // $1 and $2

    {
        Layer* layer = $$ = alloc_and_init_layer () ;

        if (layer == NULL)
        {
            FREE_POINTER (free, $2) ;

            stack_description_error (stkd, scanner, "Malloc layer failed") ;

            YYABORT ;
        }

        layer->Height   = (CellDimension_t) $1 ;
        layer->Material = find_material_in_list (stkd->MaterialsList, $2) ;

        if (layer->Material == NULL)
        {
            sprintf (error_message, "Unknown material %s", $2) ;

            FREE_POINTER (free,       $2) ;
            FREE_POINTER (free_layer, layer) ;

            stack_description_error (stkd, scanner, error_message) ;

            YYABORT ;
        }

        layer->Material->Used++ ;

        FREE_POINTER (free, $2) ;
    }
  ;

/******************************************************************************/
/******************************* Dies *****************************************/
/******************************************************************************/

dies_list

  : die                   // $1 : pointer to the first die found
    {
        stkd->DiesList = $1 ;
        $$ = $1 ;         // $1 will be the new die in the list
    }
  | dies_list die         // $1 : pointer to the last die in the list
                          // $2 : pointer to the die to add in the list
    {

        if (find_die_in_list (stkd->DiesList, $2->Id) != NULL)
        {
            sprintf (error_message, "Die %s already declared", $2->Id) ;

            FREE_POINTER (free_die, $2) ;

            stack_description_error (stkd, scanner, error_message) ;

            YYABORT ;
        }

        $1->Next = $2 ;   // insert $2 at the end of the list
        $$ = $2 ;         // $2 will be the new last element in the list
    }
  ;

die

  : DIE IDENTIFIER ':'         // $2 Die identifier
        layers_list            // $4 list of layers (above source -> top)
        source_layer           // $5 source layer
        layers_list            // $6 list of layers (bottom -> below source)
    {
        Die* die = $$ = alloc_and_init_die () ;

        if (die == NULL)
        {
            FREE_POINTER (free,             $2) ;
            FREE_POINTER (free_layers_list, $4) ;
            FREE_POINTER (free_layer,       $5) ;
            FREE_POINTER (free_layers_list, $6);

            stack_description_error (stkd, scanner, "Malloc die failed") ;

            YYABORT ;
        }

        die->Id = $2 ;
        die->SourceLayer = $5 ;

        // The layers within a die are declared in the stack file from top
        // to bottom but here we revert the order: the first layer in the list
        // LayersList will be the bottom-most layer in the die

        if ($6 != NULL)
        {
            // if there are layers below the source,
            // then the list of layers begins with $6

            die->BottomLayer = $6 ;

            die->NLayers++ ;
            die->SourceLayerOffset++ ;

            // $6 moved until the end ..

            while ($6->Next != NULL)
            {
                $6 = $6->Next ;

                die->NLayers++ ;
                die->SourceLayerOffset++ ;
            }

            // the list $6 continues with the source layer $5

            JOIN_ELEMENTS ($6, $5) ;
        }
        else
        {
            // if there aren't layers below the source, the list of layers
            // begins directly with the source layer $5

            die->BottomLayer = $5 ;
        }

        die->NLayers++ ;

        if ($4 != NULL)
        {
            // if there are layers above the source
            // $5 is connected to the list $4

            JOIN_ELEMENTS ($5, $4) ;

            die->NLayers++ ;

            // $4 moved until the end ..

            while ($4->Next != NULL)
            {
                $4 = $4->Next ;

                die->NLayers++ ;
            }

            // the list finishes with the last layer in $4

            die->TopLayer = $4 ;
        }
        else
        {
            // if there aren't layers below the source,
            // The list finishes with the source layer $5

            die->TopLayer = $5 ;
        }
    }
  ;

/******************************************************************************/
/******************************* Stack ****************************************/
/******************************************************************************/

stack

  : STACK ':'
        stack_elements
    {
        if (num_dies == 0)
        {
            stack_description_error (stkd, scanner, "Error: stack must contain at least one die") ;

            YYABORT ;
        }

        if (stkd->BottomStackElement->Type == TDICE_STACK_ELEMENT_CHANNEL)
        {
            stack_description_error (stkd, scanner, "Error: cannot declare a channel as bottom-most stack element") ;

            YYABORT ;
        }

        if (stkd->ConventionalHeatSink == NULL && stkd->Channel == NULL)

            fprintf (stderr, "Warning: neither heat sink nor channel has been declared\n") ;


        FOR_EVERY_ELEMENT_IN_LIST_FORWARD (Material, material, stkd->MaterialsList)

            if (material->Used == 0)

                fprintf (stderr, "Warning: material %s declared but not used\n", material->Id) ;


        FOR_EVERY_ELEMENT_IN_LIST_FORWARD (Die, die, stkd->DiesList)

            if (die->Used == 0)

                fprintf (stderr, "Warning: die %s declared but not used\n", die->Id) ;

        if (stkd->ConventionalHeatSink != NULL)
        {
            if (stkd->TopStackElement->Type == TDICE_STACK_ELEMENT_LAYER)
            {
                stkd->ConventionalHeatSink->TopLayer = stkd->TopStackElement->Pointer.Layer ;
            }
            else
            {
                stkd->ConventionalHeatSink->TopLayer = stkd->TopStackElement->Pointer.Die->TopLayer ;
            }
        }

        // Fix thelayer offset starting from the bottom most element in the
        // stack. This operation can be done only here since the parser processes
        // elements in the stack from the top most.

        GridDimension_t layer_index = GRIDDIMENSION_I ;

        FOR_EVERY_ELEMENT_IN_LIST_FORWARD (StackElement, stk_el, stkd->BottomStackElement)
        {
            stk_el->Offset = layer_index ;
            layer_index   += stk_el->NLayers ;
        }
    }
  ;

stack_elements

  : stack_element                 // $1 : pointer to the first stack element found
    {
        if (   stkd->TopStackElement == NULL && $1->Type == TDICE_STACK_ELEMENT_CHANNEL)
        {
            stack_description_error (stkd, scanner, "Error: cannot declare a channel as top-most stack element") ;

            YYABORT ;
        }

        stkd->TopStackElement    = $1 ;
        stkd->BottomStackElement = $1 ;
        $$ = $1 ;
    }
  | stack_elements stack_element  // $1 : pointer to the last stack element in the list
                                  // $2 : pointer to the stack element to add in the list
    {
        if (find_stack_element_in_list ($1, $2->Id) != NULL)
        {
            sprintf (error_message, "Stack element %s already declared", $2->Id) ;

            FREE_POINTER (free_stack_element, $2) ;

            stack_description_error (stkd, scanner, error_message) ;

            YYABORT ;
        }

        if (   $1->Type == TDICE_STACK_ELEMENT_CHANNEL
            && $2->Type == TDICE_STACK_ELEMENT_CHANNEL)
        {
            stack_description_error (stkd, scanner, "Error: cannot declare two consecutive channels") ;

            YYABORT ;
        }

        JOIN_ELEMENTS ($2, $1) ;

        stkd->BottomStackElement = $2 ;
        $$ = $2 ;                 // $2 will be the last stack element in the list 
    }
  ;

stack_element

  : LAYER IDENTIFIER DVALUE IDENTIFIER ';'    // $2 Identifier for the stack element
                                              // $3 Height of the layer
                                              // $4 Identifier of the material
    {
        StackElement* stack_element = $$ = alloc_and_init_stack_element () ;

        if (stack_element == NULL)
        {
            FREE_POINTER (free, $2) ;
            FREE_POINTER (free, $4) ;

            stack_description_error (stkd, scanner, "Malloc stack element failed") ;

            YYABORT ;
        }

        Layer* layer = alloc_and_init_layer () ;

        if (layer == NULL)
        {
            FREE_POINTER (free,               $2) ;
            FREE_POINTER (free,               $4) ;
            FREE_POINTER (free_stack_element, stack_element) ;

            stack_description_error (stkd, scanner, "Malloc layer failed") ;

            YYABORT ;
        }

        layer->Height   = (CellDimension_t) $3 ;
        layer->Material = find_material_in_list (stkd->MaterialsList, $4) ;

        if (layer->Material == NULL)
        {
            sprintf (error_message, "Unknown material %s", $4) ;

            FREE_POINTER (free,               $2) ;
            FREE_POINTER (free,               $4) ;
            FREE_POINTER (free_stack_element, stack_element) ;
            FREE_POINTER (free_layer,         layer) ;

            stack_description_error (stkd, scanner, error_message) ;

            YYABORT ;
        }

        layer->Material->Used++ ;

        FREE_POINTER (free, $4) ;

        stack_element->Type          = TDICE_STACK_ELEMENT_LAYER ;
        stack_element->Pointer.Layer = layer ;
        stack_element->Id            = $2 ;
        stack_element->NLayers       = 1 ;
    }

  | CHANNEL IDENTIFIER ';'  // $2 Identifier for the stack element
    {
        num_channels++ ;

        if (stkd->Channel == NULL)
        {
            stack_description_error (stkd, scanner, "Error: channel used in stack but not declared") ;

            YYABORT ;
        }

        StackElement* stack_element = $$ = alloc_and_init_stack_element () ;

        if (stack_element == NULL)
        {
            FREE_POINTER (free, $2) ;

            stack_description_error (stkd, scanner, "Malloc stack element failed") ;

            YYABORT ;
        }

        stack_element->Type            = TDICE_STACK_ELEMENT_CHANNEL ;
        stack_element->Pointer.Channel = stkd->Channel ; // This might be NULL !!!
        stack_element->Id              = $2 ;

        switch (stkd->Channel->ChannelModel)
        {
            case TDICE_CHANNEL_MODEL_MC_RM2 :
            case TDICE_CHANNEL_MODEL_PF_INLINE :
            case TDICE_CHANNEL_MODEL_PF_STAGGERED :
                stack_element->NLayers = NUM_LAYERS_2RM ;
                break;
            default : // TDICE_CHANNEL_MODEL_MC_4RM
                stack_element->NLayers = NUM_LAYERS_4RM ;
        }
    }

  | DIE IDENTIFIER IDENTIFIER FLOORPLAN PATH ';'  // $2 Identifier for the stack element
                                                  // $3 Identifier of the die
                                                  // $5 Path of the floorplan file
    {
        num_dies++ ;

        StackElement* stack_element = $$ = alloc_and_init_stack_element () ;

        if (stack_element == NULL)
        {
            FREE_POINTER (free, $2) ;
            FREE_POINTER (free, $3) ;
            FREE_POINTER (free, $5) ;

            stack_description_error (stkd, scanner, "Malloc stack element failed") ;

            YYABORT ;
        }

        stack_element->Type        = TDICE_STACK_ELEMENT_DIE ;
        stack_element->Id          = $2 ;
        stack_element->Pointer.Die = find_die_in_list (stkd->DiesList, $3) ;

        if (stack_element->Pointer.Die == NULL)
        {
            sprintf (error_message, "Unknown die %s", $3) ;

            FREE_POINTER (free,               $3) ;
            FREE_POINTER (free,               $5) ;
            FREE_POINTER (free_stack_element, stack_element) ;

            stack_description_error (stkd, scanner, error_message) ;

            YYABORT ;
        }

        stack_element->Pointer.Die->Used++ ;

        stack_element->NLayers = stack_element->Pointer.Die->NLayers ;
        stack_element->Floorplan = alloc_and_init_floorplan () ;

        if (stack_element->Floorplan == NULL)
        {
            FREE_POINTER (free,               $3) ;
            FREE_POINTER (free,               $5) ;
            FREE_POINTER (free_stack_element, stack_element) ;

            stack_description_error (stkd, scanner, "Malloc floorplan failed") ;

            YYABORT ;
        }

        stack_element->Floorplan->FileName = $5 ;

        FREE_POINTER (free, $3) ;
    }
  ;

/******************************************************************************/
/******************************* Dimensions ***********************************/
/******************************************************************************/

dimensions

  : DIMENSIONS ':'
        CHIP LENGTH DVALUE ',' WIDTH DVALUE ';'    // $5  $8
        CELL LENGTH DVALUE ',' WIDTH DVALUE ';'    // $12 $15
    {
        stkd->Dimensions = alloc_and_init_dimensions () ;

        if (stkd->Dimensions == NULL)
        {
            stack_description_error (stkd, scanner, "Malloc dimensions failed") ;
            YYABORT ;
        }

        stkd->Dimensions->Chip.Length = (ChipDimension_t) $5 ;
        stkd->Dimensions->Chip.Width  = (ChipDimension_t) $8 ;

        stkd->Dimensions->Cell.ChannelLength   = $12 ;
        stkd->Dimensions->Cell.FirstWallLength = $12 ;
        stkd->Dimensions->Cell.LastWallLength  = $12 ;
        stkd->Dimensions->Cell.WallLength      = $12 ;

        stkd->Dimensions->Cell.Width  = (CellDimension_t) $15 ;

        stkd->Dimensions->Grid.NRows    = (GridDimension_t) ($8 / $15) ;
        stkd->Dimensions->Grid.NColumns = (GridDimension_t) ($5 / $12) ;


        if (stkd->Channel != NULL)
        {
            if (stkd->Channel->ChannelModel == TDICE_CHANNEL_MODEL_MC_RM4)
            {
                stkd->Dimensions->Cell.ChannelLength   = channel_length ;
                stkd->Dimensions->Cell.FirstWallLength = first_wall_length ;
                stkd->Dimensions->Cell.LastWallLength  = last_wall_length ;
                stkd->Dimensions->Cell.WallLength      = wall_length ;

                CellDimension_t ratio
                    = ($5 - first_wall_length - last_wall_length -channel_length)
                    /
                    (channel_length + wall_length) ;

                if ( ratio - (int) ratio != 0)
                {
                    stack_description_error (stkd, scanner, "Error: cell dimensions does not fit the chip length correctly") ;

                    YYABORT ;
                }

                stkd->Dimensions->Grid.NColumns = 2 * ratio + 3 ;

                if ((stkd->Dimensions->Grid.NColumns & 1) == 0)
                {
                    stack_description_error (stkd, scanner, "Error: colum number must be odd when channel is declared") ;

                    YYABORT ;
                }

                // Check the number of columns

                if (stkd->Dimensions->Grid.NColumns < 3)
                {
                    stack_description_error (stkd, scanner, "Error: not enough columns") ;

                    YYABORT ;
                }

                stkd->Channel->NChannels = (Quantity_t) ((stkd->Dimensions->Grid.NColumns - 1 )  / 2) ;
            }
            else if (stkd->Channel->ChannelModel == TDICE_CHANNEL_MODEL_MC_RM2)
            {
                stkd->Channel->NChannels = (Quantity_t) (($5 / stkd->Channel->Pitch) + 0.5) ; // round function
            }
        }

        // Counts the number of layers

        FOR_EVERY_ELEMENT_IN_LIST_FORWARD (StackElement, stk_el, stkd->BottomStackElement)
        {
            stkd->Dimensions->Grid.NLayers += stk_el->NLayers ;
        }

        // Evaluate the number of cells and nonzero elements

        stkd->Dimensions->Grid.NCells
            =   get_number_of_layers (stkd->Dimensions)
                * get_number_of_rows (stkd->Dimensions)
                * get_number_of_columns (stkd->Dimensions) ;

        if ((uint32_t) stkd->Dimensions->Grid.NCells >  INT32_MAX)
        {
            stack_description_error
                (stkd, scanner, "Too many cells ... (SuperLU uses 'int')") ;

            YYABORT ;
        }

        if (   stkd->Channel == NULL
            || stkd->Channel->ChannelModel == TDICE_CHANNEL_MODEL_MC_RM4)
        {
            // NO CHANNEL OR IT IS TDICE_CHANNEL_MODEL_MC_4RM

            stkd->Dimensions->Grid.NConnections =

                    // number of coefficients in the diagonal
                    get_number_of_layers (stkd->Dimensions)
                    * get_number_of_rows (stkd->Dimensions)
                    * get_number_of_columns (stkd->Dimensions)
                    +
                    // number of coefficients bottom <-> top
                    2 * (get_number_of_layers (stkd->Dimensions) - 1 )
                    * get_number_of_rows (stkd->Dimensions)
                    * get_number_of_columns (stkd->Dimensions)
                    +
                    // Number of coefficients North <-> South
                    2 * get_number_of_layers (stkd->Dimensions)
                    * (get_number_of_rows (stkd->Dimensions) - 1 )
                    * get_number_of_columns (stkd->Dimensions)
                    +
                    // Number of coefficients East <-> West
                    2 * get_number_of_layers (stkd->Dimensions)
                    * get_number_of_rows (stkd->Dimensions)
                    * (get_number_of_columns (stkd->Dimensions) - 1 ) ;

        }
        else if (   stkd->Channel->ChannelModel == TDICE_CHANNEL_MODEL_PF_INLINE
                 || stkd->Channel->ChannelModel == TDICE_CHANNEL_MODEL_PF_STAGGERED)
        {

            Quantity_t num_layers_for_channel = num_channels * NUM_LAYERS_2RM ;

            Quantity_t num_layers_except_channel = get_number_of_layers (stkd->Dimensions) - num_layers_for_channel ;

            stkd->Dimensions->Grid.NConnections =
                    // For Normal Cells
                    // Number of coefficients in the diagonal
                        num_layers_except_channel
                    * get_number_of_rows (stkd->Dimensions)
                    * get_number_of_columns (stkd->Dimensions)
                    +
                    // Number of coefficients Bottom <-> Top
                    2 * num_layers_except_channel
                        * get_number_of_rows (stkd->Dimensions)
                        * get_number_of_columns (stkd->Dimensions)
                    +
                    // Number of coefficients North <-> South
                    2 * num_layers_except_channel
                        * (get_number_of_rows (stkd->Dimensions) - 1 )
                        * get_number_of_columns (stkd->Dimensions)
                    +
                    // Number of coefficients East <-> West
                    2 * num_layers_except_channel
                        * get_number_of_rows (stkd->Dimensions)
                        * (get_number_of_columns (stkd->Dimensions) - 1 )
                    +

                    // For Channel Cells
                    // Number of coefficients in the diagonal
                        num_layers_for_channel
                        * get_number_of_rows (stkd->Dimensions)
                        * get_number_of_columns (stkd->Dimensions)
                    +
                    // Number of coefficients Bottom <-> Top
                    2 * (num_layers_for_channel + num_channels)
                        * get_number_of_rows (stkd->Dimensions)
                        * get_number_of_columns (stkd->Dimensions)
                    +
                    // Number of coefficients North <-> South
                    2 * num_channels
                        * (get_number_of_rows (stkd->Dimensions) - 1 )
                        * get_number_of_columns (stkd->Dimensions)
                    +
                    // Number of coefficients East <-> West
                    0
                    ;

        }
        else
        {
            // TDICE_CHANNEL_MODEL_MC_2RM

            Quantity_t num_layers_for_channel = num_channels * NUM_LAYERS_2RM ;

            Quantity_t num_layers_except_channel = get_number_of_layers (stkd->Dimensions) - num_layers_for_channel ;

            stkd->Dimensions->Grid.NConnections =

                    // For Normal Cells
                    // Number of coefficients in the diagonal
                    num_layers_except_channel
                    * get_number_of_rows (stkd->Dimensions)
                    * get_number_of_columns (stkd->Dimensions)
                    +
                    // Number of coefficients Bottom <-> Top
                    2 * num_layers_except_channel
                    * get_number_of_rows (stkd->Dimensions)
                        * get_number_of_columns (stkd->Dimensions)
                    +
                    // Number of coefficients North <-> South
                    2 * num_layers_except_channel
                        * (get_number_of_rows (stkd->Dimensions) - 1 )
                        * get_number_of_columns (stkd->Dimensions)
                    +
                    // Number of coefficients East <-> West
                    2 * num_layers_except_channel
                        * get_number_of_rows (stkd->Dimensions)
                        * (get_number_of_columns (stkd->Dimensions) - 1 )
                    +

                    // For Channel Cells
                    // Number of coefficients in the diagonal
                        num_layers_for_channel
                        * get_number_of_rows (stkd->Dimensions)
                        * get_number_of_columns (stkd->Dimensions)
                    +
                    // Number of coefficients Bottom <-> Top
                    2 * (num_layers_for_channel + num_channels)
                        * get_number_of_rows (stkd->Dimensions)
                        * get_number_of_columns (stkd->Dimensions)
                    +
                    // Number of coefficients North <-> South
                    4 * num_channels
                        * (get_number_of_rows (stkd->Dimensions) - 1 )
                        * get_number_of_columns (stkd->Dimensions)
                    +
                    // Number of coefficients East <-> West
                    0
                    ;
        }
    }
  ;

%%

void stack_description_error
(
    StackDescription* stkd,
    yyscan_t          scanner,
    String_t          message
)
{
    fprintf (stack_description_get_out (scanner),
             "%s:%d: %s\n",
            stkd->FileName, stack_description_get_lineno (scanner), message) ;

    FREE_POINTER (free_stack_description, stkd) ;
}
