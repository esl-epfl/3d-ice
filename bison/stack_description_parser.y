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
#include <math.h>

#include "macros.h"
#include "material.h"
#include "die.h"
#include "layer.h"
#include "stack_element.h"
#include "stack_description.h"
#include "types.h"
%}

/* The YYSTYPE union used to collect the types of tokens and rules */

%union
{
   double        double_v ;
   String_t      char_p ;
   Material*     material_p ;
   Die*          die_p ;
   Layer*        layer_p ;
   StackElement* stack_element_p ;
   CoolantHTCs_t coolanthtcs_v ;
   ChannelModel_t channel_model_v ;
}

%type <double_v> first_wall_length
%type <double_v> last_wall_length
%type <channel_model_v> distribution

%type <coolanthtcs_v> coolant_heat_transfer_coefficients

%type <material_p> material
%type <material_p> materials_list

%type <die_p> die
%type <die_p> dies_list

%type <layer_p> layer
%type <layer_p> layer_content ;
%type <layer_p> layers
%type <layer_p> layers_list
%type <layer_p> source_layer

%type <stack_element_p> stack_element
%type <stack_element_p> stack_elements

%token AMBIENT               "keyword ambient"
%token CHANNEL               "keyword channel"
%token MC_4RM                "keyword microchannel_4rm"
%token MC_2RM                "keyword microchannel_2rm"
%token PF_2RM                "keyword pinfin_2rm"
%token CHIP                  "keyword chip"
%token CELL                  "keyword cell"
%token DENSITY               "keyword density"
%token DIAMETER              "keyword diameter"
%token DIE                   "keyword die"
%token DIMENSIONS            "keyword dimensions"
%token FIRST                 "keyword first"
%token FLOW                  "keyword flow"
%token RATE                  "keyword rate"
%token INCOMING              "keyword incoming"
%token TEMPERATURE           "keyword temperature"
%token LAYER                 "keyword layer"
%token LAST                  "keyword last"
%token MATERIAL              "keyword material"
%token ON                    "keyword on"
%token SOURCE                "keyword source"
%token SPECIFIC              "keyword specific"
%token HEAT                  "keyword heat"
%token THERMAL               "keyword thermal"
%token CONDUCTIVITY          "keyword conductivity"
%token HEIGHT                "keyword height"
%token WALL                  "keyword wall"
%token COOLANT               "keyword coolant"
%token TRANSFER              "keyword transfer"
%token COEFFICIENT           "keyword coefficient"
%token WIDTH                 "keyword width"
%token LENGTH                "keyword length"
%token FLOORPLAN             "keyword floorplan"
%token STACK                 "keyword stack"
%token VOLUMETRIC            "keywork volumetric"
%token CAPACITY              "keyword capacity"
%token CONVENTIONAL          "keyword conventional"
%token SINK                  "keywork sink"
%token SIDE                  "keyword side"
%token TOP                   "keyword top"
%token BOTTOM                "keyword bottom"
%token TYPE                  "keyword type"
%token PITCH                 "keyword pitch"
%token PIN                   "keyword pin"
%token PIN_FIN               "keyword pin-fin"
%token INLINE                "keyword inline"
%token STAGGERED             "keyword staggered"
%token DARCY                 "keyword darcy"
%token VELOCITY              "keyword velocity"

%token <double_v> DVALUE     "double value"
%token <char_p>   IDENTIFIER "identifier"
%token <char_p>   PATH       "path to file"
%token <channel_model_v>   CHANNEL_MODEL       "channel model"

%destructor { FREE_POINTER (free,                     $$) ; } <char_p>
%destructor { FREE_POINTER (free_layers_list,         $$) ; } <layers>
%destructor { FREE_POINTER (free_dies_list,           $$) ; } <dies>
%destructor { FREE_POINTER (free_stack_elements_list, $$) ; } <stack_elements>

%{
#include "../flex/stack_description_scanner.h"

void stack_description_error
(
  StackDescription* stack,
  yyscan_t          scanner,
  String_t          message
) ;

static StackElement*   last_stack_element    = NULL ;
static Bool_t          found_die             = FALSE_V ;
static Bool_t          found_channel         = FALSE_V ;
static CellDimension_t tmp_first_wall_length = CELLDIMENSION_I ;
static CellDimension_t tmp_last_wall_length  = CELLDIMENSION_I ;
static CellDimension_t tmp_wall_length       = CELLDIMENSION_I ;
static CellDimension_t tmp_channel_length    = CELLDIMENSION_I ;
%}

%name-prefix "stack_description_"
%output      "stack_description_parser.c"

%pure-parser
%error-verbose
%parse-param { StackDescription *stkd }
%parse-param { yyscan_t scanner }
%lex-param   { yyscan_t scanner }

%start stack_description_file

%%

/******************************************************************************/
/******************************* File content *********************************/
/******************************************************************************/

