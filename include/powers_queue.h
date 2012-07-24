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



    /*! Inits the fields of the \a pqueue structure with default values
     *
     * \param pqueue the address of the structure to initalize
     */

    void powers_queue_init (PowersQueue_t *pqueue) ;



    /*! Copies the structure \a src into \a dst , as an assignement
     *
     * The function destroys the content of \a dst and then makes the copy.
     *
     * \param dst the address of the left term sructure (destination)
     * \param src the address of the right term structure (source)
     */

    void powers_queue_copy (PowersQueue_t *dst, PowersQueue_t *src) ;



    /*! Reserves space to store power values
     *
     * The function deletes old memory, if any, calling \a powers_queue_destroy
     * on the parameter \a pqueue.  Then, it reserves new memory.
     *
     * \param pqueue the address of the power queue
     * \param capacity the new capacity of the power queue
     */

    void powers_queue_build (PowersQueue_t *pqueue, Quantity_t capacity) ;



    /*! Destroys the content of the fields of the structure \a pqueue
     *
     * The function releases any dynamic memory used by the structure and
     * resets its state calling \a powers_queue_init .
     *
     * \param pqueue the address of the structure to destroy
     */

    void powers_queue_destroy (PowersQueue_t *pqueue) ;



    /*! Allocates memory for a structure of type PowersQueue_t
     *
     * The content of the new structure is set to default values
     * calling \a powers_queue_init
     *
     * \return the pointer to the new structure
     * \return \c NULL if the memory allocation fails
     */

    PowersQueue_t *powers_queue_calloc ( void ) ;



    /*! Allocates memory for a new copy of the structure \a pqueue
     *
     * \param pqueue the address of the structure to clone
     *
     * \return a pointer to a new structure
     * \return \c NULL if the memory allocation fails
     * \return \c NULL if the parameter \a pqueue is \c NULL
     */

    PowersQueue_t *powers_queue_clone (PowersQueue_t *pqueue) ;



    /*! Frees the memory space pointed by \a pqueue
     *
     * The function destroys the structure \a pqueue and then frees
     * its memory. The pointer \a pqueue must have been returned by
     * a previous call to \a powers_queue_calloc or \a powers_queue_clone .
     *
     * If \a pqueue is \c NULL, no operation is performed.
     *
     * \param pqueue the pointer to free
     */

    void powers_queue_free (PowersQueue_t *pqueue) ;



    /*! Prints the list of power values as it looks in the floorplan file
     *
     * \param pqueue the powers queue to print
     * \param stream the output stream (must be already open)
     * \param prefix a string to be printed as prefix at the beginning
     *               of each line
     */

    void powers_queue_print

        (PowersQueue_t *pqueue, FILE *stream, String_t prefix) ;



    /*! Returns the state of the powers queue
     *
     * \param pqueue the powers queue
     *
     * \return \c TRUE if the queue is empty
     * \return \c FALSE otherwise
     */

    bool is_empty_powers_queue (PowersQueue_t *pqueue) ;



    /*! Returns the state of the powers queue
     *
     * \param pqueue the powers queue
     *
     * \return \c TRUE if the queue is full
     * \return \c FALSE otherwise
     */

    bool is_full_powers_queue (PowersQueue_t *pqueue) ;



    /*! Inserts a power value at the end of the powers queue
     *
     * If the power queue \a pqueue is full, the Capacity will be doubled.
     *
     * \param pqueue the powers queue
     * \param power the power value to insert
     */

    void put_into_powers_queue (PowersQueue_t *pqueue, Power_t power) ;



    /*! Returns and remve the power value at the beginning of a powers queue
     *
     * \param pqueue the powers queue
     *
     * \return the first power value in \a pqueue
     * \return \c 0 if the power queue \a pqueue is empty
     */

    Power_t get_from_powers_queue (PowersQueue_t *pqueue) ;

/******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* _3DICE_POWERSQUEUE_H_ */
