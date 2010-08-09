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
static int elements ;
static int values ;
static int size = 5 ;
static int lines ;

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
  ;

ids_list

  : IDENTIFIER
    {
      *flp_el_list = flp_el = alloc_and_init_floorplan_element (size) ;

      if (flp_el == NULL)
      {
        perror ("alloc_floorplan_element") ;
        hotspot_flp_error (flp_el_list, scanner, "") ;
        YYABORT ;
      }

      elements = 1 ;

      flp_el->Id = $1 ;
    }
  | ids_list IDENTIFIER
    {
      flp_el->Next = alloc_and_init_floorplan_element (size) ;

      if (flp_el->Next == NULL)
      {
        perror ("alloc_floorplan_element") ;
        hotspot_flp_error (flp_el_list, scanner, "") ;
        YYABORT ;
      }

      elements++ ;

      flp_el->Next->Id = $2 ;

      flp_el = flp_el->Next ;
    }
  ;

powers_list

  : DVALUE
    {
      flp_el = *flp_el_list ;
      values = 1 ;
      lines = 0 ;

      flp_el->PowerValuesList[lines] = $1 ;
    }
  | powers_list DVALUE
    {
      if (values == elements)
      {
        flp_el = *flp_el_list ;
        values = 1 ;
        lines++;
      }
      else
      {
        flp_el = flp_el->Next ;
        values++ ;
      }

      if (lines == size)
      {
        double* tmp_powers_list
          = (double*) malloc ( 2 * size * sizeof (double) ) ;

        if (tmp_powers_list == NULL)
        {
          perror ("alloc_tmp_power_list") ;
          hotspot_ptrace_error (flp_el_list, scanner, "") ;
          YYABORT ;
        }

        int index;
        for (index = 0; index < lines; index++)
        {
          tmp_powers_list[index] = flp_el->PowerValuesList[index] ;
        }

        free(flp_el->PowerValuesList);
        flp_el->PowerValuesList = tmp_powers_list ;
        flp_el->NPowerValues = 2 * size ;
      }

      if (size == lines && flp_el->Next == NULL)
        size *= 2 ;

      flp_el->PowerValuesList[lines] = $2 ;
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
