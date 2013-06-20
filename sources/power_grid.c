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

#include <stdlib.h> // For the memory functions malloc/calloc/free

#include "power_grid.h"
#include "macros.h"

/******************************************************************************/

void power_grid_init (PowerGrid_t *pgrid)
{
    pgrid->NLayers           = (CellIndex_t) 0u ;
    pgrid->NCells            = (CellIndex_t) 0u ;
    pgrid->NCellsLayer       = (CellIndex_t) 0u ;
    pgrid->LayersTypeProfile = NULL ;
    pgrid->FloorplansProfile = NULL ;
    pgrid->Sources           = NULL ;
    pgrid->Channel           = NULL ;
    pgrid->TopHeatSink       = NULL ;
    pgrid->BottomHeatSink    = NULL ;
    pgrid->HeatSinkTopTcs    = NULL ;
    pgrid->HeatSinkBottomTcs = NULL ;
    pgrid->CellsCapacities   = NULL ;
}

/******************************************************************************/

Error_t power_grid_build (PowerGrid_t *pgrid, Dimensions_t *dimensions)
{
    pgrid->NLayers     = get_number_of_layers (dimensions) ;
    pgrid->NCells      = get_number_of_cells  (dimensions) ;
    pgrid->NCellsLayer = get_layer_area       (dimensions) ;

    pgrid->LayersTypeProfile =

        (StackLayerType_t *) malloc (pgrid->NLayers * sizeof (StackLayerType_t)) ;

    if (pgrid->LayersTypeProfile == NULL)

        return TDICE_FAILURE ;

    pgrid->FloorplansProfile =

        (Floorplan_t **) malloc (pgrid->NLayers * sizeof (Floorplan_t *)) ;

    if (pgrid->FloorplansProfile == NULL)
    {
        free (pgrid->LayersTypeProfile) ;

        return TDICE_FAILURE ;
    }

    pgrid->Sources = (Source_t *) calloc (pgrid->NCells, sizeof(Source_t)) ;

    if (pgrid->Sources == NULL)
    {
        fprintf (stderr, "Cannot malloc source vector\n") ;

        free (pgrid->LayersTypeProfile) ;
        free (pgrid->FloorplansProfile) ;

        return TDICE_FAILURE ;
    }

    pgrid->HeatSinkTopTcs = (SolidTC_t *) calloc (pgrid->NCellsLayer, sizeof (SolidTC_t)) ;

    if (pgrid->HeatSinkTopTcs == NULL)
    {
        free (pgrid->LayersTypeProfile) ;
        free (pgrid->FloorplansProfile) ;
        free (pgrid->Sources) ;

        return TDICE_FAILURE ;
    }

    pgrid->HeatSinkBottomTcs = (SolidTC_t *) calloc (pgrid->NCellsLayer, sizeof (SolidTC_t)) ;

    if (pgrid->HeatSinkBottomTcs == NULL)
    {
        free (pgrid->LayersTypeProfile) ;
        free (pgrid->FloorplansProfile) ;
        free (pgrid->Sources) ;
        free (pgrid->HeatSinkTopTcs) ;

        return TDICE_FAILURE ;
    }

    pgrid->CellsCapacities = (Capacity_t *) calloc (pgrid->NCells, sizeof(Capacity_t)) ;

    if (pgrid->CellsCapacities == NULL)
    {
        free (pgrid->LayersTypeProfile) ;
        free (pgrid->FloorplansProfile) ;
        free (pgrid->Sources) ;
        free (pgrid->HeatSinkTopTcs) ;
        free (pgrid->HeatSinkBottomTcs) ;

        return TDICE_FAILURE ;
    }


    CellIndex_t lindex = 0 ;

    while (lindex != pgrid->NLayers)
    {
        pgrid->LayersTypeProfile [lindex] = TDICE_LAYER_NONE ;
        pgrid->FloorplansProfile [lindex] = NULL ;

        lindex++ ;
    }

    return TDICE_SUCCESS ;
}

/******************************************************************************/

