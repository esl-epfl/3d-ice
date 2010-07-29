/******************************************************************************
 *                                                                            *
 * Source file "Bison/stack_description_parser.y"                             *
 *                                                                            *
 * Compile with "bison -d stack_parser.y" to obtain the files                 *
 * "stack_parser.h" and "stack_parser.c".                                     *
 *                                                                            *
 * EPFL-STI-IEL-ESL                                                           *
 * Bâtiment ELG, ELG 130                                                      *
 * Station 11                                                                 *
 * 1015 Lausanne, Switzerland                    alessandro.vincenzi@epfl.ch  *
 ******************************************************************************/

%{

#include "material.h"
#include "die.h"
#include "layer.h"
#include "stack_element.h"
#include "stack_description.h"

%}
		
/* The YYSTYPE union used to collect the types of tokens and rules */

%union
{
   int     int_v ;
   double  double_v ;
   char    *string ;

   struct Material      *material ;
   struct Die           *die_p ;
   struct Layer         *layer_p ;
   struct StackElement  *stack_element_p ;
}

%destructor { free($$) ;                      } <string>
%destructor { free_layers_list ($$) ;         } <layers>
%destructor { free_dies_list ($$) ;           } <dies>
%destructor { free_stack_elements_list ($$) ; } <stack_elements>

%type <double_v> first_wall_length
%type <double_v> last_wall_length

%type <material> material
%type <material> materials_list

%type <die_p> die
%type <die_p> dies_list

%type <layer_p> layer
%type <layer_p> layer_content ;
%type <layer_p> layers
%type <layer_p> layers_list
%type <layer_p> source_layer

%type <stack_element_p> stack_element
%type <stack_element_p> stack_elements

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
%token UM                    "keyword um"
%token WALL                  "keyword wall"
%token COOLANT               "keyword coolant"
%token TRANSFER              "keyword transfer"
%token COEFFICIENT           "keyword coefficient"
%token WIDTH                 "keyword width"
%token LENGTH                "keyword length"
%token FLOORPLAN             "keyword floorplan"
%token STACK                 "keyword stack"
%token MM                    "keyword mm"
%token VOLUMETRIC            "keywork volumetric"
%token CAPACITY              "keyword capacity"
%token ENVIRONMENT           "keyword environment"
%token SINK                  "keywork sink"

%token <double_v> DVALUE     "float value"
%token <string>   IDENTIFIER "identifier"
%token <string>   PATH       "path to file"

%{
#include "stack_description_scanner.h"

void
stack_description_error
(
  struct StackDescription *stack  ,
  yyscan_t         scanner ,
  char             *message
) ;

static struct StackElement* tmp_stack_element = NULL;
static int found_die = 0 ;
static int tmp_first_wall_length = 0 ;
static int tmp_last_wall_length = 0 ;
static int tmp_wall_length = 0 ;
static int tmp_channel_length = 0 ;

%}

%require     "2.4.1"
%name-prefix "stack_description_"
%output      "stack_description_parser.c"

%pure-parser
%error-verbose
%parse-param { struct StackDescription *stkd }
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
    dies_list
    stack
    dimensions
    {
      if (stkd->Channel == NULL
          && stkd->EnvironmentHeatSink == NULL)
        fprintf(stderr,
                "Warning: both ambient heat sink and channels are absent.\n") ;
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

      material->Id                  = $2 ;
      material->ThermalConductivity = $6 ;
      material->VolHeatCapacity     = $11 ;

      if (find_material_in_list(stkd->MaterialsList, $2) != NULL)
      {
        char *message = (char *) malloc ((26 + strlen($2)) * sizeof (char)) ;
        sprintf (message, "Material %s already declared", $2) ;
        stack_description_error (stkd, scanner, message) ;

        free_material (material) ;
        free (message) ;
        YYABORT ;
      }
    }
  ;

/******************************************************************************/
/******************************* Heatsink *************************************/
/******************************************************************************/

