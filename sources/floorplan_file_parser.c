/******************************************************************************
 * This file is part of 3D-ICE, version 2.2 .                                 *
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

#include "floorplan_file_parser.h"

#include "../bison/floorplan_parser.h"
#include "../flex/floorplan_scanner.h"

//  From Bison manual:
//  The value returned by yyparse is 0 if parsing was successful (return is
//  due to end-of-input). The value is 1 if parsing failed (return is due to
//  a syntax error).

extern int floorplan_parser_parse

    (Floorplan_t *floorplan, Dimensions_t *dimensions, yyscan_t scanner) ;

/******************************************************************************/

Error_t parse_floorplan_file
(
    String_t      filename,
    Floorplan_t  *floorplan,
    Dimensions_t *dimensions
)
{
    FILE *input ;
    int result ;
    yyscan_t scanner ;

    input = fopen (filename, "r") ;

    if (input == NULL)
    {
        fprintf (stderr, "Unable to open floorplan file %s\n", filename) ;

        return TDICE_FAILURE ;
    }

    floorplan->FileName = strdup (filename) ;  // FIXME memory leak

    floorplan_parser_lex_init  (&scanner) ;
    floorplan_parser_set_in    (input, scanner) ;
    //floorplan_set_debug (1, scanner) ;

    result = floorplan_parser_parse (floorplan, dimensions, scanner) ;

    floorplan_parser_lex_destroy (scanner) ;
    fclose (input) ;

    if (result == 0)

        return TDICE_SUCCESS ;

    else

        return TDICE_FAILURE ;
}

/******************************************************************************/

Error_t generate_floorplan_file
(
    String_t     filename,
    Floorplan_t *floorplan
)
{
    FILE *out = fopen (filename, "w") ;

    if (out == NULL)
    {
        fprintf (stderr, "Unable to open stack file %s\n", filename) ;

        return TDICE_FAILURE ;
    }

    floorplan_print (floorplan, out, (String_t) "") ;

    fclose (out) ;

    return TDICE_SUCCESS ;
}

/******************************************************************************/

