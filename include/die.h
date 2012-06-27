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

#ifndef _3DICE_DIE_H_
#define _3DICE_DIE_H_

/*! \file die.h */

#ifdef __cplusplus
extern "C"
{
#endif

/******************************************************************************/

#include <stdio.h>

#include "types.h"

#include "layer.h"

/******************************************************************************/

    /*! \struct Die_t
     *
     *  \brief Structure used to store data about the dies that compose the 2D/3D stack.
     *
     *  Dies are one of the elements that can be used to build a 3d stack
     */

    struct Die_t
    {
        /*! The id (string) of the die */

        String_t Id ;

        /*! To know, after the parsing of a stack file, if a
         *  die has been declared but never used in the stack */

        Quantity_t Used ;

        /*! The number of layer composing the die */

        CellIndex_t NLayers ;

        /*! The offset (\# layers) of the source layer within the die */

        CellIndex_t SourceLayerOffset ;

        /*! Pointer to the top-most layer */

        Layer_t *TopLayer ;

        /*! Pointer to the source layer */

        Layer_t *SourceLayer ;

        /*! Pointer to the bottom-most layer */

        Layer_t *BottomLayer ;

        /*! To collect dies in a linked list */

        struct Die_t *Next ;

    } ;

    /*! Definition of the type Die_t */

    typedef struct Die_t Die_t ;

/******************************************************************************/



    /*! Sets all the fields to a default value (zero or \c NULL ).
     *
     * \param this the address of the die structure to initialize
     */

    void init_die (Die_t *this) ;



    /*! Allocates and inits memory for a structure of type Die_t
     *
     * \return a pointer to the allocated memory.
     * \return \c NULL in case of error
     */

    Die_t *calloc_die ( void ) ;



    /*! Frees the memory space pointed to by \a this
     *
     * The pointer \a this must have been returned by a previous call
     * to \a calloc_die . If \a this is \c NULL, no operation is performed.
     *
     * \param this the address to free
     */

    void free_die (Die_t *this) ;



    /*! Frees a list of dies
     *
     * If frees, calling \c free_die, the die pointed by the
     * parameter \a list and all the dies it finds following the
     * linked list throught the field Die_t::Next.
     *
     * \param list the pointer to the first elment in the list to be freed
     */

    void free_dies_list (Die_t *list) ;



    /*! Searches for a Die in a linked list of dies.
     *
     * Id based search of a Die structure in a list.
     *
     * \param list the pointer to the list
     * \param id   the identifier of the die to be found
     *
     * \return the address of a Die, if founded
     * \return \c NULL if the search fails
     */

    Die_t *find_die_in_list (Die_t *list, String_t id) ;



    /*! Prints the die as it looks in the stack file
     *
     * \param this   the die to print
     * \param stream the output stream (must be already open)
     * \param prefix a string to be printed as prefix at the beginning of each line
     */

    void print_die (Die_t *this, FILE *stream, String_t prefix) ;



    /*! Prints a list of dies as they look in the stack file
     *
     * \param list   the pointer to the first die in the list
     * \param stream the output stream (must be already open)
     * \param prefix a string to be printed as prefix at the beginning of each line
     */

    void print_dies_list (Die_t *list, FILE *stream, String_t prefix) ;

/******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* _3DICE_DIE_H_ */
