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

void init_channel (Channel* channel)
{
  channel->ChannelModel     = CHANNELMODEL_I;
  channel->Height           = CELLDIMENSION_I;
  channel->Length           = CELLDIMENSION_I;
  channel->Pitch            = CELLDIMENSION_I;
  channel->Porosity         = POROSITY_I;
  channel->NChannels        = GRIDDIMENSION_I;
  channel->CoolantHTCs      = COOLANTHTCS_I;
  channel->CoolantVHC       = COOLANTVHC_I;
  channel->CoolantTIn       = TEMPERATURE_I;
  channel->CoolantFR        = COOLANTFR_I;
  channel->DarcyVelocity    = DARCYVELOCITY_I;
  channel->WallMaterial     = NULL;
}

/******************************************************************************/

Channel* alloc_and_init_channel (void)
{
  Channel* channel = malloc (sizeof(*channel)) ;

  if (channel != NULL) init_channel (channel) ;

  return channel ;
}

/******************************************************************************/

void free_channel (Channel* channel)
{
  FREE_POINTER (free, channel) ;
}

/******************************************************************************/

void print_formatted_channel
(
  FILE*       stream,
  String_t    prefix,
  Channel*    channel,
  Dimensions* dimensions
)
{
  fprintf (stream,
           "%s   channel model type          %d ;\n",
           prefix, channel->ChannelModel) ;

  fprintf (stream,
           "%schannel :\n",
           prefix) ;

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
           prefix, channel->CoolantHTCs.Side) ;

  fprintf (stream,
           "%s                                       top %.4e ,\n",
           prefix, channel->CoolantHTCs.Top) ;

  fprintf (stream,
           "%s                                    bottom %.4e ;\n",
           prefix, channel->CoolantHTCs.Bottom) ;

  fprintf (stream,
           "%s   coolant volumetric heat capacity        %.4e ;\n",
           prefix, channel->CoolantVHC) ;

  fprintf (stream,
           "%s   coolant incoming temperature          %.2f ;\n",
           prefix, channel->CoolantTIn ) ;
}

/******************************************************************************/

void print_detailed_channel
(
  FILE*       stream,
  String_t    prefix,
  Channel*    channel
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
           "%s  CoolantHTCs.Side          = %.4e\n",
           prefix,  channel->CoolantHTCs.Side) ;

  fprintf (stream,
           "%s  CoolantHTCs.Top           = %.4e\n",
           prefix,  channel->CoolantHTCs.Top) ;

  fprintf (stream,
           "%s  CoolantHTCs.Bottom        = %.4e\n",
           prefix,  channel->CoolantHTCs.Bottom) ;

  fprintf (stream,
           "%s  CoolantVHC                = %.4e\n",
           prefix,  channel->CoolantVHC) ;

  fprintf (stream,
           "%s  CoolantTIn                = %.2f\n",
           prefix,  channel->CoolantTIn) ;

  fprintf (stream,
           "%s  CoolantFR                 = %.4e\n",
           prefix,  channel->CoolantFR) ;

  fprintf (stream,
           "%s  WallMaterial              = %p\n",
           prefix,  channel->WallMaterial) ;
}

/******************************************************************************/

