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

#include <stdlib.h>

#include "macros.h"
#include "thermal_grid.h"

/******************************************************************************/

void thermal_grid_init (ThermalGrid_t *tgrid)
{
    tgrid->Size             = (CellIndex_t) 0u ;
    tgrid->LayersProfile    = NULL ;
    tgrid->VHCProfile       = NULL ;
    tgrid->TCProfile        = NULL ;
    tgrid->Channel          = NULL ;
    tgrid->HeatSink         = NULL ;
}

/******************************************************************************/

Error_t thermal_grid_build (ThermalGrid_t *tgrid, Quantity_t size)
{
    tgrid->Size = size ;

    tgrid->LayersProfile =

        (StackLayerType_t *) calloc (size, sizeof (StackLayerType_t)) ;

    if (tgrid->LayersProfile == NULL)

        return TDICE_FAILURE ;

    tgrid->VHCProfile = (SolidVHC_t *) calloc (size, sizeof (SolidVHC_t)) ;

    if (tgrid->VHCProfile == NULL)
    {
        free (tgrid->LayersProfile) ;

        return TDICE_FAILURE ;
    }

    tgrid->TCProfile = (SolidTC_t *) calloc (size, sizeof (SolidTC_t)) ;

    if (tgrid->TCProfile == NULL)
    {
        free (tgrid->VHCProfile) ;
        free (tgrid->LayersProfile) ;

        return TDICE_FAILURE ;
    }

    return TDICE_SUCCESS ;
}

/******************************************************************************/

void thermal_grid_destroy (ThermalGrid_t *tgrid)
{
    free (tgrid->LayersProfile) ;
    free (tgrid->VHCProfile) ;
    free (tgrid->TCProfile) ;

    thermal_grid_init (tgrid) ;
}

/******************************************************************************/

