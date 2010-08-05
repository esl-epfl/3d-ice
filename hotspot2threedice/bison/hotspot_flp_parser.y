/******************************************************************************
 *                                                                            *
 * Source file "bison/hotspot_flp_parser.y"                                     *
 *                                                                            *
 * Compile with "bison -d filename"                                           *
 *                                                                            *
 * EPFL-STI-IEL-ESL                                                           *
 * Bâtiment ELG, ELG 130                                                      *
 * Station 11                                                                 *
 * 1015 Lausanne, Switzerland                    alessandro.vincenzi@epfl.ch  *
 ******************************************************************************/

%{
#include "floorplan_element.h"
%}

%union
{
  double                   d_value ;
  char                     *string ;

  struct FloorplanElement  *p_floorplan_element ;
}

%{
#include "../flex/hotspot_flp_scanner.h"

void hotspot_flp_error
(
  struct FloorplanElement** flp_el_list,
  yyscan_t                  yyscanner,
  char const*               msg
) ;

%}

%type <p_floorplan_element> floorplan_element ;
%type <p_floorplan_element> floorplan_element_list ;

%destructor { free($$) ; } <string>

%token <d_value> DVALUE        "double value"
%token <string>  IDENTIFIER    "identifier"

%require     "2.4.1"
%name-prefix "hotspot_flp_"
%output      "hotspot_flp_parser.c"

%pure-parser

%error-verbose

%parse-param { struct FloorplanElement **flp_el_list }
%parse-param { yyscan_t scanner }

%lex-param   { yyscan_t scanner }

%start floorplan_element_list

%%

floorplan_element_list

  : floorplan_element
    {
      *flp_el_list = $1 ;
      $$ = $1 ;
    }
  | floorplan_element_list floorplan_element
    {
      $1->Next = $2 ;
      $$ = $2 ;
    }
  ;

floorplan_element

  : IDENTIFIER DVALUE DVALUE DVALUE DVALUE
    {
      struct FloorplanElement *floorplan_element
        = $$ = alloc_and_init_floorplan_element (0) ;

      if (floorplan_element == NULL)
      {
        perror ("alloc_floorplan_element") ;
        hotspot_flp_error (flp_el_list, scanner, "") ;
        YYABORT ;
      }

      floorplan_element->Id     = $1 ;
      floorplan_element->Width  = (int) ($2 * 1.0e6) ;
      floorplan_element->Length = (int) ($3 * 1.0e6) ;
      floorplan_element->SW_X   = (int) ($4 * 1.0e6) ;
      floorplan_element->SW_Y   = (int) ($5 * 1.0e6) ;
    }
  ;

%%

void
hotspot_flp_error
(
  struct FloorplanElement** flp_el_list,
  yyscan_t   yyscanner,
  char const *msg
)
{
  fprintf (stderr,
    "%d: %s\n",
    hotspot_flp_get_lineno(yyscanner), msg) ;

  free_floorplan_elements_list (*flp_el_list) ;
  *flp_el_list = NULL ;
}
