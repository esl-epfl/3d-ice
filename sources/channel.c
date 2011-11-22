/******************************************************************************
 * This file is part of 3D-ICE, version 1.0.1 .                               *
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
 *          Martino Ruggiero                                                  *
 *          Thomas Brunschwiler                                               *
 *          David Atienza                                                     *
 *                                                                            *
 * For any comment, suggestion or request  about 3D-ICE, please  register and *
 * write to the mailing list (see http://listes.epfl.ch/doc.cgi?liste=3d-ice) *
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
    channel->ChannelModel      = TDICE_CHANNEL_MODEL_NO_CHANNEL ;
    channel->Height            = 0.0 ;
    channel->Length            = 0.0 ;
    channel->Pitch             = 0.0 ;
    channel->Porosity          = 0.0 ;
    channel->NChannels         = 0u ;
    channel->NLayers           = 0u ;
    channel->SourceLayerOffset = 0u ;
    channel->Coolant.HTCSide   = 0.0 ;
    channel->Coolant.HTCTop    = 0.0 ;
    channel->Coolant.HTCBottom = 0.0 ;
    channel->Coolant.VHC       = 0.0 ;
    channel->Coolant.TIn       = 0.0 ;
    channel->CoolantFR         = 0.0 ;
    channel->DarcyVelocity     = 0.0 ;
    channel->WallMaterial      = NULL ;
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
    char       *prefix,
    Channel    *channel,
    Dimensions *dimensions
)
{  // FIXME
    fprintf (stream,
        "%schannel :\n",
        prefix) ;

    fprintf (stream,
        "%s   channel model type          %d ;\n",
        prefix, channel->ChannelModel) ;

    fprintf (stream,
        "%s              height %7.1f ;\n",
        prefix, channel->Height) ;

    fprintf (stream,
        "%s              length %7.1f ;\n",
        prefix, channel->Length) ;

    fprintf (stream,
        "%s              pitch  %7.1f ;\n",
        prefix, channel->Pitch) ;

    fprintf (stream,
        "%s              porosity %7.1f ;\n",
        prefix, channel->Porosity) ;

    fprintf (stream,
        "%s      channel length %7.1f ;\n",
        prefix, dimensions->Cell.ChannelLength) ;

    fprintf (stream,
        "%s         wall length %7.1f ;\n",
           prefix, dimensions->Cell.WallLength) ;

    fprintf (stream,
        "%s   first wall length %7.1f ;\n",
        prefix, dimensions->Cell.FirstWallLength) ;

    fprintf (stream,
        "%s    last wall length %7.1f ;\n",
        prefix, dimensions->Cell.LastWallLength) ;

    fprintf (stream,
        "%s   wall material %s ;\n",
        prefix, channel->WallMaterial->Id) ;

    fprintf (stream,
        "%s   coolant flow rate                       %.4e ;\n",
        prefix, channel->CoolantFR) ;

    fprintf (stream,
        "%s   coolant heat transfert coefficient side %.4e ,\n",
        prefix, channel->Coolant.HTCSide) ;

    fprintf (stream,
        "%s                                       top %.4e ,\n",
        prefix, channel->Coolant.HTCTop) ;

    fprintf (stream,
        "%s                                    bottom %.4e ;\n",
        prefix, channel->Coolant.HTCBottom) ;

    fprintf (stream,
        "%s   coolant volumetric heat capacity        %.4e ;\n",
        prefix, channel->Coolant.VHC) ;

    fprintf (stream,
        "%s   coolant incoming temperature          %.2f ;\n",
        prefix, channel->Coolant.TIn ) ;
}

/******************************************************************************/

void print_detailed_channel
(
  FILE    *stream,
  char    *prefix,
  Channel *channel
)
{
    fprintf (stream,
        "%schannel                     = %p\n",
        prefix,   channel) ;

    fprintf (stream,
        "%s  Model type                = %d\n",
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
        "%s  CoolantHTCs.Side          = %.4e\n",
        prefix,  channel->Coolant.HTCSide) ;

    fprintf (stream,
        "%s  CoolantHTCs.Top           = %.4e\n",
        prefix,  channel->Coolant.HTCTop) ;

    fprintf (stream,
        "%s  CoolantHTCs.Bottom        = %.4e\n",
        prefix,  channel->Coolant.HTCBottom) ;

    fprintf (stream,
        "%s  CoolantVHC                = %.4e\n",
        prefix,  channel->Coolant.VHC) ;

    fprintf (stream,
        "%s  CoolantTIn                = %.2f\n",
        prefix,  channel->Coolant.TIn) ;

    fprintf (stream,
        "%s  CoolantFR                 = %.4e\n",
        prefix,  channel->CoolantFR) ;

    fprintf (stream,
        "%s  WallMaterial              = %p\n",
        prefix,  channel->WallMaterial) ;
}

