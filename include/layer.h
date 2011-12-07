/******************************************************************************
 * This file is part of 3D-ICE, version 2.0 .                                 *
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

#include "dimensions.h"
#include "material.h"
#include "system_matrix.h"
#include "thermal_cell.h"

/******************************************************************************/

    /*! \struct Layer
     *
     *  \brief Structure used to store data about the layers that compose the 2D/3D stack.
     *
     *  Layers are used to build dies os as stack elements
     */

    struct Layer
    {
        /*! The heigh of the layer in \f$ \mu m \f$ (1 cell) */

        CellDimension_t Height ;

        /*! The material composing the layer */

        Material *Material ;

        /*! Pointer to the 'next' layer (towards the top of the die),
         *  to collect layers in a double linked list */

        struct Layer *Next ;

        /*! Pointer to the 'previous' layer (towards the bottom of the die),
         *  to collect layers in a double linked list */

        struct Layer *Prev ;
    } ;

    /*! Definition of the type Layer */

    typedef struct Layer Layer ;

/******************************************************************************/



    /*! Sets all the fields of \a layer to a default value (zero or \c NULL ).
     *
     * \param layer the address of the layer to initialize
     */

    void init_layer (Layer *layer) ;



    /*! Allocates a Layer in memory and sets its fields to their default
     *  value with \c init_layer
     *
     * \return the pointer to a new Layer
     * \return \c NULL if the memory allocation fails
     */

    Layer *alloc_and_init_layer (void) ;



    /*! Frees the memory related to \a layer
     *
     * The parametrer \a layer must be a pointer previously obtained with
     * \c alloc_and_init_layer
     *
     * \param layer the address of the layer structure to free
     */

    void free_layer (Layer *layer) ;



    /*! Frees a list of layers
     *
     * If frees, calling \c free_layer, the layer pointed by the
     * parameter \a list and all the layer it finds following the
     * linked list throught the field Layer::Next.
     *
     * \param list the pointer to the first elment in the list to be freed
     */

    void free_layers_list (Layer *list) ;



    /*! Prints the layer as it looks in the stack file
     *
     * \param stream the output stream (must be already open)
     * \param prefix a string to be printed as prefix at the beginning of each line
     * \param layer  the layer to print
     */

    void print_formatted_layer

        (FILE *stream, String_t prefix, Layer *layer) ;



    /*! Prints a list of layers as they look in the stack file
     *
     * \param stream the output stream (must be already open)
     * \param prefix a string to be printed as prefix at the beginning of each line
     * \param list   the pointer to the first layer in the list
     */

    void print_formatted_layers_list

        (FILE *stream, String_t prefix, Layer *list) ;



    /*! Prints detailed information about all the fields of a layer
     *
     * \param stream  the output stream (must be already open)
     * \param prefix  a string to be printed as prefix at the beginning of each line
     * \param layer   the layer to print
     */

    void print_detailed_layer

        (FILE *stream, String_t prefix, Layer *layer) ;



    /*! Prints a list of detailed information about all the fields of the layers
     *
     * \param stream the output stream (must be already open)
     * \param prefix a string to be printed as prefix at the beginning of each line
     * \param list the pointer to the first layer in the list
     */

    void print_detailed_layers_list

        (FILE *stream, String_t prefix, Layer *list) ;



    /*! Fills the thermal cells corresponding to a layer in the stack
     *
     *  \param thermal_cells pointer to the first thermal cell in the 3d stack
     *  \param delta_time    the time resolution of the thermal simulation
     *  \param dimensions    pointer to the structure storing the dimensions
     *  \param layer_index   offset (\# layers) of the layer within the stack
     *  \param layer         pointer to the layer
     */

    void fill_thermal_cell_layer
    (
        ThermalCell *thermal_cells,
        Time_t       delta_time,
        Dimensions  *dimensions,
        CellIndex_t  layer_index,
        Layer       *layer
    ) ;



    /*! Fills the system matrix
     *
     *  \param dimensions    pointer to the structure storing the dimensions
     *  \param thermal_cells pointer to the first thermal cell in the 3d stack
     *  \param layer_index   offset (\# layers) of the die within the stack
     *  \param system_matrix copy of the system matrix structure
     *
     *  \return A matrix partially filled (FIXME)
     */

    SystemMatrix fill_system_matrix_layer
    (
        Dimensions   *dimensions,
        ThermalCell  *thermal_cells,
        CellIndex_t   layer_index,
        SystemMatrix  system_matrix
    ) ;

/******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* _3DICE_LAYER_H_ */