void fill_thermal_grid
(
    ThermalGrid_t      *tgrid,
    StackElementList_t *list,
    Dimensions_t       *dimensions
)
{
    StackElementListNode_t *stkeln ;

    for (stkeln  = stack_element_list_end (list) ;
         stkeln != NULL ;
         stkeln  = stack_element_list_prev (stkeln))
    {
        StackElement_t *stack_element = stack_element_list_data (stkeln) ;

        CellIndex_t index = stack_element->Offset ;

        switch (stack_element->Type)
        {
            case TDICE_STACK_ELEMENT_DIE :
            {
                CellIndex_t tmp = 0u ;

                LayerListNode_t *lnd ;

                for (lnd  = layer_list_end (&stack_element->Pointer.Die->Layers) ;
                     lnd != NULL ;
                     lnd  = layer_list_prev (lnd))
                {
                    tgrid->LayersProfile [index + tmp] = TDICE_LAYER_SOLID ;
                    tgrid->VHCProfile    [index + tmp] = layer_list_data(lnd)->Material.VolumetricHeatCapacity ;
                    tgrid->TCProfile     [index + tmp] = layer_list_data(lnd)->Material.ThermalConductivity ;

                    tmp++ ;
                }

                tmp = stack_element->Pointer.Die->SourceLayerOffset ;

                tgrid->LayersProfile [index + tmp] = TDICE_LAYER_SOURCE ;

                break ;
            }
            case TDICE_STACK_ELEMENT_LAYER :
            {
                tgrid->LayersProfile [index] = TDICE_LAYER_SOLID ;
                tgrid->VHCProfile    [index] = stack_element->Pointer.Layer->Material.VolumetricHeatCapacity ;
                tgrid->TCProfile     [index] = stack_element->Pointer.Layer->Material.ThermalConductivity ;

                break ;
            }
            case TDICE_STACK_ELEMENT_CHANNEL :
            {
                tgrid->Channel = stack_element->Pointer.Channel ;

                // Stores the wall material

                CellIndex_t tmp ;

                for (tmp = 0u ; tmp != stack_element->Pointer.Channel->NLayers ; tmp++)
                {
                    tgrid->VHCProfile [index + tmp] = tgrid->Channel->WallMaterial.VolumetricHeatCapacity ;
                    tgrid->TCProfile  [index + tmp] = tgrid->Channel->WallMaterial.ThermalConductivity ;
                }

                switch (tgrid->Channel->ChannelModel)
                {
                    case TDICE_CHANNEL_MODEL_MC_4RM :

                        tgrid->LayersProfile [index] = TDICE_LAYER_CHANNEL_4RM ;

                        break ;

                    case TDICE_CHANNEL_MODEL_MC_2RM :

                        tgrid->LayersProfile [index    ] = TDICE_LAYER_BOTTOM_WALL ;
                        tgrid->LayersProfile [index + 1] = TDICE_LAYER_VWALL_CHANNEL ;
                        tgrid->LayersProfile [index + 2] = TDICE_LAYER_CHANNEL_2RM ;
                        tgrid->LayersProfile [index + 3] = TDICE_LAYER_TOP_WALL ;

                        break ;

                    case TDICE_CHANNEL_MODEL_PF_INLINE :

                        tgrid->LayersProfile [index    ] = TDICE_LAYER_BOTTOM_WALL ;
                        tgrid->LayersProfile [index + 1] = TDICE_LAYER_VWALL_PINFINS ;
                        tgrid->LayersProfile [index + 2] = TDICE_LAYER_PINFINS_INLINE ;
                        tgrid->LayersProfile [index + 3] = TDICE_LAYER_TOP_WALL ;

                        break ;

                    case TDICE_CHANNEL_MODEL_PF_STAGGERED :

                        tgrid->LayersProfile [index    ] = TDICE_LAYER_BOTTOM_WALL ;
                        tgrid->LayersProfile [index + 1] = TDICE_LAYER_VWALL_PINFINS ;
                        tgrid->LayersProfile [index + 2] = TDICE_LAYER_PINFINS_STAGGERED ;
                        tgrid->LayersProfile [index + 3] = TDICE_LAYER_TOP_WALL ;

                        break ;

                    case TDICE_CHANNEL_MODEL_NONE :

                        fprintf (stderr, "WARNING: unset channel model\n") ;

                        break ;

                    default :

                        fprintf (stderr,
                            "WARNING: unknown channel model %d\n",
                            stack_element->Pointer.Channel->ChannelModel) ;

                        break ;
                }

                break ;
            }
            case TDICE_STACK_ELEMENT_HEATSINK :
            {
                tgrid->HeatSink = stack_element->Pointer.HeatSink ;

                switch (tgrid->HeatSink->SinkModel)
                {
                    case TDICE_HEATSINK_MODEL_CONNECTION_TO_AMBIENT :

                        if (tgrid->LayersProfile [index] == TDICE_LAYER_SOLID)

                            tgrid->LayersProfile [index] = TDICE_LAYER_SOLID_CONNECTED_TO_AMBIENT ;

                        else if (tgrid->LayersProfile [index] == TDICE_LAYER_SOURCE)

                            tgrid->LayersProfile [index] = TDICE_LAYER_SOURCE_CONNECTED_TO_AMBIENT ;

                        else

                            fprintf (stderr, "ERROR: Wrong offset of heat sink stack element\n") ;

                        tgrid->VHCProfile    [index] = tgrid->HeatSink->SinkMaterial.VolumetricHeatCapacity ;
                        tgrid->TCProfile     [index] = tgrid->HeatSink->SinkMaterial.ThermalConductivity ;

                        break ;

                    case TDICE_HEATSINK_MODEL_TRADITIONAL :

                        tgrid->LayersProfile [index] = TDICE_LAYER_SPREADER ;

                        tgrid->VHCProfile [index] = get_spreader_volumetric_heat_capacity

                            (tgrid->HeatSink, get_chip_area(dimensions)) ;

                        tgrid->TCProfile  [index] = get_spreader_thermal_conductivity

                            (tgrid->HeatSink, get_chip_area(dimensions)) ;

                        tgrid->LayersProfile [index + 1] = TDICE_LAYER_SINK ;

                        tgrid->VHCProfile [index + 1] = get_sink_volumetric_heat_capacity

                            (tgrid->HeatSink, get_chip_area(dimensions)) ;

                        tgrid->TCProfile [index + 1] = get_sink_thermal_conductivity

                             (tgrid->HeatSink, get_chip_area(dimensions)) ;

                        break ;

                    case TDICE_CHANNEL_MODEL_NONE :

                        fprintf (stderr, "WARNING: unset heatsink model\n") ;

                        break ;

                    default :

                        fprintf (stderr,
                            "WARNING: unknown heatsink model %d\n",
                            stack_element->Pointer.Channel->ChannelModel) ;

                        break ;
                }

                break ;
            }
            case TDICE_STACK_ELEMENT_NONE :

                fprintf (stderr, "Error! Found stack element with unset type\n") ;
                break ;

            default :

                fprintf (stderr, "Error! Unknown stack element type %d\n", stack_element->Type) ;

        } /* switch stack_element->Type */
    }
}

