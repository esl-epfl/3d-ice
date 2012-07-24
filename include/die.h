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

#include "floorplan.h"
#include "layer_list.h"

/******************************************************************************/

    /*! \struct Die_t
     *
     *  \brief Structure used to store data about the dies that compose
     *         the 2D/3D stack.
     *
     *  Dies are one of the elements that can be used to build a 3d stack
     */

    struct Die_t
    {
        /*! The id (string) of the die */

        String_t Id ;

        /*! The number of layer composing the die */

        CellIndex_t NLayers ;

        /*! The offset (\# layers) of the source layer within the die */

        CellIndex_t SourceLayerOffset ;

        /*! The list of layers composing the die. The list
         *  stores the layers in such a way that the head/begin/first
         *  of the list points to the top most layer while the
         *  tail/end/last points to the bottom most one. Crossing the list
         *  in a reverse order is necessary to get layers from bottom
         *  to top. */

        LayerList_t Layers ;

        /*! The floorplan used on the source layer */

        Floorplan_t Floorplan ;
    } ;

    /*! Definition of the type Die_t */

    typedef struct Die_t Die_t ;



/******************************************************************************/



    /*! Inits the fields of the \a die structure with default values
     *
     * \param die the address of the structure to initalize
     */

    void die_init (Die_t *die) ;



    /*! Copies the structure \a src into \a dst , as an assignement
     *
     * The function destroys the content of \a dst and then makes the copy
     *
     * \param dst the address of the left term sructure (destination)
     * \param src the address of the right term structure (source)
     */

    void die_copy (Die_t *dst, Die_t *src) ;



    /*! Destroys the content of the fields of the structure \a die
     *
     * The function releases any dynamic memory used by the structure and
     * resets its state calling \a die_init .
     *
     * \param die the address of the structure to destroy
     */

    void die_destroy (Die_t *die) ;



    /*! Allocates memory for a structure of type Die_t
     *
     * The content of the new structure is set to default values
     * calling \a die_init
     *
     * \return the pointer to the new structure
     * \return \c NULL if the memory allocation fails
     */

    Die_t *die_calloc ( void ) ;



    /*! Allocates memory for a new copy of the structure \a die
     *
     * \param die the address of the structure to clone
     *
     * \return a pointer to a new structure
     * \return \c NULL if the memory allocation fails
     * \return \c NULL if the parameter \a die is \c NULL
     */

    Die_t *die_clone (Die_t *die) ;



    /*! Frees the memory space pointed by \a die
     *
     * The function destroys the structure \a die and then frees
     * its memory. The pointer \a die must have been returned by
     * a previous call to \a die_calloc or \a die_clone .
     *
     * If \a die is \c NULL, no operation is performed.
     *
     * \param die the pointer to free
     */

    void die_free (Die_t *die) ;



    /*! Tests if two dies have the same Id
     *
     * \param die the first die
     * \param other the second die
     *
     * \return \c TRUE if \a die and \a other have the same Id
     * \return \c FALSE otherwise
     */

    bool die_same_id (Die_t *die, Die_t *other) ;



    /*! Prints the die declaration as it looks in the stack file
     *
     * \param die the address of the structure to print
     * \param stream the output stream (must be already open)
     * \param prefix a string to be printed as prefix at the
     *               beginning of each line
     */

    void die_print (Die_t *die, FILE *stream, String_t prefix) ;

/******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* _3DICE_DIE_H_ */