void fill_thermal_cell_channel
(
    ThermalCell*     thermalcells,
    Time_t           delta_time,
    Dimensions*      dimensions,
    GridDimension_t  layer_index,
    Channel*         channel
)
{
    thermalcells += get_cell_offset_in_stack (dimensions, layer_index , 0, 0) ;

    if (   channel->ChannelModel == TDICE_CHANNEL_MODEL_PF_INLINE
        || channel->ChannelModel == TDICE_CHANNEL_MODEL_PF_STAGGERED)
    {

        /* Bottom Wall */

        FOR_EVERY_ROW (row_index_wall_bottom, dimensions)
        {
            FOR_EVERY_COLUMN (column_index_wall_bottom, dimensions)
            {
                fill_wall_cell_pf
                (
#ifdef PRINT_THERMAL_CELLS
                    dimensions, layer_index, row_index_wall_bottom, column_index_wall_bottom,
#endif
                    thermalcells, delta_time,
                    get_cell_length(dimensions, column_index_wall_bottom),
                    get_cell_width(dimensions, row_index_wall_bottom),
                    channel->Height
                ) ;

                thermalcells++;
            }
        }

        /* Virtual wall */

        FOR_EVERY_ROW (row_index_virtual_wall, dimensions)
        {
            FOR_EVERY_COLUMN (column_index_virtual_wall, dimensions)
            {
                fill_virtual_wall_cell_pf
                (
#ifdef PRINT_THERMAL_CELLS
                    dimensions, layer_index + 1, row_index_virtual_wall, column_index_virtual_wall,
#endif
                    thermalcells, delta_time,
                    get_cell_length(dimensions, column_index_virtual_wall),
                    get_cell_width(dimensions, row_index_virtual_wall),
                    channel->Height,
                    channel->Porosity,
                    channel->WallMaterial->ThermalConductivity,
                    channel->WallMaterial->VolumetricHeatCapacity
                ) ;

                thermalcells++;
            }
        }

        /* Channel for PF 2RM */

        FOR_EVERY_ROW (row_index, dimensions)
        {
            FOR_EVERY_COLUMN (column_index, dimensions)
            {
                fill_liquid_cell_pf
                (
#ifdef PRINT_THERMAL_CELLS
                    dimensions, layer_index + 2, row_index, column_index,
#endif
                    thermalcells, delta_time,
                    get_cell_length(dimensions, column_index),
                    get_cell_width(dimensions, row_index),
                    channel->Height,
                    channel->ChannelModel,
                    channel->Porosity,
                    channel->CoolantVHC,
                    channel->DarcyVelocity
                ) ;

                thermalcells++;
            }
        }

        /* Top Wall */

        FOR_EVERY_ROW (row_index_wall_top, dimensions)
        {
            FOR_EVERY_COLUMN (column_index_wall_top, dimensions)
            {
                fill_wall_cell_pf
                (
#ifdef PRINT_THERMAL_CELLS
                    dimensions, layer_index + 3, row_index_wall_top, column_index_wall_top,
#endif
                    thermalcells, delta_time,
                    get_cell_length(dimensions, column_index_wall_top),
                    get_cell_width(dimensions, row_index_wall_top),
                    channel->Height
                ) ;

                thermalcells++;
            }
        }

    }
    else if (channel->ChannelModel == TDICE_CHANNEL_MODEL_MC_RM2)
    {

        FOR_EVERY_ROW (row_index_wall_bottom, dimensions)
        {
            FOR_EVERY_COLUMN (column_index_wall_bottom, dimensions)
            {
                fill_wall_cell_mc_2rm
                (
#ifdef PRINT_THERMAL_CELLS
                    dimensions, layer_index, row_index_wall_bottom, column_index_wall_bottom,
#endif
                    thermalcells, delta_time,
                    get_cell_length(dimensions, column_index_wall_bottom),
                    get_cell_width(dimensions, row_index_wall_bottom),
                    channel->Height
                ) ;

                thermalcells++;
            }
        }

        /* Virtual wall */

        FOR_EVERY_ROW (row_index_virtual_wall, dimensions)
        {
            FOR_EVERY_COLUMN (column_index_virtual_wall, dimensions)
            {
                fill_virtual_wall_cell_mc_2rm
                (
#ifdef PRINT_THERMAL_CELLS
                    dimensions, layer_index + 1, row_index_virtual_wall, column_index_virtual_wall,
#endif
                    thermalcells, delta_time,
                    get_cell_length(dimensions, column_index_virtual_wall),
                    get_cell_width(dimensions, row_index_virtual_wall),
                    channel->Height,
                    channel->Porosity,
                    channel->WallMaterial->ThermalConductivity,
                    channel->WallMaterial->VolumetricHeatCapacity
                ) ;

                thermalcells++;
            }
        }

        /* Channel for MC 2RM */

        FOR_EVERY_ROW (row_index, dimensions)
        {
            FOR_EVERY_COLUMN (column_index, dimensions)
            {
                fill_liquid_cell_mc_2rm
                (
#ifdef PRINT_THERMAL_CELLS
                    dimensions, layer_index + 2, row_index, column_index,
#endif
                    thermalcells, delta_time,
                    get_cell_length(dimensions, column_index),
                    get_cell_width(dimensions, row_index),
                    channel->Height,
                    channel->NChannels,
                    channel->Length,
                    channel->Pitch,
                    channel->Porosity,
                    channel->CoolantHTCs,
                    channel->CoolantVHC,
                    channel->CoolantFR
                ) ;

                thermalcells++;
            }
        }

        /* Top Wall */

        FOR_EVERY_ROW (row_index_wall_top, dimensions)
        {
            FOR_EVERY_COLUMN (column_index_wall_top, dimensions)
            {
                fill_wall_cell_mc_2rm
                (
#ifdef PRINT_THERMAL_CELLS
                    dimensions, layer_index + 3, row_index_wall_top, column_index_wall_top,
#endif
                    thermalcells, delta_time,
                    get_cell_length(dimensions, column_index_wall_top),
                    get_cell_width(dimensions, row_index_wall_top),
                    channel->Height
                ) ;

                thermalcells++;
            }
        }

    }
    else // TDICE_CHANNEL_MODEL_MC_4RM
    {

        FOR_EVERY_ROW (row_index, dimensions)
        {
            FOR_EVERY_COLUMN (column_index, dimensions)
            {
                if (IS_CHANNEL_COLUMN(column_index))

                    fill_liquid_cell_mc_4rm
                    (
#ifdef PRINT_THERMAL_CELLS
                        dimensions, layer_index, row_index, column_index,
#endif
                        thermalcells, delta_time,
                        get_cell_length(dimensions, column_index),
                        get_cell_width(dimensions, row_index),
                        channel->Height,
                        channel->NChannels,
                        channel->CoolantHTCs,
                        channel->CoolantVHC,
                        channel->CoolantFR
                    ) ;

                else

                    fill_solid_cell_central
                    (
#ifdef PRINT_THERMAL_CELLS
                        dimensions, layer_index, row_index, column_index,
#endif
                        thermalcells, delta_time,
                        get_cell_length(dimensions, column_index),
                        get_cell_width(dimensions, row_index),
                        channel->Height,
                        channel->WallMaterial->ThermalConductivity,
                        channel->WallMaterial->VolumetricHeatCapacity
                    ) ;

                thermalcells ++ ;
            }
        }
    }
}

