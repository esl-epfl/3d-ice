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
#include "math.h"

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
   double       double_v ;
   String_t     char_p ;

   Material*      material_p ;
   Die*           die_p ;
   Layer*         layer_p ;
   StackElement*  stack_element_p ;
   CoolantHTCs_t  coolanthtcs_v ;
}

%type <double_v> first_wall_length
%type <double_v> last_wall_length

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
%token CHIP                  "keyword chip"
%token CELL                  "keyword cell"
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

%token <double_v> DVALUE     "float value"
%token <char_p>   IDENTIFIER "identifier"
%token <char_p>   PATH       "path to file"

%destructor { free($$) ;                      } <char_p>
%destructor { free_layers_list ($$) ;         } <layers>
%destructor { free_dies_list ($$) ;           } <dies>
%destructor { free_stack_elements_list ($$) ; } <stack_elements>

%{
#include "../flex/stack_description_scanner.h"

void stack_description_error
(
  StackDescription* stack,
  yyscan_t          scanner,
  String_t          message
) ;

static enum StackElement_t last_stack_element = TDICE_STACK_ELEMENT_NONE ;
static int found_die     = 0 ;
static int found_channel = 0 ;

static int tmp_first_wall_length = 0 ;
static int tmp_last_wall_length = 0 ;
static int tmp_wall_length = 0 ;
static int tmp_channel_length = 0 ;
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
    environment_heat_sink
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

      Material* tmp ;
      for (tmp = stkd->MaterialsList ; tmp != NULL ; tmp = tmp->Next)
        if (tmp->Used == 0)
          fprintf(stderr,
                  "Warning: material %s declared but not used\n", tmp->Id) ;

      //
      // Counts the number of layers
      //

      StackElement *stk_el = stkd->StackElementsList ;
      for ( ; stk_el != NULL ; stk_el = stk_el->Next)
        stkd->Dimensions->Grid.NLayers += stk_el->NLayers ;

      //
      // Evaluate the number of cells and nonzeroe elements
      //

      stkd->Dimensions->Grid.NCells
        = get_number_of_layers(stkd->Dimensions)
          * get_number_of_rows(stkd->Dimensions)
          * get_number_of_columns(stkd->Dimensions) ;

      stkd->Dimensions->Grid.NNz
        =   get_number_of_layers(stkd->Dimensions)
            * (
                  get_number_of_rows(stkd->Dimensions)
                  * (3 * get_number_of_columns(stkd->Dimensions) - 2)
                + 2 * get_number_of_columns(stkd->Dimensions)
                  * (get_number_of_rows(stkd->Dimensions) - 1)
              )
          + (get_number_of_layers(stkd->Dimensions) - 1 ) * 2
            * get_number_of_rows(stkd->Dimensions)
            * get_number_of_columns(stkd->Dimensions) ;

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
        free ($2) ;
        stack_description_error
        (
          stkd, scanner, "malloc material failed"
        ) ;
        YYABORT ;
      }

      if (find_material_in_list(stkd->MaterialsList, $2) != NULL)
      {
        String_t message
          = (String_t) malloc ((26 + strlen($2)) * sizeof (char)) ;
        sprintf (message, "Material %s already declared", $2) ;

        free ($2) ;
        free_material (material) ;
        stack_description_error (stkd, scanner, message) ;
        free (message) ;
        YYABORT ;
      }

      material->Id                  = $2 ;
      material->ThermalConductivity = $6 ;
      material->VolHeatCapacity     = $11 ;

    }
  ;

/******************************************************************************/
/******************************* Heatsink *************************************/
/******************************************************************************/

environment_heat_sink

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

  |  CHANNEL ':'
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
        free ($18) ;
        stack_description_error
        (
          stkd, scanner, "malloc channel failed"
        ) ;
        YYABORT ;
      }

      stkd->Channel->Height          = $4  ;

      tmp_channel_length             = $8  ;
      tmp_wall_length                = $12 ;
      tmp_first_wall_length          = ($14 == 0) ? $12 : $14 ;
      tmp_last_wall_length           = ($15 == 0) ? $12 : $15 ;

      stkd->Channel->CoolantFR       = ( $23 * 1e+12 ) / 60.0 ;
      stkd->Channel->CoolantHTCs     = $25 ;
      stkd->Channel->CoolantVHC      = $30 ;
      stkd->Channel->CoolantTIn      = $35 ;
      stkd->Channel->Wall
        = find_material_in_list (stkd->MaterialsList, $18) ;

      if (stkd->Channel->Wall == NULL)
      {
        String_t message
          = (String_t) malloc ((18 + strlen($18)) * sizeof (char)) ;
        sprintf (message, "Unknown material %s", $18) ;

        stack_description_error (stkd, scanner, message) ;
        free ($18) ;
        free (message) ;
        YYABORT ;
      }

      stkd->Channel->Wall->Used++ ;

      free ($18) ;
    }
  ;

