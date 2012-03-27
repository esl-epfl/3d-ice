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

#ifndef _3DICE_POWERSQUEUE_H_
#define _3DICE_POWERSQUEUE_H_

/*! \file powers_queue.h */

#ifdef __cplusplus
extern "C"
{
#endif

/******************************************************************************/

#include <stdio.h>

#include "types.h"

/******************************************************************************/

    /*! \struct PowerNode_t
     *  \brief A node of a linked list to store a power value
     */

    struct PowerNode_t
    {
        /*! The power value */

        Power_t Value ;

        /*! Pointer to the following power node */

        struct PowerNode_t *Next ;
    } ;

    /*! Definition of the type PowerNode_t */

    typedef struct PowerNode_t PowerNode_t ;

/******************************************************************************/

    /*! \struct PowersQueue_t
     *  \brief A First In - First Out list to store power values
     */

    struct PowersQueue_t
    {
        /*! Pointer to the first power value in the list */

        PowerNode_t *Head ;

        /*! Pointer to the last power value in the list */

        PowerNode_t *Tail ;

        /*! The number of power values in the list */

        Quantity_t Length ;

    } ;

    /*! Definition of the type PowersQueue_t */

    typedef struct PowersQueue_t PowersQueue_t ;

/******************************************************************************/



    /*! Sets all the fields of \a powers_queue to a default value (zero or \c NULL ).
     *
     * \param powers_queue the address of the power queue to initialize
     */

    void init_powers_queue (PowersQueue_t *powers_queue) ;



    /*! Allocates a PowersQueue in memory and sets its fields to their default
     *  value with #init_powers_queue
     *
     * \return the pointer to a new PowersQueue
     * \return \c NULL if the memory allocation fails
     */

    PowersQueue_t *alloc_and_init_powers_queue (void) ;



    /*! Frees the memory related to \a powers_queue
     *
     * The parametrer \a powers_queue must be a pointer previously obtained with
     * #alloc_and_init_powers_queue
     *
     * \param powers_queue the address of the powers queue structure to free
     */

    void free_powers_queue (PowersQueue_t *powers_queue) ;



    /*! Prints detailed information about all the fields of a power queue
     *
     * \param stream the output stream (must be already open)
     * \param prefix a string to be printed as prefix at the beginning of each line
     * \param powers_queue the powers queue to print
     */

    void print_detailed_powers_queue

        (FILE *stream, String_t prefix, PowersQueue_t *powers_queue) ;



    /*! Prints the list of power values as it looks in the stack file
     *
     * \param stream the output stream (must be already open)
     * \param powers_queue the powers queue to print
     */

    void print_formatted_powers_queue

        (FILE *stream, PowersQueue_t *powers_queue) ;



    /*! Returns the state of the powers queue
     *
     * \param powers_queue the powers queue
     *
     * \return \c TRUE if the queue is empty
     * \return \c FALSE otherwise
     */

    bool is_empty_powers_queue (PowersQueue_t *powers_queue) ;



    /*! Inserts a power value at the end of the powers queue
     *
     * \param powers_queue the powers queue
     * \param power the power value to insert
     */

    void put_into_powers_queue (PowersQueue_t *powers_queue, Power_t power) ;



    /*! Returns the power value at the beginning of a powers queue
     *
     * \param powers_queue the powers queue
     *
     * \return the first power value in the list \a powers_queue
     */

    Power_t get_from_powers_queue (PowersQueue_t *powers_queue) ;



    /*! Removes the power value at the beginning of a powers queue
     *
     * \param powers_queue the powers queue
     */

    void pop_from_powers_queue (PowersQueue_t *powers_queue) ;

/******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* _3DICE_POWERSQUEUE_H_ */
