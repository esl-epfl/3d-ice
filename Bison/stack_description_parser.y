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

   struct material      *material_p ;
   struct die           *die_p ;
   struct layer         *layer_p ;
   struct stack_element *stack_element_p ;
}

%destructor { free($$) ; } <string> ;

%type <double_v> width_mm
%type <double_v> width_um
%type <double_v> length_mm
%type <double_v> length_um
%type <double_v> thermal_conductivity
%type <double_v> specific_heat
%type <double_v> height_um
%type <double_v> liquid_htc
%type <double_v> liquid_sh

%type <int_v> sources_position

%type <string> wall

%type <material_p> material
%type <material_p> materials_list

%type <die_p> die
%type <die_p> dies_list

%type <layer_p> layer
%type <layer_p> layers_list

%type <stack_element_p> stack_element
%type <stack_element_p> stack_elements

%token CHANNEL               "keyword channel"
%token CELL                  "keyword cell"
%token DIE                   "keyword die"
%token FIRST                 "keyword first"
%token LAYER                 "keyword layer"
%token LAST                  "keyword last"
%token MATERIAL              "keyword material"
%token ON                    "keyword on"
%token SOURCES               "keyword sources"
%token SPECIFIC_HEAT         "keywords specific heat"
%token THERMAL_CONDUCTIVITY  "keywords thermal conductivity"
%token HEIGHT                "keyword height"
%token UM                    "keyword um"
%token WALL                  "keyword wall"
%token LIQUID                "keyword liquid"
%token HTC                   "keywords heat transfer coefficient"
%token WIDTH                 "keyword width"
%token LENGTH                "keyword length"
%token FLOORPLAN             "keyword floorplan"
%token STACK                 "keyword stack"
%token MM                    "keyword mm"

%token <double_v> DVALUE     "float value"
%token <string>   IDENTIFIER "identifier"
%token <int_v>    IVALUE     "integer value"
%token <string>   PATH       "path to file"

%{
#include "stack_description_scanner.h"

void stack_description_error (StackDescription *stack,
                              yyscan_t yyscanner,
                              char const *msg) ;

static Layer *tmp_layer_pointer = NULL ;
static int   tmp_layer_counter  = 0 ;

%}

%require     "2.4.1"
%name-prefix "stack_description_"
%output      "Sources/stack_description_parser.c"

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
    channel
    dies_list
    {
      stkd->Dimensions = alloc_and_init_dimensions() ;

      if (stkd->Dimensions == NULL)
      {
        stack_description_error(stkd, scanner, "alloc_and_init_dimensions") ;
        YYABORT ;
      }
    }
    stack
    dimensions
  ;

/******************************************************************************/
/******************************* Materials ************************************/
/******************************************************************************/

materials_list

  : material
    {
      stkd->MaterialsList = $1 ;
      $$ = $1 ;
    }
  | materials_list material
    {
      $1->Next = $2 ;
      $$ = $2 ;
    }
  ;

material

  : MATERIAL IDENTIFIER ':'
       thermal_conductivity
       specific_heat
    {
      Material *material = alloc_and_init_material() ;

      if (material == NULL)
      {
        stack_description_error(stkd, scanner, "alloc_and_init_material") ;
        YYABORT ;
      }

      material->Id                  = $2 ;
      material->ThermalConductivity = $4 ;
      material->SpecificHeat        = $5 ;

      if (find_material_in_list(stkd->MaterialsList, $2) != NULL)
      {
        free_material (material) ;
        stack_description_error (stkd, scanner, "Material already declared") ;
        YYABORT ;
      }

      $$ = material ;
    }
  ;

/******************************************************************************/
/******************************* Channel **************************************/
/******************************************************************************/

