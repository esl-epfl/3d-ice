/******************************************************************************
 *                                                                            *
 * Source file "bison/hotspot_flp_parser.y"                                     *
 *                                                                            *
 * Compile with "bison -d filename"                                           *
 *                                                                            *
 * EPFL-STI-IEL-ESL                                                           *
 * Batiment ELG, ELG 130                                                      *
 * Station 11                                                                 *
 * 1015 Lausanne, Switzerland                    alessandro.vincenzi@epfl.ch  *
 ******************************************************************************/

%{
#include "floorplan.h"
#include "floorplan_element.h"
%}

%union
{
  double            d_value ;
  char*             string ;
  FloorplanElement* p_floorplan_element ;
}

%{
#include "../flex/hotspot_flp_scanner.h"

void hotspot_flp_error
(
  Floorplan*  floorplan,
  yyscan_t    yyscanner,
  char const* msg
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

%parse-param { Floorplan*  floorplan  }
%parse-param { yyscan_t    scanner    }

%lex-param   { yyscan_t    scanner    }

%start floorplan_element_list

%%

floorplan_element_list

  : floorplan_element
    {
      floorplan->ElementsList = $1 ;
      floorplan->NElements    = 1 ;
      $$ = $1 ;
    }
  | floorplan_element_list floorplan_element
    {
      if (check_intersections (floorplan, $2) )
      {
        free_floorplan_element ($2) ;
        floorplan_error (floorplan, scanner, "") ;
        YYABORT ;
      }

      floorplan->NElements++ ;

      $1->Next = $2 ;
      $$ = $2 ;
    }
  ;

floorplan_element

  : IDENTIFIER DVALUE DVALUE DVALUE DVALUE
    {
      FloorplanElement *floorplan_element
        = $$ = alloc_and_init_floorplan_element () ;

      if (floorplan_element == NULL)
      {
        perror ("alloc_floorplan_element") ;
        hotspot_flp_error (floorplan, scanner, "") ;
        YYABORT ;
      }

      floorplan_element->Id     = $1 ;
      floorplan_element->Width  = (int) ($2 * 1.0e6) ;
      floorplan_element->Length = (int) ($3 * 1.0e6) ;
      floorplan_element->SW_X   = (int) ($4 * 1.0e6) ;
      floorplan_element->SW_Y   = (int) ($5 * 1.0e6) ;

      printf("reading %f %f into %d %d\n", $2, $3, floorplan_element->Width,
      floorplan_element->Length );

    }
  ;

%%

void
hotspot_flp_error
(
  Floorplan  *floorplan,
  yyscan_t   yyscanner,
  char const *msg
)
{
  fprintf (stderr,
    "%s:%d: %s\n",
    floorplan->FileName, floorplan_get_lineno(yyscanner), msg) ;

  free_floorplan (floorplan) ;
}