environment_heat_sink

  : /* empty */
  | ENVIRONMENT HEAT SINK ':'
        HEAT TRANSFER COEFFICIENT DVALUE ';'
        ENVIRONMENT TEMPERATURE   DVALUE ';'
    {
      stkd->EnvironmentHeatSink = alloc_and_init_environment_heat_sink() ;

      if (stkd->EnvironmentHeatSink == NULL)
      {
        stack_description_error
        (
          stkd, scanner, "malloc environment heat sink failed"
        ) ;
        YYABORT ;
      }

      stkd->EnvironmentHeatSink->HeatTransferC = $8 ;
      stkd->EnvironmentHeatSink->EnvironmentT  = $12 ;
    }
  ;
/******************************************************************************/
/******************************* Channel **************************************/
/******************************************************************************/

channel

  :  /* empty */

  |  CHANNEL ':'
                HEIGHT DVALUE UM ';'
        CHANNEL LENGTH DVALUE UM ';'
        WALL    LENGTH DVALUE UM ';'
        first_wall_length
        last_wall_length
        WALL MATERIAL IDENTIFIER ';'
        COOLANT FLOW RATE DVALUE ';'
        COOLANT HEAT TRANSFER COEFFICIENT DVALUE ';'
        COOLANT VOLUMETRIC HEAT CAPACITY DVALUE ';'
        COOLANT INCOMING TEMPERATURE DVALUE ';'
    {
      stkd->Channel = alloc_and_init_channel() ;

      if (stkd->Channel == NULL)
      {
        stack_description_error
        (
          stkd, scanner, "malloc channel failed"
        ) ;
        free ($21) ;
        YYABORT ;
      }

      stkd->Channel->Height          = $4  ;
      tmp_channel_length             = $9  ;
      tmp_wall_length                = $14 ;
      tmp_first_wall_length          = ($17 == 0) ? $14 : $17 ;
      tmp_last_wall_length           = ($18 == 0) ? $14 : $18 ;
      stkd->Channel->CoolantFR       = ( $26 * 1e+12 ) / 60.0 ;
      stkd->Channel->CoolantHTC      = $32 ;
      stkd->Channel->CoolantVHC      = $38 ;
      stkd->Channel->CoolantTIn      = $43 ;
      stkd->Channel->FlowRateChanged = 1 ;
      stkd->Channel->Wall
        = find_material_in_list (stkd->MaterialsList, $21) ;

      if (stkd->Channel->Wall == NULL)
      {
        stack_description_error
        (
          stkd, scanner, "unknown material identifier"
        ) ;
        free ($21) ;
        YYABORT ;
      }

      free ($21) ;
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
      struct Die   *die = $$ = alloc_and_init_die() ;

      if (die == NULL)
      {
        free ($2) ;
        stack_description_error
        (
          stkd, scanner, "malloc die failed"
        ) ;
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

layer_content : DVALUE UM IDENTIFIER ';'

    {
      Layer* layer = $$ = alloc_and_init_layer() ;

      if (layer == NULL)
      {
        free ($3) ;
        stack_description_error
        (
          stkd, scanner, "malloc layer failed"
        ) ;
        YYABORT ;
      }

      layer->Height   = $1 ;
      layer->Material = find_material_in_list(stkd->MaterialsList, $3) ;

      if (layer->Material == NULL)
      {
        free ($3) ;
        free_layer(layer) ;
        stack_description_error
        (
          stkd, scanner, "unknown material identifier"
        ) ;
        YYABORT ;
      }

      free ($3) ;
    }
  ;

/******************************************************************************/
/******************************* Stack ****************************************/
/******************************************************************************/

stack

  : STACK ':'
      stack_elements
    {
      stkd->StackElementsList = $3 ;

      if (tmp_stack_element->Type == TL_STACK_ELEMENT_CHANNEL)
      {
        stack_description_error
        (
          stkd, scanner, "channel as top stack element not supported"
        ) ;

        tmp_stack_element = NULL;

        YYABORT ;
      }
      if (found_die == 0)
      {
        stack_description_error
        (
          stkd, scanner, "die declared but not used"
        ) ;

        found_die = 0 ;

        YYABORT ;
      }
    }
  ;

stack_elements

  : stack_element
    {
      if (tmp_stack_element->Type == TL_STACK_ELEMENT_CHANNEL)
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
      if (find_stack_element_in_list(stkd->StackElementsList, $2->Id) != NULL)
      {
        free_stack_element ($2) ;
        stack_description_error
        (
          stkd, scanner, "stack element id already declared"
        ) ;
        YYABORT ;
      }

      $2->Next = $1 ;
      $$ = $2 ;
    }
  ;

stack_element

  : LAYER IDENTIFIER DVALUE UM IDENTIFIER ';'
    {
      tmp_stack_element = $$ = alloc_and_init_stack_element() ;

      if (tmp_stack_element == NULL)
      {
        free ($2);
        free ($5) ;
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
        free ($5) ;
        free_stack_element (tmp_stack_element) ;
        stack_description_error
        (
          stkd, scanner, "malloc layer failed"
        ) ;
        YYABORT ;
      }

      layer->Height   = $3 ;
      layer->Material = find_material_in_list(stkd->MaterialsList, $5) ;

      if (layer->Material == NULL)
      {
        free ($2);
        free ($5) ;
        free_layer(layer) ;
        free_stack_element (tmp_stack_element) ;
        stack_description_error
        (
          stkd, scanner, "unknown material identifier"
        ) ;
        YYABORT ;
      }

      free($5) ;

      tmp_stack_element->Type          = TL_STACK_ELEMENT_LAYER ;
      tmp_stack_element->Pointer.Layer = layer ;
      tmp_stack_element->Id            = $2 ;
      tmp_stack_element->NLayers       = 1 ;
    }
  | CHANNEL IDENTIFIER ';'
    {
      if (stkd->Channel == NULL)
      {
        free ($2) ;
        stack_description_error
        (
          stkd, scanner, "channel used but not declared"
        ) ;
        YYABORT ;
      }

      if (tmp_stack_element->Type == TL_STACK_ELEMENT_CHANNEL)
      {
        free ($2) ;
        stack_description_error
        (
          stkd, scanner, "two consecutive channel layers not supported"
        ) ;
        YYABORT ;
      }
      tmp_stack_element = $$ = alloc_and_init_stack_element() ;

      if (tmp_stack_element == NULL)
      {
        free ($2) ;
        stack_description_error
        (
          stkd, scanner, "malloc stack element failed"
        ) ;
        YYABORT ;
      }

      tmp_stack_element->Type    = TL_STACK_ELEMENT_CHANNEL ;
      tmp_stack_element->Id      = $2 ;
      tmp_stack_element->NLayers = 1 ;
    }
  | DIE IDENTIFIER IDENTIFIER FLOORPLAN PATH ';'
    {
      tmp_stack_element = $$ = alloc_and_init_stack_element() ;

      if (tmp_stack_element == NULL)
      {
        free ($3) ;
        free ($2) ;
        free ($5) ;
        stack_description_error
        (
          stkd, scanner, "malloc stack element failed"
        ) ;
        YYABORT ;
      }

      tmp_stack_element->Type        = TL_STACK_ELEMENT_DIE ;
      tmp_stack_element->Id          = $2 ;
      tmp_stack_element->Pointer.Die = find_die_in_list(stkd->DiesList, $3) ;

      if (tmp_stack_element->Pointer.Die == NULL)
      {
        free($3) ;
        free($5) ;
        free_stack_element(tmp_stack_element) ;
        stack_description_error
        (
          stkd, scanner, "unknown die identifier"
        ) ;
        YYABORT ;
      }

      tmp_stack_element->NLayers = tmp_stack_element->Pointer.Die->NLayers ;

      tmp_stack_element->Floorplan = alloc_and_init_floorplan ();

      if (tmp_stack_element->Floorplan == NULL)
      {
        free($3) ;
        free($5) ;
        free_stack_element(tmp_stack_element) ;
        stack_description_error
        (
          stkd, scanner, "malloc floorplan failed"
        ) ;
        YYABORT ;
      }

      tmp_stack_element->Floorplan->FileName = $5 ;

      free($3) ;

      found_die = 1 ;
    }
  ;

/******************************************************************************/
/******************************* Dimensions ***********************************/
/******************************************************************************/

dimensions

  : DIMENSIONS ':'
      CHIP LENGTH DVALUE MM ';'
      CHIP WIDTH  DVALUE MM ';'
      CELL LENGTH DVALUE UM ';'
      CELL WIDTH  DVALUE UM ';'
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

      // Set width and NRows

      stkd->Dimensions->Chip.Width       = $10 * 1000.0 ;
      stkd->Dimensions->Cell.Width       = $20 ;

      stkd->Dimensions->Grid.NRows
        = (int) (stkd->Dimensions->Chip.Width / stkd->Dimensions->Cell.Width) ;

      // Set length and NColumns

      stkd->Dimensions->Chip.Length      = $5  * 1000.0 ;

      if (stkd->Channel == NULL)
      {
        // There are no channels in the stack

        stkd->Dimensions->StackHasChannel  = 0 ;

        stkd->Dimensions->Cell.WallLength = $15 ;

        stkd->Dimensions->Grid.NColumns
          = (int) stkd->Dimensions->Chip.Length
                  / stkd->Dimensions->Cell.WallLength ;
      }
      else
      {
        // There are channels in the stack

        stkd->Dimensions->StackHasChannel = 1 ;

        stkd->Dimensions->Cell.FirstWallLength = tmp_first_wall_length ;
        stkd->Dimensions->Cell.LastWallLength = tmp_last_wall_length ;
        stkd->Dimensions->Cell.WallLength = tmp_wall_length ;
        stkd->Dimensions->Cell.ChannelLength = tmp_channel_length ;

        stkd->Dimensions->Grid.NColumns
          = 2 * (int) (
                        (
                          stkd->Dimensions->Chip.Length
                          - stkd->Dimensions->Cell.FirstWallLength
                          - stkd->Dimensions->Cell.LastWallLength
                          - stkd->Dimensions->Cell.ChannelLength
                        )
                        /
                        ( stkd->Dimensions->Cell.ChannelLength
                            + stkd->Dimensions->Cell.WallLength
                        )
                      )
                      + 3 ;
      }

      // Check the number of ciolumns get

      if (stkd->Dimensions->Grid.NColumns < 3)
      {
        stack_description_error
        (
          stkd, scanner, "not enough columns"
        ) ;
        YYABORT ;
      }

      if (stkd->Dimensions->StackHasChannel
          && stkd->Dimensions->Grid.NColumns % 2 == 0)
      {
        stack_description_error
        (
          stkd, scanner, "using channels with even number of columns"
        ) ;
        YYABORT ;
      }

      struct StackElement *stk_el = stkd->StackElementsList ;

      for ( ; stk_el != NULL ; stk_el = stk_el->Next)

        stkd->Dimensions->Grid.NLayers += stk_el->NLayers ;

      stkd->Dimensions->Grid.NCells
        = stkd->Dimensions->Grid.NLayers
          * stkd->Dimensions->Grid.NRows
          * stkd->Dimensions->Grid.NColumns ;

      stkd->Dimensions->Grid.NNz
        =   stkd->Dimensions->Grid.NLayers
            * (
                  stkd->Dimensions->Grid.NRows
                  * (3 * stkd->Dimensions->Grid.NColumns - 2)
                + 2 * stkd->Dimensions->Grid.NColumns
                  * (stkd->Dimensions->Grid.NRows - 1)
              )
          + (stkd->Dimensions->Grid.NLayers - 1 ) * 2
            * stkd->Dimensions->Grid.NRows * stkd->Dimensions->Grid.NColumns ;
    }
  ;

first_wall_length

  : /* empty */                     { $$ = 0 ;  }
  | FIRST WALL LENGTH DVALUE UM ';' { $$ = $4 ; }
  ;

last_wall_length

  : /* empty */                    { $$ = 0 ;  }
  | LAST WALL LENGTH DVALUE UM ';' { $$ = $4 ; }
  ;

%%

void
stack_description_error
(
  struct StackDescription *stkd   ,
  yyscan_t         scanner ,
  char             *message
)
{
  fprintf (stack_description_get_out (scanner),
    "%s:%d: %s\n", stkd->FileName,
                   stack_description_get_lineno (scanner),
                   message) ;

  free_stack_description (stkd) ;
}