/******************************************************************************/


Capacity_t get_capacity
(
    ThermalGrid_t *tgrid,
    Dimensions_t  *dimensions,
    CellIndex_t    layer_index,
    CellIndex_t    row_index,
    CellIndex_t    column_index
)
{
    if (layer_index > tgrid->Size)
    {
        fprintf (stderr,
            "ERROR: layer index %d is out of range\n", layer_index) ;

        return 0.0 ;
    }

    switch (tgrid->LayersProfile [layer_index])
    {
        case TDICE_LAYER_SOLID :
        case TDICE_LAYER_SOURCE :
        case TDICE_LAYER_SOLID_CONNECTED_TO_AMBIENT :
        case TDICE_LAYER_SOURCE_CONNECTED_TO_AMBIENT :
        case TDICE_LAYER_SPREADER :
        case TDICE_LAYER_SINK :

            return (  tgrid->VHCProfile [ layer_index ]
                    * get_cell_length (dimensions, column_index)
                    * get_cell_width  (dimensions, row_index)
                    * get_cell_height (dimensions, layer_index)
                   ) ;

        case TDICE_LAYER_CHANNEL_4RM :

            if (IS_CHANNEL_COLUMN (TDICE_CHANNEL_MODEL_MC_4RM, column_index))

                return (  tgrid->Channel->Coolant.VHC
                        * get_cell_length (dimensions, column_index)
                        * get_cell_width  (dimensions, row_index)
                        * get_cell_height (dimensions, layer_index)
                       ) ;

            else

                return (  tgrid->VHCProfile [ layer_index ]
                        * get_cell_length (dimensions, column_index)
                        * get_cell_width  (dimensions, row_index)
                        * get_cell_height (dimensions, layer_index)
                       ) ;

        case TDICE_LAYER_CHANNEL_2RM :
        case TDICE_LAYER_PINFINS_INLINE :
        case TDICE_LAYER_PINFINS_STAGGERED :

            return (  tgrid->Channel->Coolant.VHC
                    * tgrid->Channel->Porosity
                    * get_cell_length (dimensions, column_index)
                    * get_cell_width  (dimensions, row_index)
                    * get_cell_height (dimensions, layer_index)
                   ) ;

        case TDICE_LAYER_VWALL_CHANNEL :
        case TDICE_LAYER_VWALL_PINFINS :

            return (  tgrid->VHCProfile [ layer_index ]
                    * (1.0 - tgrid->Channel->Porosity)
                    * get_cell_length (dimensions, column_index)
                    * get_cell_width  (dimensions, row_index)
                    * get_cell_height (dimensions, layer_index)
                   ) ;

        case TDICE_LAYER_TOP_WALL :
        case TDICE_LAYER_BOTTOM_WALL :

            return 0.0 ;

        case TDICE_LAYER_NONE :

            fprintf (stderr, "ERROR: unset layer type\n") ;

            return 0.0 ;

        default :

            fprintf (stderr, "ERROR: unknown layer type %d\n",
                tgrid->LayersProfile [layer_index]) ;

            return 0.0 ;
    }
}

