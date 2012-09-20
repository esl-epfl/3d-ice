/******************************************************************************
 * This file is part of 3D-ICE, version 2.2.3 .                               *
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

#ifndef _3DICE_STACK_FILE_PARSER_H_
#define _3DICE_STACK_FILE_PARSER_H_

/*! \file stack_file_parser.h */

#ifdef __cplusplus
extern "C"
{
#endif

#include "types.h"
#include "stack_description.h"
#include "analysis.h"
#include "output.h"

/******************************************************************************/


    /*! Fills the StackDescription Analysis structures with the content
     *  taken from a stack description file
     *
     * \param filename the path of the stack file to parse
     * \param stkd     the address of the StackDescription structure to fill
     * \param analysis the address of the Analysis structure to fill
     * \param output   the address of the Output structure to fill
     *
     * \return \c TDICE_FAILURE if the file cannot be opened or if the parsing
     *                  of the stack description fails
     * \return \c TDICE_SUCCESS otherwise
     */

    Error_t parse_stack_description_file
    (
        String_t            filename,
        StackDescription_t *stkd,
        Analysis_t         *analysis,
        Output_t           *output
    ) ;



    /*! Generates a stack description file
     *
     * \param filename the path of the stack file to generate
     * \param stkd     the address of the StackDescription structure
     * \param analysis the address of the Analysis structure
     * \param output   the address of the Output structure
     *
     * \return \c TDICE_FAILURE if the file cannot be created
     * \return \c TDICE_SUCCESS otherwise
     */

    Error_t generate_stack_description_file
    (
        String_t            filename,
        StackDescription_t *stkd,
        Analysis_t         *analysis,
        Output_t           *output
    ) ;

/******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* _3DICE_STACK_FILE_PARSER_H_ */
