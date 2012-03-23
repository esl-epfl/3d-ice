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

#include "channel.h"
#include "macros.h"

/******************************************************************************/

void init_channel (Channel *channel)
{
    channel->ChannelModel          = TDICE_CHANNEL_MODEL_NONE ;
    channel->Height                = 0.0 ;
    channel->Length                = 0.0 ;
    channel->Pitch                 = 0.0 ;
    channel->Porosity              = 0.0 ;
    channel->NChannels             = 0u ;
    channel->NLayers               = 0u ;
    channel->SourceLayerOffset     = 0u ;
    channel->Coolant.HTCSide       = 0.0 ;
    channel->Coolant.HTCTop        = 0.0 ;
    channel->Coolant.HTCBottom     = 0.0 ;
    channel->Coolant.VHC           = 0.0 ;
    channel->Coolant.FlowRate      = 0.0 ;
    channel->Coolant.DarcyVelocity = 0.0 ;
    channel->Coolant.TIn           = 0.0 ;
    channel->WallMaterial          = NULL ;
}

/******************************************************************************/

Channel *alloc_and_init_channel (void)
{
    Channel *channel = (Channel *) malloc (sizeof(Channel)) ;

    if (channel != NULL)

        init_channel (channel) ;

    return channel ;
}

/******************************************************************************/

void free_channel (Channel *channel)
{
    FREE_POINTER (free, channel) ;
}

/******************************************************************************/

