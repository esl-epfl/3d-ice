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

#ifndef _3DICE_THERMALGRID_H_
#define _3DICE_THERMALGRID_H_

/*! \file thermal_grid.h */

#ifdef __cplusplus
extern "C"
{
#endif

/******************************************************************************/

#include "types.h"

#include "channel.h"
#include "heat_sink.h"

#include "dimensions.h"
#include "stack_element.h"

/******************************************************************************/

    /*! \struct ThermalGrid_t
     *
     *  \brief Structure used to store data about the thermal cells / RC nodes
     */

    struct ThermalGrid_t
    {
        /*! The size of all the vectors that belong to this structure,
            i.e. the number of layers in the 3d-ic */

        Quantity_t Size ;

        /*! Vector storing the types of layer along the vertical profile. */

        StackLayerType_t *LayersProfile ;

        /*! Vector storing the volumetric heat capacity of the solid materials
            along the vertical profile of the 3d-ic */

        SolidVHC_t *VHCProfile ;

        /*! Vector storing the thermal conductivity of the solid materials
            along the vertical profile of the 3d-ic */

        SolidTC_t *TCProfile ;

        /*! Pointer to the channel structure */

        Channel_t  *Channel ;

        /*! Pointer to the heat sink structure */

        HeatSink_t *HeatSink ;
    } ;

    /*! Definition of the type ThermalGrid_t */

    typedef struct ThermalGrid_t ThermalGrid_t ;

/******************************************************************************/

    /*! Init a thermal grid using default values
     *
     * \param this the address of the thermal grid to initialize
     */

    void init_thermal_grid (ThermalGrid_t *this) ;



    /*! Alloc memory to store thermal grid informations
     *
     * \param this  pointer to the thermal grid structure
     * \param size  the number of layers in the 3d stack
     *
     * \return \c TDICE_ERROR   if the memory allocation fails
     * \return \c TDICE_SUCCESS otherwise
     */

    Error_t alloc_thermal_grid (ThermalGrid_t *this, Quantity_t size) ;



    /*! Release the memory used by the thermal grid (i.e. the memory allocated
     *  by \a alloc_thermal_grid )
     *
     * \param this the address of the thermal grid
     */

    void free_thermal_grid (ThermalGrid_t *this) ;



    /*! Fills a thermal grid
     *
     *  \param this pointer to the thermal cell in the 3d grid
     *  \param list pointer to the list of stack elements
     * \param  dimensions pointer to the structure storing the dimensions
     */

    void fill_thermal_grid
    (
        ThermalGrid_t  *this,
        StackElement_t *list,
        Dimensions_t   *dimensions
    ) ;



    /*! Return the capacity of a thermal cell at a given position
     *
     * The function prints a message on stderr in case of error
     *
     * \param this         pointer to the thermal grid structure
     * \param dimensions   pointer to the structure storing the dimensions
     * \param layer_index  the index of the layer
     * \param row_index    the index of the row
     * \param column_index the index of the column
     *
     * \return the capacity of the thermal cell in position (\a layer_index ,
     *         \a row_index , \a column_index ).
     */

    Capacity_t get_capacity
    (
        ThermalGrid_t *this,
        Dimensions_t  *dimensions,
        CellIndex_t    layer_index,
        CellIndex_t    row_index,
        CellIndex_t    column_index
    ) ;



    /*! Return the top conductance of a thermal cell at a given position
     *
     * The function prints a message on stderr in case of error
     *
     * \param this         pointer to the thermal grid structure
     * \param dimensions   pointer to the structure storing the dimensions
     * \param layer_index  the index of the layer
     * \param row_index    the index of the row
     * \param column_index the index of the column
     *
     * \return the top conductance of the thermal cell in position (\a layer_index ,
     *         \a row_index , \a column_index ).
     */

    Conductance_t get_conductance_top
    (
        ThermalGrid_t *this,
        Dimensions_t  *dimensions,
        CellIndex_t    layer_index,
        CellIndex_t    row_index,
        CellIndex_t    column_index
    ) ;



    /*! Return the bottom conductance of a thermal cell at a given position
     *
     * The function prints a message on stderr in case of error
     *
     * \param this         pointer to the thermal grid structure
     * \param dimensions   pointer to the structure storing the dimensions
     * \param layer_index  the index of the layer
     * \param row_index    the index of the row
     * \param column_index the index of the column
     *
     * \return the bottom conductance of the thermal cell in position (\a layer_index ,
     *         \a row_index , \a column_index ).
     */

    Conductance_t get_conductance_bottom
    (
        ThermalGrid_t *this,
        Dimensions_t  *dimensions,
        CellIndex_t    layer_index,
        CellIndex_t    row_index,
        CellIndex_t    column_index
    ) ;



    /*! Return the north conductance of a thermal cell at a given position
     *
     * The function prints a message on stderr in case of error
     *
     * \param this         pointer to the thermal grid structure
     * \param dimensions   pointer to the structure storing the dimensions
     * \param layer_index  the index of the layer
     * \param row_index    the index of the row
     * \param column_index the index of the column
     *
     * \return the north conductance of the thermal cell in position (\a layer_index ,
     *         \a row_index , \a column_index ).
     */

    Conductance_t get_conductance_north
    (
        ThermalGrid_t *this,
        Dimensions_t  *dimensions,
        CellIndex_t    layer_index,
        CellIndex_t    row_index,
        CellIndex_t    column_index
    ) ;



    /*! Return the south conductance of a thermal cell at a given position
     *
     * The function prints a message on stderr in case of error
     *
     * \param this         pointer to the thermal grid structure
     * \param dimensions   pointer to the structure storing the dimensions
     * \param layer_index  the index of the layer
     * \param row_index    the index of the row
     * \param column_index the index of the column
     *
     * \return the south conductance of the thermal cell in position (\a layer_index ,
     *         \a row_index , \a column_index ).
     */

    Conductance_t get_conductance_south
    (
        ThermalGrid_t *this,
        Dimensions_t  *dimensions,
        CellIndex_t    layer_index,
        CellIndex_t    row_index,
        CellIndex_t    column_index
    ) ;



    /*! Return the east conductance of a thermal cell at a given position
     *
     * The function prints a message on stderr in case of error
     *
     * \param this         pointer to the thermal grid structure
     * \param dimensions   pointer to the structure storing the dimensions
     * \param layer_index  the index of the layer
     * \param row_index    the index of the row
     * \param column_index the index of the column
     *
     * \return the east conductance of the thermal cell in position (\a layer_index ,
     *         \a row_index , \a column_index ).
     */

    Conductance_t get_conductance_east
    (
        ThermalGrid_t *this,
        Dimensions_t  *dimensions,
        CellIndex_t    layer_index,
        CellIndex_t    row_index,
        CellIndex_t    column_index
    ) ;



    /*! Return the west conductance of a thermal cell at a given position
     *
     * The function prints a message on stderr in case of error
     *
     * \param this         pointer to the thermal grid structure
     * \param dimensions   pointer to the structure storing the dimensions
     * \param layer_index  the index of the layer
     * \param row_index    the index of the row
     * \param column_index the index of the column
     *
     * \return the west conductance of the thermal cell in position (\a layer_index ,
     *         \a row_index , \a column_index ).
     */

    Conductance_t get_conductance_west
    (
        ThermalGrid_t *this,
        Dimensions_t  *dimensions,
        CellIndex_t    layer_index,
        CellIndex_t    row_index,
        CellIndex_t    column_index
    ) ;

/******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* _3DICE_THERMALGRID_H_ */