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

    /*! \struct PowersQueue_t
     *  \brief A First In - First Out circular queue to store power values
     */

    struct PowersQueue_t
    {
        /*! The number of power values that the queue can store */

        Quantity_t Capacity ;

        /*! Pointer to the memory that stores the power values */

        Power_t *Memory ;

        /*! The number of power values stored in the queue */

        Quantity_t Size ;

        /*! PlaceHolder to indicate the end of the queue */

        Quantity_t End;

        /*! PlaceHolder to indicate the begining of the queue */

        Quantity_t Start ;
    } ;

    /*! Definition of the type PowersQueue_t */

    typedef struct PowersQueue_t PowersQueue_t ;

/******************************************************************************/



    /*! Sets all the fields to a default value (zero or \c NULL ).
     *
     * \param this the address of the powers queue structure to initialize
     */

    void init_powers_queue (PowersQueue_t *this) ;



    /*! Reserves space to store power values
     *
     * The function deletes old memory, if any, calling \a destroy_powers_queue
     * on the parameter \a this.  Then, it reserves new memory.
     *
     * \param this the address of the power queue
     * \param capacity the new capacity of the power queue
     */

    void build_powers_queue (PowersQueue_t *this, Quantity_t capacity) ;



    /*! Releases the memory that stores power values and reset all the fields
     *  to a default value calling \a init_powers_queue
     *
     * \param this the address of the power queue
     */

    void destroy_powers_queue (PowersQueue_t *this) ;



    /*! Copies the queue \a src into the queue \a dest.
     *
     * All the power values previously stored in \a src get lost. After the copy
     * \a src will have a new capacity if its own was smaller than the capacity
     * of \a dst. The internal state of \a dst can be different from the state
     * of \a src but the queue \a dst will store the same power values as
     * \a src anyway. The function works also when \a dst has no storage.
     *
     * \param dst the left term of the assignement (destination)
     * \param src the right term of the assignement (source)
     */

    void copy_powers_queue (PowersQueue_t *dst, PowersQueue_t *src) ;



    /*! Allocates and inits memory for a structure of type PowersQueue_t
     *
     * The new power power queue will be empty and without memory for storage.
     *
     * \return a pointer to the allocated memory.
     * \return \c NULL in case of error
     */

    PowersQueue_t *calloc_powers_queue ( void ) ;



    /*! Frees the memory space pointed to by \a this
     *
     * The pointer \a this must have been returned by a previous call
     * to \a calloc_powers_queue . If \a this is \c NULL,
     * no operation is performed.
     *
     * \param this the address to free
     */

    void free_powers_queue (PowersQueue_t *this) ;



    /*! Prints the list of power values as it looks in the stack file
     *
     * \param this the powers queue to print
     * \param stream the output stream (must be already open)
     * \param prefix a string to be printed as prefix at the beginning of each line
     */

    void print_powers_queue

        (PowersQueue_t *this, FILE *stream, String_t prefix) ;



    /*! Returns the state of the powers queue
     *
     * \param this the powers queue
     *
     * \return \c TRUE if the queue is empty
     * \return \c FALSE otherwise
     */

    bool is_empty_powers_queue (PowersQueue_t *this) ;



    /*! Returns the state of the powers queue
     *
     * \param this the powers queue
     *
     * \return \c TRUE if the queue is full
     * \return \c FALSE otherwise
     */

    bool is_full_powers_queue (PowersQueue_t *this) ;



    /*! Inserts a power value at the end of the powers queue
     *
     * If the power queue \a this is full, the Capacity will be doubled.
     *
     * \param this the powers queue
     * \param power the power value to insert
     */

    void put_into_powers_queue (PowersQueue_t *this, Power_t power) ;



    /*! Returns and remve the power value at the beginning of a powers queue
     *
     * \param this the powers queue
     *
     * \return the first power value in \a this
     * \return \c 0 if the power queue \a this is empty
     */

    Power_t get_from_powers_queue (PowersQueue_t *this) ;

/******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* _3DICE_POWERSQUEUE_H_ */