/******************************************************************************/

Conductance_t get_conductance_top
(
    ThermalGrid_t *tgrid,
    Dimensions_t  *dimensions,
    CellIndex_t    layer_index,
    CellIndex_t    row_index,
    CellIndex_t    column_index
)
{
    if (layer_index > tgrid->Size)
    {
        fprintf (stderr,
            "ERROR: layer index %d is out of range\n", layer_index) ;

        return 0.0 ;
    }

    switch (tgrid->LayersProfile [layer_index])
    {
        case TDICE_LAYER_SOLID :
        case TDICE_LAYER_SOURCE :

            if (IS_LAST_LAYER (layer_index, dimensions))

                return 0.0 ;

            else if (IS_FIRST_LAYER (layer_index))

                return (  tgrid->TCProfile [ layer_index ]
                        * get_cell_length (dimensions, column_index)
                        * get_cell_width  (dimensions, row_index)
                       )
                        / get_cell_height (dimensions, layer_index) ;

            else

                return (  tgrid->TCProfile [ layer_index ]
                        * get_cell_length (dimensions, column_index)
                        * get_cell_width  (dimensions, row_index)
                       )
                        / (get_cell_height (dimensions, layer_index) / 2.0) ;

        case TDICE_LAYER_SOLID_CONNECTED_TO_AMBIENT :
        case TDICE_LAYER_SOURCE_CONNECTED_TO_AMBIENT :

            return (  2.0
                    * tgrid->TCProfile [ layer_index ]
                    * tgrid->HeatSink->AmbientHTC
                    * get_cell_length (dimensions, column_index)
                    * get_cell_width  (dimensions, row_index)
                   )
                   /
                   (  get_cell_height (dimensions, layer_index)
                    * tgrid->HeatSink->AmbientHTC
                    + 2.0
                    * tgrid->TCProfile [ layer_index ]
                   ) ;

        case TDICE_LAYER_SINK :


            return (  2.0
                    * tgrid->TCProfile [ layer_index ]
                    * tgrid->HeatSink->AmbientHTC
                    * (tgrid->HeatSink->SinkArea / get_chip_area (dimensions))
                    * get_cell_length (dimensions, column_index)
                    * get_cell_width  (dimensions, row_index)
                   )
                   /
                   (  get_cell_height (dimensions, layer_index)
                    * tgrid->HeatSink->AmbientHTC
                    * (tgrid->HeatSink->SinkArea / get_chip_area (dimensions))
                    + 2.0
                    * tgrid->TCProfile [ layer_index ]
                   ) ;

        case TDICE_LAYER_SPREADER :

            return (  tgrid->TCProfile [ layer_index ]
                    * get_cell_length (dimensions, column_index)
                    * get_cell_width  (dimensions, row_index)
                    )
                    / (get_cell_height (dimensions, layer_index) / 2.0) ;

        case TDICE_LAYER_CHANNEL_4RM :

            if (IS_CHANNEL_COLUMN (TDICE_CHANNEL_MODEL_MC_4RM, column_index))

                return    tgrid->Channel->Coolant.HTCTop
                        * get_cell_width  (dimensions, row_index)
                        * get_cell_length (dimensions, column_index) ;

            else

                // We assume that layer_index is not the top most layer
                // or the bottom most layer

                return (  tgrid->TCProfile [ layer_index ]
                        * get_cell_length (dimensions, column_index)
                        * get_cell_width  (dimensions, row_index)
                       )
                        / (get_cell_height (dimensions, layer_index) / 2.0) ;

        case TDICE_LAYER_CHANNEL_2RM :

            return    tgrid->Channel->Coolant.HTCTop
                    * get_cell_width  (dimensions, row_index)
                    * get_cell_length (dimensions, column_index) ;

        case TDICE_LAYER_PINFINS_INLINE :

            return    EFFECTIVE_HTC_PF_INLINE (tgrid->Channel->Coolant.DarcyVelocity)
                    * get_cell_width  (dimensions, row_index)
                    * get_cell_length (dimensions, column_index) ;

        case TDICE_LAYER_PINFINS_STAGGERED :

            return    EFFECTIVE_HTC_PF_STAGGERED (tgrid->Channel->Coolant.DarcyVelocity)
                    * get_cell_width  (dimensions, row_index)
                    * get_cell_length (dimensions, column_index) ;

        case TDICE_LAYER_VWALL_CHANNEL :
        case TDICE_LAYER_VWALL_PINFINS :

            return (  tgrid->TCProfile [ layer_index ]
                    * get_cell_length (dimensions, column_index)
                    * get_cell_width  (dimensions, row_index)
                   )
                    / (get_cell_height (dimensions, layer_index) / 2.0)
                    * (1.0 - tgrid->Channel->Porosity) ;

        case TDICE_LAYER_TOP_WALL :
        case TDICE_LAYER_BOTTOM_WALL :

            return 0.0 ;

        case TDICE_LAYER_NONE :

            fprintf (stderr, "ERROR: unset layer type\n") ;

            return 0.0 ;

        default :

            fprintf (stderr, "ERROR: unknown layer type %d\n",
                tgrid->LayersProfile [layer_index]) ;

            return 0.0 ;
    }
}

