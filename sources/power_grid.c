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
#include "power_grid.h"

/******************************************************************************/

void init_power_grid (PowerGrid_t *this)
{
    this->NLayers           = 0u ;
    this->NCells            = 0u ;
    this->LayersProfile     = NULL ;
    this->FloorplansProfile = NULL ;
    this->Sources           = NULL ;
    this->Channel           = NULL ;
    this->HeatSink          = NULL ;
}

/******************************************************************************/

Error_t alloc_power_grid
(
    PowerGrid_t *this,
    Quantity_t   nlayers,
    Quantity_t   ncells
)
{
    this->NLayers = nlayers ;
    this->NCells  = ncells ;

    this->LayersProfile =

        (StackLayerType_t *) calloc (nlayers, sizeof (StackLayerType_t)) ;

    if (this->LayersProfile == NULL)

        return TDICE_FAILURE ;

    this->FloorplansProfile =

        (Floorplan_t **) calloc (nlayers, sizeof (Floorplan_t *)) ;

    if (this->FloorplansProfile == NULL)
    {
        FREE_POINTER (free, this->LayersProfile) ;

        return TDICE_FAILURE ;
    }

    while (--nlayers != 0)
    {
        this->LayersProfile     [nlayers] = TDICE_LAYER_NONE ;
        this->FloorplansProfile [nlayers] = NULL ;
    }

    this->Sources = (Source_t *) calloc (ncells, sizeof(Source_t)) ;

    if (this->Sources == NULL)
    {
        fprintf (stderr, "Cannot malloc source vector\n") ;

        FREE_POINTER (free, this->LayersProfile) ;
        FREE_POINTER (free, this->FloorplansProfile) ;

        return TDICE_FAILURE ;
    }

    return TDICE_SUCCESS ;
}

/******************************************************************************/

void free_power_grid (PowerGrid_t *this)
{
    FREE_POINTER (free, this->LayersProfile) ;
    FREE_POINTER (free, this->FloorplansProfile) ;
    FREE_POINTER (free, this->Sources) ;
}

/******************************************************************************/

void fill_power_grid (PowerGrid_t *this, StackElement_t *list)
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
                    {
                        this->LayersProfile     [index + tmp] = TDICE_LAYER_SOURCE ;
                        this->FloorplansProfile [index + tmp] = stack_element->Floorplan ;
                    }
                    else

                        this->LayersProfile     [index + tmp] = TDICE_LAYER_SOLID ;

                    tmp++ ;
                }

                break ;
            }
            case TDICE_STACK_ELEMENT_LAYER :
            {
                this->LayersProfile     [index] = TDICE_LAYER_SOLID ;
                this->FloorplansProfile [index] = NULL ;

                break ;
            }
            case TDICE_STACK_ELEMENT_CHANNEL :
            {
                this->Channel = stack_element->Pointer.Channel ;

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

                        this->LayersProfile [index] = TDICE_LAYER_SOLID_CONNECTED_TO_AMBIENT ;

                        break ;

                    case TDICE_HEATSINK_MODEL_TRADITIONAL :

                        this->LayersProfile [index    ] = TDICE_LAYER_SPREADER ;
                        this->LayersProfile [index + 1] = TDICE_LAYER_SINK ;

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
    PowerGrid_t    *this,
    ThermalGrid_t  *thermal_grid,
    Dimensions_t   *dimensions
)
{
    // reset all the source vector to 0

    Quantity_t   layer ;
    CellIndex_t  ccounter ;
    Source_t    *sources ;

    for (ccounter  = 0u,            sources = this->Sources ;
         ccounter != this->NCells ;
         ccounter++,                sources++)

        *sources = (Source_t) 0.0 ;

    for (layer  = 0u,             sources = this->Sources ;
         layer != this->NLayers ;
         layer++,                 sources += get_layer_area (dimensions))
    {
        switch (this->LayersProfile [layer])
        {
            case TDICE_LAYER_SOURCE :
            {
                Error_t error = fill_sources_floorplan

                    (this->FloorplansProfile [layer], sources) ;

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
                        *tmp++ += this->HeatSink->AmbientTemperature

                                     * get_conductance_top (thermal_grid,
                                           dimensions, layer, row, column) ;

                    } // FOR_EVERY_COLUMN
                } // FOR_EVERY_ROW

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
                    if (IS_CHANNEL_COLUMN (this->Channel->ChannelModel, column) == true)
                    {
                        *tmp =   (Source_t) 2.0

                               * get_convective_term

                                    (this->Channel, dimensions, layer, 0, column)

                               * this->Channel->Coolant.TIn ;

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

                return 0.0 ;

            default :

                fprintf (stderr, "ERROR: unknown layer type %d\n",
                    this->LayersProfile [layer]) ;

                return 0.0 ;
        }
    }

    return TDICE_SUCCESS ;
}

/******************************************************************************/

void update_channel_sources (PowerGrid_t *this, Dimensions_t *dimensions)
{
    Quantity_t   layer ;
    Source_t    *sources ;

    for (layer  = 0u,             sources = this->Sources ;
         layer != this->NLayers ;
         layer++,                 sources += get_layer_area (dimensions))
    {
        switch (this->LayersProfile [layer])
        {
            case TDICE_LAYER_CHANNEL_4RM :
            case TDICE_LAYER_CHANNEL_2RM :
            case TDICE_LAYER_PINFINS_INLINE :
            case TDICE_LAYER_PINFINS_STAGGERED :
            {
                Source_t *tmp = sources ;

                FOR_EVERY_COLUMN (column, dimensions)
                {
                    if (IS_CHANNEL_COLUMN (this->Channel->ChannelModel, column) == true)
                    {
                        *tmp =   (Source_t) 2.0

                               * get_convective_term

                                    (this->Channel, dimensions, layer, 0, column)

                               * this->Channel->Coolant.TIn ;

                    }

                    tmp++ ;

                } // FOR_EVERY_COLUMN

                break ;
            }

            case TDICE_LAYER_SOURCE :
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

                return ;

            default :

                fprintf (stderr, "ERROR: unknown layer type %d\n",
                    this->LayersProfile [layer]) ;

                return ;
        }
    }
}

/******************************************************************************/

Error_t insert_power_values (PowerGrid_t *this, PowersQueue_t *pvalues)
{
    Quantity_t layer ;

    for (layer = 0u ; layer != this->NLayers ; layer++)
    {
        switch (this->LayersProfile [layer])
        {
            case TDICE_LAYER_SOURCE :
            {
                Error_t result = insert_power_values_floorplan

                    (this->FloorplansProfile [layer], pvalues) ;

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
                    this->LayersProfile [layer]) ;

                return TDICE_FAILURE ;
        }
    }

    return TDICE_SUCCESS ;
}
