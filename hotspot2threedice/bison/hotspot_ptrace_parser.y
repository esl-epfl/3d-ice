/******************************************************************************
 *                                                                            *
 * Source file "bison/hotspot_ptrace_parser.y"                                     *
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
#include "../flex/hotspot_ptrace_scanner.h"

void hotspot_ptrace_error
(
  struct FloorplanElement** flp_el_list,
  yyscan_t                  yyscanner,
  char const*               msg
) ;

static struct FloorplanElement* flp_el ;
static int flp_el_found ;
static int values_counter ;

%}

%destructor { free($$) ; } <string>

%token <d_value> DVALUE        "double value"
%token <string>  IDENTIFIER    "identifier"

%require     "2.4.1"
%name-prefix "hotspot_ptrace_"
%output      "hotspot_ptrace_parser.c"

%pure-parser

%error-verbose

%parse-param { struct FloorplanElement **flp_el_list }
%parse-param { yyscan_t scanner }

%lex-param   { yyscan_t scanner }

%start ptraces

%%

ptraces

  : ids_list
    powers_list
    {
      if (values_counter != flp_el_found)
      {
        perror ("There are missing power values") ;
        hotspot_flp_error (flp_el_list, scanner, "") ;
        YYABORT ;
      }
    }
  ;

ids_list

  : IDENTIFIER
    {
      *flp_el_list = flp_el = alloc_and_init_floorplan_element () ;

      if (flp_el == NULL)
      {
        perror ("alloc_floorplan_element") ;
        hotspot_flp_error (flp_el_list, scanner, "") ;
        YYABORT ;
      }

      flp_el->PowerValues = alloc_and_init_powers_queue () ;

      if (flp_el->PowerValues == NULL)
      {
        perror ("alloc_powers_queue") ;
        hotspot_flp_error (flp_el_list, scanner, "") ;
        YYABORT ;
      }

      flp_el_found = 1 ;
      flp_el->Id = $1 ;
    }
  | ids_list IDENTIFIER
    {
      flp_el->Next = alloc_and_init_floorplan_element () ;

      if (flp_el->Next == NULL)
      {
        perror ("alloc_floorplan_element") ;
        hotspot_flp_error (flp_el_list, scanner, "") ;
        YYABORT ;
      }

      flp_el->Next->PowerValues = alloc_and_init_powers_queue () ;

      if (flp_el->PowerValues == NULL)
      {
        perror ("alloc_powers_queue") ;
        hotspot_flp_error (flp_el_list, scanner, "") ;
        YYABORT ;
      }

      flp_el_found++ ;
      flp_el->Next->Id = $2 ;

      flp_el = flp_el->Next ;
    }
  ;

powers_list

  : DVALUE
    {
      flp_el = *flp_el_list ;
      values_counter = 1 ;

      put_into_powers_queue(flp_el->PowerValues, $1) ;
    }
  | powers_list DVALUE
    {
      if (values_counter == flp_el_found)
      {
        flp_el = *flp_el_list ;
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
