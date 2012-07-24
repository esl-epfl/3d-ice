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

        /*! Number of steps simulated */

        Quantity_t CurrentTime ;

        /*! Initial Temperature if the IC stack */

        Temperature_t InitialTemperature ;
    } ;

    /*! Definition of the type Analysis_t */

    typedef struct Analysis_t Analysis_t;



/******************************************************************************/



    /*! Inits the fields of the \a analysis structure with default values
     *
     * \param analysis the address of the structure to initalize
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



    /*! Destroys the content of the fields of the structure \a analysis
     *
     * The function releases any dynamic memory used by the structure and
     * resets its state calling \a analysis_init .
     *
     * \param analysis the address of the structure to destroy
     */

    void analysis_destroy (Analysis_t *analysis) ;



    /*! Allocates memory for a structure of type Analysis_t
     *
     * The content of the new structure is set to default values
     * calling \a analysis_init
     *
     * \return the pointer to the new structure
     * \return \c NULL if the memory allocation fails
     */

    Analysis_t *analysis_calloc ( void ) ;



    /*! Allocates memory for a new copy of the structure \a analysis
     *
     * \param analysis the address of the structure to clone
     *
     * \return a pointer to a new structure
     * \return \c NULL if the memory allocation fails
     * \return \c NULL if the parameter \a analysis is \c NULL
     */

    Analysis_t *analysis_clone (Analysis_t *analysis) ;



    /*! Frees the memory space pointed by \a analysis
     *
     * The function destroys the structure \a analysis and then frees
     * its memory. The pointer \a analysis must have been returned by
     * a previous call to \a analysis_calloc or \a analysis_clone .
     *
     * If \a analysis is \c NULL, no operation is performed.
     *
     * \param analysis the pointer to free
     */

    void analysis_free (Analysis_t *analysis) ;



    /*! Prints the analysis declaration as it looks in the stack file
     *
     * \param analysis the address of the structure to print
     * \param stream the output stream (must be already open)
     * \param prefix a string to be printed as prefix at the
     *               beginning of each line
     */

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
