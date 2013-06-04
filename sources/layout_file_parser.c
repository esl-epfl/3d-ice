/******************************************************************************
 * This file is part of 3D-ICE, version 2.2.4 .                               *
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

#include <stdio.h> // For the file type FILE

#include "layout_file_parser.h"

#include "../bison/layout_parser.h"
#include "../flex/layout_scanner.h"

//  From Bison manual:
//  The value returned by yyparse is 0 if parsing was successful (return is
//  due to end-of-input). The value is 1 if parsing failed (return is due to
//  a syntax error).

extern int layout_parser_parse

    (Layer_t        *layer,     Dimensions_t *dimensions,
     MaterialList_t *materials, yyscan_t      scanner) ;

/******************************************************************************/

Error_t parse_layout_file
(
    String_t        filename,
    Layer_t        *layer,
    MaterialList_t *materials,
    Dimensions_t   *dimensions
)
{
    FILE *input ;
    int result ;
    yyscan_t scanner ;

    input = fopen (filename, "r") ;

    if (input == NULL)
    {
        fprintf (stderr, "Unable to open layout file %s\n", filename) ;

        return TDICE_FAILURE ;
    }

    string_copy (&layer->LayoutFileName, &filename) ;  // FIXME memory leak

    layout_parser_lex_init  (&scanner) ;
    layout_parser_set_in    (input, scanner) ;
    //layout_set_debug (1, scanner) ;

    result = layout_parser_parse (layer, dimensions, materials, scanner) ;

    layout_parser_lex_destroy (scanner) ;
    fclose (input) ;

    if (result == 0)

        return TDICE_SUCCESS ;

    else

        return TDICE_FAILURE ;
}

/******************************************************************************/