channel

  : CHANNEL ':'
      height_um
      wall
      liquid_htc
      liquid_sh
    {
      stkd->Channel = alloc_and_init_channel() ;

      if (stkd->Channel == NULL)
      {
        free($4) ;
        stack_description_error(stkd, scanner, "alloc_and_init_channel") ;
        YYABORT ;
      }

      stkd->Channel->Height       = $3 ;
      stkd->Channel->LiquidHTC    = $5 ;
      stkd->Channel->LiquidSH     = $6 ;
      stkd->Channel->WallMaterial
        = find_material_in_list(stkd->MaterialsList, $4) ;

      if (stkd->Channel->WallMaterial == NULL)
      {
        free($4) ;
        stack_description_error(stkd, scanner, "Unknown material id") ;
        YYABORT ;
      }
      free($4) ;
    }
  ;

wall

  : WALL MATERIAL IDENTIFIER ';'
    { $$ = $3; }
  ;

/******************************************************************************/
/******************************* Dies *****************************************/
/******************************************************************************/

dies_list

  : die
    {
      stkd->DiesList = $1 ;
      $$ = $1 ;
    }
  | dies_list die
    {
      $1->Next = $2 ;
      $$ = $2 ;
    }
  ;

die

  : DIE IDENTIFIER ':'
       layers_list
       sources_position
    {
      Die *die = alloc_and_init_die() ;

      if (die == NULL)
      {
        stack_description_error(stkd, scanner, "alloc_and_init_die") ;
        YYABORT ;
      }

      die->Id          = $2 ;
      die->LayersList  = tmp_layer_pointer ;
      die->NLayers     = tmp_layer_counter ;
      die->SourcesId   = $5 ;

      tmp_layer_pointer = NULL ;
      tmp_layer_counter = 0 ;

      $$ = die ;
    }
  ;

sources_position

  : SOURCES ON LAYER IVALUE ';'
    { $$ = $4 }
  ;

/******************************************************************************/
/******************************* Layers ***************************************/
/******************************************************************************/

layers_list

  : layer
    {
      tmp_layer_pointer = $1 ;
      tmp_layer_counter = 1 ;

      $$ = $1 ;
    }
  | layers_list layer
    {
      if (find_layer_in_list(tmp_layer_pointer, $2->Id) != NULL)
      {
        free_layer($2) ;
        free_layers_list(tmp_layer_pointer) ;
        stack_description_error (stkd, scanner, "Layer id already declared") ;
        YYABORT ;
      }

      tmp_layer_pointer = $2 ;
      $2->Next = $1 ;

      tmp_layer_counter++ ;

      $$ = $2 ;
    }
  ;

layer

  : LAYER IVALUE DVALUE UM IDENTIFIER ';'
    {
      Layer *layer = alloc_and_init_layer() ;

      if (layer == NULL)
      {
        free($5) ;
        free_layers_list(tmp_layer_pointer) ;
        stack_description_error(stkd, scanner, "alloc_and_init_layer") ;
        YYABORT ;
      }

      layer->Id       = $2 ;
      layer->Height   = $3 ;
      layer->Material = find_material_in_list(stkd->MaterialsList, $5) ;

      if (layer->Material == NULL)
      {
        free($5) ;
        free_layer(layer) ;
        free_layers_list(tmp_layer_pointer) ;
        stack_description_error(stkd, scanner, "Unknown material id") ;
        YYABORT ;
      }

      free($5) ;

      $$ = layer ;
    }
  ;

/******************************************************************************/
/******************************* Stack ****************************************/
/******************************************************************************/

stack

  : STACK ':'
        length_mm
        width_mm
        stack_elements
    {
      stkd->Dimensions->Chip.Length = $3;
      stkd->Dimensions->Chip.Width  = $4;
    }
  ;

stack_elements

  : stack_element
    {
      stkd->StackElementsList = $1 ;

      stkd->Dimensions->Grid.NLayers += $1->NLayers ;

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

      stkd->StackElementsList = $2 ;
      $2->Next = $1 ;

      stkd->Dimensions->Grid.NLayers += $2->NLayers ;

      $$ = $2 ;
    }
  ;

