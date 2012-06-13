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

/******************************************************************************/

    /*! \struct Analysis_t
     *
     *  \brief Informations about the type of thermal simulation to be run,
     *         timing and its initial settings
     */

    struct Analysis_t
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
    } ;

    /*! Definition of the type Analysis_t */

    typedef struct Analysis_t Analysis_t;

/******************************************************************************/



    /*! Sets all the fields to a default value (zero or \c NULL ).
     *
     * \param this the address of the analysis structure to initialize
     */

    void init_analysis (Analysis_t *this) ;



    /*! Returns the time, in seconds, sum of the time steps emulated so far
     *
     * \param this the address of the analysis structure
     * \return the simulated amount of time in seconds
     */

    Time_t get_simulated_time (Analysis_t *this) ;



    /*! Increase the simulation time by a step
     *
     * \param this the address of the analysis structure
     */

    void increase_by_step_time (Analysis_t *this) ;



    /*! Returns the state of the slot simulation
     *
     * \param this the address of the analysis structure
     *
     * \return \c TRUE  if the number of simulation steps done so far matches a
     *                  multiple of the slot length, i.e. it is time to update
     *                  the source vector with new power values
     * \return \c FALSE otherwise
     */

    bool slot_completed (Analysis_t *this) ;



    /*! Prints the analysis informations as they look in the stack file
     *
     * \param this   the analysis information to print
     * \param stream the output stream (must be already open)
     * \param prefix a string to be printed as prefix at the beginning of each line
     */

    void print_formatted_analysis

        (Analysis_t *this, FILE *stream, String_t prefix) ;



    /*! Prints detailed information about all the fields of analysis
     *
     * \param this    the analysis information to print
     * \param stream  the output stream (must be already open)
     * \param prefix  a string to be printed as prefix at the beginning of each line
     */

    void print_detailed_analysis

        (Analysis_t *this, FILE *stream, String_t prefix) ;


/******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* _3DICE_ANALYSIS_H_ */
