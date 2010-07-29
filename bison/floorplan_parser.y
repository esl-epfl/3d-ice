/******************************************************************************
 *                                                                            *
 * Source file "Bison/floorplan_parser.y"                                     *
 *                                                                            *
 * Compile with "bison -d filename"                                           *
 *                                                                            *
 * EPFL-STI-IEL-ESL                                                           *
 * Bâtiment ELG, ELG 130                                                      *
 * Station 11                                                                 *
 * 1015 Lausanne, Switzerland                    alessandro.vincenzi@epfl.ch  *
 ******************************************************************************/

%{
#include "dimensions.h"
#include "floorplan.h"
#include "floorplan_element.h"
%}

%union
{
  int                      i_value ;
  double                   d_value ;
  char                     *string ;

  struct FloorplanElement  *p_floorplan_element ;
}

%{
#include "../flex/floorplan_scanner.h"

void
floorplan_error (
                 struct Floorplan *floorplan,
                 Dimensions* dimensions,
                 yyscan_t    yyscanner,
                 char const *msg
                ) ;

static double* powers_list        = NULL ;
static int     length             = 2 ;
static int     power_values_found = 0 ;
static int     first_length_found = 0 ;
%}

%type <p_floorplan_element> floorplan_element ;
%type <p_floorplan_element> floorplan_element_list ;

%destructor { free($$) ; } <string>

%token POSITION   "keyword position"
%token DIMENSION  "keyword dimension"
%token POWER      "keyword power"
%token VALUES     "keyword values"

%token <i_value> UIVALUE       "integer value"
%token <d_value> DVALUE        "double value"
%token <string>  IDENTIFIER    "identifier"

%require     "2.4.1"
%name-prefix "floorplan_"
%output      "floorplan_parser.c"

%pure-parser

%error-verbose

%parse-param { struct Floorplan *floorplan }
%parse-param { Dimensions* dimensions }
%parse-param { yyscan_t scanner }

%lex-param   { yyscan_t scanner }

%start floorplan_element_list

%%

floorplan_element_list

  : floorplan_element
    {
      int tmp_1 = check_location (floorplan, $1, dimensions) ;
      int tmp_2 = align_to_grid  (floorplan, $1, dimensions) ;

      if (tmp_1 || tmp_2)
      {
        free_floorplan_element ($1) ;
        floorplan_error (floorplan, dimensions, scanner, "") ;
        YYABORT ;
      }

      floorplan->ElementsList = $1 ;
      floorplan->NElements    = 1 ;
      $$ = $1 ;
    }
  | floorplan_element_list floorplan_element
    {
      int tmp_1 = check_intersections (floorplan, $2) ;
      int tmp_2 = check_location      (floorplan, $2, dimensions) ;
      int tmp_3 = align_to_grid       (floorplan, $2, dimensions) ;

      if (tmp_1 || tmp_2 || tmp_3 )
      {
        free_floorplan_element ($2) ;
        floorplan_error (floorplan, dimensions, scanner, "") ;
        YYABORT ;
      }
      $1->Next = $2 ;

      floorplan->NElements++ ;

      $$ = $2 ;
    }
  ;

floorplan_element

  : IDENTIFIER ':'
      POSITION  UIVALUE ',' UIVALUE ';'
      DIMENSION UIVALUE ',' UIVALUE ';'
      POWER VALUES power_values_list ';'
    {
      struct FloorplanElement *floorplan_element
        = $$ = alloc_and_init_floorplan_element (power_values_found) ;

      if (floorplan_element == NULL)
      {
        perror ("alloc_floorplan_element") ;
        floorplan_error (floorplan, dimensions, scanner, "") ;
        YYABORT ;
      }

      floorplan_element->Id     = $1  ;
      floorplan_element->SW_X   = $4  ;
      floorplan_element->SW_Y   = $6  ;
      floorplan_element->Length = $9  ;
      floorplan_element->Width  = $11 ;

      int index;
      for (index = 0; index < power_values_found; index++)
        floorplan_element->PowerValuesList[index] = powers_list[index] ;

      if (first_length_found == 0)

        first_length_found = power_values_found ;

      else

        if (power_values_found < first_length_found)
        {
          power_values_found = 0 ;
          floorplan_error (floorplan, dimensions, scanner, "Missing power value!") ;
          YYABORT ;
        }

      power_values_found = 0 ;
    }
  ;

power_values_list

  : DVALUE
    {
      if (powers_list == NULL)
      {
        powers_list = (double*) malloc ( length * sizeof (double) ) ;
        if (powers_list == NULL)
        {
          perror ("alloc_power_list") ;
          floorplan_error (floorplan, dimensions, scanner, "") ;
          YYABORT ;
        }
      }
      powers_list[power_values_found++] = $1 ;
    }

  | power_values_list ',' DVALUE
    {
      if (power_values_found == length)
      {
        length *= 2 ;

        double* tmp_powers_list
          = (double*) malloc ( length * sizeof (double) ) ;

        if (tmp_powers_list == NULL)
        {
          perror ("alloc_tmp_power_list") ;
          floorplan_error (floorplan, dimensions, scanner, "") ;
          YYABORT ;
        }

        int index;
        for (index = 0; index < power_values_found; index++)
        {
          tmp_powers_list[index] = powers_list[index] ;
        }

        free(powers_list);
        powers_list = tmp_powers_list ;
      }

      if (first_length_found != 0
          && first_length_found < power_values_found)

        fprintf (stderr, "Warning: discarding value %f\n", $3);

      else

        powers_list[power_values_found++] = $3 ;
    }
  ;

%%

void
floorplan_error
(
  struct Floorplan  *floorplan,
  Dimensions* dimensions,
  yyscan_t   yyscanner,
  char const *msg
)
{
  fprintf (stderr,
    "%s:%d: %s\n",
    floorplan->FileName, floorplan_get_lineno(yyscanner), msg) ;

  get_chip_length (dimensions) ;  // FIXME

  free_floorplan (floorplan) ;
}
