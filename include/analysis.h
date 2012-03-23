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

#ifndef _3DICE_ANALYSIS_H_
#define _3DICE_ANALYSIS_H_

/*! \file analysis.h */

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

    /*! \struct Analysis
     *
     *  \brief Informations about the type of thermal simulation to be run and its initial settings
     */

    struct Analysis
    {
        /*! The analysis type */

        AnalysisType_t AnalysisType ;

        /*! The stepping time for Transient Analysis */

        Time_t StepTime ;

        /*! The slot time for Transient Analysis */

        Time_t SlotTime ;

        /*! Number of steps to do to complete a slot */

        Quantity_t SlotLength ;

        /*! Number of steps simulated so far ... */

        Quantity_t CurrentTime ;

        /*! Initial Temperature if the IC stack */

        Temperature_t InitialTemperature ;

        /*! Pointer to the list of inspection points to print
         *  at the end of the simulation */

        InspectionPoint *InspectionPointListFinal ;

        /*! Pointer to the list of inspection points to print
         *  at the end of the time slot */

        InspectionPoint *InspectionPointListSlot ;

        /*! Pointer to the list of inspection points to print
         *  at every time step */

        InspectionPoint *InspectionPointListStep ;

    } ;

    /*! Definition of the type Analysis */

    typedef struct Analysis Analysis ;

/******************************************************************************/



    /*! Sets all the fields of \a analysis to a default value (zero or \c NULL ).
     *
     * \param analysis the address of the structure to initialize
     */

    void init_analysis (Analysis *analysis) ;



    /*! Allocates a structure Analysis in memory and sets its fields to
     *  their default value with #init_analysis
     *
     * \return the pointer to a new Analysis
     * \return \c NULL if the memory allocation fails
     */

    Analysis *alloc_and_init_analysis (void) ;



    /*! Frees the memory related to \a analysis
     *
     *  \a analysis must be the address of an automatic variable
     *
     * \param analysis the address of the structure to free
     */

    void free_analysis (Analysis *analysis) ;



    /*! Returns the time, in seconds, sum of the time steps emulated so far
     *
     * \param analysis the address of the analysis structure
     * \return the simulated amount of time in seconds
     */

    Time_t get_simulated_time (Analysis *analysis) ;



    /*! Returns the number of a specific type of insection point
     *
     *  \param analysis the address of the analysis structure to query
     *  \param instant  the instant of the output (slot, step, final)
     *  \param type     the type of the inspection point (tcell, tmap, ...)
     *
     *  \return the number of inspection points recorded in \a analysis
     *  \return \c 0 if either \a instant or \a type are not known
     */

    Quantity_t get_number_of_inspection_points

        (Analysis *analysis, OutputInstant_t instant, OutputType_t type) ;



    /*! Returns the state of the slot simulation
     *
     * \param analysis the address of the analysis structure
     *
     * \return \c TRUE  if the number of simulation steps done so far matches a
     *                  multiple of the slot length, i.e. it is time to update
     *                  the source vector with new power values
     * \return \c FALSE otherwise
     */

    bool slot_completed (Analysis *analysis) ;



    /*! Prints the analysis informations as they look in the stack file
     *
     * \param stream   the output stream (must be already open)
     * \param prefix   a string to be printed as prefix at the beginning of each line
     * \param analysis the analysis information to print
     */

    void print_formatted_analysis

        (FILE *stream, String_t prefix, Analysis *analysis) ;



    /*! Prints detailed information about all the fields of analysis
     *
     * \param stream   the output stream (must be already open)
     * \param prefix   a string to be printed as prefix at the beginning of each line
     * \param analysis the analysis information to print
     */

    void print_detailed_analysis

        (FILE *stream, String_t prefix, Analysis *analysis) ;



    /*! Inserts an inspection point into the corresponding queue
     *
     * \param analysis     pointer to the analysis structure
     * \param inspection_point pointer to the inspection point to add
     */

    void add_inspection_point_to_analysis

        (Analysis *analysis, InspectionPoint *inspection_point) ;



    /*! Initializes output files for each inspection point
     *
     * Generates, for every inspection point, the output file and print the header
     * If the target output file is already there, it will be overwritten.
     *
     * \param analysis pointer to the analysis structure
     * \param dimensions pointer to the structure containing the dimensions of the IC
     * \param prefix string to be printed as suffix for every line in the header
     *
     * \return \c TDICE_SUCCESS if the operation terminates with success
     * \return \c TDICE_FAILURE if one of the files can not be created
     */

    Error_t generate_analysis_headers

        (Analysis* analysis, Dimensions *dimensions, String_t prefix) ;



    /*! Generates thermal outputs for each inspection point
     *
     * \param analysis pointer to the analysis structure
     * \param dimensions pointer to the structure containing the dimensions of the IC
     * \param temperatures pointer to the first element of the temparature array
     * \param output_instant the instant of the output (slot, step, final)
     *
     * \return \c TDICE_SUCCESS if the operation terminates with success
     * \return \c TDICE_FAILURE if one of the output cannot be generated
     */

    Error_t generate_analysis_output
    (
        Analysis        *analysis,
        Dimensions      *dimensions,
        Temperature_t   *temperatures,
        OutputInstant_t  output_instant
    ) ;



    /*! Fills a network message with thermal outputs for a specific set of inspection points
     *
     * \param analysis pointer to the analysis structure
     * \param dimensions pointer to the structure containing the dimensions of the IC
     * \param temperatures pointer to the first element of the temparature array
     * \param output_instant the instant of the output (slot, step, final)
     * \param type the type of the ispection point to generate
     * \param message the message to fill
     *
     * \return \c TDICE_SUCCESS if the operation terminates with success
     * \return \c TDICE_FAILURE if one of the output cannot be generated
     */

    Error_t fill_analysis_message
    (
        Analysis        *analysis,
        Dimensions      *dimensions,
        Temperature_t   *temperatures,
        OutputInstant_t  output_instant,
        OutputType_t     type,
        NetworkMessage  *message
    ) ;

/******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* _3DICE_CHANNEL_H_ */
