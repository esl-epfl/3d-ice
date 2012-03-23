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

#ifndef _3DICE_CONVENTIONAL_HEAT_SINK_H_
#define _3DICE_CONVENTIONAL_HEAT_SINK_H_

/*! \file conventional_heat_sink.h */

#ifdef __cplusplus
extern "C"
{
#endif

/******************************************************************************/

#include <stdio.h>

#include "types.h"

#include "dimensions.h"
#include "layer.h"
#include "system_matrix.h"
#include "thermal_cell.h"

/******************************************************************************/

     /*! \struct ConventionalHeatSink
     *
     *  \brief Structure used to store data about the conventional heat sink on
     *         top of the 2D/3D stack.
     */

    struct ConventionalHeatSink
    {
        /*! The heat transfert coefficient (from 3d stack to the environment) */

        AmbientHTC_t AmbientHTC ;

        /*! The temperarute of the environment in \f$ K \f$ */

        Temperature_t AmbientTemperature ;

        /*! Pointer to the top-most layer in the 3D stack */

        Layer *TopLayer ;
    } ;

    /*! Definition of the type ConventionalHeatSink */

    typedef struct ConventionalHeatSink ConventionalHeatSink ;

/******************************************************************************/



    /*! Sets all the fields of a conventional heat sink to a default value (zero or \c NULL ).
     *
     * \param conventional_heat_sink the address of the conventional heat sink element to initialize
     */

    void init_conventional_heat_sink (ConventionalHeatSink *conventional_heat_sink) ;



    /*! Allocates a conventional heat sink in memory and sets its fields to
     *  their default value with #init_conventional_heat_sink
     *
     * \return the pointer to a new ConventionalHeatSink
     * \return \c NULL if the memory allocation fails
     */

    ConventionalHeatSink *alloc_and_init_conventional_heat_sink (void) ;



    /*! Frees the memory related to a conventional heat sink
     *
     * The parametrer \a conventional_heat_sink must be a pointer previously
     * obtained with #alloc_and_init_conventional_heat_sink
     *
     * \param conventional_heat_sink the address of the conventional_heat_sink structure to free
     */

    void free_conventional_heat_sink (ConventionalHeatSink *conventional_heat_sink) ;



    /*! Prints a conventional heat sink as it looks in the stack file
     *
     * \param stream the output stream (must be already open)
     * \param prefix a string to be printed as prefix at the beginning of each line
     * \param conventional_heat_sink the pointer to the conventional heat sink to print
     */

    void print_formatted_conventional_heat_sink

        (FILE *stream, String_t prefix, ConventionalHeatSink *conventional_heat_sink) ;



    /*! Prints a list of detailed information about all the fields of the conventional heat sink
     *
     * \param stream the output stream (must be already open)
     * \param prefix a string to be printed as prefix at the beginning of each line
     * \param conventional_heat_sink the pointer to the conventional heat sink
     */

    void print_detailed_conventional_heat_sink

        (FILE *stream, String_t prefix, ConventionalHeatSink *conventional_heat_sink) ;



    /*! Applies the conventional heat sink to the grid of thermal cells
     *
     *  The function adapts the top-most layer of the 3d stack to connect it to
     *  the enviroment and to enable heat dissipation.
     *
     *  \param thermal_cells          pointer to the first thermal cell in the 3d grid
     *  \param dimensions             pointer to the structure storing the dimensions
     *  \param conventional_heat_sink pointer to the conventional heat sink structure
     */

    void fill_thermal_cell_conventional_heat_sink
    (
       ThermalCell          *thermal_cells,
       Dimensions           *dimensions,
       ConventionalHeatSink *conventional_heat_sink
    ) ;



    /*! Fills the source vector to enable heat dissipation through the heat sink
     *
     *  \param sources                pointer to the first element in the source vector
     *  \param thermal_cells          pointer to the first thermal cell in the 3d grid
     *  \param dimensions             pointer to the structure storing the dimensions
     *  \param conventional_heat_sink pointer to the conventional heat sink structure
     */

    void fill_sources_conventional_heat_sink
    (
        Source_t             *sources,
        ThermalCell          *thermal_cells,
        Dimensions           *dimensions,
        ConventionalHeatSink *conventional_heat_sink
    ) ;



    /*! Fills the system matrix
     *
     *  Add the conductance in the Top direction for every coefficient in the
     *  main diagonal of the matrix (only last layer). This allow heat to flow
     *  in the vertical direction.
     *
     *  \param system_matrix copy of the system matrix structure
     *  \param dimensions    pointer to the structure storing the dimensions
     *  \param thermal_cells pointer to the first thermal cell in the 3d grid
     */

    void fill_system_matrix_conventional_heat_sink
    (
        SystemMatrix  system_matrix,
        Dimensions   *dimensions,
        ThermalCell  *thermal_cells
    ) ;

/******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* _3DICE_CONVENTIONAL_HEAT_SINK_H_ */