/******************************************************************************/

Conductance_t get_conductance_bottom
(
    ThermalGrid_t *tgrid,
    Dimensions_t  *dimensions,
    CellIndex_t    layer_index,
    CellIndex_t    row_index,
    CellIndex_t    column_index
)
{
    if (layer_index > tgrid->Size)
    {
        fprintf (stderr,
            "ERROR: layer index %d is out of range\n", layer_index) ;

        return 0.0 ;
    }

    switch (tgrid->LayersProfile [layer_index])
    {
        case TDICE_LAYER_SOLID :
        case TDICE_LAYER_SOURCE :

            if (IS_FIRST_LAYER (layer_index))

                return 0.0 ;

            else if (IS_LAST_LAYER (layer_index, dimensions))

                return (  tgrid->TCProfile [ layer_index ]
                        * get_cell_length (dimensions, column_index)
                        * get_cell_width  (dimensions, row_index)
                       )
                        / get_cell_height (dimensions, layer_index) ;

            else

                return (  tgrid->TCProfile [ layer_index ]
                        * get_cell_length (dimensions, column_index)
                        * get_cell_width  (dimensions, row_index)
                       )
                        / (get_cell_height (dimensions, layer_index) / 2.0) ;

        case TDICE_LAYER_SOLID_CONNECTED_TO_AMBIENT :
        case TDICE_LAYER_SOURCE_CONNECTED_TO_AMBIENT :
        case TDICE_LAYER_SPREADER :
        case TDICE_LAYER_SINK :

            return (  tgrid->TCProfile [ layer_index ]
                    * get_cell_length (dimensions, column_index)
                    * get_cell_width  (dimensions, row_index)
                    )
                    / (get_cell_height (dimensions, layer_index) / 2.0) ;

        case TDICE_LAYER_CHANNEL_4RM :

            if (IS_CHANNEL_COLUMN (TDICE_CHANNEL_MODEL_MC_4RM, column_index))

                return    tgrid->Channel->Coolant.HTCBottom
                        * get_cell_width  (dimensions, row_index)
                        * get_cell_length (dimensions, column_index) ;

            else

                // We assume that layer_index is not the top most layer
                // or the bottom most layer

                return (  tgrid->TCProfile [ layer_index ]
                        * get_cell_length (dimensions, column_index)
                        * get_cell_width  (dimensions, row_index)
                       )
                        / (get_cell_height (dimensions, layer_index) / 2.0) ;

        case TDICE_LAYER_CHANNEL_2RM :

            return    tgrid->Channel->Coolant.HTCBottom
                    * get_cell_width  (dimensions, row_index)
                    * get_cell_length (dimensions, column_index) ;

        case TDICE_LAYER_PINFINS_INLINE :

            return    EFFECTIVE_HTC_PF_INLINE (tgrid->Channel->Coolant.DarcyVelocity)
                    * get_cell_width  (dimensions, row_index)
                    * get_cell_length (dimensions, column_index) ;

        case TDICE_LAYER_PINFINS_STAGGERED :

            return    EFFECTIVE_HTC_PF_STAGGERED (tgrid->Channel->Coolant.DarcyVelocity)
                    * get_cell_width  (dimensions, row_index)
                    * get_cell_length (dimensions, column_index) ;

        case TDICE_LAYER_VWALL_CHANNEL :
        case TDICE_LAYER_VWALL_PINFINS :

            return (  tgrid->TCProfile [ layer_index ]
                    * get_cell_length (dimensions, column_index)
                    * get_cell_width  (dimensions, row_index)
                   )
                    / (get_cell_height (dimensions, layer_index) / 2.0)
                    * (1.0 - tgrid->Channel->Porosity) ;

        case TDICE_LAYER_TOP_WALL :
        case TDICE_LAYER_BOTTOM_WALL :

            return 0.0 ;

        case TDICE_LAYER_NONE :

            fprintf (stderr, "ERROR: unset layer type\n") ;

            return 0.0 ;

        default :

            fprintf (stderr, "ERROR: unknown layer type %d\n",
                tgrid->LayersProfile [layer_index]) ;

            return 0.0 ;
    }
}

