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

#ifndef _3DICE_LAYER_H_
#define _3DICE_LAYER_H_

/*! \file layer.h */

#ifdef __cplusplus
extern "C"
{
#endif

/******************************************************************************/

#include <stdio.h>

#include "types.h"

#include "material.h"

/******************************************************************************/

    /*! \struct Layer_t
     *
     *  \brief Structure used to store data about the layers that compose the 2D/3D stack.
     *
     *  Layers are used to build dies os as stack elements
     */

    struct Layer_t
    {
        /*! The heigh of the layer in \f$ \mu m \f$ (1 cell) */

        CellDimension_t Height ;

        /*! The material composing the layer */

        Material_t *Material ;

        /*! The Id given to the layer if declared for stack element */

        String_t Id ;

        /*!
         * To know, after the parsing of a stack file, if a
         * layer has been declared but never used
         */

        Quantity_t Used ;

        /*! Pointer to the 'next' layer (towards the top of the die),
         *  to collect layers in a double linked list */

        struct Layer_t *Next ;

        /*! Pointer to the 'previous' layer (towards the bottom of the die),
         *  to collect layers in a double linked list */

        struct Layer_t *Prev ;
    } ;

    /*! Definition of the type Layer_t */

    typedef struct Layer_t Layer_t ;

/******************************************************************************/



    /*! Sets all the fields to a default value (zero or \c NULL ).
     *
     * \param layer the address of the layer structure to initialize
     */

    void init_layer (Layer_t *layer) ;



    /*! Allocates and inits memory for a structure of type Layer_t
     *
     * \return a pointer to the allocated memory.
     * \return \c NULL in case of error
     */

    Layer_t *calloc_layer ( void ) ;



    /*! Frees the memory space pointed to by \a layer
     *
     * The pointer \a layer must have been returned by a previous call
     * to \a calloc_layer . If \a layer is \c NULL,
     * no operation is performed.
     *
     * \param layer the address to free
     */

    void free_layer (Layer_t *layer) ;



    /*! Frees a list of layers
     *
     * If frees, calling \c free_layer, the layer pointed by the
     * parameter \a list and all the layers it finds following the
     * linked list throught the field Layer_t::Next.
     *
     * \param list the pointer to the first elment in the list to be freed
     */

    void free_layers_list (Layer_t *list) ;



    /*! Searches for a Layer_t in a linked list of layers.
     *
     * Id based search of a Layer_t structure in a list.
     *
     * \param list the pointer to the list
     * \param id   the identifier of the layer to be found
     *
     * \return the address of a Layer_t, if founded
     * \return \c NULL if the search fails
     */

    Layer_t *find_layer_in_list (Layer_t *list, String_t id) ;



    /*! Prints the layer as it looks in the stack file
     *
     * \param layer  the layer to print
     * \param stream the output stream (must be already open)
     * \param prefix a string to be printed as prefix at the beginning of each line
     */

    void print_layer (Layer_t *layer, FILE *stream, String_t prefix) ;



    /*! Prints a list of layers as they look in the stack file
     *
     * \param list   the pointer to the first layer in the list
     * \param stream the output stream (must be already open)
     * \param prefix a string to be printed as prefix at the beginning of each line
     */

    void print_layers_list (Layer_t *list, FILE *stream, String_t prefix) ;


/******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* _3DICE_LAYER_H_ */