/******************************************************************************/

static
void fill_thermal_cell_channel_4rm
(
    ThermalCell *thermalcells,
    double       delta_time,
    Dimensions  *dimensions,
    uint32_t     layer_index,
    Channel     *channel
)
{
    uint32_t cell_index =

        get_cell_offset_in_stack (dimensions, layer_index , 0, 0) ;

    thermalcells += cell_index ;

    FOR_EVERY_ROW (row_index, dimensions)
    {
        FOR_EVERY_COLUMN (column_index, dimensions)
        {
#ifdef PRINT_THERMAL_CELLS
            fprintf (stderr,
                "  l %2d r %4d c %4d [%7d] ",
                layer_index, row_index, column_index, cell_index++) ;
#endif

            if (IS_CHANNEL_COLUMN(channel->ChannelModel, column_index) == TRUE_V)

                fill_liquid_cell_mc_4rm
                (
                    thermalcells, delta_time,
                    get_cell_length(dimensions, column_index),
                    get_cell_width(dimensions, row_index),
                    channel->Height,
                    channel->NChannels,
                    channel->Coolant,
                    channel->CoolantFR
                ) ;

            else

                fill_solid_cell_central
                (
                    thermalcells, delta_time,
                    get_cell_length(dimensions, column_index),
                    get_cell_width(dimensions, row_index),
                    channel->Height,
                    channel->WallMaterial->ThermalConductivity,
                    channel->WallMaterial->VolumetricHeatCapacity
                ) ;

            thermalcells ++ ;

        } // FOR_EVERY_COLUMN
    } // FOR_EVERY_ROW
}

/******************************************************************************/

static void fill_thermal_cell_channel_2rm
(
    ThermalCell *thermalcells,
    double       delta_time,
    Dimensions  *dimensions,
    uint32_t     layer_index,
    Channel     *channel
)
{
    uint32_t cell_index =

        get_cell_offset_in_stack (dimensions, layer_index , 0, 0) ;

    thermalcells += cell_index ;

    /* Bottom Wall */

    {
        FOR_EVERY_ROW (row_index, dimensions)
        {
            FOR_EVERY_COLUMN (column_index, dimensions)
            {
#ifdef PRINT_THERMAL_CELLS
                fprintf (stderr,
                    "  l %2d r %4d c %4d [%7d] ",
                    layer_index, row_index, column_index, cell_index++) ;
#endif

                init_thermal_cell (thermalcells) ;

                thermalcells++;

            } // FOR_EVERY_COLUMN
        } // FOR_EVERY_ROW
    }

    layer_index++ ;

    /* Virtual wall */

    {
        FOR_EVERY_ROW (row_index, dimensions)
        {
            FOR_EVERY_COLUMN (column_index, dimensions)
            {
#ifdef PRINT_THERMAL_CELLS
                fprintf (stderr,
                    "  l %2d r %4d c %4d [%7d] ",
                    layer_index, row_index, column_index, cell_index++) ;
#endif

                fill_virtual_wall_cell_mc_2rm
                (
                    thermalcells, delta_time,
                    get_cell_length(dimensions, column_index),
                    get_cell_width(dimensions, row_index),
                    channel->Height,
                    channel->Porosity,
                    channel->WallMaterial->ThermalConductivity,
                    channel->WallMaterial->VolumetricHeatCapacity
                ) ;

                thermalcells++;

            } // FOR_EVERY_COLUMN
        } // FOR_EVERY_ROW
    }

    layer_index++ ;

    /* Channel for PF 2RM */

    {
        FOR_EVERY_ROW (row_index, dimensions)
        {
            FOR_EVERY_COLUMN (column_index, dimensions)
            {
#ifdef PRINT_THERMAL_CELLS
                fprintf (stderr,
                    "  l %2d r %4d c %4d [%7d] ",
                    layer_index, row_index, column_index, cell_index++) ;
#endif

                fill_liquid_cell_mc_2rm
                (
                    thermalcells, delta_time,
                    get_cell_length(dimensions, column_index),
                    get_cell_width(dimensions, row_index),
                    channel->Height,
                    channel->NChannels, channel->Length, channel->Pitch,
                    channel->Porosity, channel->Coolant, channel->CoolantFR
                ) ;

                thermalcells++;

            } // FOR_EVERY_COLUMN
        } // FOR_EVERY_ROW
    }

    layer_index++ ;

    /* Top Wall */

    {
        FOR_EVERY_ROW (row_index, dimensions)
        {
            FOR_EVERY_COLUMN (column_index, dimensions)
            {
#ifdef PRINT_THERMAL_CELLS
                fprintf (stderr,
                    "  l %2d r %4d c %4d [%7d] ",
                    layer_index, row_index, column_index, cell_index++) ;
#endif

                init_thermal_cell (thermalcells) ;

                thermalcells++;

            } // FOR_EVERY_COLUMN
        } // FOR_EVERY_ROW
    }
}

