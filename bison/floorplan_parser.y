/******************************************************************************
 * This file is part of 3D-ICE, version 2.0 .                                 *
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
 *          Giseong Bak                                                       *
 *          Martino Ruggiero                                                  *
 *          Thomas Brunschwiler                                               *
 *          David Atienza                                                     *
 *                                                                            *
 * For any comment, suggestion or request  about 3D-ICE, please  register and *
 * write to the mailing list (see http://listes.epfl.ch/doc.cgi?liste=3d-ice) *
 * Any usage  of 3D-ICE  for research,  commercial or other  purposes must be *
 * properly acknowledged in the resulting products or publications.           *
 *                                                                            *
 * EPFL-STI-IEL-ESL                                                           *
 * Batiment ELG, ELG 130                Mail : 3d-ice@listes.epfl.ch          *
 * Station 11                                  (SUBSCRIPTION IS NECESSARY)    *
 * 1015 Lausanne, Switzerland           Url  : http://esl.epfl.ch/3d-ice.html *
 ******************************************************************************/

%code requires
{
    #include "types.h"
    #include "floorplan_element.h"
    #include "powers_queue.h"
}

%union
{
    Power_t           power_value ;
    String_t          identifier ;
    FloorplanElement *p_floorplan_element ;
    PowersQueue      *p_powers_queue ;
}

%code
{
    #include "dimensions.h"
    #include "floorplan.h"
    #include "macros.h"

    #include "../flex/floorplan_scanner.h"

    void floorplan_error

        (Floorplan *floorplan, Dimensions *dimensions,
         yyscan_t yyscanner, String_t msg) ;

    static char error_message [100] ;

    static bool local_abort ;
}

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

%initial-action
{
    local_abort = false ;
} ;

%start floorplan_file

%%

floorplan_file

  : floorplan_element_list
    {
        if (local_abort == true)
        {
            FREE_POINTER (free_floorplan, floorplan) ;

            YYABORT ;
        }
    }
  ;

/******************************************************************************/
/************************* List of floorplan elements *************************/
/******************************************************************************/

floorplan_element_list

  : floorplan_element             // $1 : pointer to the first floorplan element found
    {
        if (check_location (dimensions, $1) == true)
        {
            sprintf (error_message, "Floorplan element %s is outside of the IC", $1->Id) ;

            floorplan_error (floorplan, dimensions, scanner, error_message) ;

            local_abort = true ;
        }

        align_to_grid  (dimensions, $1) ;

        floorplan->ElementsList = $1 ;
        floorplan->NElements    = 1 ;

        $$ = $1 ;                 // $1 will be the new last element in the list
    }
  | floorplan_element_list floorplan_element // $1 : pointer to the last element in the list
                                             // $2 : pointer to the element to add in the list
    {
        if (find_floorplan_element_in_list(floorplan->ElementsList, $2->Id) != NULL)
        {
            sprintf (error_message, "Floorplan element %s already declared", $2->Id) ;

            floorplan_error (floorplan, dimensions, scanner, error_message) ;

            local_abort = true ;
        }

        if (check_location (dimensions, $2) == true)
        {
            sprintf (error_message, "Floorplan element %s is outside of the IC", $2->Id) ;

            floorplan_error (floorplan, dimensions, scanner, error_message) ;

            local_abort = true ;
        }

        FloorplanElement *flp_el = floorplan->ElementsList ;

        do
        {
            flp_el = find_intersection_in_list (flp_el, $2) ;

            if (flp_el != NULL)
            {
                sprintf (error_message, "Found intersection between %s and %s", $2->Id, flp_el->Id) ;

                floorplan_error (floorplan, dimensions, scanner, error_message) ;

                flp_el = flp_el->Next ;

                local_abort = true ;
            }

        }   while (flp_el != NULL) ;

        align_to_grid (dimensions, $2) ;

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
    Dimensions *__attribute__ ((unused)) dimensions,
    yyscan_t    yyscanner,
    String_t    msg
)
{
    fprintf (stderr, "%s:%d: %s\n",
             floorplan->FileName, floorplan_get_lineno(yyscanner), msg) ;
}
