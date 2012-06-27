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

#include <stdlib.h>

#include "macros.h"
#include "thermal_grid.h"

/******************************************************************************/

void init_thermal_grid (ThermalGrid_t *this)
{
    this->Size             = 0u ;
    this->LayersProfile    = NULL ;
    this->VHCProfile       = NULL ;
    this->TCProfile        = NULL ;
    this->Channel          = NULL ;
    this->HeatSink         = NULL ;
}

/******************************************************************************/

Error_t build_thermal_grid (ThermalGrid_t *this, Quantity_t size)
{
    this->Size = size ;

    this->LayersProfile =

        (StackLayerType_t *) calloc (size, sizeof (StackLayerType_t)) ;

    if (this->LayersProfile == NULL)

        return TDICE_FAILURE ;

    this->VHCProfile = (SolidVHC_t *) calloc (size, sizeof (SolidVHC_t)) ;

    if (this->VHCProfile == NULL)
    {
        FREE_POINTER (free, this->LayersProfile) ;

        return TDICE_FAILURE ;
    }

    this->TCProfile = (SolidTC_t *) calloc (size, sizeof (SolidTC_t)) ;

    if (this->TCProfile == NULL)
    {
        FREE_POINTER (free, this->VHCProfile) ;
        FREE_POINTER (free, this->LayersProfile) ;

        return TDICE_FAILURE ;
    }

    return TDICE_SUCCESS ;
}

/******************************************************************************/

void destroy_thermal_grid (ThermalGrid_t *this)
{
    FREE_POINTER (free, this->LayersProfile) ;
    FREE_POINTER (free, this->VHCProfile) ;
    FREE_POINTER (free, this->TCProfile) ;
}

/******************************************************************************/

