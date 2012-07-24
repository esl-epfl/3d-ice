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
#include "power_grid.h"

/******************************************************************************/

void power_grid_init (PowerGrid_t *pgrid)
{
    pgrid->NLayers           = (CellIndex_t) 0u ;
    pgrid->NCells            = (CellIndex_t) 0u ;
    pgrid->LayersProfile     = NULL ;
    pgrid->FloorplansProfile = NULL ;
    pgrid->Sources           = NULL ;
    pgrid->Channel           = NULL ;
    pgrid->HeatSink          = NULL ;
}

/******************************************************************************/

Error_t power_grid_build
(
    PowerGrid_t *pgrid,
    Quantity_t   nlayers,
    Quantity_t   ncells
)
{
    pgrid->NLayers = nlayers ;
    pgrid->NCells  = ncells ;

    pgrid->LayersProfile =

        (StackLayerType_t *) calloc (nlayers, sizeof (StackLayerType_t)) ;

    if (pgrid->LayersProfile == NULL)

        return TDICE_FAILURE ;

    pgrid->FloorplansProfile =

        (Floorplan_t **) calloc (nlayers, sizeof (Floorplan_t *)) ;

    if (pgrid->FloorplansProfile == NULL)
    {
        free (pgrid->LayersProfile) ;

        return TDICE_FAILURE ;
    }

    while (--nlayers != 0)
    {
        pgrid->LayersProfile     [nlayers] = TDICE_LAYER_NONE ;
        pgrid->FloorplansProfile [nlayers] = NULL ;
    }

    pgrid->Sources = (Source_t *) calloc (ncells, sizeof(Source_t)) ;

    if (pgrid->Sources == NULL)
    {
        fprintf (stderr, "Cannot malloc source vector\n") ;

        free (pgrid->LayersProfile) ;
        free (pgrid->FloorplansProfile) ;

        return TDICE_FAILURE ;
    }

    return TDICE_SUCCESS ;
}

/******************************************************************************/

void power_grid_destroy (PowerGrid_t *pgrid)
{
    if (pgrid->LayersProfile != NULL)

        free (pgrid->LayersProfile) ;

    if (pgrid->FloorplansProfile != NULL)

        free (pgrid->FloorplansProfile) ;

    if (pgrid->Sources != NULL)

        free (pgrid->Sources) ;

    power_grid_init (pgrid) ;
}

/******************************************************************************/

