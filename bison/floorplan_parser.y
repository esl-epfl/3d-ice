/******************************************************************************
 * This file is part of 3D-ICE, version 1.0 .                                 *
 *                                                                            *
 * 3D-ICE is free software: you can  redistribute it and/or  modify it  under *
 * the terms of the  GNU General  Public  License as  published by  the  Free *
 * Software  Foundation, either  version  3  of  the License,  or  any  later *
 * version.                                                                   *
 *                                                                            *
 * 3D-ICE is  distributed  in the hope  that it will  be useful, but  WITHOUT *
 * ANY  WARRANTY; without  even the  implied warranty  of MERCHANTABILITY  or *
 * FITNESS  FOR A PARTICULAR  PURPOSE. See the GNU General Public License for *
 * more details.                                                              *
 *                                                                            *
 * You should have  received a copy of  the GNU General  Public License along *
 * with 3D-ICe. If not, see <http://www.gnu.org/licenses/>.                   *
 *                                                                            *
 *                             Copyright (C) 2010                             *
 *   Embedded Systems Laboratory - Ecole Polytechnique Federale de Lausanne   *
 *                            All Rights Reserved.                            *
 *                                                                            *
 * Authors: Arvind Sridhar                                                    *
 *          Alessandro Vincenzi                                               *
 *          Martino Ruggiero                                                  *
 *          Thomas Brunschwiler                                               *
 *          David Atienza                                                     *
 *                                                                            *
 * For any comment, suggestion or request  about 3D-ICE, please  register and *
 * write to the mailing list (see http://listes.epfl.ch/doc.cgi?liste=3d-ice) *                                                                            *
 *                                                                            *
 * EPFL-STI-IEL-ESL                                                           *
 * Batiment ELG, ELG 130                Mail : 3d-ice@listes.epfl.ch          *
 * Station 11                                  (SUBSCRIPTION IS NECESSARY)    *
 * 1015 Lausanne, Switzerland           Url  : http://esl.epfl.ch/3d-ice.html *
 ******************************************************************************/

%{
#include "dimensions.h"
#include "floorplan.h"
#include "floorplan_element.h"
%}

%union
{
  double                   d_value ;
  char                     *string ;

  FloorplanElement  *p_floorplan_element ;
  PowersQueue       *p_powers_queue ;
}

%{
#include "../flex/floorplan_scanner.h"

void floorplan_error
(
  Floorplan*  floorplan,
  Dimensions* dimensions,
  yyscan_t    yyscanner,
  char const* msg
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

//%token <i_value> UIVALUE       "integer value"
%token <d_value> DVALUE        "double value"
%token <string>  IDENTIFIER    "identifier"

%name-prefix "floorplan_"
%output      "floorplan_parser.c"

%pure-parser

%error-verbose

%parse-param { Floorplan*  floorplan  }
%parse-param { Dimensions* dimensions }
%parse-param { yyscan_t    scanner    }

%lex-param   { yyscan_t scanner       }

%start floorplan_element_list

%%

floorplan_element_list

  : floorplan_element
    {
      Bool_t tmp_1 = check_location (floorplan, $1, dimensions) ;
      Bool_t tmp_2 = align_to_grid  (floorplan, $1, dimensions) ;

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
      if (find_floorplan_element_in_list($1, $2->Id) != NULL)
      {
        String_t message
          = (String_t) malloc ((37 + strlen($2->Id)) * sizeof (char)) ;
        sprintf (message, "Floorplan element %s already declared", $2->Id) ;

        floorplan_error (floorplan, dimensions, scanner, message) ;
        free_floorplan_element ($2) ;
        free (message) ;
        YYABORT ;
      }


      Bool_t tmp_1 = check_intersections (floorplan, $2) ;
      Bool_t tmp_2 = check_location      (floorplan, $2, dimensions) ;
      Bool_t tmp_3 = align_to_grid       (floorplan, $2, dimensions) ;

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
      POSITION  DVALUE ',' DVALUE ';'
      DIMENSION DVALUE ',' DVALUE ';'
      POWER VALUES power_values_list ';'
    {
      FloorplanElement *floorplan_element
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

        fprintf (stderr, "%s:%d: Warning: discarding value %f\n",
                 floorplan->FileName, floorplan_get_lineno(scanner), $3);

      else

        put_into_powers_queue ($1, $3) ;

      $$ = $1 ;
    }
  ;

%%

void
floorplan_error
(
  Floorplan  *floorplan,
  Dimensions* dimensions,
  yyscan_t   yyscanner,
  char const *msg
)
{
  fprintf (stderr, "%s:%d: %s\n", floorplan->FileName,
                                  floorplan_get_lineno(yyscanner),
                                  msg) ;

  get_chip_length (dimensions) ;  // FIXME
}