void fill_thermal_grid
(
    ThermalGrid_t  *this,
    StackElement_t *list,
    Dimensions_t   *dimensions
)
{
    FOR_EVERY_ELEMENT_IN_LIST_NEXT (StackElement_t, stack_element, list)
    {
        CellIndex_t index = stack_element->Offset ;

        switch (stack_element->Type)
        {
            case TDICE_STACK_ELEMENT_DIE :
            {
                CellIndex_t tmp = 0u ;

                FOR_EVERY_ELEMENT_IN_LIST_NEXT

                (Layer_t, layer, stack_element->Pointer.Die->BottomLayer)
                {
                    if (layer == stack_element->Pointer.Die->SourceLayer)

                        this->LayersProfile [index + tmp] = TDICE_LAYER_SOURCE ;

                    else

                        this->LayersProfile [index + tmp] = TDICE_LAYER_SOLID ;

                    this->VHCProfile    [index + tmp] = layer->Material->VolumetricHeatCapacity ;
                    this->TCProfile     [index + tmp] = layer->Material->ThermalConductivity ;

                    tmp++ ;
                }

                break ;
            }
            case TDICE_STACK_ELEMENT_LAYER :
            {
                this->LayersProfile [index] = TDICE_LAYER_SOLID ;
                this->VHCProfile    [index] = stack_element->Pointer.Layer->Material->VolumetricHeatCapacity ;
                this->TCProfile     [index] = stack_element->Pointer.Layer->Material->ThermalConductivity ;

                break ;
            }
            case TDICE_STACK_ELEMENT_CHANNEL :
            {
                this->Channel = stack_element->Pointer.Channel ;

                // Stores the wall material

                CellIndex_t tmp ;

                for (tmp = 0u ; tmp != stack_element->Pointer.Channel->NLayers ; tmp++)
                {
                    this->VHCProfile [index + tmp] = this->Channel->WallMaterial->VolumetricHeatCapacity ;
                    this->TCProfile  [index + tmp] = this->Channel->WallMaterial->ThermalConductivity ;
                }

                switch (this->Channel->ChannelModel)
                {
                    case TDICE_CHANNEL_MODEL_MC_4RM :

                        this->LayersProfile [index] = TDICE_LAYER_CHANNEL_4RM ;

                        break ;

                    case TDICE_CHANNEL_MODEL_MC_2RM :

                        this->LayersProfile [index    ] = TDICE_LAYER_BOTTOM_WALL ;
                        this->LayersProfile [index + 1] = TDICE_LAYER_VWALL_CHANNEL ;
                        this->LayersProfile [index + 2] = TDICE_LAYER_CHANNEL_2RM ;
                        this->LayersProfile [index + 3] = TDICE_LAYER_TOP_WALL ;

                        break ;

                    case TDICE_CHANNEL_MODEL_PF_INLINE :

                        this->LayersProfile [index    ] = TDICE_LAYER_BOTTOM_WALL ;
                        this->LayersProfile [index + 1] = TDICE_LAYER_VWALL_PINFINS ;
                        this->LayersProfile [index + 2] = TDICE_LAYER_PINFINS_INLINE ;
                        this->LayersProfile [index + 3] = TDICE_LAYER_TOP_WALL ;

                        break ;

                    case TDICE_CHANNEL_MODEL_PF_STAGGERED :

                        this->LayersProfile [index    ] = TDICE_LAYER_BOTTOM_WALL ;
                        this->LayersProfile [index + 1] = TDICE_LAYER_VWALL_PINFINS ;
                        this->LayersProfile [index + 2] = TDICE_LAYER_PINFINS_STAGGERED ;
                        this->LayersProfile [index + 3] = TDICE_LAYER_TOP_WALL ;

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
                this->HeatSink = stack_element->Pointer.HeatSink ;

                switch (this->HeatSink->SinkModel)
                {
                    case TDICE_HEATSINK_MODEL_CONNECTION_TO_AMBIENT :

                        if (this->LayersProfile [index] == TDICE_LAYER_SOLID)

                            this->LayersProfile [index] = TDICE_LAYER_SOLID_CONNECTED_TO_AMBIENT ;

                        else if (this->LayersProfile [index] == TDICE_LAYER_SOURCE)

                            this->LayersProfile [index] = TDICE_LAYER_SOURCE_CONNECTED_TO_AMBIENT ;

                        else

                            fprintf (stderr, "ERROR: Wrong offset of heat sink stack element\n") ;

                        this->VHCProfile    [index] = this->HeatSink->SinkMaterial->VolumetricHeatCapacity ;
                        this->TCProfile     [index] = this->HeatSink->SinkMaterial->ThermalConductivity ;

                        break ;

                    case TDICE_HEATSINK_MODEL_TRADITIONAL :

                        this->LayersProfile [index] = TDICE_LAYER_SPREADER ;

                        this->VHCProfile [index] = get_spreader_volumetric_heat_capacity

                            (this->HeatSink, get_chip_area(dimensions)) ;

                        this->TCProfile  [index] = get_spreader_thermal_conductivity

                            (this->HeatSink, get_chip_area(dimensions)) ;

                        this->LayersProfile [index + 1] = TDICE_LAYER_SINK ;

                        this->VHCProfile [index + 1] = get_sink_volumetric_heat_capacity

                            (this->HeatSink, get_chip_area(dimensions)) ;

                        this->TCProfile [index + 1] = get_sink_thermal_conductivity

                             (this->HeatSink, get_chip_area(dimensions)) ;

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
    ThermalGrid_t *this,
    Dimensions_t  *dimensions,
    CellIndex_t    layer_index,
    CellIndex_t    row_index,
    CellIndex_t    column_index
)
{
    if (layer_index > this->Size)
    {
        fprintf (stderr,
            "ERROR: layer index %d is out of range\n", layer_index) ;

        return 0.0 ;
    }

    switch (this->LayersProfile [layer_index])
    {
        case TDICE_LAYER_SOLID :
        case TDICE_LAYER_SOURCE :
        case TDICE_LAYER_SOLID_CONNECTED_TO_AMBIENT :
        case TDICE_LAYER_SOURCE_CONNECTED_TO_AMBIENT :
        case TDICE_LAYER_SPREADER :
        case TDICE_LAYER_SINK :

            return (  this->VHCProfile [ layer_index ]
                    * get_cell_length (dimensions, column_index)
                    * get_cell_width  (dimensions, row_index)
                    * get_cell_height (dimensions, layer_index)
                   ) ;

        case TDICE_LAYER_CHANNEL_4RM :

            if (IS_CHANNEL_COLUMN (TDICE_CHANNEL_MODEL_MC_4RM, column_index))

                return (  this->Channel->Coolant.VHC
                        * get_cell_length (dimensions, column_index)
                        * get_cell_width  (dimensions, row_index)
                        * get_cell_height (dimensions, layer_index)
                       ) ;

            else

                return (  this->VHCProfile [ layer_index ]
                        * get_cell_length (dimensions, column_index)
                        * get_cell_width  (dimensions, row_index)
                        * get_cell_height (dimensions, layer_index)
                       ) ;

        case TDICE_LAYER_CHANNEL_2RM :
        case TDICE_LAYER_PINFINS_INLINE :
        case TDICE_LAYER_PINFINS_STAGGERED :

            return (  this->Channel->Coolant.VHC
                    * this->Channel->Porosity
                    * get_cell_length (dimensions, column_index)
                    * get_cell_width  (dimensions, row_index)
                    * get_cell_height (dimensions, layer_index)
                   ) ;

        case TDICE_LAYER_VWALL_CHANNEL :
        case TDICE_LAYER_VWALL_PINFINS :

            return (  this->VHCProfile [ layer_index ]
                    * (1.0 - this->Channel->Porosity)
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
                this->LayersProfile [layer_index]) ;

            return 0.0 ;
    }
}

/******************************************************************************/

Conductance_t get_conductance_top
(
    ThermalGrid_t *this,
    Dimensions_t  *dimensions,
    CellIndex_t    layer_index,
    CellIndex_t    row_index,
    CellIndex_t    column_index
)
{
    if (layer_index > this->Size)
    {
        fprintf (stderr,
            "ERROR: layer index %d is out of range\n", layer_index) ;

        return 0.0 ;
    }

    switch (this->LayersProfile [layer_index])
    {
        case TDICE_LAYER_SOLID :
        case TDICE_LAYER_SOURCE :

            if (IS_LAST_LAYER (layer_index, dimensions))

                return 0.0 ;

            else if (IS_FIRST_LAYER (layer_index))

                return (  this->TCProfile [ layer_index ]
                        * get_cell_length (dimensions, column_index)
                        * get_cell_width  (dimensions, row_index)
                       )
                        / get_cell_height (dimensions, layer_index) ;

            else

                return (  this->TCProfile [ layer_index ]
                        * get_cell_length (dimensions, column_index)
                        * get_cell_width  (dimensions, row_index)
                       )
                        / (get_cell_height (dimensions, layer_index) / 2.0) ;

        case TDICE_LAYER_SOLID_CONNECTED_TO_AMBIENT :
        case TDICE_LAYER_SOURCE_CONNECTED_TO_AMBIENT :

            return (  2.0
                    * this->TCProfile [ layer_index ]
                    * this->HeatSink->AmbientHTC
                    * get_cell_length (dimensions, column_index)
                    * get_cell_width  (dimensions, row_index)
                   )
                   /
                   (  get_cell_height (dimensions, layer_index)
                    * this->HeatSink->AmbientHTC
                    + 2.0
                    * this->TCProfile [ layer_index ]
                   ) ;

        case TDICE_LAYER_SINK :


            return (  2.0
                    * this->TCProfile [ layer_index ]
                    * this->HeatSink->AmbientHTC
                    * (this->HeatSink->SinkArea / get_chip_area (dimensions))
                    * get_cell_length (dimensions, column_index)
                    * get_cell_width  (dimensions, row_index)
                   )
                   /
                   (  get_cell_height (dimensions, layer_index)
                    * this->HeatSink->AmbientHTC
                    * (this->HeatSink->SinkArea / get_chip_area (dimensions))
                    + 2.0
                    * this->TCProfile [ layer_index ]
                   ) ;

        case TDICE_LAYER_SPREADER :

            return (  this->TCProfile [ layer_index ]
                    * get_cell_length (dimensions, column_index)
                    * get_cell_width  (dimensions, row_index)
                    )
                    / (get_cell_height (dimensions, layer_index) / 2.0) ;

        case TDICE_LAYER_CHANNEL_4RM :

            if (IS_CHANNEL_COLUMN (TDICE_CHANNEL_MODEL_MC_4RM, column_index))

                return    this->Channel->Coolant.HTCTop
                        * get_cell_width  (dimensions, row_index)
                        * get_cell_length (dimensions, column_index) ;

            else

                // We assume that layer_index is not the top most layer
                // or the bottom most layer

                return (  this->TCProfile [ layer_index ]
                        * get_cell_length (dimensions, column_index)
                        * get_cell_width  (dimensions, row_index)
                       )
                        / (get_cell_height (dimensions, layer_index) / 2.0) ;

        case TDICE_LAYER_CHANNEL_2RM :

            return    this->Channel->Coolant.HTCTop
                    * get_cell_width  (dimensions, row_index)
                    * get_cell_length (dimensions, column_index) ;

        case TDICE_LAYER_PINFINS_INLINE :

            return    EFFECTIVE_HTC_PF_INLINE (this->Channel->Coolant.DarcyVelocity)
                    * get_cell_width  (dimensions, row_index)
                    * get_cell_length (dimensions, column_index) ;

        case TDICE_LAYER_PINFINS_STAGGERED :

            return    EFFECTIVE_HTC_PF_STAGGERED (this->Channel->Coolant.DarcyVelocity)
                    * get_cell_width  (dimensions, row_index)
                    * get_cell_length (dimensions, column_index) ;

        case TDICE_LAYER_VWALL_CHANNEL :
        case TDICE_LAYER_VWALL_PINFINS :

            return (  this->TCProfile [ layer_index ]
                    * get_cell_length (dimensions, column_index)
                    * get_cell_width  (dimensions, row_index)
                   )
                    / (get_cell_height (dimensions, layer_index) / 2.0)
                    * (1.0 - this->Channel->Porosity) ;

        case TDICE_LAYER_TOP_WALL :
        case TDICE_LAYER_BOTTOM_WALL :

            return 0.0 ;

        case TDICE_LAYER_NONE :

            fprintf (stderr, "ERROR: unset layer type\n") ;

            return 0.0 ;

        default :

            fprintf (stderr, "ERROR: unknown layer type %d\n",
                this->LayersProfile [layer_index]) ;

            return 0.0 ;
    }
}

/******************************************************************************/

Conductance_t get_conductance_bottom
(
    ThermalGrid_t *this,
    Dimensions_t  *dimensions,
    CellIndex_t    layer_index,
    CellIndex_t    row_index,
    CellIndex_t    column_index
)
{
    if (layer_index > this->Size)
    {
        fprintf (stderr,
            "ERROR: layer index %d is out of range\n", layer_index) ;

        return 0.0 ;
    }

    switch (this->LayersProfile [layer_index])
    {
        case TDICE_LAYER_SOLID :
        case TDICE_LAYER_SOURCE :

            if (IS_FIRST_LAYER (layer_index))

                return 0.0 ;

            else if (IS_LAST_LAYER (layer_index, dimensions))

                return (  this->TCProfile [ layer_index ]
                        * get_cell_length (dimensions, column_index)
                        * get_cell_width  (dimensions, row_index)
                       )
                        / get_cell_height (dimensions, layer_index) ;

            else

                return (  this->TCProfile [ layer_index ]
                        * get_cell_length (dimensions, column_index)
                        * get_cell_width  (dimensions, row_index)
                       )
                        / (get_cell_height (dimensions, layer_index) / 2.0) ;

        case TDICE_LAYER_SOLID_CONNECTED_TO_AMBIENT :
        case TDICE_LAYER_SOURCE_CONNECTED_TO_AMBIENT :
        case TDICE_LAYER_SPREADER :
        case TDICE_LAYER_SINK :

            return (  this->TCProfile [ layer_index ]
                    * get_cell_length (dimensions, column_index)
                    * get_cell_width  (dimensions, row_index)
                    )
                    / (get_cell_height (dimensions, layer_index) / 2.0) ;

        case TDICE_LAYER_CHANNEL_4RM :

            if (IS_CHANNEL_COLUMN (TDICE_CHANNEL_MODEL_MC_4RM, column_index))

                return    this->Channel->Coolant.HTCBottom
                        * get_cell_width  (dimensions, row_index)
                        * get_cell_length (dimensions, column_index) ;

            else

                // We assume that layer_index is not the top most layer
                // or the bottom most layer

                return (  this->TCProfile [ layer_index ]
                        * get_cell_length (dimensions, column_index)
                        * get_cell_width  (dimensions, row_index)
                       )
                        / (get_cell_height (dimensions, layer_index) / 2.0) ;

        case TDICE_LAYER_CHANNEL_2RM :

            return    this->Channel->Coolant.HTCBottom
                    * get_cell_width  (dimensions, row_index)
                    * get_cell_length (dimensions, column_index) ;

        case TDICE_LAYER_PINFINS_INLINE :

            return    EFFECTIVE_HTC_PF_INLINE (this->Channel->Coolant.DarcyVelocity)
                    * get_cell_width  (dimensions, row_index)
                    * get_cell_length (dimensions, column_index) ;

        case TDICE_LAYER_PINFINS_STAGGERED :

            return    EFFECTIVE_HTC_PF_STAGGERED (this->Channel->Coolant.DarcyVelocity)
                    * get_cell_width  (dimensions, row_index)
                    * get_cell_length (dimensions, column_index) ;

        case TDICE_LAYER_VWALL_CHANNEL :
        case TDICE_LAYER_VWALL_PINFINS :

            return (  this->TCProfile [ layer_index ]
                    * get_cell_length (dimensions, column_index)
                    * get_cell_width  (dimensions, row_index)
                   )
                    / (get_cell_height (dimensions, layer_index) / 2.0)
                    * (1.0 - this->Channel->Porosity) ;

        case TDICE_LAYER_TOP_WALL :
        case TDICE_LAYER_BOTTOM_WALL :

            return 0.0 ;

        case TDICE_LAYER_NONE :

            fprintf (stderr, "ERROR: unset layer type\n") ;

            return 0.0 ;

        default :

            fprintf (stderr, "ERROR: unknown layer type %d\n",
                this->LayersProfile [layer_index]) ;

            return 0.0 ;
    }
}

/******************************************************************************/

Conductance_t get_conductance_north
(
    ThermalGrid_t *this,
    Dimensions_t  *dimensions,
    CellIndex_t    layer_index,
    CellIndex_t    row_index,
    CellIndex_t    column_index
)
{
    if (layer_index > this->Size)
    {
        fprintf (stderr,
            "ERROR: layer index %d is out of range\n", layer_index) ;

        return 0.0 ;
    }

    switch (this->LayersProfile [layer_index])
    {
        case TDICE_LAYER_SOLID :
        case TDICE_LAYER_SOURCE :
        case TDICE_LAYER_SOLID_CONNECTED_TO_AMBIENT :
        case TDICE_LAYER_SOURCE_CONNECTED_TO_AMBIENT :

            return (  this->TCProfile [ layer_index ]
                    * get_cell_length (dimensions, column_index)
                    * get_cell_height (dimensions, layer_index)
                   )
                    / (get_cell_width (dimensions, row_index) / 2.0) ;

        case TDICE_LAYER_CHANNEL_4RM :

            if (IS_CHANNEL_COLUMN (TDICE_CHANNEL_MODEL_MC_4RM, column_index))

                return get_convective_term

                    (this->Channel, dimensions, layer_index, row_index, column_index) ;

            else

                return (  this->TCProfile [ layer_index ]
                        * get_cell_length (dimensions, column_index)
                        * get_cell_height (dimensions, layer_index)
                       )
                        / (get_cell_width (dimensions, row_index) / 2.0) ;

        case TDICE_LAYER_CHANNEL_2RM :
        case TDICE_LAYER_PINFINS_INLINE :
        case TDICE_LAYER_PINFINS_STAGGERED :

            return get_convective_term

                    (this->Channel, dimensions, layer_index, row_index, column_index) ;

        case TDICE_LAYER_VWALL_CHANNEL :

            return (  this->TCProfile [ layer_index ]
                    * get_cell_length (dimensions, column_index)
                    * get_cell_height (dimensions, layer_index)
                   )
                    / (get_cell_width (dimensions, row_index) / 2.0)
                    * (1.0 - this->Channel->Porosity) ;

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
                this->LayersProfile [layer_index]) ;

            return 0.0 ;
    }
}

/******************************************************************************/

Conductance_t get_conductance_south
(
    ThermalGrid_t *this,
    Dimensions_t  *dimensions,
    CellIndex_t    layer_index,
    CellIndex_t    row_index,
    CellIndex_t    column_index
)
{
    if (layer_index > this->Size)
    {
        fprintf (stderr,
            "ERROR: layer index %d is out of range\n", layer_index) ;

        return 0.0 ;
    }

    switch (this->LayersProfile [layer_index])
    {
        case TDICE_LAYER_SOLID :
        case TDICE_LAYER_SOURCE :
        case TDICE_LAYER_SOLID_CONNECTED_TO_AMBIENT :
        case TDICE_LAYER_SOURCE_CONNECTED_TO_AMBIENT :

            return (  this->TCProfile [ layer_index ]
                    * get_cell_length (dimensions, column_index)
                    * get_cell_height (dimensions, layer_index)
                   )
                    / (get_cell_width (dimensions, row_index) / 2.0) ;

        case TDICE_LAYER_CHANNEL_4RM :

            if (IS_CHANNEL_COLUMN (TDICE_CHANNEL_MODEL_MC_4RM, column_index))

                return - get_convective_term

                    (this->Channel, dimensions, layer_index, row_index, column_index) ;

            else

                return (  this->TCProfile [ layer_index ]
                        * get_cell_length (dimensions, column_index)
                        * get_cell_height (dimensions, layer_index)
                       )
                        / (get_cell_width (dimensions, row_index) / 2.0) ;

        case TDICE_LAYER_CHANNEL_2RM :
        case TDICE_LAYER_PINFINS_INLINE :
        case TDICE_LAYER_PINFINS_STAGGERED :

            return - get_convective_term

                (this->Channel, dimensions, layer_index, row_index, column_index) ;

        case TDICE_LAYER_VWALL_CHANNEL :

            return (  this->TCProfile [ layer_index ]
                    * get_cell_length (dimensions, column_index)
                    * get_cell_height (dimensions, layer_index)
                   )
                    / (get_cell_width (dimensions, row_index) / 2.0)
                    * (1.0 - this->Channel->Porosity) ;

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
                this->LayersProfile [layer_index]) ;

            return 0.0 ;
    }
}

/******************************************************************************/

Conductance_t get_conductance_east
(
    ThermalGrid_t *this,
    Dimensions_t  *dimensions,
    CellIndex_t    layer_index,
    CellIndex_t    row_index,
    CellIndex_t    column_index
)
{
    if (layer_index > this->Size)
    {
        fprintf (stderr,
            "ERROR: layer index %d is out of range\n", layer_index) ;

        return 0.0 ;
    }

    switch (this->LayersProfile [layer_index])
    {
        case TDICE_LAYER_SOLID :
        case TDICE_LAYER_SOURCE :
        case TDICE_LAYER_SOLID_CONNECTED_TO_AMBIENT :
        case TDICE_LAYER_SOURCE_CONNECTED_TO_AMBIENT :

            return (  this->TCProfile [ layer_index ]
                    * get_cell_width  (dimensions, row_index)
                    * get_cell_height (dimensions, layer_index)
                   )
                    / (get_cell_length (dimensions, column_index) / 2.0) ;

        case TDICE_LAYER_CHANNEL_4RM :

            if (IS_CHANNEL_COLUMN (TDICE_CHANNEL_MODEL_MC_4RM, column_index))

                return this->Channel->Coolant.HTCSide
                    * get_cell_width  (dimensions, row_index)
                    * get_cell_height (dimensions, layer_index) ;

            else

                return (  this->TCProfile [ layer_index ]
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
                this->LayersProfile [layer_index]) ;

            return 0.0 ;
    }
}

/******************************************************************************/

Conductance_t get_conductance_west
(
    ThermalGrid_t *this,
    Dimensions_t  *dimensions,
    CellIndex_t    layer_index,
    CellIndex_t    row_index,
    CellIndex_t    column_index
)
{
    if (layer_index > this->Size)
    {
        fprintf (stderr,
            "ERROR: layer index %d is out of range\n", layer_index) ;

        return 0.0 ;
    }

    switch (this->LayersProfile [layer_index])
    {
        case TDICE_LAYER_SOLID :
        case TDICE_LAYER_SOURCE :
        case TDICE_LAYER_SOLID_CONNECTED_TO_AMBIENT :
        case TDICE_LAYER_SOURCE_CONNECTED_TO_AMBIENT :

            return (  this->TCProfile [ layer_index ]
                    * get_cell_width  (dimensions, row_index)
                    * get_cell_height (dimensions, layer_index)
                   )
                    / (get_cell_length (dimensions, column_index) / 2.0) ;

        case TDICE_LAYER_CHANNEL_4RM :

            if (IS_CHANNEL_COLUMN (TDICE_CHANNEL_MODEL_MC_4RM, column_index))

                return this->Channel->Coolant.HTCSide
                        * get_cell_width  (dimensions, row_index)
                        * get_cell_height (dimensions, layer_index) ;

            else

                return (  this->TCProfile [ layer_index ]
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
                this->LayersProfile [layer_index]) ;

            return 0.0 ;
    }
}

/******************************************************************************/