void print_formatted_channel
(
    FILE       *stream,
    String_t    prefix,
    Channel    *channel,
    Dimensions *dimensions
)
{
    if (channel->ChannelModel == TDICE_CHANNEL_MODEL_MC_4RM)
    {
        fprintf (stream, "%smicrochannel 4rm :\n", prefix) ;
        fprintf (stream, "%s   height            %7.1f ;\n", prefix, channel->Height) ;
        fprintf (stream, "%s\n", prefix) ;
        fprintf (stream, "%s   channel    length %7.1f ;\n", prefix, dimensions->Cell.ChannelLength) ;
        fprintf (stream, "%s   wall       length %7.1f ;\n", prefix, dimensions->Cell.WallLength) ;
        fprintf (stream, "%s\n", prefix) ;
        fprintf (stream, "%s   first wall length %7.1f ;\n", prefix, dimensions->Cell.FirstWallLength) ;
        fprintf (stream, "%s    last wall length %7.1f ;\n", prefix, dimensions->Cell.LastWallLength) ;
        fprintf (stream, "%s\n", prefix) ;
        fprintf (stream, "%s   wall material %s ;\n", prefix, channel->WallMaterial->Id) ;
        fprintf (stream, "%s\n", prefix) ;
        fprintf (stream, "%s   coolant flow rate  %.2f ;\n", prefix, FLOW_RATE_FROM_UM3SEC_TO_MLMIN(channel->Coolant.FlowRate)) ;
        fprintf (stream, "%s\n", prefix) ;
        fprintf (stream, "%s   coolant heat transfer coefficient side   %.4e ,\n", prefix, channel->Coolant.HTCSide) ;
        fprintf (stream, "%s                                     top    %.4e ,\n", prefix, channel->Coolant.HTCTop) ;
        fprintf (stream, "%s                                     bottom %.4e ;\n",  prefix, channel->Coolant.HTCBottom) ;
        fprintf (stream, "%s\n", prefix) ;
        fprintf (stream, "%s   coolant volumetric heat capacity %.4e ;\n", prefix, channel->Coolant.VHC) ;
        fprintf (stream, "%s\n", prefix) ;
        fprintf (stream, "%s   coolant incoming temperature  %.2f ;\n", prefix, channel->Coolant.TIn ) ;
    }

    if (channel->ChannelModel == TDICE_CHANNEL_MODEL_MC_2RM)
    {
        fprintf (stream, "%smicrochannel 2rm :\n", prefix) ;
        fprintf (stream, "%s   height            %7.1f ;\n", prefix, channel->Height) ;
        fprintf (stream, "%s\n", prefix) ;
        fprintf (stream, "%s   channel    length %7.1f ;\n", prefix, channel->Length) ;
        fprintf (stream, "%s   wall       length %7.1f ;\n", prefix, channel->Pitch - channel->Length) ;
        fprintf (stream, "%s\n", prefix) ;
        fprintf (stream, "%s   wall material %s ;\n", prefix, channel->WallMaterial->Id) ;
        fprintf (stream, "%s\n", prefix) ;
        fprintf (stream, "%s   coolant flow rate  %.2f ;\n", prefix, FLOW_RATE_FROM_UM3SEC_TO_MLMIN(channel->Coolant.FlowRate)) ;
        fprintf (stream, "%s\n", prefix) ;
        fprintf (stream, "%s   coolant heat transfer coefficient top    %.4e ,\n", prefix, channel->Coolant.HTCTop) ;
        fprintf (stream, "%s                                     bottom %.4e ;\n",  prefix, channel->Coolant.HTCBottom) ;
        fprintf (stream, "%s\n", prefix) ;
        fprintf (stream, "%s   coolant volumetric heat capacity %.4e ;\n", prefix, channel->Coolant.VHC) ;
        fprintf (stream, "%s\n", prefix) ;
        fprintf (stream, "%s   coolant incoming temperature  %.2f ;\n", prefix, channel->Coolant.TIn ) ;
    }

    if (channel->ChannelModel == TDICE_CHANNEL_MODEL_PF_INLINE)
    {
        fprintf (stream, "%spinfin :\n", prefix) ;
        fprintf (stream, "%s   height            %7.1f ;\n", prefix, channel->Height) ;
        fprintf (stream, "%s\n", prefix) ;
        fprintf (stream, "%s   pin diameter %7.1f ;\n",   prefix, DIAMETER(channel->Porosity, channel->Pitch)) ;
        fprintf (stream, "%s   pin pitch    %7.1f ;\n", prefix, channel->Pitch) ;
        fprintf (stream, "%s\n", prefix) ;
        fprintf (stream, "%s   pin distribution inline ;\n", prefix) ;
        fprintf (stream, "%s\n", prefix) ;
        fprintf (stream, "%s   pin material %s ;\n", prefix, channel->WallMaterial->Id) ;
        fprintf (stream, "%s\n", prefix) ;
        fprintf (stream, "%s   darcy velocity  %.4e ;\n", prefix, channel->Coolant.DarcyVelocity) ;
        fprintf (stream, "%s\n", prefix) ;
        fprintf (stream, "%s   coolant volumetric heat capacity %.4e ;\n", prefix, channel->Coolant.VHC) ;
        fprintf (stream, "%s\n", prefix) ;
        fprintf (stream, "%s   coolant incoming temperature  %.2f ;\n", prefix, channel->Coolant.TIn ) ;
    }

    if (channel->ChannelModel == TDICE_CHANNEL_MODEL_PF_STAGGERED)
    {
        fprintf (stream, "%spinfin :\n", prefix) ;
        fprintf (stream, "%s   height            %7.1f ;\n", prefix, channel->Height) ;
        fprintf (stream, "%s\n", prefix) ;
        fprintf (stream, "%s   pin diameter %7.1f ;\n",   prefix, DIAMETER(channel->Porosity, channel->Pitch)) ;
        fprintf (stream, "%s   pin pitch    %7.1f ;\n", prefix, channel->Pitch) ;
        fprintf (stream, "%s\n", prefix) ;
        fprintf (stream, "%s   pin distribution staggered ;\n", prefix) ;
        fprintf (stream, "%s\n", prefix) ;
        fprintf (stream, "%s   pin material %s ;\n", prefix, channel->WallMaterial->Id) ;
        fprintf (stream, "%s\n", prefix) ;
        fprintf (stream, "%s   darcy velocity  %.4e ;\n", prefix, channel->Coolant.DarcyVelocity) ;
        fprintf (stream, "%s\n", prefix) ;
        fprintf (stream, "%s   coolant volumetric heat capacity %.4e ;\n", prefix, channel->Coolant.VHC) ;
        fprintf (stream, "%s\n", prefix) ;
        fprintf (stream, "%s   coolant incoming temperature  %.2f ;\n", prefix, channel->Coolant.TIn ) ;
    }
}

/******************************************************************************/

