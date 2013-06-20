/******************************************************************************
 * This file is part of 3D-ICE, version 2.2.4 .                               *
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

#include <stdlib.h> // For the memory functions calloc/free

#include "thermal_grid.h"
#include "macros.h"

/******************************************************************************/

void thermal_grid_init (ThermalGrid_t *tgrid)
{
    tgrid->NLayers           = (CellIndex_t) 0u ;
    tgrid->LayersTypeProfile = NULL ;
    tgrid->LayersProfile     = NULL ;
    tgrid->Channel           = NULL ;
    tgrid->TopHeatSink       = NULL ;
    tgrid->BottomHeatSink    = NULL ;
}

/******************************************************************************/

Error_t thermal_grid_build (ThermalGrid_t *tgrid, Dimensions_t *dimensions)
{
    tgrid->NLayers = get_number_of_layers (dimensions) ;

    tgrid->LayersTypeProfile =

        (StackLayerType_t *) calloc (tgrid->NLayers, sizeof (StackLayerType_t)) ;

    if (tgrid->LayersTypeProfile == NULL)

        return TDICE_FAILURE ;

    tgrid->LayersProfile = (Layer_t *) calloc (tgrid->NLayers, sizeof (Layer_t)) ;

    if (tgrid->LayersProfile == NULL)
    {
        free (tgrid->LayersTypeProfile) ;

        return TDICE_FAILURE ;
    }

    CellIndex_t lindex = 0 ;

    while (lindex != tgrid->NLayers)
    {
        tgrid->LayersTypeProfile [lindex] = TDICE_LAYER_NONE ;

        layer_init (tgrid->LayersProfile + lindex) ;

        lindex ++ ;
    }

    return TDICE_SUCCESS ;
}

/******************************************************************************/

void thermal_grid_destroy (ThermalGrid_t *tgrid)
{
    if (tgrid->LayersTypeProfile != NULL ) free (tgrid->LayersTypeProfile) ;
    if (tgrid->LayersProfile     != NULL )
    {
        CellIndex_t lindex = 0 ;

        while (lindex != tgrid->NLayers)
        {
            layer_destroy (tgrid->LayersProfile + lindex) ;

            lindex ++ ;
        }
        free (tgrid->LayersProfile) ;
    }

    thermal_grid_init (tgrid) ;
}

/******************************************************************************/