/******************************************************************************/

static void fill_thermal_cell_channel_pf
(
    ThermalCell *thermalcells,
    double       delta_time,
    Dimensions  *dimensions,
    uint32_t     layer_index,
    Channel     *channel
)
{
    uint32_t cell_index =

        get_cell_offset_in_stack (dimensions, layer_index , 0, 0) ;

    thermalcells += cell_index ;

    /* Bottom Wall */

    {
        FOR_EVERY_ROW (row_index, dimensions)
        {
            FOR_EVERY_COLUMN (column_index, dimensions)
            {
#ifdef PRINT_THERMAL_CELLS
                fprintf (stderr,
                    "  l %2d r %4d c %4d [%7d] ",
                    layer_index, row_index, column_index, cell_index++) ;
#endif

                init_thermal_cell (thermalcells) ;

                thermalcells++;

            } // FOR_EVERY_COLUMN
        } // FOR_EVERY_ROW
    }

    layer_index++ ;

    /* Virtual wall */

    {
        FOR_EVERY_ROW (row_index, dimensions)
        {
            FOR_EVERY_COLUMN (column_index, dimensions)
            {
#ifdef PRINT_THERMAL_CELLS
                fprintf (stderr,
                    "  l %2d r %4d c %4d [%7d] ",
                    layer_index, row_index, column_index, cell_index++) ;
#endif

                fill_virtual_wall_cell_pf
                (
                    thermalcells, delta_time,
                    get_cell_length(dimensions, column_index),
                    get_cell_width(dimensions, row_index),
                    channel->Height,
                    channel->Porosity,
                    channel->WallMaterial->ThermalConductivity,
                    channel->WallMaterial->VolumetricHeatCapacity
                ) ;

                thermalcells++;

            } // FOR_EVERY_COLUMN
        } // FOR_EVERY_ROW
    }

    layer_index++ ;

    /* Channel for PF 2RM */

    {
        FOR_EVERY_ROW (row_index, dimensions)
        {
            FOR_EVERY_COLUMN (column_index, dimensions)
            {
#ifdef PRINT_THERMAL_CELLS
                fprintf (stderr,
                    "  l %2d r %4d c %4d [%7d] ",
                    layer_index, row_index, column_index, cell_index++) ;
#endif

                fill_liquid_cell_pf
                (
                    thermalcells, delta_time,
                    get_cell_length(dimensions, column_index),
                    get_cell_width(dimensions, row_index),
                    channel->Height,
                    channel->ChannelModel,
                    channel->Porosity, channel->Coolant, channel->DarcyVelocity
                ) ;

                thermalcells++;

            } // FOR_EVERY_COLUMN
        } // FOR_EVERY_ROW
    }

    layer_index++ ;

    /* Top Wall */

    {
        FOR_EVERY_ROW (row_index, dimensions)
        {
            FOR_EVERY_COLUMN (column_index, dimensions)
            {
#ifdef PRINT_THERMAL_CELLS
                fprintf (stderr,
                    "  l %2d r %4d c %4d [%7d] ",
                    layer_index, row_index, column_index, cell_index++) ;
#endif

                init_thermal_cell (thermalcells) ;

                thermalcells++;

            } // FOR_EVERY_COLUMN
        } // FOR_EVERY_ROW
    }
}

/******************************************************************************/