/******************************************************************************/

Conductance_t get_conductance_north
(
    ThermalGrid_t *tgrid,
    Dimensions_t  *dimensions,
    CellIndex_t    layer_index,
    CellIndex_t    row_index,
    CellIndex_t    column_index
)
{
    if (layer_index > tgrid->Size)
    {
        fprintf (stderr,
            "ERROR: layer index %d is out of range\n", layer_index) ;

        return 0.0 ;
    }

    switch (tgrid->LayersProfile [layer_index])
    {
        case TDICE_LAYER_SOLID :
        case TDICE_LAYER_SOURCE :
            case TDICE_LAYER_SOLID_CONNECTED_TO_AMBIENT :
        case TDICE_LAYER_SOURCE_CONNECTED_TO_AMBIENT :

            return (  tgrid->TCProfile [ layer_index ]
                    * get_cell_length (dimensions, column_index)
                    * get_cell_height (dimensions, layer_index)
                   )
                    / (get_cell_width (dimensions, row_index) / 2.0) ;

        case TDICE_LAYER_CHANNEL_4RM :

            if (IS_CHANNEL_COLUMN (TDICE_CHANNEL_MODEL_MC_4RM, column_index))

                return get_convective_term

                    (tgrid->Channel, dimensions, layer_index, row_index, column_index) ;

            else

                return (  tgrid->TCProfile [ layer_index ]
                        * get_cell_length (dimensions, column_index)
                        * get_cell_height (dimensions, layer_index)
                       )
                        / (get_cell_width (dimensions, row_index) / 2.0) ;

        case TDICE_LAYER_CHANNEL_2RM :
        case TDICE_LAYER_PINFINS_INLINE :
        case TDICE_LAYER_PINFINS_STAGGERED :

            return get_convective_term

                    (tgrid->Channel, dimensions, layer_index, row_index, column_index) ;

        case TDICE_LAYER_VWALL_CHANNEL :

            return (  tgrid->TCProfile [ layer_index ]
                    * get_cell_length (dimensions, column_index)
                    * get_cell_height (dimensions, layer_index)
                   )
                    / (get_cell_width (dimensions, row_index) / 2.0)
                    * (1.0 - tgrid->Channel->Porosity) ;

        case TDICE_LAYER_SPREADER :
        case TDICE_LAYER_SINK :
        case TDICE_LAYER_VWALL_PINFINS :
        case TDICE_LAYER_TOP_WALL :
        case TDICE_LAYER_BOTTOM_WALL :

            return 0.0 ;

        case TDICE_LAYER_NONE :

            fprintf (stderr, "ERROR: unset layer type\n") ;

            return 0.0 ;

        default :

            fprintf (stderr, "ERROR: unknown layer type %d\n",
                tgrid->LayersProfile [layer_index]) ;

            return 0.0 ;
    }
}

