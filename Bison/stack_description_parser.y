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
%token HEATSINK              "keywork heatsink"

%token <double_v> DVALUE     "float value"
%token <string>   IDENTIFIER "identifier"
%token <int_v>    IVALUE     "integer value"
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
    heatsink
    channel
    dies_list
    stack
    dimensions
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
        stack_description_error (stkd, scanner, "alloc material failed") ;
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

heatsink

  : /* empty */
  | HEATSINK ':'
        HEAT TRANSFER COEFFICIENT DVALUE ';'
        ENVIRONMENT TEMPERATURE   DVALUE ';'
    {
    }
  ;
/******************************************************************************/
/******************************* Channel **************************************/
/******************************************************************************/

channel

  :  CHANNEL ':'
        HEIGHT DVALUE UM ';'
        WALL MATERIAL IDENTIFIER ';'
        COOLANT FLOW RATE DVALUE ';'
        COOLANT HEAT TRANSFER COEFFICIENT DVALUE ';'
        COOLANT VOLUMETRIC HEAT CAPACITY DVALUE ';'
        COOLANT INCOMING TEMPERATURE DVALUE ';'
    {
      stkd->Channel = alloc_and_init_channel() ;

      if (stkd->Channel == NULL)
      {
        stack_description_error(stkd, scanner, "alloc_and_init_channel") ;
        free ($9) ;
        YYABORT ;
      }

      stkd->Channel->Height          = $4  ;
      stkd->Channel->CoolantFR       = ( $14 * 1e+12 ) / 60.0 ;
      stkd->Channel->CoolantHTC      = $20 ;
      stkd->Channel->CoolantVHC      = $26 ;
      stkd->Channel->CoolantTIn      = $31 ;
      stkd->Channel->FlowRateChanged = 1 ;
      stkd->Channel->Wall
        = find_material_in_list (stkd->MaterialsList, $9) ;

      if (stkd->Channel->Wall == NULL)
      {
        stack_description_error(stkd, scanner, "Unknown material id") ;
        free ($9) ;
        YYABORT ;
      }

      free ($9) ;
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
        stack_description_error(stkd, scanner, "alloc_and_init_die") ;
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
        stack_description_error(stkd, scanner, "alloc_and_init_layer") ;
        YYABORT ;
      }

      layer->Height   = $1 ;
      layer->Material = find_material_in_list(stkd->MaterialsList, $3) ;

      if (layer->Material == NULL)
      {
        free ($3) ;
        free_layer(layer) ;
        stack_description_error(stkd, scanner, "Unknown material id") ;
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
    }
  ;

stack_elements

  : stack_element
    {
      $$ = $1 ;
    }
  | stack_elements stack_element
    {
      if (find_stack_element_in_list(stkd->StackElementsList, $2->Id) != NULL)
      {
        free_stack_element ($2) ;
        stack_description_error (stkd, scanner,
          "stack element id already declared") ;
        YYABORT ;
      }

      $2->Next = $1 ;
      $$ = $2 ;
    }
  ;

stack_element

  : LAYER IDENTIFIER DVALUE UM IDENTIFIER ';'
    {
      struct StackElement *stack_element = $$ = alloc_and_init_stack_element() ;

      if (stack_element == NULL)
      {
        free ($2);
        free ($5) ;
        stack_description_error(stkd, scanner, "alloc_and_init_stack_element") ;
        YYABORT ;
      }

      Layer* layer = alloc_and_init_layer() ;

      if (layer == NULL)
      {
        free ($2);
        free ($5) ;
        free_stack_element (stack_element) ;
        stack_description_error(stkd, scanner, "alloc_and_init_layer") ;
        YYABORT ;
      }

      layer->Height   = $3 ;
      layer->Material = find_material_in_list(stkd->MaterialsList, $5) ;

      if (layer->Material == NULL)
      {
        free ($2);
        free ($5) ;
        free_layer(layer) ;
        free_stack_element (stack_element) ;
        stack_description_error(stkd, scanner, "Unknown material id") ;
        YYABORT ;
      }

      free($5) ;

      stack_element->Type          = TL_STACK_ELEMENT_LAYER ;
      stack_element->Pointer.Layer = layer ;
      stack_element->Id            = $2 ;
      stack_element->NLayers       = 1 ;
    }
  | CHANNEL IDENTIFIER ';'
    {
      struct StackElement *stack_element = $$ = alloc_and_init_stack_element() ;

      if (stack_element == NULL)
      {
        free ($2) ;
        stack_description_error(stkd, scanner, "alloc_and_init_stack_element") ;
        YYABORT ;
      }

      stack_element->Type    = TL_STACK_ELEMENT_CHANNEL ;
      stack_element->Id      = $2 ;
      stack_element->NLayers = 1 ;
    }
  | DIE IDENTIFIER IDENTIFIER FLOORPLAN PATH ';'
    {
      struct StackElement *stack_element = $$ = alloc_and_init_stack_element() ;

      if (stack_element == NULL)
      {
        free ($3) ;
        free ($2) ;
        free ($5) ;
        stack_description_error(stkd, scanner, "alloc_and_init_stack_element") ;
        YYABORT ;
      }

      stack_element->Type        = TL_STACK_ELEMENT_DIE ;
      stack_element->Id          = $2 ;
      stack_element->Pointer.Die = find_die_in_list(stkd->DiesList, $3) ;

      if (stack_element->Pointer.Die == NULL)
      {
        free($3) ;
        free($5) ;
        free_stack_element(stack_element) ;
        stack_description_error(stkd, scanner, "Unknown die id") ;
        YYABORT ;
      }

      stack_element->NLayers = stack_element->Pointer.Die->NLayers ;

      stack_element->Floorplan = alloc_and_init_floorplan ();

      if (stack_element->Floorplan == NULL)
      {
        free($3) ;
        free($5) ;
        free_stack_element(stack_element) ;
        stack_description_error(stkd, scanner, "alloc_and_init_floorplan") ;
        YYABORT ;
      }

      stack_element->Floorplan->FileName = $5 ;

      free($3) ;
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
      FIRST CELL LENGTH DVALUE UM ';'
      LAST  CELL LENGTH DVALUE UM ';'
    {
      stkd->Dimensions = alloc_and_init_dimensions() ;

      if (stkd->Dimensions == NULL)
      {
        stack_description_error(stkd, scanner, "alloc_and_init_dimensions") ;
        YYABORT ;
      }

      stkd->Dimensions->Chip.Length      = $5  * 1000.0 ;
      stkd->Dimensions->Chip.Width       = $10 * 1000.0 ;
      stkd->Dimensions->Cell.Length      = $15 ;
      stkd->Dimensions->Cell.Width       = $20 ;
      stkd->Dimensions->Cell.FirstLength = $26 ;
      stkd->Dimensions->Cell.LastLength  = $32 ;

      stkd->Dimensions->Grid.NRows
        = (int) (stkd->Dimensions->Chip.Width / stkd->Dimensions->Cell.Width) ;

      stkd->Dimensions->Grid.NColumns
        = (int) (( ( stkd->Dimensions->Chip.Length
                     - stkd->Dimensions->Cell.FirstLength
                     - stkd->Dimensions->Cell.LastLength )
                   / stkd->Dimensions->Cell.Length )
                 + 2) ;

      if (stkd->Dimensions->Grid.NColumns % 2 == 0)
      {
        stack_description_error(stkd, scanner, "even number of columns") ;
        YYABORT ;
      }

      if (stkd->Dimensions->Grid.NColumns < 3)
      {
        stack_description_error(stkd, scanner, "not enough columns") ;
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
