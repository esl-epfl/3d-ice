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



    /*! Inits the fields of the \a analysis structure with default values
     *
     * \param anlysis the address of the structure to initalize
     */

    void analysis_init (Analysis_t *analysis) ;



    /*! Copies the structure \a src into \a dst , as an assignement
     *
     * The function destroys the content of \a dst and then makes the copy
     *
     * \param dst the address of the left term sructure (destination)
     * \param src the address of the right term structure (source)
     */

    void analysis_copy (Analysis_t *dst, Analysis_t *src) ;



    /*! Destroys the content of the fields of the structure \a anaysis
     *
     * The function releases any dynamic memory used by the structure and
     * resets its state calling \a analysis_init .
     *
     * \param anlysis the address of the structure to destroy
     */

    void analysis_destroy (Analysis_t *analysis) ;

    Analysis_t *analysis_calloc ( void ) ;

    Analysis_t *analysis_clone (Analysis_t *analysis) ;

    void analysis_free (Analysis_t *analysis) ;

    void analysis_print (Analysis_t *analysis, FILE *stream, String_t prefix) ;


    /*! Returns the time, in seconds, sum of the time steps emulated so far
     *
     * \param analysis the address of the analysis structure
     * \return the simulated amount of time in seconds
     */

    Time_t get_simulated_time (Analysis_t *analysis) ;



    /*! Increase the simulation time by a step
     *
     * \param analysis the address of the analysis structure
     */

    void increase_by_step_time (Analysis_t *analysis) ;



    /*! Returns the state of the slot simulation
     *
     * \param analysis the address of the analysis structure
     *
     * \return \c TRUE  if the number of simulation steps done so far matches a
     *                  multiple of the slot length, i.e. it is time to update
     *                  the source vector with new power values
     * \return \c FALSE otherwise
     */

    bool slot_completed (Analysis_t *analysis) ;


/******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* _3DICE_ANALYSIS_H_ */