void print_detailed_channel
(
  FILE    *stream,
  String_t prefix,
  Channel *channel
)
{
    fprintf (stream,
        "%schannel                     = %p\n",
        prefix,   channel) ;

    fprintf (stream,
        "%s  ChannelModel              = %d\n",
        prefix, channel->ChannelModel) ;

    fprintf (stream,
        "%s  Height                    = %.1f\n",
        prefix,  channel->Height) ;

    fprintf (stream,
        "%s  Length                    = %.1f\n",
        prefix,  channel->Length) ;

    fprintf (stream,
        "%s  Pitch                     = %.1f\n",
        prefix,  channel->Pitch) ;

    fprintf (stream,
        "%s  Porosity                  = %.1f\n",
        prefix,  channel->Porosity) ;

    fprintf (stream,
        "%s  NChannels                 = %d\n",
        prefix,  channel->NChannels) ;

    fprintf (stream,
        "%s  NLayers                   = %d\n",
        prefix,  channel->NLayers) ;

    fprintf (stream,
        "%s  SourceLayerOffset         = %d\n",
        prefix, channel->SourceLayerOffset) ;

    fprintf (stream,
        "%s  Coolant.HTCSide           = %.4e\n",
        prefix,  channel->Coolant.HTCSide) ;

    fprintf (stream,
        "%s  Coolant.HTCTop            = %.4e\n",
        prefix,  channel->Coolant.HTCTop) ;

    fprintf (stream,
        "%s  Coolant.HTCBottom         = %.4e\n",
        prefix,  channel->Coolant.HTCBottom) ;

    fprintf (stream,
        "%s  Coolant.VHC               = %.4e\n",
        prefix,  channel->Coolant.VHC) ;

    fprintf (stream,
        "%s  Coolant.TIn               = %.2f\n",
        prefix,  channel->Coolant.TIn) ;

    fprintf (stream,
        "%s  CoolantFR                 = %.4e\n",
        prefix,  channel->Coolant.FlowRate) ;

    fprintf (stream,
        "%s  DarcyVelocity             = %.4e\n",
        prefix,  channel->Coolant.DarcyVelocity) ;

    fprintf (stream,
        "%s  WallMaterial              = %p\n",
        prefix,  channel->WallMaterial) ;
}

/******************************************************************************/

static
void fill_thermal_cell_channel_4rm
(
    ThermalCell *thermalcells,
    Time_t       delta_time,
    Dimensions  *dimensions,
    CellIndex_t  layer_index,
    Channel     *channel
)
{
#ifdef PRINT_THERMAL_CELLS
    CellIndex_t cell_index =

        get_cell_offset_in_stack (dimensions, layer_index , 0, 0) ;
#endif

    thermalcells += layer_index * get_number_of_columns (dimensions) ;

    FOR_EVERY_COLUMN (column_index, dimensions)
    {
#ifdef PRINT_THERMAL_CELLS
        fprintf (stderr,
            "  l %2d c %4d [%7d] ",
            layer_index, column_index, cell_index++) ;
#endif

        if (IS_CHANNEL_COLUMN(channel->ChannelModel, column_index) == true)

            fill_liquid_cell_mc_4rm
            (
                thermalcells, delta_time,
                get_cell_length(dimensions, column_index),
                get_cell_width(dimensions, 0),
                channel->Height,
                channel->NChannels,
                channel->Coolant
            ) ;

        else

            fill_solid_cell_central
            (
                thermalcells, delta_time,
                get_cell_length(dimensions, column_index),
                get_cell_width(dimensions, 0),
                channel->Height,
                channel->WallMaterial->ThermalConductivity,
                channel->WallMaterial->VolumetricHeatCapacity
            ) ;

        thermalcells ++ ;

    } // FOR_EVERY_COLUMN
}

/******************************************************************************/