void fill_thermal_cell_channel
(
    ThermalCell *thermalcells,
    double       delta_time,
    Dimensions  *dimensions,
    uint32_t     layer_index,
    Channel     *channel
)
{
    switch (channel->ChannelModel)
    {
        case TDICE_CHANNEL_MODEL_MC_RM4 :

            fill_thermal_cell_channel_4rm

                (thermalcells, delta_time, dimensions, layer_index, channel) ;

            break ;

        case TDICE_CHANNEL_MODEL_MC_RM2 :

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
    double     *sources,
    Dimensions *dimensions,
    uint32_t    layer_index,
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

    double C;

    if (   channel->ChannelModel == TDICE_CHANNEL_MODEL_PF_INLINE
        || channel->ChannelModel == TDICE_CHANNEL_MODEL_PF_STAGGERED)
    {

        C = CCONV_PF

            (channel->Coolant.VHC, channel->DarcyVelocity,
             get_cell_length (dimensions, 0), channel->Height);

    }
    else if (channel->ChannelModel == TDICE_CHANNEL_MODEL_MC_RM2)
    {

        C = CCONV_MC_2RM

            (channel->NChannels, channel->Coolant.VHC, channel->CoolantFR,
             channel->Porosity, get_cell_length(dimensions,0), channel->Length) ;

    }
    else //TDICE_CHANNEL_MODEL_MC_4RM
    {

        C = CCONV_MC_4RM

            (channel->NChannels, channel->Coolant.VHC, channel->CoolantFR);

    }

    sources += get_cell_offset_in_stack (dimensions, layer_index, 0, 0) ;

    double tmp = 2.0 * C * channel->Coolant.TIn ;

    FOR_EVERY_COLUMN (column_index, dimensions)
    {
        if (IS_CHANNEL_COLUMN (channel->ChannelModel, column_index) == TRUE_V)
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
    Channel         *channel,
    Dimensions      *dimensions,
    ThermalCell     *thermalcells,
    uint32_t  layer_index,
    SystemMatrix     system_matrix
)
{
# ifdef PRINT_SYSTEM_MATRIX
    fprintf (stderr,
        "(l %2d) fill_system_matrix_channel %s \n",
        layer_index, channel->WallMaterial->Id) ;
# endif

    if (channel->ChannelModel == TDICE_CHANNEL_MODEL_MC_RM4)
    {
        FOR_EVERY_ROW (row_index, dimensions)
        {
            FOR_EVERY_COLUMN (column_index, dimensions)
            {
                if (IS_CHANNEL_COLUMN (channel->ChannelModel, column_index) == TRUE_V)

                    system_matrix = add_liquid_column_mc_4rm

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
        typedef SystemMatrix (*add_column_fp)

            (Dimensions*, ThermalCell*, uint32_t, uint32_t, uint32_t, SystemMatrix)  ;

        add_column_fp bottom_layer ;
        add_column_fp virtual_layer ;
        add_column_fp liquid_layer ;
        add_column_fp top_layer ;

        if (   channel->ChannelModel == TDICE_CHANNEL_MODEL_PF_INLINE
            || channel->ChannelModel == TDICE_CHANNEL_MODEL_PF_STAGGERED)
        {
            bottom_layer  = &add_bottom_wall_column_pf ;
            virtual_layer = &add_virtual_wall_column_pf ;
            liquid_layer  = &add_liquid_column_pf ;
            top_layer     = &add_top_wall_column_pf ;
        }
        else  // TDICE_CHANNEL_MODEL_MC_RM2
        {
            bottom_layer  = &add_bottom_wall_column_mc_rm2 ;
            virtual_layer = &add_virtual_wall_column_mc_rm2 ;
            liquid_layer  = &add_liquid_column_mc_rm2 ;
            top_layer     = &add_top_wall_column_mc_rm2 ;
        }

        // Bottom Wall

        {
            FOR_EVERY_ROW (row_index, dimensions)
            {
                FOR_EVERY_COLUMN (column_index, dimensions)
                {
                    system_matrix = bottom_layer

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
                    system_matrix = virtual_layer

                        (dimensions, thermalcells,
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
                    system_matrix = liquid_layer

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
                    system_matrix = top_layer

                        (dimensions, thermalcells,
                         layer_index, row_index, column_index, system_matrix) ;

                } // FOR_EVERY_COLUMN
            }  // FOR_EVERY_ROW
        }
    }

    return system_matrix ;
}

  /******************************************************************************/
