/******************************************************************************
 * This file is part of 3D-ICE, version 2.1 .                                 *
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

#ifndef _3DICE_OUTPUT_H_
#define _3DICE_OUTPUT_H_

/*! \file output.h */

#ifdef __cplusplus
extern "C"
{
#endif

/******************************************************************************/

#include <stdio.h>

#include "types.h"

#include "dimensions.h"
#include "inspection_point.h"
#include "network_message.h"

/******************************************************************************/

    /*! \struct Output_t
     *
     *  \brief Informations about the type of thermal simulation to be run and its initial settings
     */

    struct Output_t
    {
        /*! Pointer to the list of inspection points to print
         *  at the end of the simulation */

        InspectionPoint_t *InspectionPointListFinal ;

        /*! Pointer to the list of inspection points to print
         *  at the end of the time slot */

        InspectionPoint_t *InspectionPointListSlot ;

        /*! Pointer to the list of inspection points to print
         *  at every time step */

        InspectionPoint_t *InspectionPointListStep ;

    } ;

    /*! Definition of the type Output_t */

    typedef struct Output_t Output_t;

/******************************************************************************/



    /*! Sets all the fields to a default value (zero or \c NULL ).
     *
     * \param output the address of the output structure to initialize
     */

    void init_output (Output_t *output) ;



    /*! Frees the memory used by the output structure, i.e. releases the
     * memory used to store the inspection points.
     *
     * The parametrer \a output must be the address of a static variable
     *
     * \param output the address of the output structure to free
     */

    void destroy_output (Output_t *output) ;



    /*! Returns the number of a specific type of insection point
     *
     *  \param output     the address of the output structure to query
     *  \param instant  the instant of the output (slot, step, final)
     *  \param type     the type of the inspection point (tcell, tmap, ...)
     *  \param quantity the quantity to be measured (max, min, avg)
     *
     *  \return the number of inspection points recorded in \a output
     *  \return \c 0 if either \a instant or \a type are not known
     */

    Quantity_t get_number_of_inspection_points
    (
        Output_t         *output,
        OutputInstant_t   instant,
        OutputType_t      type,
        OutputQuantity_t  quantity
    ) ;



    /*! Prints the output informations as they look in the stack file
     *
     * \param output   the output information to print
     * \param stream the output stream (must be already open)
     * \param prefix a string to be printed as prefix at the beginning of each line
     */

    void print_output (Output_t *output, FILE *stream, String_t prefix) ;



    /*! Inserts an inspection point into the corresponding queue
     *
     * \param output             pointer to the output structure
     * \param inspection_point pointer to the inspection point to add
     */

    void add_inspection_point

        (Output_t *output, InspectionPoint_t *inspection_point) ;



    /*! Initializes output files for each inspection point
     *
     * Generates, for every inspection point, the output file and print the header
     * If the target output file is already there, it will be overwritten.
     *
     * \param output       pointer to the output structure
     * \param dimensions pointer to the structure containing the dimensions of the IC
     * \param prefix string to be printed as suffix for every line in the header
     *
     * \return \c TDICE_SUCCESS if the operation terminates with success
     * \return \c TDICE_FAILURE if one of the files can not be created
     */

    Error_t generate_output_headers

        (Output_t *output, Dimensions_t *dimensions, String_t prefix) ;



    /*! Generates thermal outputs for each inspection point
     *
     * \param output       pointer to the output structure
     * \param dimensions pointer to the structure containing the dimensions of the IC
     * \param temperatures pointer to the first element of the temparature array
     * \param sources      pointer to the first element of the source array
     * \param current_time the time instant at which the output is printed
     * \param output_instant the instant of the output (slot, step, final)
     *
     * \return \c TDICE_SUCCESS if the operation terminates with success
     * \return \c TDICE_FAILURE if one of the output cannot be generated
     */

    Error_t generate_output
    (
        Output_t        *output,
        Dimensions_t    *dimensions,
        Temperature_t   *temperatures,
        Source_t        *sources,
        Time_t           current_time,
        OutputInstant_t  output_instant
    ) ;



    /*! Fills a network message with thermal outputs for a specific
     *  set of inspection points
     *
     * \param output       pointer to the output structure
     * \param dimensions pointer to the structure containing the dimensions of the IC
     * \param temperatures pointer to the first element of the temparature array
     * \param sources      pointer to the first element of the source array
     * \param output_instant the instant of the output (slot, step, final)
     * \param output_type the type of the ispection point to generate
     * \param output_quantity the quantity to report (max, min, avg)
     * \param message the message to fill
     *
     * \return \c TDICE_SUCCESS if the operation terminates with success
     * \return \c TDICE_FAILURE if one of the output cannot be generated
     */

    Error_t fill_output_message
    (
        Output_t         *output,
        Dimensions_t     *dimensions,
        Temperature_t    *temperatures,
        Source_t         *sources,
        OutputInstant_t   output_instant,
        OutputType_t      output_type,
        OutputQuantity_t  output_quantity,
        NetworkMessage_t *message
    ) ;

/******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* _3DICE_OUTPUT_H_ */