static void fill_thermal_cell_channel_2rm
(
    ThermalCell *thermalcells,
    Time_t       delta_time,
    Dimensions  *dimensions,
    CellIndex_t  layer_index,
    Channel     *channel
)
{
#ifdef PRINT_THERMAL_CELLS
    CellIndex_t cell_index =

        get_cell_offset_in_stack (dimensions, layer_index , 0, 0) ;
#endif

    thermalcells += layer_index * get_number_of_columns (dimensions) ;

    /* Bottom Wall */

    {
        FOR_EVERY_COLUMN (column_index, dimensions)
        {
            init_thermal_cell (thermalcells) ;

            thermalcells++;

        } // FOR_EVERY_COLUMN
    }

    layer_index++ ;

    /* Virtual wall */

    {
        FOR_EVERY_COLUMN (column_index, dimensions)
        {
#ifdef PRINT_THERMAL_CELLS
            fprintf (stderr,
                "  l %2d c %4d [%7d] ",
                layer_index, column_index, cell_index++) ;
#endif

            fill_virtual_wall_cell_mc_2rm
            (
                thermalcells, delta_time,
                get_cell_length(dimensions, column_index),
                get_cell_width(dimensions, 0),
                channel->Height,
                channel->Porosity,
                channel->WallMaterial->ThermalConductivity,
                channel->WallMaterial->VolumetricHeatCapacity
            ) ;

            thermalcells++;

        } // FOR_EVERY_COLUMN
    }

    layer_index++ ;

    /* Channel for PF 2RM */

    {
        FOR_EVERY_COLUMN (column_index, dimensions)
        {
#ifdef PRINT_THERMAL_CELLS
            fprintf (stderr,
                "  l %2d c %4d [%7d] ",
                layer_index, column_index, cell_index++) ;
#endif

            fill_liquid_cell_mc_2rm
            (
                thermalcells, delta_time,
                get_cell_length(dimensions, column_index),
                get_cell_width(dimensions, 0),
                channel->Height,
                channel->NChannels, channel->Length,
                channel->Porosity, channel->Coolant
            ) ;

            thermalcells++;

        } // FOR_EVERY_COLUMN
    }

    layer_index++ ;

    /* Top Wall */

    {
        FOR_EVERY_COLUMN (column_index, dimensions)
        {
            init_thermal_cell (thermalcells) ;

            thermalcells++;

        } // FOR_EVERY_COLUMN
    }
}

/******************************************************************************/

static void fill_thermal_cell_channel_pf
(
    ThermalCell *thermalcells,
    Time_t       delta_time,
    Dimensions  *dimensions,
    CellIndex_t  layer_index,
    Channel     *channel
)
{
#ifdef PRINT_THERMAL_CELLS
    CellIndex_t cell_index =

        get_cell_offset_in_stack (dimensions, layer_index , 0, 0) ;
#endif

    thermalcells += layer_index * get_number_of_columns (dimensions) ;

    /* Bottom Wall */

    {
        FOR_EVERY_COLUMN (column_index, dimensions)
        {
            init_thermal_cell (thermalcells) ;

            thermalcells++;

        } // FOR_EVERY_COLUMN
    }

    layer_index++ ;

    /* Virtual wall */

    {
        FOR_EVERY_COLUMN (column_index, dimensions)
        {
#ifdef PRINT_THERMAL_CELLS
            fprintf (stderr,
                "  l %2d c %4d [%7d] ",
                layer_index, column_index, cell_index++) ;
#endif

            fill_virtual_wall_cell_pf
            (
                thermalcells, delta_time,
                get_cell_length(dimensions, column_index),
                get_cell_width(dimensions, 0),
                channel->Height,
                channel->Porosity,
                channel->WallMaterial->ThermalConductivity,
                channel->WallMaterial->VolumetricHeatCapacity
            ) ;

            thermalcells++;

        } // FOR_EVERY_COLUMN
    }

    layer_index++ ;

    /* Channel for PF 2RM */

    {
        FOR_EVERY_COLUMN (column_index, dimensions)
        {
#ifdef PRINT_THERMAL_CELLS
            fprintf (stderr,
                "  l %2d c %4d [%7d] ",
                layer_index, column_index, cell_index++) ;
#endif

            fill_liquid_cell_pf
            (
                thermalcells, delta_time,
                get_cell_length(dimensions, column_index),
                get_cell_width(dimensions, 0),
                channel->Height,
                channel->ChannelModel,
                channel->Porosity, channel->Coolant
            ) ;

            thermalcells++;

        } // FOR_EVERY_COLUMN
    }

    layer_index++ ;

    /* Top Wall */

    {
        FOR_EVERY_COLUMN (column_index, dimensions)
        {
            init_thermal_cell (thermalcells) ;

            thermalcells++;

        } // FOR_EVERY_COLUMN
    }
}

/******************************************************************************/