/******************************************************************************/

Conductance_t get_conductance_south
(
    ThermalGrid_t *tgrid,
    Dimensions_t  *dimensions,
    CellIndex_t    layer_index,
    CellIndex_t    row_index,
    CellIndex_t    column_index
)
{
    if (layer_index > tgrid->Size)
    {
        fprintf (stderr,
            "ERROR: layer index %d is out of range\n", layer_index) ;

        return 0.0 ;
    }

    switch (tgrid->LayersProfile [layer_index])
    {
        case TDICE_LAYER_SOLID :
        case TDICE_LAYER_SOURCE :
        case TDICE_LAYER_SOLID_CONNECTED_TO_AMBIENT :
        case TDICE_LAYER_SOURCE_CONNECTED_TO_AMBIENT :

            return (  tgrid->TCProfile [ layer_index ]
                    * get_cell_length (dimensions, column_index)
                    * get_cell_height (dimensions, layer_index)
                   )
                    / (get_cell_width (dimensions, row_index) / 2.0) ;

        case TDICE_LAYER_CHANNEL_4RM :

            if (IS_CHANNEL_COLUMN (TDICE_CHANNEL_MODEL_MC_4RM, column_index))

                return - get_convective_term

                    (tgrid->Channel, dimensions, layer_index, row_index, column_index) ;

            else

                return (  tgrid->TCProfile [ layer_index ]
                        * get_cell_length (dimensions, column_index)
                        * get_cell_height (dimensions, layer_index)
                       )
                        / (get_cell_width (dimensions, row_index) / 2.0) ;

        case TDICE_LAYER_CHANNEL_2RM :
        case TDICE_LAYER_PINFINS_INLINE :
        case TDICE_LAYER_PINFINS_STAGGERED :

            return - get_convective_term

                (tgrid->Channel, dimensions, layer_index, row_index, column_index) ;

        case TDICE_LAYER_VWALL_CHANNEL :

            return (  tgrid->TCProfile [ layer_index ]
                    * get_cell_length (dimensions, column_index)
                    * get_cell_height (dimensions, layer_index)
                   )
                    / (get_cell_width (dimensions, row_index) / 2.0)
                    * (1.0 - tgrid->Channel->Porosity) ;

        case TDICE_LAYER_SPREADER :
        case TDICE_LAYER_SINK :
        case TDICE_LAYER_VWALL_PINFINS :
        case TDICE_LAYER_TOP_WALL :
        case TDICE_LAYER_BOTTOM_WALL :

            return 0.0 ;

        case TDICE_LAYER_NONE :

            fprintf (stderr, "ERROR: unset layer type\n") ;

            return 0.0 ;

        default :

            fprintf (stderr, "ERROR: unknown layer type %d\n",
                tgrid->LayersProfile [layer_index]) ;

            return 0.0 ;
    }
}

/******************************************************************************/

