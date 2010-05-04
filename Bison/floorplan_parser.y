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
  int  integer ;
  char *string ;

  struct integer_couple
  {
    int a ;
    int b ;
  } integers ;

  struct floorplan_element *p_floorplan_element ;
}

%{
#include "floorplan_scanner.h"

void floorplan_error (Floorplan *floorplan,
                      Dimensions *dim,
                      yyscan_t yyscanner,
                      char const *msg) ;
%}

%type <integers>            dimension ;
%type <integers>            position ;
%type <p_floorplan_element> floorplan_element ;
%type <p_floorplan_element> floorplan_element_list ;

%destructor { free($$) ; } <string>

%token POSITION   "keyword position"
%token DIMENSION  "keyword dimension"

%token <integer> UIVALUE       "integer value"
%token <string>  IDENTIFIER    "identifier"

%require     "2.4.1"
%name-prefix "floorplan_"
%output      "Sources/floorplan_parser.c"

%pure-parser

%error-verbose

%parse-param { Floorplan *floorplan }
%parse-param { Dimensions *dim }
%parse-param { yyscan_t scanner }

%lex-param   { yyscan_t scanner }

%start floorplan_element_list

%%

floorplan_element_list

  : floorplan_element
    {
      int tmp_1 = check_location (floorplan, $1, &dim->Chip) ;
      int tmp_2 = align_to_grid  (floorplan, $1, dim) ;

      if (tmp_1 || tmp_2)
        {
          free_floorplan_element($1) ;
          YYABORT ;
        }

      floorplan->ElementsList = $1 ;
      floorplan->NElements  = 1 ;
      $$ = $1 ;
    }
  | floorplan_element_list floorplan_element
    {
      int tmp_1 = check_intersections (floorplan, $2) ;
      int tmp_2 = check_location      (floorplan, $2, &dim->Chip) ;
      int tmp_3 = align_to_grid       (floorplan, $2, dim) ;

      if (tmp_1 || tmp_2 || tmp_3 )
        {
          free_floorplan_element($2) ;
          floorplan_error (floorplan, dim, scanner, "") ;
          YYABORT ;
        }
      $1 -> Next = $2 ;

      floorplan->NElements++ ;

      $$ = $2 ;
    }
  ;

floorplan_element

  : IDENTIFIER ':' position dimension
    {
      FloorplanElement *floorplan_element
        = alloc_and_init_floorplan_element () ;

      if (floorplan_element == NULL)
      {
        perror ("alloc_floorplan_element") ;
        floorplan_error (floorplan, dim, scanner, "") ;
        YYABORT ;
      }

      floorplan_element->Id     = $1 ;
      floorplan_element->SW_X   = $3.a ;
      floorplan_element->SW_Y   = $3.b ;
      floorplan_element->Length = $4.a ;
      floorplan_element->Width  = $4.b ;

      $$ = floorplan_element ;
    }
  ;

position

  : POSITION UIVALUE ',' UIVALUE ';'
    {
      $$.a = $2 ;
      $$.b = $4 ;
    }
  ;

dimension

  : DIMENSION UIVALUE ',' UIVALUE ';'
    {
      $$.a = $2 ;
      $$.b  = $4 ;
    }
  ;

%%

void
floorplan_error (Floorplan *floorplan,
                 Dimensions *dim,
                 yyscan_t yyscanner,
                 char const *msg)
{
  fprintf(stderr, "%s:%d: %s\n",
    floorplan->FileName, floorplan_get_lineno(yyscanner), msg) ;

  free_dimensions (dim) ;
  free_floorplan  (floorplan) ;
}