/******************************************************************************/

void fill_sources_channel
(
  Source_t*       sources,
  Dimensions*     dimensions,
  GridDimension_t layer_index,
  Channel*        channel
)
{
  if (channel->ChannelModel == TDICE_CHANNEL_MODEL_MC_RM2 ||
      channel->ChannelModel == TDICE_CHANNEL_MODEL_PF_INLINE ||
      channel->ChannelModel == TDICE_CHANNEL_MODEL_PF_STAGGERED) {
    // In 2RM, the channel offset is the layer index of the bottom wall,
    // so we need to add the offset of channel to "layer_index", which represents the layer index
    // of the channel.
    layer_index += 2;
  }

# ifdef PRINT_SOURCES
  fprintf (stderr,
    "layer_index = %d\tfill_sources_channel %s\n",
    layer_index, channel->WallMaterial->Id) ;
# endif

  Cconv_t C;

  if (channel->ChannelModel == TDICE_CHANNEL_MODEL_PF_INLINE ||
      channel->ChannelModel == TDICE_CHANNEL_MODEL_PF_STAGGERED) {

    C = CCONV_PF(channel->CoolantVHC, channel->DarcyVelocity, get_cell_length(dimensions, 0), channel->Height);

  } else if (channel->ChannelModel == TDICE_CHANNEL_MODEL_MC_RM2) {

    Quantity_t nchannels = (Quantity_t) ((get_chip_length(dimensions) / channel->Pitch) + 0.5); // round function
    C = CCONV_MC_2RM(nchannels, channel->CoolantVHC, channel->CoolantFR, channel->Porosity, get_cell_length(dimensions,0), channel->Length);

  } else { //TDICE_CHANNEL_MODEL_MC_4RM

    Quantity_t nchannels = (Quantity_t) ((get_number_of_columns (dimensions) - 1)  / 2) ;
    C = CCONV_MC_4RM(nchannels, channel->CoolantVHC, channel->CoolantFR);

  }

  sources += get_cell_offset_in_stack (dimensions, layer_index, 0, 0) ;

  FOR_EVERY_COLUMN (column_index, dimensions)
  {
    if (channel->ChannelModel != TDICE_CHANNEL_MODEL_MC_RM4
        || IS_CHANNEL_COLUMN(column_index))
    {
      *sources = 2.0 * C * channel->CoolantTIn ;

#     ifdef PRINT_SOURCES
      fprintf (stderr,
        "liquid cell  | l %2d r %4d [%7d] "
        "| %.5e [source] = 2 * %.2f [Tin] * %.5e [C]\n",
        layer_index, column_index,
        get_cell_offset_in_stack (dimensions, layer_index, 0, column_index),
        *sources, channel->CoolantTIn, C) ;
#     endif
    }

    sources++ ;

  } // FOR_EVERY_COLUMN
}

