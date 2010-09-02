/******************************************************************************
 *                                                                            *
 * Source file "bison/hotspot_ptrace_parser.y"                                     *
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
#include "../flex/hotspot_ptrace_scanner.h"

void hotspot_ptrace_error
(
  Floorplan*  floorplan,
  yyscan_t    yyscanner,
  char const* msg
) ;

static int values_counter = 1;
FloorplanElement* flp_el ;
%}

%destructor { free($$) ; } <string>

%token <d_value> DVALUE        "double value"
%token <string>  IDENTIFIER    "identifier"

%name-prefix "hotspot_ptrace_"
%output      "hotspot_ptrace_parser.c"

%pure-parser

%error-verbose

%parse-param { Floorplan* floorplan }
%parse-param { yyscan_t   scanner   }

%lex-param   { yyscan_t   scanner   }

%start ptraces

%%

ptraces

  : ids_list
    powers_list
    {
      if (values_counter != floorplan->NElements)
      {
        perror ("There are missing power values") ;
        hotspot_flp_error (floorplan, scanner, "") ;
        YYABORT ;
      }
    }
  ;

ids_list

  : /* empty */

  | ids_list IDENTIFIER
    {
      flp_el = alloc_and_init_floorplan_element () ;

      if (flp_el == NULL)
      {
        perror ("alloc_floorplan_element") ;
        hotspot_flp_error (floorplan, scanner, "") ;
        YYABORT ;
      }

      flp_el->Id = $2 ;
      flp_el->PowerValues = alloc_and_init_powers_queue () ;

      if (flp_el->PowerValues == NULL)
      {
        perror ("alloc_powers_queue") ;
        hotspot_flp_error (floorplan, scanner, "") ;
        YYABORT ;
      }

      floorplan->NElements++ ;

      FloorplanElement* tmp_flp_el = floorplan->ElementsList ;
      while (tmp_flp_el != NULL)
        tmp_flp_el  = tmp_flp_el->Next ;

      tmp_flp_el = flp_el ;
    }
  ;

powers_list

  : /* empty */

  | powers_list DVALUE
    {
      if (values_counter == floorplan->NElements)
      {
        flp_el = floorplan->ElementsList ;
        values_counter = 1 ;
      }
      else
      {
        flp_el = flp_el->Next ;
        values_counter++ ;
      }

      put_into_powers_queue(flp_el->PowerValues, $2) ;
    }
  ;

%%

void
hotspot_ptrace_error
(
  Floorplan*  floorplan,
  yyscan_t    yyscanner,
  char const* msg
)
{
  fprintf (stderr,
    "%s:%d: %s\n",
    floorplan->FileName, floorplan_get_lineno(yyscanner), msg) ;

  free_floorplan (floorplan) ;
}