void thermal_grid_fill (ThermalGrid_t *tgrid, StackElementList_t *list)
{
    StackElementListNode_t *stkeln ;

    for (stkeln  = stack_element_list_end (list) ;
         stkeln != NULL ;
         stkeln  = stack_element_list_prev (stkeln))
    {
        StackElement_t *stack_element = stack_element_list_data (stkeln) ;

        CellIndex_t index = stack_element->Offset ;

        switch (stack_element->SEType)
        {
            case TDICE_STACK_ELEMENT_DIE :
            {
                CellIndex_t tmp = 0u ;
                LayerListNode_t *lnd ;

                for (lnd  = layer_list_end (&stack_element->Pointer.Die->Layers) ;
                     lnd != NULL ;
                     lnd  = layer_list_prev (lnd))
                {
                    tgrid->LayersTypeProfile [index + tmp] = TDICE_LAYER_SOLID ;

                    layer_copy (tgrid->LayersProfile + index + tmp, layer_list_data(lnd)) ;

                    tmp++ ;
                }

                tmp = stack_element->Pointer.Die->SourceLayerOffset ;

                tgrid->LayersTypeProfile [index + tmp] = TDICE_LAYER_SOURCE ;

                break ;
            }
            case TDICE_STACK_ELEMENT_LAYER :
            {
                tgrid->LayersTypeProfile [index] = TDICE_LAYER_SOLID ;

                layer_copy (tgrid->LayersProfile + index, stack_element->Pointer.Layer) ;

                break ;
            }
            case TDICE_STACK_ELEMENT_CHANNEL :
            {
                tgrid->Channel = stack_element->Pointer.Channel ;

                // Stores the wall material as a new layer (without ID and layout)

                Layer_t tmplayer ;

                layer_init (&tmplayer) ;

                tmplayer.Height = stack_element->Pointer.Channel->Height ;

                material_copy (&(tmplayer.Material), &(stack_element->Pointer.Channel->WallMaterial)) ;

                CellIndex_t tmp ;

                for (tmp = 0u ; tmp != stack_element->Pointer.Channel->NLayers ; tmp++)

                    layer_copy (tgrid->LayersProfile + index + tmp, &tmplayer) ;

                layer_destroy (&tmplayer) ;

                switch (stack_element->Pointer.Channel->ChannelModel)
                {
                    case TDICE_CHANNEL_MODEL_MC_4RM :

                        tgrid->LayersTypeProfile [index] = TDICE_LAYER_CHANNEL_4RM ;

                        break ;

                    case TDICE_CHANNEL_MODEL_MC_2RM :

                        tgrid->LayersTypeProfile [index    ] = TDICE_LAYER_BOTTOM_WALL ;
                        tgrid->LayersTypeProfile [index + 1] = TDICE_LAYER_VWALL_CHANNEL ;
                        tgrid->LayersTypeProfile [index + 2] = TDICE_LAYER_CHANNEL_2RM ;
                        tgrid->LayersTypeProfile [index + 3] = TDICE_LAYER_TOP_WALL ;

                        break ;

                    case TDICE_CHANNEL_MODEL_PF_INLINE :

                        tgrid->LayersTypeProfile [index    ] = TDICE_LAYER_BOTTOM_WALL ;
                        tgrid->LayersTypeProfile [index + 1] = TDICE_LAYER_VWALL_PINFINS ;
                        tgrid->LayersTypeProfile [index + 2] = TDICE_LAYER_PINFINS_INLINE ;
                        tgrid->LayersTypeProfile [index + 3] = TDICE_LAYER_TOP_WALL ;

                        break ;

                    case TDICE_CHANNEL_MODEL_PF_STAGGERED :

                        tgrid->LayersTypeProfile [index    ] = TDICE_LAYER_BOTTOM_WALL ;
                        tgrid->LayersTypeProfile [index + 1] = TDICE_LAYER_VWALL_PINFINS ;
                        tgrid->LayersTypeProfile [index + 2] = TDICE_LAYER_PINFINS_STAGGERED ;
                        tgrid->LayersTypeProfile [index + 3] = TDICE_LAYER_TOP_WALL ;

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
            case TDICE_STACK_ELEMENT_NONE :

                fprintf (stderr, "Error! Found stack element with unset type\n") ;
                break ;

            default :

                fprintf (stderr, "Error! Unknown stack element type %d\n", stack_element->SEType) ;

        } /* switch stack_element->Type */
    }

    StackElement_t *bmost = stack_element_list_data (stack_element_list_end   (list)) ;
    StackElement_t *tmost = stack_element_list_data (stack_element_list_begin (list)) ;

    if (tmost->TopSink != NULL)
    {
        tgrid->TopHeatSink = tmost->TopSink ;

        if (tgrid->LayersTypeProfile [tgrid->NLayers - 1] == TDICE_LAYER_SOLID)

            tgrid->LayersTypeProfile [tgrid->NLayers - 1] = TDICE_LAYER_SOLID_CONNECTED_TO_AMBIENT ;

        else if (tgrid->LayersTypeProfile [tgrid->NLayers - 1] == TDICE_LAYER_SOURCE)

            tgrid->LayersTypeProfile [tgrid->NLayers - 1] = TDICE_LAYER_SOURCE_CONNECTED_TO_AMBIENT ;
    }

    if (bmost->BottomSink != NULL)
    {
        tgrid->BottomHeatSink = bmost->BottomSink ;

        if (tgrid->LayersTypeProfile [ 0 ] == TDICE_LAYER_SOLID)

            tgrid->LayersTypeProfile [ 0 ] = TDICE_LAYER_SOLID_CONNECTED_TO_PCB ;

        else if (tgrid->LayersTypeProfile [ 0 ] == TDICE_LAYER_SOURCE)

            tgrid->LayersTypeProfile [ 0 ] = TDICE_LAYER_SOURCE_CONNECTED_TO_PCB ;

        else if (   tgrid->LayersTypeProfile [ 0 ] == TDICE_LAYER_SOLID_CONNECTED_TO_AMBIENT
                 || tgrid->LayersTypeProfile [ 0 ] == TDICE_LAYER_SOURCE_CONNECTED_TO_AMBIENT)

            fprintf (stderr, "Top and bottom sink on the same layer ! not handled yed\n") ;
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
    if (layer_index > tgrid->NLayers)
    {
        fprintf (stderr,
            "ERROR: layer index %d is out of range\n", layer_index) ;

        return 0.0 ;
    }

    switch (tgrid->LayersTypeProfile [layer_index])
    {
        case TDICE_LAYER_SOLID :
        case TDICE_LAYER_SOURCE :
        case TDICE_LAYER_SOLID_CONNECTED_TO_AMBIENT :
        case TDICE_LAYER_SOURCE_CONNECTED_TO_AMBIENT :
        case TDICE_LAYER_SOLID_CONNECTED_TO_PCB :
        case TDICE_LAYER_SOURCE_CONNECTED_TO_PCB :

            return (  get_volumetric_heat_capacity (tgrid->LayersProfile + layer_index,
                                                    row_index, column_index,
                                                    dimensions)
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

                return (  get_volumetric_heat_capacity (tgrid->LayersProfile + layer_index,
                                                        row_index, column_index,
                                                        dimensions)
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

            return (  get_volumetric_heat_capacity (tgrid->LayersProfile + layer_index,
                                                    row_index, column_index,
                                                    dimensions)
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
                tgrid->LayersTypeProfile [layer_index]) ;

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
    if (layer_index > tgrid->NLayers)
    {
        fprintf (stderr,
            "ERROR: layer index %d is out of range\n", layer_index) ;

        return 0.0 ;
    }

    switch (tgrid->LayersTypeProfile [layer_index])
    {
        case TDICE_LAYER_SOLID :
        case TDICE_LAYER_SOURCE :

            if (layer_index == last_layer (dimensions))

                return 0.0 ;

            else if (layer_index == first_layer (dimensions))

                return (  get_thermal_conductivity (tgrid->LayersProfile + layer_index,
                                                    row_index, column_index,
                                                    dimensions)
                        * get_cell_length (dimensions, column_index)
                        * get_cell_width  (dimensions, row_index)
                       )
                        / get_cell_height (dimensions, layer_index) ;

            else

                return (  get_thermal_conductivity (tgrid->LayersProfile + layer_index,
                                                    row_index, column_index,
                                                    dimensions)
                        * get_cell_length (dimensions, column_index)
                        * get_cell_width  (dimensions, row_index)
                       )
                        / (get_cell_height (dimensions, layer_index) / 2.0) ;

        case TDICE_LAYER_SOLID_CONNECTED_TO_AMBIENT :
        case TDICE_LAYER_SOURCE_CONNECTED_TO_AMBIENT :

            return (  2.0
                    * get_thermal_conductivity (tgrid->LayersProfile + layer_index,
                                                row_index, column_index,
                                                dimensions)
                    * tgrid->TopHeatSink->AmbientHTC
                    * get_cell_length (dimensions, column_index)
                    * get_cell_width  (dimensions, row_index)
                   )
                   /
                   (  get_cell_height (dimensions, layer_index)
                    * tgrid->TopHeatSink->AmbientHTC
                    + 2.0
                    * get_thermal_conductivity (tgrid->LayersProfile + layer_index,
                                                row_index, column_index,
                                                dimensions)
                   ) ;

        case TDICE_LAYER_SOLID_CONNECTED_TO_PCB :
        case TDICE_LAYER_SOURCE_CONNECTED_TO_PCB :

            return (  get_thermal_conductivity (tgrid->LayersProfile + layer_index,
                                                row_index, column_index,
                                                dimensions)
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

                return (  get_thermal_conductivity (tgrid->LayersProfile + layer_index,
                                                    row_index, column_index,
                                                    dimensions)
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

            return (  get_thermal_conductivity (tgrid->LayersProfile + layer_index,
                                                row_index, column_index,
                                                dimensions)
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
                tgrid->LayersTypeProfile [layer_index]) ;

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
    if (layer_index > tgrid->NLayers)
    {
        fprintf (stderr,
            "ERROR: layer index %d is out of range\n", layer_index) ;

        return 0.0 ;
    }

    switch (tgrid->LayersTypeProfile [layer_index])
    {
        case TDICE_LAYER_SOLID :
        case TDICE_LAYER_SOURCE :

            if (layer_index == first_layer (dimensions))

                return 0.0 ;

            else if (layer_index == last_layer (dimensions))

                return (  get_thermal_conductivity (tgrid->LayersProfile + layer_index,
                                                    row_index, column_index,
                                                    dimensions)
                        * get_cell_length (dimensions, column_index)
                        * get_cell_width  (dimensions, row_index)
                       )
                        / get_cell_height (dimensions, layer_index) ;

            else

                return (  get_thermal_conductivity (tgrid->LayersProfile + layer_index,
                                                    row_index, column_index,
                                                    dimensions)
                        * get_cell_length (dimensions, column_index)
                        * get_cell_width  (dimensions, row_index)
                       )
                        / (get_cell_height (dimensions, layer_index) / 2.0) ;

        case TDICE_LAYER_SOLID_CONNECTED_TO_AMBIENT :
        case TDICE_LAYER_SOURCE_CONNECTED_TO_AMBIENT :

            return (  get_thermal_conductivity (tgrid->LayersProfile + layer_index,
                                                row_index, column_index,
                                                dimensions)
                    * get_cell_length (dimensions, column_index)
                    * get_cell_width  (dimensions, row_index)
                    )
                    / (get_cell_height (dimensions, layer_index) / 2.0) ;

        case TDICE_LAYER_SOLID_CONNECTED_TO_PCB :
        case TDICE_LAYER_SOURCE_CONNECTED_TO_PCB :

            return (  2.0
                    * get_thermal_conductivity (tgrid->LayersProfile + layer_index,
                                                row_index, column_index,
                                                dimensions)
                    * tgrid->BottomHeatSink->AmbientHTC
                    * get_cell_length (dimensions, column_index)
                    * get_cell_width  (dimensions, row_index)
                   )
                   /
                   (  get_cell_height (dimensions, layer_index)
                    * tgrid->BottomHeatSink->AmbientHTC
                    + 2.0
                    * get_thermal_conductivity (tgrid->LayersProfile + layer_index,
                                                row_index, column_index,
                                                dimensions)
                   ) ;

        case TDICE_LAYER_CHANNEL_4RM :

            if (IS_CHANNEL_COLUMN (TDICE_CHANNEL_MODEL_MC_4RM, column_index))

                return    tgrid->Channel->Coolant.HTCBottom
                        * get_cell_width  (dimensions, row_index)
                        * get_cell_length (dimensions, column_index) ;

            else

                // We assume that layer_index is not the top most layer
                // or the bottom most layer

                return (  get_thermal_conductivity (tgrid->LayersProfile + layer_index,
                                                    row_index, column_index,
                                                    dimensions)
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

            return (  get_thermal_conductivity (tgrid->LayersProfile + layer_index,
                                                row_index, column_index,
                                                dimensions)
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
                tgrid->LayersTypeProfile [layer_index]) ;

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
    if (layer_index > tgrid->NLayers)
    {
        fprintf (stderr,
            "ERROR: layer index %d is out of range\n", layer_index) ;

        return 0.0 ;
    }

    switch (tgrid->LayersTypeProfile [layer_index])
    {
        case TDICE_LAYER_SOLID :
        case TDICE_LAYER_SOURCE :
        case TDICE_LAYER_SOLID_CONNECTED_TO_AMBIENT :
        case TDICE_LAYER_SOURCE_CONNECTED_TO_AMBIENT :
        case TDICE_LAYER_SOLID_CONNECTED_TO_PCB :
        case TDICE_LAYER_SOURCE_CONNECTED_TO_PCB :

            return (  get_thermal_conductivity (tgrid->LayersProfile + layer_index,
                                                row_index, column_index,
                                                dimensions)
                    * get_cell_length (dimensions, column_index)
                    * get_cell_height (dimensions, layer_index)
                   )
                    / (get_cell_width (dimensions, row_index) / 2.0) ;

        case TDICE_LAYER_CHANNEL_4RM :

            if (IS_CHANNEL_COLUMN (TDICE_CHANNEL_MODEL_MC_4RM, column_index))

                return get_convective_term

                    (tgrid->Channel, dimensions, layer_index, row_index, column_index) ;

            else

                return (  get_thermal_conductivity (tgrid->LayersProfile + layer_index,
                                                    row_index, column_index,
                                                    dimensions)
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

            return (  get_thermal_conductivity (tgrid->LayersProfile + layer_index,
                                                row_index, column_index,
                                                dimensions)
                    * get_cell_length (dimensions, column_index)
                    * get_cell_height (dimensions, layer_index)
                   )
                    / (get_cell_width (dimensions, row_index) / 2.0)
                    * (1.0 - tgrid->Channel->Porosity) ;

        case TDICE_LAYER_VWALL_PINFINS :
        case TDICE_LAYER_TOP_WALL :
        case TDICE_LAYER_BOTTOM_WALL :

            return 0.0 ;

        case TDICE_LAYER_NONE :

            fprintf (stderr, "ERROR: unset layer type\n") ;

            return 0.0 ;

        default :

            fprintf (stderr, "ERROR: unknown layer type %d\n",
                tgrid->LayersTypeProfile [layer_index]) ;

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
    if (layer_index > tgrid->NLayers)
    {
        fprintf (stderr,
            "ERROR: layer index %d is out of range\n", layer_index) ;

        return 0.0 ;
    }

    switch (tgrid->LayersTypeProfile [layer_index])
    {
        case TDICE_LAYER_SOLID :
        case TDICE_LAYER_SOURCE :
        case TDICE_LAYER_SOLID_CONNECTED_TO_AMBIENT :
        case TDICE_LAYER_SOURCE_CONNECTED_TO_AMBIENT :
        case TDICE_LAYER_SOLID_CONNECTED_TO_PCB :
        case TDICE_LAYER_SOURCE_CONNECTED_TO_PCB :

            return (  get_thermal_conductivity (tgrid->LayersProfile + layer_index,
                                                row_index, column_index,
                                                dimensions)
                    * get_cell_length (dimensions, column_index)
                    * get_cell_height (dimensions, layer_index)
                   )
                    / (get_cell_width (dimensions, row_index) / 2.0) ;

        case TDICE_LAYER_CHANNEL_4RM :

            if (IS_CHANNEL_COLUMN (TDICE_CHANNEL_MODEL_MC_4RM, column_index))

                return - get_convective_term

                    (tgrid->Channel, dimensions, layer_index, row_index, column_index) ;

            else

                return (  get_thermal_conductivity (tgrid->LayersProfile + layer_index,
                                                    row_index, column_index,
                                                    dimensions)
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

            return (  get_thermal_conductivity (tgrid->LayersProfile + layer_index,
                                                row_index, column_index,
                                                dimensions)
                    * get_cell_length (dimensions, column_index)
                    * get_cell_height (dimensions, layer_index)
                   )
                    / (get_cell_width (dimensions, row_index) / 2.0)
                    * (1.0 - tgrid->Channel->Porosity) ;

        case TDICE_LAYER_VWALL_PINFINS :
        case TDICE_LAYER_TOP_WALL :
        case TDICE_LAYER_BOTTOM_WALL :

            return 0.0 ;

        case TDICE_LAYER_NONE :

            fprintf (stderr, "ERROR: unset layer type\n") ;

            return 0.0 ;

        default :

            fprintf (stderr, "ERROR: unknown layer type %d\n",
                tgrid->LayersTypeProfile [layer_index]) ;

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
    if (layer_index > tgrid->NLayers)
    {
        fprintf (stderr,
            "ERROR: layer index %d is out of range\n", layer_index) ;

        return 0.0 ;
    }

    switch (tgrid->LayersTypeProfile [layer_index])
    {
        case TDICE_LAYER_SOLID :
        case TDICE_LAYER_SOURCE :
        case TDICE_LAYER_SOLID_CONNECTED_TO_AMBIENT :
        case TDICE_LAYER_SOURCE_CONNECTED_TO_AMBIENT :
        case TDICE_LAYER_SOLID_CONNECTED_TO_PCB :
        case TDICE_LAYER_SOURCE_CONNECTED_TO_PCB :

            return (  get_thermal_conductivity (tgrid->LayersProfile + layer_index,
                                                row_index, column_index,
                                                dimensions)
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

                return (  get_thermal_conductivity (tgrid->LayersProfile + layer_index,
                                                    row_index, column_index,
                                                    dimensions)
                        * get_cell_width  (dimensions, row_index)
                        * get_cell_height (dimensions, layer_index)
                       )
                        / (get_cell_length (dimensions, column_index) / 2.0) ;

        case TDICE_LAYER_CHANNEL_2RM :
        case TDICE_LAYER_PINFINS_INLINE :
        case TDICE_LAYER_PINFINS_STAGGERED :
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
                tgrid->LayersTypeProfile [layer_index]) ;

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
    if (layer_index > tgrid->NLayers)
    {
        fprintf (stderr,
            "ERROR: layer index %d is out of range\n", layer_index) ;

        return 0.0 ;
    }

    switch (tgrid->LayersTypeProfile [layer_index])
    {
        case TDICE_LAYER_SOLID :
        case TDICE_LAYER_SOURCE :
        case TDICE_LAYER_SOLID_CONNECTED_TO_AMBIENT :
        case TDICE_LAYER_SOURCE_CONNECTED_TO_AMBIENT :
        case TDICE_LAYER_SOLID_CONNECTED_TO_PCB :
        case TDICE_LAYER_SOURCE_CONNECTED_TO_PCB :

            return (  get_thermal_conductivity (tgrid->LayersProfile + layer_index,
                                                row_index, column_index,
                                                dimensions)
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

                return (  get_thermal_conductivity (tgrid->LayersProfile + layer_index,
                                                    row_index, column_index,
                                                    dimensions)
                        * get_cell_width  (dimensions, row_index)
                        * get_cell_height (dimensions, layer_index)
                       )
                        / (get_cell_length (dimensions, column_index) / 2.0) ;

        case TDICE_LAYER_CHANNEL_2RM :
        case TDICE_LAYER_PINFINS_INLINE :
        case TDICE_LAYER_PINFINS_STAGGERED :
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
                tgrid->LayersTypeProfile [layer_index]) ;

            return 0.0 ;
    }
}

/******************************************************************************/