void fill_power_grid (PowerGrid_t *pgrid, StackElementList_t *list)
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
                CellIndex_t        tmp = 0u ;
                LayerListNode_t *lnd ;

                for (lnd = layer_list_end (&stack_element->Pointer.Die->Layers) ;
                     lnd != NULL ;
                     lnd = layer_list_prev (lnd))
                {
                    pgrid->LayersProfile [index + tmp] = TDICE_LAYER_SOLID ;

                    tmp++ ;
                }

                tmp = stack_element->Pointer.Die->SourceLayerOffset ;

                pgrid->LayersProfile     [index + tmp] = TDICE_LAYER_SOURCE ;
                pgrid->FloorplansProfile [index + tmp] = &stack_element->Pointer.Die->Floorplan ;

                break ;
            }
            case TDICE_STACK_ELEMENT_LAYER :
            {
                pgrid->LayersProfile     [index] = TDICE_LAYER_SOLID ;
                pgrid->FloorplansProfile [index] = NULL ;

                break ;
            }
            case TDICE_STACK_ELEMENT_CHANNEL :
            {
                pgrid->Channel = stack_element->Pointer.Channel ;

                switch (pgrid->Channel->ChannelModel)
                {
                    case TDICE_CHANNEL_MODEL_MC_4RM :

                        pgrid->LayersProfile [index] = TDICE_LAYER_CHANNEL_4RM ;

                        break ;

                    case TDICE_CHANNEL_MODEL_MC_2RM :

                        pgrid->LayersProfile [index    ] = TDICE_LAYER_BOTTOM_WALL ;
                        pgrid->LayersProfile [index + 1] = TDICE_LAYER_VWALL_CHANNEL ;
                        pgrid->LayersProfile [index + 2] = TDICE_LAYER_CHANNEL_2RM ;
                        pgrid->LayersProfile [index + 3] = TDICE_LAYER_TOP_WALL ;

                        break ;

                    case TDICE_CHANNEL_MODEL_PF_INLINE :

                        pgrid->LayersProfile [index    ] = TDICE_LAYER_BOTTOM_WALL ;
                        pgrid->LayersProfile [index + 1] = TDICE_LAYER_VWALL_PINFINS ;
                        pgrid->LayersProfile [index + 2] = TDICE_LAYER_PINFINS_INLINE ;
                        pgrid->LayersProfile [index + 3] = TDICE_LAYER_TOP_WALL ;

                        break ;

                    case TDICE_CHANNEL_MODEL_PF_STAGGERED :

                        pgrid->LayersProfile [index    ] = TDICE_LAYER_BOTTOM_WALL ;
                        pgrid->LayersProfile [index + 1] = TDICE_LAYER_VWALL_PINFINS ;
                        pgrid->LayersProfile [index + 2] = TDICE_LAYER_PINFINS_STAGGERED ;
                        pgrid->LayersProfile [index + 3] = TDICE_LAYER_TOP_WALL ;

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
                pgrid->HeatSink = stack_element->Pointer.HeatSink ;

                switch (pgrid->HeatSink->SinkModel)
                {
                    case TDICE_HEATSINK_MODEL_CONNECTION_TO_AMBIENT :

                        if (pgrid->LayersProfile [index] == TDICE_LAYER_SOLID)

                            pgrid->LayersProfile [index] = TDICE_LAYER_SOLID_CONNECTED_TO_AMBIENT ;

                        else if (pgrid->LayersProfile [index] == TDICE_LAYER_SOURCE)

                            pgrid->LayersProfile [index] = TDICE_LAYER_SOURCE_CONNECTED_TO_AMBIENT ;

                        else

                            fprintf (stderr, "ERROR: Wrong offset of heat sink stack element\n") ;


                        break ;

                    case TDICE_HEATSINK_MODEL_TRADITIONAL :

                        pgrid->LayersProfile [index    ] = TDICE_LAYER_SPREADER ;
                        pgrid->LayersProfile [index + 1] = TDICE_LAYER_SINK ;

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

Error_t update_source_vector
(
    PowerGrid_t    *pgrid,
    ThermalGrid_t  *thermal_grid,
    Dimensions_t   *dimensions
)
{
    // reset all the source vector to 0

    Quantity_t   layer ;
    CellIndex_t  ccounter ;
    Source_t    *sources ;

    for (ccounter  = 0u,            sources = pgrid->Sources ;
         ccounter != pgrid->NCells ;
         ccounter++,                sources++)

        *sources = (Source_t) 0.0 ;

    for (layer  = 0u,             sources = pgrid->Sources ;
         layer != pgrid->NLayers ;
         layer++,                 sources += get_layer_area (dimensions))
    {
        switch (pgrid->LayersProfile [layer])
        {
            case TDICE_LAYER_SOURCE :
            {
                Error_t error = fill_sources_floorplan

                    (pgrid->FloorplansProfile [layer], sources) ;

                if (error == TDICE_FAILURE)

                    return TDICE_FAILURE ;

                break ;
            }

            case TDICE_LAYER_SOLID_CONNECTED_TO_AMBIENT :
            case TDICE_LAYER_SINK :
            {
                Source_t *tmp = sources ;

                FOR_EVERY_ROW (row, dimensions)
                {
                    FOR_EVERY_COLUMN (column, dimensions)
                    {
                        *tmp++ += pgrid->HeatSink->AmbientTemperature

                                     * get_conductance_top (thermal_grid,
                                           dimensions, layer, row, column) ;

                    } // FOR_EVERY_COLUMN
                } // FOR_EVERY_ROW

                break ;
            }

            case TDICE_LAYER_SOURCE_CONNECTED_TO_AMBIENT :
            {
                Source_t *tmp = sources ;

                FOR_EVERY_ROW (row, dimensions)
                {
                    FOR_EVERY_COLUMN (column, dimensions)
                    {
                        *tmp++ += pgrid->HeatSink->AmbientTemperature

                                     * get_conductance_top (thermal_grid,
                                           dimensions, layer, row, column) ;

                    } // FOR_EVERY_COLUMN
                } // FOR_EVERY_ROW

                Error_t error = fill_sources_floorplan

                    (pgrid->FloorplansProfile [layer], sources) ;

                if (error == TDICE_FAILURE)

                    return TDICE_FAILURE ;

                break ;
            }

            case TDICE_LAYER_CHANNEL_4RM :
            case TDICE_LAYER_CHANNEL_2RM :
            case TDICE_LAYER_PINFINS_INLINE :
            case TDICE_LAYER_PINFINS_STAGGERED :
            {
                Source_t *tmp = sources ;

                FOR_EVERY_COLUMN (column, dimensions)
                {
                    if (IS_CHANNEL_COLUMN (pgrid->Channel->ChannelModel, column) == true)
                    {
                        *tmp =   (Source_t) 2.0

                               * get_convective_term

                                    (pgrid->Channel, dimensions, layer, 0, column)

                               * pgrid->Channel->Coolant.TIn ;

                    }

                    tmp++ ;

                } // FOR_EVERY_COLUMN

                break ;
            }

            case TDICE_LAYER_SOLID :
            case TDICE_LAYER_SPREADER :
            case TDICE_LAYER_VWALL_CHANNEL :
            case TDICE_LAYER_VWALL_PINFINS :
            case TDICE_LAYER_TOP_WALL :
            case TDICE_LAYER_BOTTOM_WALL :

                break ;

            case TDICE_LAYER_NONE :

                fprintf (stderr, "ERROR: unset layer type\n") ;

                return TDICE_FAILURE ;

            default :

                fprintf (stderr, "ERROR: unknown layer type %d\n",
                    pgrid->LayersProfile [layer]) ;

                return TDICE_FAILURE ;
        }
    }

    return TDICE_SUCCESS ;
}

/******************************************************************************/

void update_channel_sources (PowerGrid_t *pgrid, Dimensions_t *dimensions)
{
    Quantity_t   layer ;
    Source_t    *sources ;

    for (layer  = 0u,             sources = pgrid->Sources ;
         layer != pgrid->NLayers ;
         layer++,                 sources += get_layer_area (dimensions))
    {
        switch (pgrid->LayersProfile [layer])
        {
            case TDICE_LAYER_CHANNEL_4RM :
            case TDICE_LAYER_CHANNEL_2RM :
            case TDICE_LAYER_PINFINS_INLINE :
            case TDICE_LAYER_PINFINS_STAGGERED :
            {
                Source_t *tmp = sources ;

                FOR_EVERY_COLUMN (column, dimensions)
                {
                    if (IS_CHANNEL_COLUMN (pgrid->Channel->ChannelModel, column) == true)
                    {
                        *tmp =   (Source_t) 2.0

                               * get_convective_term

                                    (pgrid->Channel, dimensions, layer, 0, column)

                               * pgrid->Channel->Coolant.TIn ;

                    }

                    tmp++ ;

                } // FOR_EVERY_COLUMN

                break ;
            }

            case TDICE_LAYER_SOURCE :
            case TDICE_LAYER_SOLID_CONNECTED_TO_AMBIENT :
            case TDICE_LAYER_SOURCE_CONNECTED_TO_AMBIENT :
            case TDICE_LAYER_SINK :
            case TDICE_LAYER_SOLID :
            case TDICE_LAYER_SPREADER :
            case TDICE_LAYER_VWALL_CHANNEL :
            case TDICE_LAYER_VWALL_PINFINS :
            case TDICE_LAYER_TOP_WALL :
            case TDICE_LAYER_BOTTOM_WALL :

                break ;

            case TDICE_LAYER_NONE :

                fprintf (stderr, "ERROR: unset layer type\n") ;

                return ;

            default :

                fprintf (stderr, "ERROR: unknown layer type %d\n",
                    pgrid->LayersProfile [layer]) ;

                return ;
        }
    }
}

/******************************************************************************/

Error_t insert_power_values (PowerGrid_t *pgrid, PowersQueue_t *pvalues)
{
    Quantity_t layer ;

    for (layer = 0u ; layer != pgrid->NLayers ; layer++)
    {
        switch (pgrid->LayersProfile [layer])
        {
            case TDICE_LAYER_SOURCE :
            case TDICE_LAYER_SOURCE_CONNECTED_TO_AMBIENT :
            {
                Error_t result = insert_power_values_floorplan

                    (pgrid->FloorplansProfile [layer], pvalues) ;

                if (result == TDICE_FAILURE)

                    return TDICE_FAILURE ;

                break ;
            }

            case TDICE_LAYER_CHANNEL_4RM :
            case TDICE_LAYER_CHANNEL_2RM :
            case TDICE_LAYER_PINFINS_INLINE :
            case TDICE_LAYER_PINFINS_STAGGERED :
            case TDICE_LAYER_SOLID_CONNECTED_TO_AMBIENT :
            case TDICE_LAYER_SINK :
            case TDICE_LAYER_SOLID :
            case TDICE_LAYER_SPREADER :
            case TDICE_LAYER_VWALL_CHANNEL :
            case TDICE_LAYER_VWALL_PINFINS :
            case TDICE_LAYER_TOP_WALL :
            case TDICE_LAYER_BOTTOM_WALL :

                break ;

            case TDICE_LAYER_NONE :

                fprintf (stderr, "ERROR: unset layer type\n") ;

                return TDICE_FAILURE ;

            default :

                fprintf (stderr, "ERROR: unknown layer type %d\n",
                    pgrid->LayersProfile [layer]) ;

                return TDICE_FAILURE ;
        }
    }

    return TDICE_SUCCESS ;
}
