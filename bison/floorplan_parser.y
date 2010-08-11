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
  struct PowersQueue       *p_powers_queue ;
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

static int first_length_found = 0 ;
%}

%type <p_floorplan_element> floorplan_element ;
%type <p_floorplan_element> floorplan_element_list ;
%type <p_powers_queue>      power_values_list ;

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
        = $$ = alloc_and_init_floorplan_element ( ) ;

      if (floorplan_element == NULL)
      {
        perror ("alloc_floorplan_element") ;
        floorplan_error (floorplan, dimensions, scanner, "") ;
        YYABORT ;
      }

      floorplan_element->Id          = $1  ;
      floorplan_element->SW_X        = $4  ;
      floorplan_element->SW_Y        = $6  ;
      floorplan_element->Length      = $9  ;
      floorplan_element->Width       = $11 ;
      floorplan_element->PowerValues = $15 ;

      if (first_length_found == 0)
        first_length_found = $15->Length ;
      else
        if ($15->Length < first_length_found)
        {
          floorplan_error (floorplan, dimensions, scanner, "Missing power value!") ;
          YYABORT ;
        }
    }
  ;

power_values_list

  : DVALUE
    {
      PowersQueue* powers_list = $$ = alloc_and_init_powers_queue() ;

      if (powers_list == NULL)
      {
        perror ("alloc_powers_queue") ;
        floorplan_error (floorplan, dimensions, scanner, "") ;
        YYABORT ;
      }

      put_into_powers_queue (powers_list, $1) ;
    }

  | power_values_list ',' DVALUE
    {
      if (first_length_found != 0
          && $1->Length + 1 > first_length_found)

        fprintf (stderr, "Warning: discarding value %f\n", $3);

      else

        put_into_powers_queue ($1, $3) ;

      $$ = $1 ;
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
