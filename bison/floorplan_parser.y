/******************************************************************************
 * This file is part of 3D-ICE, version 1.0.2 .                               *
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
 * with 3D-ICE. If not, see <http://www.gnu.org/licenses/>.                   *
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
 * write to the mailing list (see http://listes.epfl.ch/doc.cgi?liste=3d-ice) *
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
#include "macros.h"
%}

%union
{
  Power_t  power_value ;
  String_t identifier ;

  FloorplanElement  *p_floorplan_element ;
  PowersQueue       *p_powers_queue ;
}

%{
#include "../flex/floorplan_scanner.h"

void floorplan_error
(
   Floorplan*  floorplan,
   Dimensions *dimensions,
   yyscan_t    yyscanner,
   char const *msg
) ;

static char error_message [100] ;
%}

%type <p_floorplan_element> floorplan_element ;
%type <p_floorplan_element> floorplan_element_list ;
%type <p_powers_queue>      power_values_list ;

%destructor { FREE_POINTER (free, $$) ; } <identifier>

%token POSITION   "keyword position"
%token DIMENSION  "keyword dimension"
%token POWER      "keyword power"
%token VALUES     "keyword values"

%token <power_value> DVALUE     "double value"
%token <identifier>  IDENTIFIER "identifier"

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

/******************************************************************************/
/************************* List of floorplan elements *************************/
/******************************************************************************/

floorplan_element_list

  : floorplan_element             // $1 : pointer to the first floorplan element found
    {
        Bool_t tmp_1 = check_location (floorplan, $1, dimensions) ;
        Bool_t tmp_2 = align_to_grid  (floorplan, $1, dimensions) ;

        if (tmp_1 || tmp_2)
        {
            FREE_POINTER (free_floorplan_element, $1) ;

            floorplan_error (floorplan, dimensions, scanner, "") ;

            YYABORT ;
        }

        floorplan->ElementsList = $1 ;
        floorplan->NElements    = 1 ;

        $$ = $1 ;                 // $1 will be the new last element in the list
    }
  | floorplan_element_list floorplan_element // $1 : pointer to the last element in the list
                                             // $2 : pointer to the element to add in the list
    {
        if (find_floorplan_element_in_list($1, $2->Id) != NULL)
        {
            sprintf (error_message, "Floorplan element %s already declared", $2->Id) ;

            floorplan_error (floorplan, dimensions, scanner, error_message) ;

            FREE_POINTER (free_floorplan_element, $2) ;

            YYABORT ;
        }


        Bool_t tmp_1 = check_intersections (floorplan, $2) ;
        Bool_t tmp_2 = check_location      (floorplan, $2, dimensions) ;
        Bool_t tmp_3 = align_to_grid       (floorplan, $2, dimensions) ;

        if (tmp_1 || tmp_2 || tmp_3 )
        {
            FREE_POINTER (free_floorplan_element, $2) ;

            floorplan_error (floorplan, dimensions, scanner, "") ;

            YYABORT ;
        }

        floorplan->NElements++ ;

        $1->Next = $2 ;           // insert $2 at the end of the list
        $$ = $2 ;                 // $2 will be the new last element in the list
    }
  ;

/******************************************************************************/
/************************* Floorplan element **********************************/
/******************************************************************************/

floorplan_element

  : IDENTIFIER ':'                             // $1
      POSITION  DVALUE ',' DVALUE ';'          // $4 $6
      DIMENSION DVALUE ',' DVALUE ';'          // $9 $11
      POWER VALUES power_values_list ';'       // $15
    {
        FloorplanElement *floorplan_element = $$ = alloc_and_init_floorplan_element ( ) ;

        if (floorplan_element == NULL)
        {
            FREE_POINTER (free, $1) ;

            floorplan_error (floorplan, dimensions, scanner, "Malloc floorplan element failed") ;

            YYABORT ;
        }

        floorplan_element->Id          = $1 ;
        floorplan_element->SW_X        = $4 ;
        floorplan_element->SW_Y        = $6 ;
        floorplan_element->Length      = $9 ;
        floorplan_element->Width       = $11 ;
        floorplan_element->PowerValues = $15 ;

    }
  ;

/******************************************************************************/
/************************* List of power values *******************************/
/******************************************************************************/

power_values_list

  : DVALUE              // $1
                        // There must be at least one power value
    {
        PowersQueue* powers_list = $$ = alloc_and_init_powers_queue() ;

        if (powers_list == NULL)
        {
            floorplan_error (floorplan, dimensions, scanner, "Malloc power list failed") ;

            YYABORT ;
        }

        put_into_powers_queue (powers_list, $1) ;
    }

  | power_values_list ',' DVALUE         // $1 the power list so far ...
                                         // $3 the poer value to add
    {
        put_into_powers_queue ($1, $3) ;

        $$ = $1 ;
    }
  ;

%%

void floorplan_error
(
    Floorplan  *floorplan,
    Dimensions* __attribute__ ((unused)) dimensions,
    yyscan_t   yyscanner,
    char const *msg
)
{
    fprintf (stderr, "%s:%d: %s\n",
             floorplan->FileName, floorplan_get_lineno(yyscanner), msg) ;
}
