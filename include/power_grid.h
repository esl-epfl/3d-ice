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

#ifndef _3DICE_POWER_GRID_H_
#define _3DICE_POWER_GRID_H_

/*! \file power_grid.h */

#ifdef __cplusplus
extern "C"
{
#endif

/******************************************************************************/

#include "types.h"

#include "channel.h"
#include "heat_sink.h"
#include "floorplan.h"

#include "thermal_grid.h"
#include "stack_element.h"
#include "powers_queue.h"
#include "dimensions.h"

/******************************************************************************/

    /*! \struct PowerGrid_t
     *
     *  \brief Structure used to store data about the power sources
     */

    struct PowerGrid_t
    {
        /*! The number of layers/floorplans in the 3d-ic */

        Quantity_t NLayers ;

        /*! The number of cells in the 3d-ic */

        Quantity_t NCells ;

        /*! Vector storing the types of layer along the vertical profile. */

        StackLayerType_t *LayersProfile ;

        /*! Vector storing a pointer to floorplans along the vertical profile */

        Floorplan_t **FloorplansProfile ;

        /*! Array containing the source value of each cell in the 3d-grid */

        Source_t *Sources ;

        /*! Pointer to the channel structure */

        Channel_t  *Channel ;

        /*! Pointer to the heat sink structure */

        HeatSink_t *HeatSink ;
    } ;

    /*! Definition of the type PowerGrid_t */

    typedef struct PowerGrid_t PowerGrid_t ;

/******************************************************************************/

    /*! Sets all the fields to a default value (zero or \c NULL ).
     *
     * \param this the address of the power grid structure to initialize
     */

    void init_power_grid (PowerGrid_t *this) ;



    /*! Allocs internal memory to store power grid informations
     *
     * \param this    pointer to the power grid structure
     * \param nlayers the number of layers in the 3d stack
     * \param ncells  the number of cells in the 3d stack
     *
     * \return \c TDICE_ERROR   if the memory allocation fails
     * \return \c TDICE_SUCCESS otherwise
     */

    Error_t build_power_grid
    (
        PowerGrid_t *this,
        Quantity_t   nlayers,
        Quantity_t   ncells
    ) ;



    /*! Release the memory used by the power grid
     *  (i.e. the memory allocated by \a build_power_grid )
     *
     * \param this the address of the power grid
     */

    void destroy_power_grid (PowerGrid_t *this) ;



    /*! Fills a power grid
     *
     *  \param this pointer to the power grid
     *  \param list pointer to the list of stack elements
     */

    void fill_power_grid (PowerGrid_t *this, StackElement_t *list) ;



    /*! Update the source vector
     *
     * \param this address of the PowerGrid structure storing the sources
     * \param thermal_grid pointer to the ThermalGrid structure
     * \param dimensions the dimensions of the IC
     *
     *  \return \c TDICE_SUCCESS if the source vector has been updated successfully
     *  \return \c TDICE_FAILURE if it not possible to fill the source vector
     *                           (at least one floorplan element with no power
     *                            values in its queue)
     */

    Error_t update_source_vector
    (
        PowerGrid_t    *this,
        ThermalGrid_t  *thermal_grid,
        Dimensions_t   *dimensions
    ) ;



    /*! Update channel sources
     *
     * \param this address of the PowerGrid structure storing the sources
     * \param dimensions the dimensions of the IC
     *
     */

    void update_channel_sources (PowerGrid_t *this, Dimensions_t *dimensions) ;



    /*! Inserts one power values from a power queue into each
     *  floorplan element in the entire stack
     *
     *  The queue \a pvalues must contain at least as many power values as
     *  the number of floorplan elements in the entire stack. \a pvalues is a
     *  FIFO queue and, whithin the stack, elements are counted from the
     *  bottom (the last element declared in the stack section of the
     *  stack description file). If the stack has two dies \c A (bottom) and
     *  \c B (top) and A has a floorplan with \c n elements while \c B has \c m
     *  elements, then \a pvalues must contain at least \c nm elements. The
     *  first \c n power values will be given to \c A and the remaining will
     *  be assigned to \c B
     *
     *  \param this    address of the PowerGrid structure
     *  \param pvalues pointer to the list of power values
     *
     *  \return \c TDICE_FAILURE if the queue \a pvalues does not
     *                           contain enough power values
     *  \return \c TDICE_SUCCESS otherwise
     */

    Error_t insert_power_values (PowerGrid_t *this, PowersQueue_t *pvalues) ;

/******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* _3DICE_POWER_GRID_H_ */
