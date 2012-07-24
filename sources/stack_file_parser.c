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

#include "stack_file_parser.h"

#include "../bison/stack_description_parser.h"
#include "../flex/stack_description_scanner.h"

// From Bison manual:
// The value returned by yyparse is 0 if parsing was successful (return is
// due to end-of-input). The value is 1 if parsing failed (return is due to
// a syntax error).

extern int stack_description_parse
(
    StackDescription_t *stkd,
    Analysis_t         *analysis,
    Output_t           *output,
    yyscan_t            scanner
) ;

/******************************************************************************/

Error_t parse_stack_description_file
(
    String_t            filename,
    StackDescription_t *stkd,
    Analysis_t         *analysis,
    Output_t           *output
)
{
    FILE*    input ;
    int      result ;
    yyscan_t scanner ;

    input = fopen (filename, "r") ;
    if (input == NULL)
    {
        fprintf (stderr, "Unable to open stack file %s\n", filename) ;

        return TDICE_FAILURE ;
    }

    stkd->FileName = strdup (filename) ;  // FIXME memory leak

    stack_description_lex_init (&scanner) ;
    stack_description_set_in (input, scanner) ;

    result = stack_description_parse (stkd, analysis, output, scanner) ;

    stack_description_lex_destroy (scanner) ;
    fclose (input) ;

//  From Bison manual:
//  The value returned by yyparse is 0 if parsing was successful (return is
//  due to end-of-input). The value is 1 if parsing failed (return is due to
//  a syntax error).

    if (result == 0)

        return TDICE_SUCCESS ;

    else

        return TDICE_FAILURE ;
}

/******************************************************************************/

Error_t generate_stack_description_file
(
    String_t            filename,
    StackDescription_t *stkd,
    Analysis_t         *analysis,
    Output_t           *output
)
{
    FILE *out = fopen (filename, "w") ;

    if (out == NULL)
    {
        fprintf (stderr, "Unable to open stack file %s\n", filename) ;

        return TDICE_FAILURE ;
    }

    stack_description_print (stkd, out, (String_t) "") ;

    fprintf (out, "\n") ;

    analysis_print (analysis, out, (String_t) "") ;

    fprintf (out, "\n") ;

    output_print (output, out, (String_t) "") ;

    fclose (out) ;

    return TDICE_SUCCESS ;
}

/******************************************************************************/