void power_grid_destroy (PowerGrid_t *pgrid)
{
    if (pgrid->LayersTypeProfile != NULL)  free (pgrid->LayersTypeProfile) ;

    if (pgrid->FloorplansProfile != NULL)  free (pgrid->FloorplansProfile) ;

    if (pgrid->Sources != NULL)            free (pgrid->Sources) ;

    if (pgrid->HeatSinkTopTcs != NULL)     free (pgrid->HeatSinkTopTcs) ;

    if (pgrid->HeatSinkBottomTcs != NULL)  free (pgrid->HeatSinkBottomTcs) ;

    if (pgrid->CellsCapacities != NULL)    free (pgrid->CellsCapacities) ;

    power_grid_init (pgrid) ;
}

/******************************************************************************/

void power_grid_fill
(
    PowerGrid_t        *pgrid,
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
                    pgrid->LayersTypeProfile [index + tmp] = TDICE_LAYER_SOLID ;

                    pgrid->FloorplansProfile [index + tmp] = NULL ;

                    tmp++ ;
                }

                tmp = stack_element->Pointer.Die->SourceLayerOffset ;

                pgrid->LayersTypeProfile [index + tmp] = TDICE_LAYER_SOURCE ;

                pgrid->FloorplansProfile [index + tmp] = &stack_element->Pointer.Die->Floorplan ;

                break ;
            }
            case TDICE_STACK_ELEMENT_LAYER :
            {
                pgrid->LayersTypeProfile [index] = TDICE_LAYER_SOLID ;

                pgrid->FloorplansProfile [index] = NULL ;

                break ;
            }
            case TDICE_STACK_ELEMENT_CHANNEL :
            {
                pgrid->Channel = stack_element->Pointer.Channel ;

                switch (pgrid->Channel->ChannelModel)
                {
                    case TDICE_CHANNEL_MODEL_MC_4RM :

                        pgrid->LayersTypeProfile [index] = TDICE_LAYER_CHANNEL_4RM ;

                        break ;

                    case TDICE_CHANNEL_MODEL_MC_2RM :

                        pgrid->LayersTypeProfile [index    ] = TDICE_LAYER_BOTTOM_WALL ;
                        pgrid->LayersTypeProfile [index + 1] = TDICE_LAYER_VWALL_CHANNEL ;
                        pgrid->LayersTypeProfile [index + 2] = TDICE_LAYER_CHANNEL_2RM ;
                        pgrid->LayersTypeProfile [index + 3] = TDICE_LAYER_TOP_WALL ;

                        break ;

                    case TDICE_CHANNEL_MODEL_PF_INLINE :

                        pgrid->LayersTypeProfile [index    ] = TDICE_LAYER_BOTTOM_WALL ;
                        pgrid->LayersTypeProfile [index + 1] = TDICE_LAYER_VWALL_PINFINS ;
                        pgrid->LayersTypeProfile [index + 2] = TDICE_LAYER_PINFINS_INLINE ;
                        pgrid->LayersTypeProfile [index + 3] = TDICE_LAYER_TOP_WALL ;

                        break ;

                    case TDICE_CHANNEL_MODEL_PF_STAGGERED :

                        pgrid->LayersTypeProfile [index    ] = TDICE_LAYER_BOTTOM_WALL ;
                        pgrid->LayersTypeProfile [index + 1] = TDICE_LAYER_VWALL_PINFINS ;
                        pgrid->LayersTypeProfile [index + 2] = TDICE_LAYER_PINFINS_STAGGERED ;
                        pgrid->LayersTypeProfile [index + 3] = TDICE_LAYER_TOP_WALL ;

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

    CellIndex_t layer ;
    CellIndex_t row ;
    CellIndex_t column ;

    Capacity_t *tmp = pgrid->CellsCapacities ;

    for (layer  = first_layer (dimensions) ;
         layer <= last_layer  (dimensions) ; layer++)

        for (row  = first_row (dimensions) ;
             row <= last_row  (dimensions) ; row++)

            for (column  = first_column (dimensions) ;
                 column <= last_column  (dimensions) ; column++)

                *tmp++ = get_capacity (tgrid, dimensions, layer, row, column) ;


    StackElement_t *bmost = stack_element_list_data (stack_element_list_end   (list)) ;
    StackElement_t *tmost = stack_element_list_data (stack_element_list_begin (list)) ;

    if (tmost->TopSink != NULL)
    {
        pgrid->TopHeatSink = tmost->TopSink ;

        if (pgrid->LayersTypeProfile [pgrid->NLayers - 1] == TDICE_LAYER_SOLID)

            pgrid->LayersTypeProfile [pgrid->NLayers - 1] = TDICE_LAYER_SOLID_CONNECTED_TO_AMBIENT ;

        else if (pgrid->LayersTypeProfile [pgrid->NLayers - 1] == TDICE_LAYER_SOURCE)

            pgrid->LayersTypeProfile [pgrid->NLayers - 1] = TDICE_LAYER_SOURCE_CONNECTED_TO_AMBIENT ;

        row    = (CellIndex_t) 0u ;
        column = (CellIndex_t) 0u ;
        layer  = last_layer  (dimensions) ;

        SolidTC_t *tmp = pgrid->HeatSinkTopTcs ;

        for (row  = first_row (dimensions) ;
             row <= last_row  (dimensions) ; row++)
        {
            for (column  = first_column (dimensions) ;
                 column <= last_column  (dimensions) ; column++)
            {
                *tmp++ += get_conductance_top (tgrid, dimensions, layer, row, column) ;

            } // FOR_EVERY_COLUMN
        } // FOR_EVERY_ROW
    }

    if (bmost->BottomSink != NULL)
    {
        pgrid->BottomHeatSink = bmost->BottomSink ;

        if (pgrid->LayersTypeProfile [ 0 ] == TDICE_LAYER_SOLID)

            pgrid->LayersTypeProfile [ 0 ] = TDICE_LAYER_SOLID_CONNECTED_TO_PCB ;

        else if (pgrid->LayersTypeProfile [ 0 ] == TDICE_LAYER_SOURCE)

            pgrid->LayersTypeProfile [ 0 ] = TDICE_LAYER_SOURCE_CONNECTED_TO_PCB ;

        else if (   pgrid->LayersTypeProfile [ 0 ] == TDICE_LAYER_SOLID_CONNECTED_TO_AMBIENT
                 || pgrid->LayersTypeProfile [ 0 ] == TDICE_LAYER_SOURCE_CONNECTED_TO_AMBIENT)

            fprintf (stderr, "Top and bottom sink on the same layer ! not handled yed\n") ;

        row    = (CellIndex_t) 0u ;
        column = (CellIndex_t) 0u ;

        SolidTC_t *tmp = pgrid->HeatSinkBottomTcs ;

        for (row  = first_row (dimensions) ;
             row <= last_row  (dimensions) ; row++)
        {
            for (column  = first_column (dimensions) ;
                 column <= last_column  (dimensions) ; column++)
            {
                *tmp++ += get_conductance_bottom (tgrid, dimensions, 0, row, column) ;

            } // FOR_EVERY_COLUMN
        } // FOR_EVERY_ROW
   }
}

/******************************************************************************/

Error_t update_source_vector
(
    PowerGrid_t    *pgrid,
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
        switch (pgrid->LayersTypeProfile [layer])
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
            {
                Source_t  *tmpS = sources ;
                SolidTC_t *tmpT = pgrid->HeatSinkTopTcs ;

                CellIndex_t index = (CellIndex_t) 0u ;

                for (index  = (CellIndex_t) 0u ;
                     index != pgrid->NCellsLayer ; index++)

                        *tmpS++ += pgrid->TopHeatSink->AmbientTemperature * *tmpT++ ;

                break ;
            }

            case TDICE_LAYER_SOURCE_CONNECTED_TO_AMBIENT :
            {
                Source_t  *tmpS = sources ;
                SolidTC_t *tmpT = pgrid->HeatSinkTopTcs ;

                CellIndex_t index = (CellIndex_t) 0u ;

                for (index  = (CellIndex_t) 0u ;
                     index != pgrid->NCellsLayer ; index++)

                        *tmpS++ += pgrid->TopHeatSink->AmbientTemperature * *tmpT++ ;

                Error_t error = fill_sources_floorplan

                    (pgrid->FloorplansProfile [layer], sources) ;

                if (error == TDICE_FAILURE)

                    return TDICE_FAILURE ;

                break ;
            }

            case TDICE_LAYER_SOLID_CONNECTED_TO_PCB :
            {
                Source_t  *tmpS = sources ;
                SolidTC_t *tmpT = pgrid->HeatSinkBottomTcs ;

                CellIndex_t index = (CellIndex_t) 0u ;

                for (index  = (CellIndex_t) 0u ;
                     index != pgrid->NCellsLayer ; index++)

                        *tmpS++ += pgrid->BottomHeatSink->AmbientTemperature * *tmpT++ ;

                break ;
            }

            case TDICE_LAYER_SOURCE_CONNECTED_TO_PCB :
            {
                Source_t  *tmpS = sources ;
                SolidTC_t *tmpT = pgrid->HeatSinkBottomTcs ;

                CellIndex_t index = (CellIndex_t) 0u ;

                for (index  = (CellIndex_t) 0u ;
                     index != pgrid->NCellsLayer ; index++)

                        *tmpS++ += pgrid->BottomHeatSink->AmbientTemperature * *tmpT++ ;

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

                CellIndex_t column ;

                for (column = first_column (dimensions) ; column <= last_column (dimensions) ; column++)
                {
                    if (IS_CHANNEL_COLUMN (pgrid->Channel->ChannelModel, column) == true)
                    {
                        *tmp =   (Source_t) 2.0

                               * get_convective_term

                                    (pgrid->Channel, dimensions, layer, first_row (dimensions), column)

                               * pgrid->Channel->Coolant.TIn ;

                    }

                    tmp++ ;

                } // FOR_EVERY_COLUMN

                break ;
            }

            case TDICE_LAYER_SOLID :
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
                   pgrid->LayersTypeProfile [layer]) ;

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
            switch (pgrid->LayersTypeProfile [layer])
        {
            case TDICE_LAYER_CHANNEL_4RM :
            case TDICE_LAYER_CHANNEL_2RM :
            case TDICE_LAYER_PINFINS_INLINE :
            case TDICE_LAYER_PINFINS_STAGGERED :
            {
                Source_t *tmp = sources ;

                CellIndex_t column ;

                for (column = first_column (dimensions) ; column <= last_column (dimensions) ; column++)
                {
                    if (IS_CHANNEL_COLUMN (pgrid->Channel->ChannelModel, column) == true)
                    {
                        *tmp =   (Source_t) 2.0

                               * get_convective_term

                                    (pgrid->Channel, dimensions, layer, first_row (dimensions), column)

                               * pgrid->Channel->Coolant.TIn ;

                    }

                    tmp++ ;

                } // FOR_EVERY_COLUMN

                break ;
            }

            case TDICE_LAYER_SOURCE :
            case TDICE_LAYER_SOLID_CONNECTED_TO_AMBIENT :
            case TDICE_LAYER_SOLID_CONNECTED_TO_PCB :
            case TDICE_LAYER_SOURCE_CONNECTED_TO_AMBIENT :
            case TDICE_LAYER_SOURCE_CONNECTED_TO_PCB :
            case TDICE_LAYER_SOLID :
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
                    pgrid->LayersTypeProfile [layer]) ;

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
        switch (pgrid->LayersTypeProfile [layer])
        {
            case TDICE_LAYER_SOURCE :
            case TDICE_LAYER_SOURCE_CONNECTED_TO_AMBIENT :
            case TDICE_LAYER_SOURCE_CONNECTED_TO_PCB :
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
            case TDICE_LAYER_SOLID_CONNECTED_TO_PCB :
            case TDICE_LAYER_SOLID :
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
                    pgrid->LayersTypeProfile [layer]) ;

                return TDICE_FAILURE ;
        }
    }

    return TDICE_SUCCESS ;
}