stack_element

  : layer
    {
      StackElement *stack_element = alloc_and_init_stack_element() ;

      if (stack_element == NULL)
      {
        free_layer ($1) ;
        stack_description_error(stkd, scanner, "alloc_and_init_stack_element") ;
        YYABORT ;
      }

      stack_element->Type          = TL_STACK_ELEMENT_LAYER ;
      stack_element->Pointer.Layer = $1 ;
      stack_element->Id            = $1->Id ;
      stack_element->NLayers       = 1 ;

      $$ = stack_element ;
    }
  | CHANNEL IVALUE ';'
    {
      StackElement *stack_element = alloc_and_init_stack_element() ;

      if (stack_element == NULL)
      {
        stack_description_error(stkd, scanner, "alloc_and_init_stack_element") ;
        YYABORT ;
      }

      stack_element->Type    = TL_STACK_ELEMENT_CHANNEL ;
      stack_element->Id      = $2 ;
      stack_element->NLayers = 1 ;

      $$ = stack_element ;
    }
  | DIE IVALUE IDENTIFIER FLOORPLAN '"' PATH '"' ';'
    {
      StackElement *stack_element = alloc_and_init_stack_element() ;

      if (stack_element == NULL)
      {
        free($3) ;
        free($6) ;
        stack_description_error(stkd, scanner, "alloc_and_init_stack_element") ;
        YYABORT ;
      }

      stack_element->Type        = TL_STACK_ELEMENT_DIE ;
      stack_element->Id          = $2 ;
      stack_element->Pointer.Die = find_die_in_list(stkd->DiesList, $3) ;

      if (stack_element->Pointer.Die == NULL)
      {
        free($3) ;
        free($6) ;
        free_stack_element(stack_element) ;
        stack_description_error(stkd, scanner, "Unknown die id") ;
        YYABORT ;
      }

      stack_element->NLayers = stack_element->Pointer.Die->NLayers ;

      stack_element->Floorplan = alloc_and_init_floorplan ();

      if (stack_element->Floorplan == NULL)
      {
        free($3) ;
        free($6) ;
        free_stack_element(stack_element) ;
        stack_description_error(stkd, scanner, "alloc_and_init_floorplan") ;
        YYABORT ;
      }

      stack_element->Floorplan->FileName = $6 ;

      free($3) ;

      $$ = stack_element ;
    }
  ;

/******************************************************************************/
/******************************* Dimensions ***********************************/
/******************************************************************************/

dimensions

  : CELL length_um
    CELL width_um
    FIRST CELL length_um
    LAST CELL length_um
    {
      stkd->Dimensions->Cell.Length      = $2 ;
      stkd->Dimensions->Cell.Width       = $4 ;
      stkd->Dimensions->Cell.FirstLength = $7 ;
      stkd->Dimensions->Cell.LastLength  = $10 ;
    }
  ;

/******************************************************************************/
/******************************* Float and integer values *********************/
/******************************************************************************/

thermal_conductivity

  : THERMAL_CONDUCTIVITY DVALUE ';' { $$ = $2 ; } ;

specific_heat

  : SPECIFIC_HEAT DVALUE ';' { $$ = $2 ; } ;

height_um

  : HEIGHT DVALUE UM ';' { $$ = $2 ; } ;

liquid_htc

  : LIQUID HTC DVALUE ';' { $$ = $3 ; } ;

liquid_sh

  : LIQUID SPECIFIC_HEAT DVALUE ';' { $$ = $3 ; } ;

length_um

  : LENGTH DVALUE UM ';' { $$ = $2 ; } ;

width_um

  : WIDTH DVALUE UM ';' { $$ = $2 ; } ;

length_mm

  : LENGTH DVALUE MM ';' { $$ = $2 ; } ;


width_mm

  : WIDTH DVALUE MM ';' { $$ = $2 ; }  ;

%%

void
stack_description_error (StackDescription *stkd,
                         void *yyscanner,
                         char const *msg)
{
  fprintf(stderr, "%s:%d: %s\n",
    stkd->FileName, stack_description_get_lineno(yyscanner), msg) ;

  free_stack_description(stkd) ;
}
