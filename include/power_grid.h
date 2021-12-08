/******************************************************************************
 * This file is part of 3D-ICE, version 3.1.0 .                               *
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
 *                             Copyright (C) 2021                             *
 *   Embedded Systems Laboratory - Ecole Polytechnique Federale de Lausanne   *
 *                            All Rights Reserved.                            *
 *                                                                            *
 * Authors: Arvind Sridhar              Alessandro Vincenzi                   *
 *          Giseong Bak                 Martino Ruggiero                      *
 *          Thomas Brunschwiler         Eder Zulian                           *
 *          Federico Terraneo           Darong Huang                          *
 *          Luis Costero                Marina Zapater                        *
 *          David Atienza                                                     *
 *                                                                            *
 * For any comment, suggestion or request  about 3D-ICE, please  register and *
 * write to the mailing list (see http://listes.epfl.ch/doc.cgi?liste=3d-ice) *
 * Any usage  of 3D-ICE  for research,  commercial or other  purposes must be *
 * properly acknowledged in the resulting products or publications.           *
 *                                                                            *
 * EPFL-STI-IEL-ESL                     Mail : 3d-ice@listes.epfl.ch          *
 * Batiment ELG, ELG 130                       (SUBSCRIPTION IS NECESSARY)    *
 * Station 11                                                                 *
 * 1015 Lausanne, Switzerland           Url  : http://esl.epfl.ch/3d-ice      *
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
#include "stack_element_list.h"
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

        CellIndex_t NLayers ;

        /*! The number of cells in the 3d-ic */

        CellIndex_t NCells ;

        /*! The number of thermal cells in a layer */

        CellIndex_t NCellsLayer ;

        /*! Vector storing the types of layer along the vertical profile. */

        StackLayerType_t *LayersTypeProfile ;

        /*! Vector storing a pointer to floorplans along the vertical profile */

        Floorplan_t **FloorplansProfile ;

        /*! Array containing the source value of each cell in the 3d-grid */

        Source_t *Sources ;

        /*! Pointer to the channel structure */

        Channel_t  *Channel ;

        /*! Pointer to the top heat sink structure */

        HeatSink_t *TopHeatSink ;

        /*! Pointer to the bottom heat sink structure */

        HeatSink_t *BottomHeatSink ;

        /*! Pointer to a vector storing the TOP thermal conductivities
         *  of the thermal cells in the topmost layer. This vector is filled
         *  whenever the HeatSink is used to improve the performance of the
         *  simulator when the source vector is updated.
         */

        SolidTC_t *HeatSinkTopTcs ;

        /*! Pointer to a vector storing the BOTTOM thermal conductivities
         *  of the thermal cells in the bottommost layer. This vector is filled
         *  whenever the SecondaryPath is used to improve the performance of the
         *  simulator when the source vector is updated.
         */

        SolidTC_t *HeatSinkBottomTcs ;

        /*! Pointer to a vector storing the thermal capacities
         *  of the thermal cells in the entire 3d-ic. This vector is used
         *  to improve the performance of the simulator when the system vector
         *  is updated.
         */

        Capacity_t *CellsCapacities ;
    } ;

    /*! Definition of the type PowerGrid_t */

    typedef struct PowerGrid_t PowerGrid_t ;



/******************************************************************************/



    /*! Inits the fields of the \a pgrid structure with default values
     *
     * \param pgrid the address of the structure to initalize
     */

    void power_grid_init (PowerGrid_t *pgrid) ;



    /*! Allocs internal memory to store power grid information
     *
     * \param pgrid    pointer to the power grid structure
     * \param dimensions pointer to the structure storing the dimensions
     *
     * \return \c TDICE_ERROR   if the memory allocation fails
     * \return \c TDICE_SUCCESS otherwise
     */

    Error_t power_grid_build (PowerGrid_t *pgrid, Dimensions_t *dimensions) ;



    /*! Destroys the content of the fields of the structure \a pgrid
     *
     * The function releases any dynamic memory used by the structure and
     * resets its state calling \a power_grid_init .
     *
     * \param pgrid the address of the structure to destroy
     */

    void power_grid_destroy (PowerGrid_t *pgrid) ;



    /*! Fills a power grid
     *
     *  \param pgrid pointer to the power grid
     *  \param tgrid pointer to the ThermalGrid structure
     *  \param list pointer to the list of stack elements
     *  \param dimensions pointer to the structure storing the dimensions
     */

    void power_grid_fill
    (
        PowerGrid_t        *pgrid,
        ThermalGrid_t      *tgrid,
        StackElementList_t *list,
        Dimensions_t       *dimensions
    ) ;



    /*! Update the source vector
     *
     * \param pgrid address of the PowerGrid structure storing the sources
     * \param dimensions the dimensions of the IC
     *
     *  \return \c TDICE_SUCCESS if the source vector has been updated
     *  \return \c TDICE_FAILURE if it not possible to fill the source vector
     *                           (at least one floorplan element with no power
     *                            values in its queue)
     */

    Error_t update_source_vector (PowerGrid_t *pgrid, Dimensions_t *dimensions) ;



    /*! Update channel sources
     *
     * \param pgrid address of the PowerGrid structure storing the sources
     * \param dimensions the dimensions of the IC
     *
     */

    void update_channel_sources (PowerGrid_t *pgrid, Dimensions_t *dimensions) ;



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
     *  \param pgrid    address of the PowerGrid structure
     *  \param pvalues pointer to the list of power values
     *
     *  \return \c TDICE_FAILURE if the queue \a pvalues does not
     *                           contain enough power values
     *  \return \c TDICE_SUCCESS otherwise
     */

    Error_t insert_power_values (PowerGrid_t *pgrid, PowersQueue_t *pvalues) ;

/******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* _3DICE_POWER_GRID_H_ */
