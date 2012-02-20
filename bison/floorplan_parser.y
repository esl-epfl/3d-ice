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
    #include "ic_element.h"
    #include "powers_queue.h"
}

%union
{
    Power_t           power_value ;
    String_t          identifier ;
    ICElement        *p_icelement ;
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
%type <p_powers_queue>      optional_power_values_list ;
%type <p_powers_queue>      power_values_list ;
%type <p_icelement>         ic_element ;

%destructor { FREE_POINTER (free, $$) ; } <identifier>

%token DIMENSION  "keyword dimension"
%token POSITION   "keyword position"
%token POWER      "keyword power"
%token RECTANGLE  "keywork rectangle"
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
        if (check_location (dimensions, $1->MainElement) == true)
        {
            sprintf (error_message, "Floorplan element %s is outside of the IC", $1->Id) ;

            floorplan_error (floorplan, dimensions, scanner, error_message) ;

            local_abort = true ;
        }

        align_to_grid  (dimensions, $1->MainElement) ;

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

        if (check_location (dimensions, $2->MainElement) == true)
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

        align_to_grid (dimensions, $2->MainElement) ;

        floorplan->NElements++ ;

        $1->Next = $2 ;           // insert $2 at the end of the list
        $$ = $2 ;                 // $2 will be the new last element in the list
    }
  ;

/******************************************************************************/
/************************* Floorplan element **********************************/
/******************************************************************************/

floorplan_element

  : IDENTIFIER ':'                        // $1
      ic_element                          // $3
      optional_power_values_list          // $4
    {
        FloorplanElement *floorplan_element = $$ = alloc_and_init_floorplan_element ( ) ;

        if (floorplan_element == NULL)
        {
            FREE_POINTER (free, $1) ;

            floorplan_error (floorplan, dimensions, scanner, "Malloc floorplan element failed") ;

            YYABORT ;
        }

        floorplan_element->Id          = $1 ;
        floorplan_element->MainElement = $3 ;
        floorplan_element->PowerValues = $4 ;
    }
  ;

ic_element

  : POSITION  DVALUE ',' DVALUE ';'  // $2 $4
    DIMENSION DVALUE ',' DVALUE ';'  // $7 $9
    {
        ICElement *icelement = $$ = alloc_and_init_ic_element () ;

        if (icelement == NULL)
        {
            floorplan_error (floorplan, dimensions, scanner, "Malloc ic element failed") ;

            YYABORT ;
        }

        icelement->SW_X   = $2 ;
        icelement->SW_Y   = $4 ;
        icelement->Length = $7 ;
        icelement->Width  = $9 ;
    }
  | RECTANGLE '(' DVALUE ',' DVALUE ',' DVALUE ',' DVALUE ')' ';'  // $3 $5 $7 $9
    {
        ICElement *icelement = $$ = alloc_and_init_ic_element () ;

        if (icelement == NULL)
        {
            floorplan_error (floorplan, dimensions, scanner, "Malloc ic element failed") ;

            YYABORT ;
        }

        icelement->SW_X   = $3 ;
        icelement->SW_Y   = $5 ;
        icelement->Length = $7 ;
        icelement->Width  = $9 ;
    }
  ;

/******************************************************************************/
/************************* List of power values *******************************/
/******************************************************************************/

optional_power_values_list

  : // Declaring the entire subsection of power values is not mandatory

    {
        PowersQueue* powers_list = $$ = alloc_and_init_powers_queue() ;

        if (powers_list == NULL)
        {
            floorplan_error (floorplan, dimensions, scanner, "Malloc power list failed") ;

            YYABORT ;
        }
    }

  | POWER VALUES power_values_list ';' // $3

    {
        $$ = $3 ;
    }
  ;

power_values_list

  : DVALUE              // $1
                        // Here at least one power value is mandatory
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