/******************************************************************************/

SystemMatrix fill_system_matrix_channel
(
    Channel          *channel,
    Dimensions       *dimensions,
    ThermalCell      *thermalcells,
    GridDimension_t  layer_index,
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

                if (IS_CHANNEL_COLUMN(column_index))

                    system_matrix = add_liquid_column_mc_4rm
                                    (
                                        dimensions, thermalcells,
                                        layer_index, row_index, column_index,
                                        system_matrix
                                    ) ;

                else

                    system_matrix = add_solid_column
                                    (
                                        dimensions, thermalcells,
                                        layer_index, row_index, column_index,
                                        system_matrix
                                    ) ;

            } // FOR_EVERY_COLUMN
        }  // FOR_EVERY_ROW
    }
    else
    {
        typedef SystemMatrix (*add_column_fp)
            (Dimensions*, ThermalCell*,
             GridDimension_t, GridDimension_t, GridDimension_t,
             SystemMatrix)  ;

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
            bottom_layer  = &add_bottom_wall_column_mc_2rm ;
            virtual_layer = &add_virtual_wall_column_mc_2rm ;
            liquid_layer  = &add_liquid_column_mc_2rm ;
            top_layer     = &add_top_wall_column_mc_2rm ;
        }

        // Bottom Wall

        FOR_EVERY_ROW (row_index_wall_bottom, dimensions)
        {
            FOR_EVERY_COLUMN (column_index_wall_bottom, dimensions)
            {
                system_matrix = bottom_layer
                                (
                                    dimensions, thermalcells,
                                    layer_index, row_index_wall_bottom, column_index_wall_bottom,
                                    system_matrix
                                ) ;

            } // FOR_EVERY_COLUMN
        }  // FOR_EVERY_ROW

        layer_index++ ;

        // Virtual Wall

        FOR_EVERY_ROW (row_index_virtual_wall, dimensions)
        {
            FOR_EVERY_COLUMN (column_index_virtual_wall, dimensions)
            {
                system_matrix = virtual_layer
                                (
                                    dimensions, thermalcells,
                                    layer_index, row_index_virtual_wall, column_index_virtual_wall,
                                    system_matrix
                                ) ;

            } // FOR_EVERY_COLUMN
        }  // FOR_EVERY_ROW

        layer_index++ ;

        // Channel for 2RM

        FOR_EVERY_ROW (row_index, dimensions)
        {
            FOR_EVERY_COLUMN (column_index, dimensions)
            {
                system_matrix = liquid_layer
                                (
                                    dimensions, thermalcells,
                                    layer_index, row_index, column_index,
                                    system_matrix
                                ) ;

            } // FOR_EVERY_COLUMN
        }  // FOR_EVERY_ROW

        layer_index++ ;

        // Top Wall

        FOR_EVERY_ROW (row_index_wall_top, dimensions)
        {
            FOR_EVERY_COLUMN (column_index_wall_top, dimensions)
            {
                system_matrix = top_layer
                                (
                                    dimensions, thermalcells,
                                    layer_index, row_index_wall_top, column_index_wall_top,
                                    system_matrix
                                ) ;

            } // FOR_EVERY_COLUMN
        }  // FOR_EVERY_ROW

    }

    return system_matrix ;
}

  /******************************************************************************/