Conductance_t get_conductance_east
(
    ThermalGrid_t *tgrid,
    Dimensions_t  *dimensions,
    CellIndex_t    layer_index,
    CellIndex_t    row_index,
    CellIndex_t    column_index
)
{
    if (layer_index > tgrid->Size)
    {
        fprintf (stderr,
            "ERROR: layer index %d is out of range\n", layer_index) ;

        return 0.0 ;
    }

    switch (tgrid->LayersProfile [layer_index])
    {
        case TDICE_LAYER_SOLID :
        case TDICE_LAYER_SOURCE :
        case TDICE_LAYER_SOLID_CONNECTED_TO_AMBIENT :
        case TDICE_LAYER_SOURCE_CONNECTED_TO_AMBIENT :

            return (  tgrid->TCProfile [ layer_index ]
                    * get_cell_width  (dimensions, row_index)
                    * get_cell_height (dimensions, layer_index)
                   )
                    / (get_cell_length (dimensions, column_index) / 2.0) ;

        case TDICE_LAYER_CHANNEL_4RM :

            if (IS_CHANNEL_COLUMN (TDICE_CHANNEL_MODEL_MC_4RM, column_index))

                return tgrid->Channel->Coolant.HTCSide
                    * get_cell_width  (dimensions, row_index)
                    * get_cell_height (dimensions, layer_index) ;

            else

                return (  tgrid->TCProfile [ layer_index ]
                        * get_cell_width  (dimensions, row_index)
                        * get_cell_height (dimensions, layer_index)
                       )
                        / (get_cell_length (dimensions, column_index) / 2.0) ;

        case TDICE_LAYER_CHANNEL_2RM :
        case TDICE_LAYER_PINFINS_INLINE :
        case TDICE_LAYER_PINFINS_STAGGERED :
        case TDICE_LAYER_SPREADER :
        case TDICE_LAYER_SINK :
        case TDICE_LAYER_VWALL_CHANNEL :
        case TDICE_LAYER_VWALL_PINFINS :
        case TDICE_LAYER_TOP_WALL :
        case TDICE_LAYER_BOTTOM_WALL :

            return 0.0 ;

        case TDICE_LAYER_NONE :

            fprintf (stderr, "ERROR: unset layer type\n") ;

            return 0.0 ;

        default :

            fprintf (stderr, "ERROR: unknown layer type %d\n",
                tgrid->LayersProfile [layer_index]) ;

            return 0.0 ;
    }
}

/******************************************************************************/

Conductance_t get_conductance_west
(
    ThermalGrid_t *tgrid,
    Dimensions_t  *dimensions,
    CellIndex_t    layer_index,
    CellIndex_t    row_index,
    CellIndex_t    column_index
)
{
    if (layer_index > tgrid->Size)
    {
        fprintf (stderr,
            "ERROR: layer index %d is out of range\n", layer_index) ;

        return 0.0 ;
    }

    switch (tgrid->LayersProfile [layer_index])
    {
        case TDICE_LAYER_SOLID :
        case TDICE_LAYER_SOURCE :
        case TDICE_LAYER_SOLID_CONNECTED_TO_AMBIENT :
        case TDICE_LAYER_SOURCE_CONNECTED_TO_AMBIENT :

            return (  tgrid->TCProfile [ layer_index ]
                    * get_cell_width  (dimensions, row_index)
                    * get_cell_height (dimensions, layer_index)
                   )
                    / (get_cell_length (dimensions, column_index) / 2.0) ;

        case TDICE_LAYER_CHANNEL_4RM :

            if (IS_CHANNEL_COLUMN (TDICE_CHANNEL_MODEL_MC_4RM, column_index))

                return tgrid->Channel->Coolant.HTCSide
                        * get_cell_width  (dimensions, row_index)
                        * get_cell_height (dimensions, layer_index) ;

            else

                return (  tgrid->TCProfile [ layer_index ]
                        * get_cell_width  (dimensions, row_index)
                        * get_cell_height (dimensions, layer_index)
                       )
                        / (get_cell_length (dimensions, column_index) / 2.0) ;

        case TDICE_LAYER_CHANNEL_2RM :
        case TDICE_LAYER_PINFINS_INLINE :
        case TDICE_LAYER_PINFINS_STAGGERED :
        case TDICE_LAYER_SPREADER :
        case TDICE_LAYER_SINK :
        case TDICE_LAYER_VWALL_CHANNEL :
        case TDICE_LAYER_VWALL_PINFINS :
        case TDICE_LAYER_TOP_WALL :
        case TDICE_LAYER_BOTTOM_WALL :

            return 0.0 ;

        case TDICE_LAYER_NONE :

            fprintf (stderr, "ERROR: unset layer type\n") ;

            return 0.0 ;

        default :

            fprintf (stderr, "ERROR: unknown layer type %d\n",
                tgrid->LayersProfile [layer_index]) ;

            return 0.0 ;
    }
}

/******************************************************************************/
