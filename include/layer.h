/******************************************************************************
 * This file is part of 3D-ICE, version 3.0.0 .                               *
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

#include <stdio.h> // For the file type FILE

#include "types.h"
#include "string_t.h"

#include "material.h"
#include "material_list.h"
#include "dimensions.h"
#include "material_element_list.h"

/******************************************************************************/

    /*! \struct Layer_t
     *
     *  \brief Structure used to store data about the layers that compose
     *         the 2D/3D stack.
     *
     *  Layers are used to build dies os as stack elements
     */

    struct Layer_t
    {
        /*! The heigh of the layer in \f$ \mu m \f$ (1 cell) */

        CellDimension_t Height ;

        /*! The material composing the layer */

        Material_t Material ;

        /*! The Id given to the layer if declared for stack element */

        String_t Id ;

        /*! The name of the file storing a material layout */

        String_t LayoutFileName ;

        /*! The list of the material layout */

        MaterialElementList_t MaterialLayout ;
    } ;

    /*! Definition of the type Layer_t */

    typedef struct Layer_t Layer_t ;



/******************************************************************************/



    /*! Inits the fields of the \a layer structure with default values
     *
     * \param layer the address of the structure to initalize
     */

    void layer_init (Layer_t *layer) ;



    /*! Copies the structure \a src into \a dst , as an assignement
     *
     * The function destroys the content of \a dst and then makes the copy
     *
     * \param dst the address of the left term sructure (destination)
     * \param src the address of the right term structure (source)
     */

    void layer_copy (Layer_t *dst, Layer_t *src) ;



    /*! Destroys the content of the fields of the structure \a layer
     *
     * The function releases any dynamic memory used by the structure and
     * resets its state calling \a layer_init .
     *
     * \param layer the address of the structure to destroy
     */

    void layer_destroy (Layer_t *layer) ;



    /*! Allocates memory for a structure of type Layer_t
     *
     * The content of the new structure is set to default values
     * calling \a layer_init
     *
     * \return the pointer to the new structure
     * \return \c NULL if the memory allocation fails
     */

    Layer_t *layer_calloc ( void ) ;



    /*! Allocates memory for a new copy of the structure \a layer
     *
     * \param layer the address of the structure to clone
     *
     * \return a pointer to a new structure
     * \return \c NULL if the memory allocation fails
     * \return \c NULL if the parameter \a layer is \c NULL
     */

    Layer_t *layer_clone (Layer_t *layer) ;



    /*! Frees the memory space pointed by \a layer
     *
     * The function destroys the structure \a layer and then frees
     * its memory. The pointer \a layer must have been returned by
     * a previous call to \a layer_calloc or \a layer_clone .
     *
     * If \a layer is \c NULL, no operation is performed.
     *
     * \param layer the pointer to free
     */

    void layer_free (Layer_t *layer) ;



    /*! Tests if two layers have the same Id
     *
     * \param layer the first layer
     * \param other the second layer
     *
     * \return \c TRUE if \a layer and \a other have the same Id
     * \return \c FALSE otherwise
     */

    bool layer_same_id (Layer_t *layer, Layer_t *other) ;



    /*! Prints the layer declaration as it looks in the stack file
     *
     * \param layer the address of the structure to print
     * \param stream the output stream (must be already open)
     * \param prefix a string to be printed as prefix at the
     *               beginning of each line
     */

    void layer_print (Layer_t *layer, FILE *stream, String_t prefix) ;



    /*! Parses the layout file and fills the MaterialLayout structure
     *
     * FIXME is this the right place ? do we need a strucure as floorplan?
     *
     * \param layer      the layer structure to fill
     * \param dimensions pointer to the structure storing the dimensions of the stack
     * \param materials  the list of materials already known
     * \param filename   path to the floorplan file to parse
     *
     * \return \c TDICE_FAILURE if the file cannot be opened or if the parsing
     *                  of the floorplan fails
     * \return \c TDICE_FAILURE otherwise
     */

    Error_t fill_layout (Layer_t        *layer,
                         Dimensions_t   *dimensions,
                         MaterialList_t *materials,
                         String_t        filename) ;



    /*! Returns the thermal conductivity of a cell in a given location
     *
     * \param layer        the layer structure to query
     * \param row_index    the index of the row of the thermal cell
     * \param column_index the index of the column of the thermal cell
     * \param dimensions   pointer to the structure storing the dimensions
     *                     of the stack
     *
     * \return The thermal conductivity of the cell in location
     *         (\a row_index, \a column_index )
     */

    SolidTC_t get_thermal_conductivity (Layer_t      *layer,
                                        CellIndex_t   row_index,
                                        CellIndex_t   column_index,
                                        Dimensions_t *dimensions) ;



    /*! Returns the volumetric heat capacity of a cell in a given location
     *
     * \param layer        the layer structure to query
     * \param row_index    the index of the row of the thermal cell
     * \param column_index the index of the column of the thermal cell
     * \param dimensions   pointer to the structure storing the dimensions
     *                     of the stack
     *
     * \return The volumetric heat capacity of the cell in location
     *         (\a row_index, \a column_index )
     */

    SolidVHC_t get_volumetric_heat_capacity (Layer_t      *layer,
                                             CellIndex_t   row_index,
                                             CellIndex_t   column_index,
                                             Dimensions_t *dimensions) ;

/******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* _3DICE_LAYER_H_ */