void fill_thermal_cell_channel
(
    ThermalCell *thermalcells,
    Time_t       delta_time,
    Dimensions  *dimensions,
    CellIndex_t  layer_index,
    Channel     *channel
)
{
    switch (channel->ChannelModel)
    {
        case TDICE_CHANNEL_MODEL_MC_4RM :

            fill_thermal_cell_channel_4rm

                (thermalcells, delta_time, dimensions, layer_index, channel) ;

            break ;

        case TDICE_CHANNEL_MODEL_MC_2RM :

            fill_thermal_cell_channel_2rm

                (thermalcells, delta_time, dimensions, layer_index, channel) ;

            break ;

        case TDICE_CHANNEL_MODEL_PF_INLINE :
        case TDICE_CHANNEL_MODEL_PF_STAGGERED :

            fill_thermal_cell_channel_pf

                (thermalcells, delta_time, dimensions, layer_index, channel) ;

            break ;

        default :

            fprintf (stderr,
                "Error: unkonw channel model %d\n",
                channel->ChannelModel) ;
    }
}

/******************************************************************************/

void fill_sources_channel
(
    Source_t   *sources,
    Dimensions *dimensions,
    CellIndex_t layer_index,
    Channel    *channel
)
{
    // In 2RM, the channel offset is the layer index of the bottom wall,
    // so we need to add the offset of channel to "layer_index", which
    // represents the layer index of the channel.

    layer_index += channel->SourceLayerOffset ;

# ifdef PRINT_SOURCES
    fprintf (stderr,
        "layer_index = %d\tfill_sources_channel %s\n",
        layer_index, channel->WallMaterial->Id) ;
# endif

    // TODO

    // This piece of code works as long as the 2rm channel model
    // is implemented with an homogeneous cell length. Here we use the length
    // of the cell in the first column. Otherwise, the computation of C must
    // me put inside the column loop.

    Cconv_t C;

    if (   channel->ChannelModel == TDICE_CHANNEL_MODEL_PF_INLINE
        || channel->ChannelModel == TDICE_CHANNEL_MODEL_PF_STAGGERED)
    {

        C = CCONV_PF

            (channel->Coolant.VHC, channel->Coolant.DarcyVelocity,
             get_cell_length (dimensions, 0), channel->Height);

    }
    else if (channel->ChannelModel == TDICE_CHANNEL_MODEL_MC_2RM)
    {

        C = CCONV_MC_2RM

            (channel->NChannels, channel->Coolant.VHC, channel->Coolant.FlowRate,
             channel->Porosity, get_cell_length(dimensions,0), channel->Length) ;

    }
    else //TDICE_CHANNEL_MODEL_MC_4RM
    {

        C = CCONV_MC_4RM

            (channel->NChannels, channel->Coolant.VHC, channel->Coolant.FlowRate);

    }

    sources += get_cell_offset_in_stack (dimensions, layer_index, 0, 0) ;

    Source_t tmp = (Source_t) 2.0 * C * channel->Coolant.TIn ;

    FOR_EVERY_COLUMN (column_index, dimensions)
    {
        if (IS_CHANNEL_COLUMN (channel->ChannelModel, column_index) == true)
        {
            *sources = tmp ;

#ifdef PRINT_SOURCES
            fprintf (stderr,
                "liquid cell  | r %4d c    0 | l %6.1f w %6.1f "
                " | %.5e [source] = 2 * %.2f [Tin] * %.5e [C]\n",
                column_index,
                get_cell_length (dimensions, column_index), get_cell_width (dimensions, 0),
                *sources, channel->Coolant.TIn, C) ;
#endif
        }

        sources++ ;

    } // FOR_EVERY_COLUMN
}

/******************************************************************************/