stack_description_file

  : materials_list
    conventional_heat_sink
    channel
    {
      if (stkd->Channel == NULL && stkd->ConventionalHeatSink == NULL)

        fprintf(stderr,
                "Warning: both ambient heat sink and channels are absent.\n") ;

    }
    dies_list
    stack
    dimensions
    {
      //
      // Checks if all the materials are used
      //

      FOR_EVERY_ELEMENT_IN_LIST_FORWARD (Material, material, stkd->MaterialsList)

        if (material->Used == 0)

          fprintf(stderr, "Warning: material %s declared but not used\n",
                  material->Id) ;

      //
      // Checks if all the dies are used
      //

      FOR_EVERY_ELEMENT_IN_LIST_FORWARD (Die, die, stkd->DiesList)

        if (die->Used == 0)

          fprintf(stderr, "Warning: die %s declared but not used\n",
                  die->Id) ;

      //
      // Counts the number of layers and channels
      //

      Quantity_t num_channels = QUANTITY_I;
      FOR_EVERY_ELEMENT_IN_LIST_FORWARD (StackElement, stk_el, stkd->StackElementsList) {

        stkd->Dimensions->Grid.NLayers += stk_el->NLayers ;
        if (stk_el->Type == TDICE_STACK_ELEMENT_CHANNEL) num_channels++;

      }

      //
      // Evaluate the number of cells and nonzero elements
      //

      stkd->Dimensions->Grid.NCells
        =   get_number_of_layers(stkd->Dimensions)
          * get_number_of_rows(stkd->Dimensions)
          * get_number_of_columns(stkd->Dimensions) ;

      if (stkd->Channel->ChannelModel == TDICE_CHANNEL_MODEL_PF_INLINE ||
          stkd->Channel->ChannelModel == TDICE_CHANNEL_MODEL_PF_STAGGERED) {

        Quantity_t num_layers_for_channel    =
          num_channels * NUM_LAYERS_2RM;

        Quantity_t num_layers_except_channel =
            get_number_of_layers(stkd->Dimensions) - num_layers_for_channel;

          stkd->Dimensions->Grid.NNz

            = // For Normal Cells
              // Number of coefficients in the diagonal
                num_layers_except_channel
              * get_number_of_rows(stkd->Dimensions)
              * get_number_of_columns(stkd->Dimensions)
              +
              // Number of coefficients Bottom <-> Top
              2 * num_layers_except_channel
                * get_number_of_rows(stkd->Dimensions)
                * get_number_of_columns(stkd->Dimensions)
              +
              // Number of coefficients North <-> South
              2 * num_layers_except_channel
                * (get_number_of_rows(stkd->Dimensions) - 1 )
                * get_number_of_columns(stkd->Dimensions)
              +
              // Number of coefficients East <-> West
              2 * num_layers_except_channel
                * get_number_of_rows(stkd->Dimensions)
                * (get_number_of_columns(stkd->Dimensions) - 1 )
              +

              // For Channel Cells
              // Number of coefficients in the diagonal
                  num_layers_for_channel
                * get_number_of_rows(stkd->Dimensions)
                * get_number_of_columns(stkd->Dimensions)
              +
              // Number of coefficients Bottom <-> Top
              2 * (num_layers_for_channel + num_channels)
                * get_number_of_rows(stkd->Dimensions)
                * get_number_of_columns(stkd->Dimensions)
              +
              // Number of coefficients North <-> South
              2 * num_channels
                * (get_number_of_rows(stkd->Dimensions) - 1 )
                * get_number_of_columns(stkd->Dimensions)
              +
              // Number of coefficients East <-> West
              0
              ;

      } else if (stkd->Channel->ChannelModel == TDICE_CHANNEL_MODEL_MC_2RM) {

        Quantity_t num_layers_for_channel    =
          num_channels * NUM_LAYERS_2RM;

        Quantity_t num_layers_except_channel =
          get_number_of_layers(stkd->Dimensions) - num_layers_for_channel;

        stkd->Dimensions->Grid.NNz

          = // For Normal Cells
            // Number of coefficients in the diagonal
              num_layers_except_channel
            * get_number_of_rows(stkd->Dimensions)
            * get_number_of_columns(stkd->Dimensions)
            +
            // Number of coefficients Bottom <-> Top
            2 * num_layers_except_channel
              * get_number_of_rows(stkd->Dimensions)
                * get_number_of_columns(stkd->Dimensions)
              +
              // Number of coefficients North <-> South
              2 * num_layers_except_channel
                * (get_number_of_rows(stkd->Dimensions) - 1 )
                * get_number_of_columns(stkd->Dimensions)
              +
              // Number of coefficients East <-> West
              2 * num_layers_except_channel
                * get_number_of_rows(stkd->Dimensions)
                * (get_number_of_columns(stkd->Dimensions) - 1 )
              +

              // For Channel Cells
              // Number of coefficients in the diagonal
                  num_layers_for_channel
                * get_number_of_rows(stkd->Dimensions)
                * get_number_of_columns(stkd->Dimensions)
              +
              // Number of coefficients Bottom <-> Top
              2 * (num_layers_for_channel + num_channels)
                * get_number_of_rows(stkd->Dimensions)
                * get_number_of_columns(stkd->Dimensions)
              +
              // Number of coefficients North <-> South
              4 * num_channels
                * (get_number_of_rows(stkd->Dimensions) - 1 )
                * get_number_of_columns(stkd->Dimensions)
              +
              // Number of coefficients East <-> West
              0
              ;

      } else { // TDICE_CHANNEL_MODEL_MC_4RM

        stkd->Dimensions->Grid.NNz

          = // number of coefficients in the diagonal
              get_number_of_layers(stkd->Dimensions)
            * get_number_of_rows(stkd->Dimensions)
            * get_number_of_columns(stkd->Dimensions)
            +
            // number of coefficients bottom <-> top
            2 * (get_number_of_layers(stkd->Dimensions) - 1 )
              * get_number_of_rows(stkd->Dimensions)
              * get_number_of_columns(stkd->Dimensions)
            +
            // Number of coefficients North <-> South
            2 * get_number_of_layers(stkd->Dimensions)
              * (get_number_of_rows(stkd->Dimensions) - 1 )
              * get_number_of_columns(stkd->Dimensions)
            +
            // Number of coefficients East <-> West
            2 * get_number_of_layers(stkd->Dimensions)
              * get_number_of_rows(stkd->Dimensions)
              * (get_number_of_columns(stkd->Dimensions) - 1 ) ;

      }
    }
  ;