coolant_heat_transfer_coefficients

  : COOLANT HEAT TRANSFER COEFFICIENT DVALUE ';'
    {
      $$.Side   = $5;
      $$.Top    = $5;
      $$.Bottom = $5;
    }
  | COOLANT HEAT TRANSFER COEFFICIENT SIDE   DVALUE ','
                                      TOP    DVALUE ','
                                      BOTTOM DVALUE ';'
    {
      $$.Side   = $6 ;
      $$.Top    = $9 ;
      $$.Bottom = $12;
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

  : DIE IDENTIFIER ':'
       layers_list
       source_layer
       layers_list
    {
      Layer* layer ;
      Die* die = $$ = alloc_and_init_die() ;

      if (die == NULL)
      {
        free ($2) ;
        free_layers_list ($4) ;
        free_layer ($5) ;
        free_layers_list ($6) ;

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

        free ($2) ;
        free_layers_list ($4) ;
        free_layer ($5) ;
        free_layers_list ($6) ;
        free_die (die) ;
        stack_description_error (stkd, scanner, message) ;
        free (message) ;
        YYABORT ;
      }

      die->Id = $2 ;

      if ($6 != NULL)
      {
        die->LayersList = $6 ;

        layer = $6 ;
        while (layer->Next != NULL)
          layer = layer->Next ;
        layer->Next = $5 ;
      }
      else
        die->LayersList = $5 ;

      die->SourceLayer = $5 ;
      $5->Next = $4 ;

      layer = die->LayersList ;
      while (layer != NULL)
      {
        die->NLayers++ ;
        layer = layer->Next ;
      }
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

  : layer         {                 $$ = $1 ; }
  | layers layer  { $2->Next = $1 ; $$ = $2 ; }
  ;

layer         : LAYER  layer_content { $$ = $2 ; } ;

source_layer  : SOURCE layer_content { $$ = $2 ; } ;

layer_content : DVALUE IDENTIFIER ';'

    {
      Layer* layer = $$ = alloc_and_init_layer() ;

      if (layer == NULL)
      {
        free ($2) ;
        stack_description_error
        (
          stkd, scanner, "malloc layer failed"
        ) ;
        YYABORT ;
      }

      layer->Height   = $1 ;
      layer->Material = find_material_in_list(stkd->MaterialsList, $2) ;

      if (layer->Material == NULL)
      {
        String_t message
          = (String_t) malloc ((18 + strlen($2)) * sizeof (char)) ;
        sprintf (message, "Unknown material %s", $2) ;

        free ($2) ;
        free (message) ;
        free_layer(layer) ;
        YYABORT ;
      }

      layer->Material->Used++ ;

      free ($2) ;
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

      if (last_stack_element == TDICE_STACK_ELEMENT_CHANNEL)
      {
        // Reset for the next parsing ...
        last_stack_element = TDICE_STACK_ELEMENT_NONE ;
        found_die          = 0 ;
        found_channel      = 0 ;

        stack_description_error
        (
          stkd, scanner, "channel as bottom stack element not supported"
        ) ;
        YYABORT ;
      }

      if (found_die == 0)
      {
        // Reset for the next parsing ...
        last_stack_element = TDICE_STACK_ELEMENT_NONE ;
        found_die          = 0 ;
        found_channel      = 0 ;

        stack_description_error
        (
          stkd, scanner, "there must be at least one die in the stack section"
        ) ;
        YYABORT ;
      }

      if (found_channel == 0 && stkd->Channel != NULL)

        fprintf (stderr, "Warning: channel section declared but not used\n") ;

      // Reset for the next parsing ...
      last_stack_element = TDICE_STACK_ELEMENT_NONE ;
      found_die          = 0 ;
      found_channel      = 0 ;
    }
  ;

stack_elements

  : stack_element
    {
      //
      // After parsing with success the first stack element ...
      //

      if (last_stack_element == TDICE_STACK_ELEMENT_CHANNEL)
      {
        free_stack_element ($1) ;
        stack_description_error
        (
          stkd, scanner, "channel as top stack element not supported"
        ) ;
        YYABORT ;
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
        String_t message
          = (String_t) malloc ((31 + strlen($2->Id)) * sizeof (char)) ;
        sprintf (message, "Stack element %s already declared", $2->Id) ;

        free_stack_element ($2) ;
        free_stack_elements_list ($1) ;
        stack_description_error (stkd, scanner, message) ;
        free (message) ;
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
        free ($2) ;
        free ($4) ;
        stack_description_error
        (
          stkd, scanner, "malloc stack element failed"
        ) ;
        YYABORT ;
      }

      Layer* layer = alloc_and_init_layer() ;

      if (layer == NULL)
      {
        free ($2);
        free ($4) ;
        free_stack_element (stack_element) ;
        stack_description_error
        (
          stkd, scanner, "malloc layer failed"
        ) ;
        YYABORT ;
      }

      layer->Height   = $3 ;
      layer->Material = find_material_in_list(stkd->MaterialsList, $4) ;

      if (layer->Material == NULL)
      {
        String_t message
          = (String_t) malloc ((18 + strlen($4)) * sizeof (char)) ;
        sprintf (message, "Unknown material %s", $4) ;

        stack_description_error (stkd, scanner, message) ;

        free ($2);
        free ($4) ;
        free (message) ;
        free_stack_element (stack_element) ;
        free_layer(layer) ;
        YYABORT ;
      }

      layer->Material->Used++ ;

      free($4) ;

      stack_element->Type          = TDICE_STACK_ELEMENT_LAYER ;
      stack_element->Pointer.Layer = layer ;
      stack_element->Id            = $2 ;
      stack_element->NLayers       = 1 ;

      last_stack_element = TDICE_STACK_ELEMENT_LAYER ;
    }
  | CHANNEL IDENTIFIER ';'
    {
      //
      // After parsing with success a layer ...
      //

      if (stkd->Channel == NULL)
      {
        free ($2) ;
        stack_description_error
        (
          stkd, scanner, "channel used in stack but not declared"
        ) ;
        YYABORT ;
      }

      if (last_stack_element == TDICE_STACK_ELEMENT_CHANNEL)
      {
        free ($2) ;
        stack_description_error
        (
          stkd, scanner, "two consecutive channel layers not supported"
        ) ;
        YYABORT ;
      }

      StackElement* stack_element = $$ = alloc_and_init_stack_element() ;

      if (stack_element == NULL)
      {
        free ($2) ;
        stack_description_error
        (
          stkd, scanner, "malloc stack element failed"
        ) ;
        YYABORT ;
      }

      stack_element->Type    = TDICE_STACK_ELEMENT_CHANNEL ;
      stack_element->Id      = $2 ;
      stack_element->NLayers = 1 ;

      found_channel = 1 ;
      last_stack_element = TDICE_STACK_ELEMENT_CHANNEL ;
    }
  | DIE IDENTIFIER IDENTIFIER FLOORPLAN PATH ';'
    {
      //
      // After parsing with success a die ...
      //

      StackElement* stack_element = $$ = alloc_and_init_stack_element() ;

      if (stack_element == NULL)
      {
        free ($2) ;
        free ($3) ;
        free ($5) ;
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
        String_t message
          = (String_t) malloc ((13 + strlen($3)) * sizeof (char)) ;
        sprintf (message, "Unknown die %s", $3) ;

        stack_description_error (stkd, scanner, message) ;

        free($3) ;
        free($5) ;
        free(message) ;
        free_stack_element (stack_element) ;
        YYABORT ;
      }

      stack_element->NLayers = stack_element->Pointer.Die->NLayers ;
      stack_element->Floorplan = alloc_and_init_floorplan ();

      if (stack_element->Floorplan == NULL)
      {
        free($3) ;
        free($5) ;
        free_stack_element (stack_element) ;
        stack_description_error
        (
          stkd, scanner, "malloc floorplan failed"
        ) ;
        YYABORT ;
      }

      stack_element->Floorplan->FileName = $5 ;

      free($3) ;

      found_die = 1 ;
      last_stack_element = TDICE_STACK_ELEMENT_DIE ;
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
      stkd->Dimensions->Cell.Length = (CellDimension_t) $12 ;

      if (stkd->Channel == NULL)
      {
        /* There are no channels in the stack */

        stkd->Dimensions->StackHasChannel = FALSE_V ;

        stkd->Dimensions->Grid.NColumns
          = (GridDimension_t)
            stkd->Dimensions->Chip.Length / stkd->Dimensions->Cell.Length ;
      }
      else
      {
        /* There are channels in the stack */

        stkd->Dimensions->StackHasChannel = TRUE_V ;

        stkd->Dimensions->Cell.FirstWallLength = tmp_first_wall_length ;
        stkd->Dimensions->Cell.LastWallLength  = tmp_last_wall_length ;
        stkd->Dimensions->Cell.WallLength      = tmp_wall_length ;
        stkd->Dimensions->Cell.ChannelLength   = tmp_channel_length ;

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

        /* Check the number of ciolumns get */

        if (stkd->Dimensions->Grid.NColumns < 3)
        {
          stack_description_error (stkd, scanner, "not enough columns") ;
          YYABORT ;
        }
      }
    }
  ;

first_wall_length
  : /* empty */                  { $$ = 0 ;  }
  | FIRST WALL LENGTH DVALUE ';' { $$ = $4 ; }
  ;

last_wall_length
  : /* empty */                 { $$ = 0 ;  }
  | LAST WALL LENGTH DVALUE ';' { $$ = $4 ; }
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
  free_stack_description (stkd) ;
}