SystemMatrix fill_system_matrix_channel
(
    Channel      *channel,
    Dimensions   *dimensions,
    ThermalCell  *thermalcells,
    CellIndex_t   layer_index,
    SystemMatrix  system_matrix
)
{
# ifdef PRINT_SYSTEM_MATRIX
    fprintf (stderr,
        "(l %2d) fill_system_matrix_channel %s \n",
        layer_index, channel->WallMaterial->Id) ;
# endif

    if (channel->ChannelModel == TDICE_CHANNEL_MODEL_MC_4RM)
    {
        FOR_EVERY_ROW (row_index, dimensions)
        {
            FOR_EVERY_COLUMN (column_index, dimensions)
            {
                if (IS_CHANNEL_COLUMN (channel->ChannelModel, column_index) == true)

                    system_matrix = add_liquid_column_4rm

                        (dimensions, thermalcells,
                         layer_index, row_index, column_index, system_matrix) ;

                else

                    system_matrix = add_solid_column

                        (dimensions, thermalcells,
                         layer_index, row_index, column_index, system_matrix) ;

            } // FOR_EVERY_COLUMN
        }  // FOR_EVERY_ROW
    }
    else
    {
        // Bottom Wall

        {
            FOR_EVERY_ROW (row_index, dimensions)
            {
                FOR_EVERY_COLUMN (column_index, dimensions)
                {
                    system_matrix = add_bottom_wall_column_2rm

                        (dimensions, thermalcells,
                         layer_index, row_index, column_index, system_matrix) ;

                } // FOR_EVERY_COLUMN
            }  // FOR_EVERY_ROW
        }

        layer_index++ ;

        // Virtual Wall
        {
            FOR_EVERY_ROW (row_index, dimensions)
            {
                FOR_EVERY_COLUMN (column_index, dimensions)
                {
                    system_matrix = add_virtual_wall_column_2rm

                        (dimensions, thermalcells, channel->ChannelModel,
                         layer_index, row_index, column_index, system_matrix) ;

                } // FOR_EVERY_COLUMN
            }  // FOR_EVERY_ROW
        }

        layer_index++ ;

        // Channel for 2RM

        {
            FOR_EVERY_ROW (row_index, dimensions)
            {
                FOR_EVERY_COLUMN (column_index, dimensions)
                {
                    system_matrix = add_liquid_column_2rm

                        (dimensions, thermalcells,
                         layer_index, row_index, column_index, system_matrix) ;

                } // FOR_EVERY_COLUMN
            }  // FOR_EVERY_ROW
        }

        layer_index++ ;

        // Top Wall
        {
            FOR_EVERY_ROW (row_index, dimensions)
            {
                FOR_EVERY_COLUMN (column_index, dimensions)
                {
                    system_matrix = add_top_wall_column_2rm

                        (dimensions, thermalcells,
                         layer_index, row_index, column_index, system_matrix) ;

                } // FOR_EVERY_COLUMN
            }  // FOR_EVERY_ROW
        }
    }

    return system_matrix ;
}

/******************************************************************************/

Temperature_t get_max_temperature_channel_outlet
(
    Channel       *channel,
    Dimensions    *dimensions,
    Temperature_t *temperatures
)
{
    temperatures += get_cell_offset_in_layer

        (dimensions, LAST_ROW_INDEX(dimensions), 0) ;

    Temperature_t max = *temperatures ;

    FOR_EVERY_COLUMN (column_index, dimensions)
    {
        if (IS_CHANNEL_COLUMN(channel->ChannelModel, column_index) == true)

            max = MAX (max, *temperatures) ;

        temperatures++ ;
    }

    return max ;
}

/******************************************************************************/

Temperature_t get_min_temperature_channel_outlet
(
    Channel       *channel,
    Dimensions    *dimensions,
    Temperature_t *temperatures
)
{
    temperatures += get_cell_offset_in_layer

        (dimensions, LAST_ROW_INDEX(dimensions), 0) ;

    Temperature_t min = *temperatures ;

    FOR_EVERY_COLUMN (column_index, dimensions)
    {
        if (IS_CHANNEL_COLUMN(channel->ChannelModel, column_index) == true)

            min = MIN (min, *temperatures) ;

        temperatures++ ;
    }

    return min ;
}

/******************************************************************************/

Temperature_t get_avg_temperature_channel_outlet
(
    Channel       *channel,
    Dimensions    *dimensions,
    Temperature_t *temperatures
)
{
    temperatures += get_cell_offset_in_layer

        (dimensions, LAST_ROW_INDEX(dimensions), 0) ;

    Temperature_t avg = *temperatures ;

    FOR_EVERY_COLUMN (column_index, dimensions)
    {
        if (IS_CHANNEL_COLUMN(channel->ChannelModel, column_index) == true)

            avg += *temperatures ;

        temperatures++ ;
    }

    return avg / (Temperature_t) channel->NChannels ;
}

/******************************************************************************/