/******************************************************************************/
/******************************* Materials ************************************/
/******************************************************************************/

materials_list

  : material                { stkd->MaterialsList = $1 ; $$ = $1 ; }
  | materials_list material { $1->Next = $2            ; $$ = $2 ; }
  ;

material

  : MATERIAL IDENTIFIER ':'
       THERMAL CONDUCTIVITY     DVALUE ';'
       VOLUMETRIC HEAT CAPACITY DVALUE ';'
    {
      Material *material = $$ = alloc_and_init_material() ;

      if (material == NULL)
      {
        FREE_POINTER (free, $2) ;

        stack_description_error
        (
          stkd, scanner, "malloc material failed"
        ) ;

        YYABORT ;
      }

      if (find_material_in_list(stkd->MaterialsList, $2) != NULL)
      {
        String_t message = malloc (sizeof(*message) * (26 + strlen($2))) ;

        if (message == NULL)
        {
          FREE_POINTER (free,          $2) ;
          FREE_POINTER (free_material, material) ;

          stack_description_error (stkd, scanner, "Malloc error") ;
          YYABORT ;
        }
        sprintf (message, "Material %s already declared", $2) ;

        FREE_POINTER (free,          $2) ;
        FREE_POINTER (free_material, material) ;

        stack_description_error (stkd, scanner, message) ;

        FREE_POINTER (free, message) ;

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

  : /* empty */
  | CONVENTIONAL HEAT SINK ':'
        HEAT TRANSFER COEFFICIENT DVALUE ';'
        AMBIENT TEMPERATURE       DVALUE ';'
    {
      stkd->ConventionalHeatSink = alloc_and_init_conventional_heat_sink() ;

      if (stkd->ConventionalHeatSink == NULL)
      {
        stack_description_error
        (
          stkd, scanner, "malloc environment heat sink failed"
        ) ;
        YYABORT ;
      }

      stkd->ConventionalHeatSink->AmbientHTC = $8 ;
      stkd->ConventionalHeatSink->AmbientTemperature = $12 ;
    }
  ;

/******************************************************************************/
/******************************* Channel **************************************/
/******************************************************************************/

channel

  :  /* empty */

  |  MC_4RM ':'
        HEIGHT DVALUE ';'
        CHANNEL LENGTH DVALUE ';'
        WALL    LENGTH DVALUE ';'
        first_wall_length
        last_wall_length
        WALL MATERIAL IDENTIFIER ';'
        COOLANT FLOW RATE DVALUE ';'
        coolant_heat_transfer_coefficients
        COOLANT VOLUMETRIC HEAT CAPACITY DVALUE ';'
        COOLANT INCOMING TEMPERATURE DVALUE ';'
    {
      stkd->Channel = alloc_and_init_channel() ;

      if (stkd->Channel == NULL)
      {
        FREE_POINTER (free, $18) ;

        stack_description_error
        (
          stkd, scanner, "malloc channel failed"
        ) ;

        YYABORT ;
      }

      stkd->Channel->ChannelModel    = TDICE_CHANNEL_MODEL_MC_4RM ;

      stkd->Channel->Height          = $4 ;

      tmp_channel_length             = $8 ;
      tmp_wall_length                = $12 ;
      tmp_first_wall_length          = ($14 > 0.0) ? $14 : $12 ;
      tmp_last_wall_length           = ($15 > 0.0) ? $15 : $12 ;

      stkd->Channel->CoolantFR       = CONVERT_COOLANT_FLOW_RATE($23) ;
      stkd->Channel->CoolantHTCs     = $25 ;
      stkd->Channel->CoolantVHC      = $30 ;
      stkd->Channel->CoolantTIn      = $35 ;
      stkd->Channel->WallMaterial
        = find_material_in_list (stkd->MaterialsList, $18) ;

      if (stkd->Channel->WallMaterial == NULL)
      {
        String_t message = malloc (sizeof(*message) * (18 + strlen($18))) ;

        if (message == NULL)
        {
          FREE_POINTER (free, $18) ;

          stack_description_error (stkd, scanner, "Malloc error") ;

          YYABORT ;
        }

        sprintf (message, "Unknown material %s", $18) ;

        FREE_POINTER (free, $18) ;

        stack_description_error (stkd, scanner, message) ;

        FREE_POINTER (free, message) ;

        YYABORT ;
      }

      stkd->Channel->WallMaterial->Used++ ;

      FREE_POINTER (free, $18) ;
    }
  |  MC_2RM ':'
        HEIGHT DVALUE ';'
        CHANNEL WIDTH DVALUE ';'
        CHANNEL PITCH  DVALUE ';'
        WALL MATERIAL IDENTIFIER ';'
        COOLANT FLOW RATE DVALUE ';'
        coolant_heat_transfer_coefficients
        COOLANT VOLUMETRIC HEAT CAPACITY DVALUE ';'
        COOLANT INCOMING TEMPERATURE DVALUE ';'
    {
      stkd->Channel = alloc_and_init_channel() ;

      if (stkd->Channel == NULL)
      {
        FREE_POINTER (free, $16) ;

        stack_description_error
        (
          stkd, scanner, "malloc channel failed"
        ) ;

        YYABORT ;
      }

      stkd->Channel->ChannelModel    = TDICE_CHANNEL_MODEL_MC_2RM ;

      stkd->Channel->Height          = $4 ;

      stkd->Channel->Width           = $8 ;

      stkd->Channel->Pitch           = $12 ;

      stkd->Channel->Porosity        = stkd->Channel->Width / stkd->Channel->Pitch ;

      stkd->Channel->CoolantFR       = CONVERT_COOLANT_FLOW_RATE($21) ;
      stkd->Channel->CoolantHTCs     = $23 ;
      stkd->Channel->CoolantVHC      = $28 ;
      stkd->Channel->CoolantTIn      = $33 ;
      stkd->Channel->WallMaterial
        = find_material_in_list (stkd->MaterialsList, $16) ;

      if (stkd->Channel->WallMaterial == NULL)
      {
        String_t message = malloc (sizeof(*message) * (18 + strlen($16))) ;

        if (message == NULL)
        {
          FREE_POINTER (free, $16) ;

          stack_description_error (stkd, scanner, "Malloc error") ;

          YYABORT ;
        }

        sprintf (message, "Unknown material %s", $16) ;

        FREE_POINTER (free, $16) ;

        stack_description_error (stkd, scanner, message) ;

        FREE_POINTER (free, message) ;

        YYABORT ;
      }

      stkd->Channel->WallMaterial->Used++ ;

      FREE_POINTER (free, $16) ;
    }
  |  PF_2RM ':'
        HEIGHT DVALUE ';'
        PIN DIAMETER DVALUE ';'
        PIN PITCH  DVALUE ';'
        distribution ';'
        PIN MATERIAL IDENTIFIER ';'
        DARCY VELOCITY DVALUE ';'
        COOLANT VOLUMETRIC HEAT CAPACITY DVALUE ';'
        COOLANT INCOMING TEMPERATURE DVALUE ';'
    {
      stkd->Channel = alloc_and_init_channel() ;

      if (stkd->Channel == NULL)
      {
        FREE_POINTER (free, $18) ;

        stack_description_error
        (
          stkd, scanner, "malloc channel failed"
        ) ;

        YYABORT ;
      }

      stkd->Channel->Height          = $4 ;

      stkd->Channel->Porosity = 1 - (PI * $8 * $8 / 4) / ($12 * $12) ;

      stkd->Channel->ChannelModel    = $14 ;

      stkd->Channel->DarcyVelocity   = $22 ;

      stkd->Channel->CoolantVHC      = $28 ;
      stkd->Channel->CoolantTIn      = $33 ;
      stkd->Channel->WallMaterial
        = find_material_in_list (stkd->MaterialsList, $18) ;

      if (stkd->Channel->WallMaterial == NULL)
      {
        String_t message = malloc (sizeof(*message) * (18 + strlen($18))) ;

        if (message == NULL)
        {
          FREE_POINTER (free, $18) ;

          stack_description_error (stkd, scanner, "Malloc error") ;

          YYABORT ;
        }

        sprintf (message, "Unknown material %s", $18) ;

        FREE_POINTER (free, $18) ;

        stack_description_error (stkd, scanner, message) ;

        FREE_POINTER (free, message) ;

        YYABORT ;
      }

      stkd->Channel->WallMaterial->Used++ ;

      FREE_POINTER (free, $18) ;
    }
  ;

coolant_heat_transfer_coefficients

  : COOLANT HEAT TRANSFER COEFFICIENT DVALUE ';'
    {
      $$.Side   = $5 ;
      $$.Top    = $5 ;
      $$.Bottom = $5 ;
    }
  | COOLANT HEAT TRANSFER COEFFICIENT SIDE   DVALUE ','
                                      TOP    DVALUE ','
                                      BOTTOM DVALUE ';'
    {
      $$.Side   = $6 ;
      $$.Top    = $9 ;
      $$.Bottom = $12 ;
    }

  | COOLANT HEAT TRANSFER COEFFICIENT TOP    DVALUE ','
                                      BOTTOM DVALUE ';'
    {
      $$.Top    = $6 ;
      $$.Bottom = $9 ;
    }
  ;

/******************************************************************************/
/******************************* Dies *****************************************/
/******************************************************************************/

dies_list

  : die             { stkd->DiesList = $1 ; $$ = $1 ; }
  | dies_list die   { $1->Next = $2       ; $$ = $2 ; }
  ;

die

  : DIE IDENTIFIER ':'         /* $2 Die identifier                          */
       layers_list             /* $4 list of layers (above source -> top)    */
       source_layer            /* $5 source layer                            */
       layers_list             /* $6 list of layers (bottom -> below source) */
    {
      Die* die = $$ = alloc_and_init_die() ;

      if (die == NULL)
      {
        FREE_POINTER (free,             $2) ;
        FREE_POINTER (free_layers_list, $4) ;
        FREE_POINTER (free_layer,       $5) ;
        FREE_POINTER (free_layers_list, $6);

        stack_description_error
        (
          stkd, scanner, "malloc die failed"
        ) ;

        YYABORT ;
      }

      if (find_die_in_list(stkd->DiesList, $2) != NULL)
      {
        String_t message
          = (String_t) malloc ((21 + strlen($2)) * sizeof (char)) ;
        sprintf (message, "Die %s already declared", $2) ;

        FREE_POINTER (free,             $2) ;
        FREE_POINTER (free_layers_list, $4) ;
        FREE_POINTER (free_layer,       $5) ;
        FREE_POINTER (free_layers_list, $6);
        FREE_POINTER (free_die,         die) ;

        stack_description_error (stkd, scanner, message) ;

        FREE_POINTER (free, message) ;

        YYABORT ;
      }

      die->Id = $2 ;
      die->SourceLayer = $5 ;

      /* The layers within a die are declared from top to bottom       */
      /* but here we revert it: the first layer in the list LayersList */
      /* will be the bottom-most layer in the die                      */

      if ($6 != NULL)
      {
        /* if there are layers below the source, */
        /* the list of layers begins with $6     */

        die->BottomLayer = $6 ;

        /* $6 moved until the end .. */

        while ($6->Next != NULL) $6 = $6->Next ;

        /* the list $6 continues with the source layer $5 */

        JOIN_ELEMENTS ($6, $5) ;
      }
      else
      {
        /* if there aren't layers below the source, the list of layers */
        /* begins directly with the source layer $5                    */

        die->BottomLayer = $5 ;
      }

      if ($4 != NULL)
      {
        /* if there are layers above the source  */
        /* $5 is connected to the list $4        */

        JOIN_ELEMENTS ($5, $4) ;

        /* $4 moved until the end .. */

        while ($4->Next != NULL) $4 = $4->Next ;

        /* the list finishes with the last layer in $4 */

        die->TopLayer = $4 ;
      }
      else
      {
        /* if there aren't layers below the source,   */
        /* The list finishes with the source layer $5 */

        die->TopLayer = $5 ;
      }


      GridDimension_t layer_offset = GRIDDIMENSION_I ;

      FOR_EVERY_ELEMENT_IN_LIST_FORWARD (Layer, layer, die->BottomLayer)

        layer->Offset = layer_offset++ ;

      die->NLayers = layer_offset ;
    }
  ;

/******************************************************************************/
/******************************* Layers ***************************************/
/******************************************************************************/

layers_list

  : /* empty */   { $$ = NULL ; }
  | layers        { $$ = $1   ; }
  ;

layers

  : layer         {                         $$ = $1 ; }
  | layers layer  { JOIN_ELEMENTS($2, $1) ; $$ = $2 ; }
  ;

layer         : LAYER  layer_content { $$ = $2 ; } ;

source_layer  : SOURCE layer_content { $$ = $2 ; } ;

layer_content : DVALUE IDENTIFIER ';'

    {
      Layer* layer = $$ = alloc_and_init_layer() ;

      if (layer == NULL)
      {
        FREE_POINTER (free, $2) ;

        stack_description_error
        (
          stkd, scanner, "malloc layer failed"
        ) ;

        YYABORT ;
      }

      layer->Height   = (CellDimension_t) $1 ;
      layer->Material = find_material_in_list(stkd->MaterialsList, $2) ;

      if (layer->Material == NULL)
      {
        String_t message = malloc (sizeof(*message) * (18 + strlen($2))) ;

        if (message == NULL)
        {
          FREE_POINTER (free,       $2) ;
          FREE_POINTER (free_layer, layer) ;

          stack_description_error (stkd, scanner, "Malloc error") ;

          YYABORT ;
        }

        sprintf (message, "Unknown material %s", $2) ;

        FREE_POINTER (free,       $2) ;
        FREE_POINTER (free_layer, layer) ;

        stack_description_error (stkd, scanner, message) ;

        FREE_POINTER (free, message) ;

        YYABORT ;
      }

      layer->Material->Used++ ;

      FREE_POINTER (free, $2) ;
    }
  ;

/******************************************************************************/
/******************************* Stack ****************************************/
/******************************************************************************/

stack

  : STACK ':'
      stack_elements
    {
      //
      // After parsing with success the list of stack elements ...
      //

      stkd->StackElementsList = $3 ;

      if (   last_stack_element != NULL
          && last_stack_element->Type == TDICE_STACK_ELEMENT_CHANNEL)
      {
        // Reset for the next parsing ...
        last_stack_element = NULL ;
        found_die          = FALSE_V ;
        found_channel      = FALSE_V ;

        stack_description_error
        (
          stkd, scanner, "channel as bottom stack element not supported"
        ) ;
        YYABORT ;
      }

      if (found_die == FALSE_V)
      {
        // Reset for the next parsing ...
        last_stack_element = NULL ;
        found_die          = FALSE_V ;
        found_channel      = FALSE_V ;

        stack_description_error
        (
          stkd, scanner, "there must be at least one die in the stack section"
        ) ;
        YYABORT ;
      }

      if (found_channel == FALSE_V && stkd->Channel != NULL)

        fprintf (stderr, "Warning: channel section declared but not used\n") ;


      GridDimension_t layer_index = GRIDDIMENSION_I ;

      FOR_EVERY_ELEMENT_IN_LIST_FORWARD (StackElement, stk_el, stkd->StackElementsList)
      {
        stk_el->Offset = layer_index ;
        layer_index   += stk_el->NLayers ;
      }

      // Reset for the next parsing ...
      last_stack_element = NULL ;
      found_die          = FALSE_V ;
      found_channel      = FALSE_V ;
    }
  ;

stack_elements

  : stack_element
    {
      //
      // After parsing with success the first stack element ...
      //

      if (   last_stack_element != NULL
          && last_stack_element->Type == TDICE_STACK_ELEMENT_CHANNEL)
      {
        FREE_POINTER (free_stack_element, $1) ;

        stack_description_error
        (
          stkd, scanner, "channel as top stack element not supported"
        ) ;

        YYABORT ;
      }

      if (stkd->ConventionalHeatSink != NULL)
      {

        if (   last_stack_element != NULL
            && last_stack_element->Type == TDICE_STACK_ELEMENT_LAYER)

          stkd->ConventionalHeatSink->TopLayer
            = last_stack_element->Pointer.Layer ;

        else
        {
          stkd->ConventionalHeatSink->TopLayer
            = last_stack_element->Pointer.Die->TopLayer ;

          if (   last_stack_element->Pointer.Die->TopLayer
              == last_stack_element->Pointer.Die->SourceLayer)

            stkd->ConventionalHeatSink->IsSourceLayer = TRUE_V ;
        }
      }


      $$ = $1 ;
    }
  | stack_elements stack_element
    {
      //
      // After parsing with success all the other stack elements ...
      //

      if (find_stack_element_in_list($1, $2->Id) != NULL)
      {
        String_t message = malloc (sizeof(*message) * (31 + strlen($2->Id))) ;

        if (message == NULL)
        {
          FREE_POINTER (free_stack_element,       $2) ;
          FREE_POINTER (free_stack_elements_list, $1) ;

          stack_description_error (stkd, scanner, "Malloc error") ;

          YYABORT ;
        }

        sprintf (message, "Stack element %s already declared", $2->Id) ;

        FREE_POINTER (free_stack_element,       $2) ;
        FREE_POINTER (free_stack_elements_list, $1) ;

        stack_description_error (stkd, scanner, message) ;

        FREE_POINTER (free, message) ;

        YYABORT ;
      }

      $2->Next = $1 ;
      $$ = $2 ;
    }
  ;

stack_element

  : LAYER IDENTIFIER DVALUE IDENTIFIER ';'
    {
      //
      // After parsing with success a layer ...
      //

      StackElement* stack_element = $$ = alloc_and_init_stack_element() ;

      if (stack_element == NULL)
      {
        FREE_POINTER (free, $2) ;
        FREE_POINTER (free, $4) ;

        stack_description_error
        (
          stkd, scanner, "malloc stack element failed"
        ) ;

        YYABORT ;
      }

      Layer* layer = alloc_and_init_layer() ;

      if (layer == NULL)
      {
        FREE_POINTER (free,               $2) ;
        FREE_POINTER (free,               $4) ;
        FREE_POINTER (free_stack_element, stack_element) ;

        stack_description_error
        (
          stkd, scanner, "malloc layer failed"
        ) ;

        YYABORT ;
      }

      layer->Height   = (CellDimension_t) $3 ;
      layer->Material = find_material_in_list(stkd->MaterialsList, $4) ;

      if (layer->Material == NULL)
      {
        String_t message = malloc (sizeof(*message) * (18 + strlen($4))) ;

        if (message == NULL)
        {
          FREE_POINTER (free,               $2) ;
          FREE_POINTER (free,               $4) ;
          FREE_POINTER (free_stack_element, stack_element) ;
          FREE_POINTER (free_layer,         layer) ;

          stack_description_error (stkd, scanner, "Malloc error") ;

          YYABORT ;
        }

        sprintf (message, "Unknown material %s", $4) ;

        FREE_POINTER (free,               $2) ;
        FREE_POINTER (free,               $4) ;
        FREE_POINTER (free_stack_element, stack_element) ;
        FREE_POINTER (free_layer,         layer) ;

        stack_description_error (stkd, scanner, message) ;

        FREE_POINTER (free, message) ;

        YYABORT ;
      }

      layer->Material->Used++ ;

      FREE_POINTER (free, $4) ;

      stack_element->Type          = TDICE_STACK_ELEMENT_LAYER ;
      stack_element->Pointer.Layer = layer ;
      stack_element->Id            = $2 ;
      stack_element->NLayers       = 1 ;

      last_stack_element = stack_element ;
    }
  | CHANNEL IDENTIFIER ';'
    {
      //
      // After parsing with success a layer ...
      //

      if (stkd->Channel == NULL)
      {
        FREE_POINTER (free, $2) ;

        stack_description_error
        (
          stkd, scanner, "channel used in stack but not declared"
        ) ;

        YYABORT ;
      }

      if (   last_stack_element != NULL
          && last_stack_element->Type == TDICE_STACK_ELEMENT_CHANNEL)
      {
        FREE_POINTER (free, $2) ;

        stack_description_error
        (
          stkd, scanner, "two consecutive channel layers not supported"
        ) ;

        YYABORT ;
      }

      StackElement* stack_element = $$ = alloc_and_init_stack_element() ;

      if (stack_element == NULL)
      {
        FREE_POINTER (free, $2) ;

        stack_description_error
        (
          stkd, scanner, "malloc stack element failed"
        ) ;

        YYABORT ;
      }

      stack_element->Type            = TDICE_STACK_ELEMENT_CHANNEL ;
      stack_element->Pointer.Channel = stkd->Channel ;
      stack_element->Id              = $2 ;

      switch (stkd->Channel->ChannelModel) {
      case TDICE_CHANNEL_MODEL_MC_2RM:
      case TDICE_CHANNEL_MODEL_PF_INLINE:
      case TDICE_CHANNEL_MODEL_PF_STAGGERED:
        stack_element->NLayers       = NUM_LAYERS_2RM ;
        break;
      default: // TDICE_CHANNEL_MODEL_MC_4RM
        stack_element->NLayers       = NUM_LAYERS_4RM ;
      }

      found_channel = TRUE_V ;
      last_stack_element = stack_element ;
    }
  | DIE IDENTIFIER IDENTIFIER FLOORPLAN PATH ';'
    {
      //
      // After parsing with success a die ...
      //

      StackElement* stack_element = $$ = alloc_and_init_stack_element() ;

      if (stack_element == NULL)
      {
        FREE_POINTER (free, $2) ;
        FREE_POINTER (free, $3) ;
        FREE_POINTER (free, $5) ;

        stack_description_error
        (
          stkd, scanner, "malloc stack element failed"
        ) ;

        YYABORT ;
      }

      stack_element->Type        = TDICE_STACK_ELEMENT_DIE ;
      stack_element->Id          = $2 ;
      stack_element->Pointer.Die = find_die_in_list(stkd->DiesList, $3) ;

      if (stack_element->Pointer.Die == NULL)
      {
        String_t message = malloc (sizeof(*message) * (13 + strlen($3))) ;

        if (message == NULL)
        {
          FREE_POINTER (free,               $3) ;
          FREE_POINTER (free,               $5) ;
          FREE_POINTER (free_stack_element, stack_element) ;

          stack_description_error (stkd, scanner, "Malloc error") ;

          YYABORT ;
        }

        sprintf (message, "Unknown die %s", $3) ;

        FREE_POINTER (free,               $3) ;
        FREE_POINTER (free,               $5) ;
        FREE_POINTER (free_stack_element, stack_element) ;

        stack_description_error (stkd, scanner, message) ;

        FREE_POINTER (free, message) ;

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

        stack_description_error
        (
          stkd, scanner, "malloc floorplan failed"
        ) ;

        YYABORT ;
      }

      stack_element->Floorplan->FileName = $5 ;

      FREE_POINTER (free, $3) ;

      found_die = TRUE_V ;
      last_stack_element = stack_element ;
    }
  ;

/******************************************************************************/
/******************************* Dimensions ***********************************/
/******************************************************************************/

dimensions

  : DIMENSIONS ':'
      CHIP LENGTH DVALUE ',' WIDTH DVALUE ';'
      CELL LENGTH DVALUE ',' WIDTH DVALUE ';'
    {
      stkd->Dimensions = alloc_and_init_dimensions() ;

      if (stkd->Dimensions == NULL)
      {
        stack_description_error
        (
          stkd, scanner, "malloc dimensions failed"
        ) ;
        YYABORT ;
      }

      /* Set widths and NRows */

      stkd->Dimensions->Chip.Width  = (ChipDimension_t) $8 ;
      stkd->Dimensions->Cell.Width  = (CellDimension_t) $15 ;

      stkd->Dimensions->Grid.NRows
        = (GridDimension_t)
          (stkd->Dimensions->Chip.Width / stkd->Dimensions->Cell.Width) ;

      /* Set lengths and NColumns */

      stkd->Dimensions->Chip.Length = (ChipDimension_t) $5 ;

      if (stkd->Channel == NULL) {
        /* There are no channels in the stack */

        stkd->Dimensions->StackHasChannel = FALSE_V ;

        stkd->Dimensions->Cell.WallLength = (CellDimension_t) $12 ;

        stkd->Dimensions->Grid.NColumns
          = (GridDimension_t)
            stkd->Dimensions->Chip.Length / stkd->Dimensions->Cell.WallLength ;

      } else {
        /* There are channels in the stack */

        stkd->Dimensions->StackHasChannel  = TRUE_V ;
        stkd->Dimensions->ChannelModel = stkd->Channel->ChannelModel ;

        if (stkd->Dimensions->ChannelModel == TDICE_CHANNEL_MODEL_MC_2RM ||
            stkd->Dimensions->ChannelModel == TDICE_CHANNEL_MODEL_PF_INLINE ||
            stkd->Dimensions->ChannelModel == TDICE_CHANNEL_MODEL_PF_STAGGERED ) {

            stkd->Dimensions->Cell.WallLength = (CellDimension_t) $12 ;

            stkd->Dimensions->Grid.NColumns
              = (GridDimension_t)
                stkd->Dimensions->Chip.Length / stkd->Dimensions->Cell.WallLength ;

        } else { // TDICE_CHANNEL_MODEL_MC_4RM

          stkd->Dimensions->Cell.ChannelLength   = tmp_channel_length ;
          stkd->Dimensions->Cell.FirstWallLength = tmp_first_wall_length ;
          stkd->Dimensions->Cell.LastWallLength  = tmp_last_wall_length ;
          stkd->Dimensions->Cell.WallLength      = tmp_wall_length ;

          CellDimension_t ratio
          = (stkd->Dimensions->Chip.Length
              - stkd->Dimensions->Cell.FirstWallLength
              - stkd->Dimensions->Cell.LastWallLength
              - stkd->Dimensions->Cell.ChannelLength
            )
            /
            ( stkd->Dimensions->Cell.ChannelLength
              + stkd->Dimensions->Cell.WallLength
            ) ;
          if ( ratio - (int) ratio != 0)
          {
            stack_description_error
            (
              stkd, scanner, "cell dimensions does not fit the chip length correctly"
            ) ;
            YYABORT ;
          }
          stkd->Dimensions->Grid.NColumns = 2 * ratio + 3 ;

          if ((stkd->Dimensions->Grid.NColumns & 1) == 0)
          {
            stack_description_error
            (
              stkd, scanner, "colum number must be odd when channel is declared"
            ) ;
            YYABORT ;
          }
        }

        /* Check the number of columns */

        if (stkd->Dimensions->Grid.NColumns < 3)
        {
          stack_description_error (stkd, scanner, "not enough columns") ;
          YYABORT ;
        }
      }
    }
  ;

first_wall_length
  : /* empty */                  { $$ = 0.0 ; }
  | FIRST WALL LENGTH DVALUE ';' { $$ =  $4 ; }
  ;

last_wall_length
  : /* empty */                 { $$ = 0.0 ; }
  | LAST WALL LENGTH DVALUE ';' { $$ =  $4 ; }
  ;

distribution
  : INLINE                      { $$ = TDICE_CHANNEL_MODEL_PF_INLINE ; }
  | STAGGERED                   { $$ = TDICE_CHANNEL_MODEL_PF_STAGGERED ; }
  ;

%%

void stack_description_error
(
  StackDescription* stkd,
  yyscan_t          scanner,
  String_t          message
)
{
  fprintf
  (
    stack_description_get_out (scanner),
    "%s:%d: %s\n",
    stkd->FileName,
    stack_description_get_lineno (scanner),
    message
  ) ;
  FREE_POINTER (free_stack_description, stkd) ;
}
