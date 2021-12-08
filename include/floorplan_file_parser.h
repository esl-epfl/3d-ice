/******************************************************************************
 * This file is part of 3D-ICE, version 3.1.0 .                               *
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
 *                             Copyright (C) 2021                             *
 *   Embedded Systems Laboratory - Ecole Polytechnique Federale de Lausanne   *
 *                            All Rights Reserved.                            *
 *                                                                            *
 * Authors: Arvind Sridhar              Alessandro Vincenzi                   *
 *          Giseong Bak                 Martino Ruggiero                      *
 *          Thomas Brunschwiler         Eder Zulian                           *
 *          Federico Terraneo           Darong Huang                          *
 *          Luis Costero                Marina Zapater                        *
 *          David Atienza                                                     *
 *                                                                            *
 * For any comment, suggestion or request  about 3D-ICE, please  register and *
 * write to the mailing list (see http://listes.epfl.ch/doc.cgi?liste=3d-ice) *
 * Any usage  of 3D-ICE  for research,  commercial or other  purposes must be *
 * properly acknowledged in the resulting products or publications.           *
 *                                                                            *
 * EPFL-STI-IEL-ESL                     Mail : 3d-ice@listes.epfl.ch          *
 * Batiment ELG, ELG 130                       (SUBSCRIPTION IS NECESSARY)    *
 * Station 11                                                                 *
 * 1015 Lausanne, Switzerland           Url  : http://esl.epfl.ch/3d-ice      *
 ******************************************************************************/

#ifndef _3DICE_FLOORPLAN_FILE_PARSER_H_
#define _3DICE_FLOORPLAN_FILE_PARSER_H_

/*! \file floorplan_file_parser.h */

#ifdef __cplusplus
extern "C"
{
#endif

#include "types.h"
#include "string_t.h"

#include "floorplan.h"
#include "dimensions.h"

/******************************************************************************/

    /*! Fills the Floorplan structure with the content taken from a
     *  floorplan file
     *
     * \param filename   path to the floorplan file to parse
     * \param floorplan  address of the Floorplan structure to fill
     * \param dimensions pointer to the structure storing the dimensions
     *                   of the stack where the floorplan is used
     *
     * \return \c TDICE_FAILURE if the file cannot be opened or if the parsing
     *                  of the floorplan fails
     * \return \c TDICE_FAILURE otherwise
     */

    Error_t parse_floorplan_file

        (String_t filename, Floorplan_t *floorplan, Dimensions_t *dimensions) ;



    /*! Generates a floorplan file
     *
     * \param filename  the path of the stack file to generate
     * \param floorplan the floorplan to print
     *
     * \return \c TDICE_FAILURE if the file cannot be created
     * \return \c TDICE_SUCCESS otherwise
     */

    Error_t generate_floorplan_file

        (String_t filename, Floorplan_t *floorplan) ;

/******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* _3DICE_FLOORPLAN_FILE_PARSER_H_ */
