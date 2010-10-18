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
  channel->Height             = CELLDIMENSION_I ;
  channel->CoolantHTCs.Side   = COOLANTHTC_I ;
  channel->CoolantHTCs.Top    = COOLANTHTC_I ;
  channel->CoolantHTCs.Bottom = COOLANTHTC_I ;
  channel->CoolantVHC         = COOLANTVHC_I ;
  channel->CoolantTIn         = TEMPERATURE_I ;
  channel->CoolantFR          = COOLANTFR_I ;
  channel->WallMaterial       = NULL ;
}

/******************************************************************************/

Channel* alloc_and_init_channel (void)
{
  Channel* channel = (Channel*) malloc ( sizeof(Channel) ) ;

  if (channel != NULL) init_channel (channel) ;

  return channel ;
}

/******************************************************************************/

void free_channel (Channel* channel)
{
  free (channel) ;
}

/******************************************************************************/

void print_channel (FILE* stream, String_t prefix, Channel* channel)
{
  fprintf(stream,
    "%sChannel\n",                                   prefix) ;
  fprintf(stream,
    "%s  Height                            %5.2f\n", prefix,
                                                     channel->Height) ;
  fprintf(stream,
    "%s  Coolant Heat Transfert Coefficents s %.5e t %.5e b %.5e\n",
                                                  prefix,
                                                  channel->CoolantHTCs.Side,
                                                  channel->CoolantHTCs.Top,
                                                  channel->CoolantHTCs.Bottom) ;
  fprintf(stream,
    "%s  Coolant Volumetric Heat Capacity  %.4e\n",  prefix,
                                                     channel->CoolantVHC) ;
  fprintf(stream,
    "%s  Coolant incoming temperature      %.4e\n",  prefix,
                                                     channel->CoolantTIn) ;
  fprintf(stream,
    "%s  Flow Rate                         %.4e\n",  prefix,
                                                     channel->CoolantFR) ;

  Material* wall = channel->WallMaterial ;

  if (wall != NULL)
  {
    fprintf(stream,
    "%s  WallMaterial thermal conductivity         %.4e (%s)\n",
    prefix, wall->ThermalConductivity, wall->Id) ;
    fprintf(stream,
    "%s  WallMaterial Volum. Heat Capacity         %.4e (%s)\n",
    prefix, wall->VolumetricHeatCapacity, wall->Id) ;
  }
  else
    fprintf(stream,
    "%s  WallMaterial material not specified\n", prefix) ;
}

/******************************************************************************/

Conductances* fill_conductances_channel
(
# ifdef PRINT_CONDUCTANCES
  GridDimension_t layer_index,
# endif
  Channel*        channel,
  Conductances*   conductances,
  Dimensions*     dimensions
)
{
# ifdef PRINT_CONDUCTANCES
  fprintf (stderr, "fill_conductances_channel %s\n",
           channel->WallMaterial->Id) ;
# endif

  FOR_EVERY_ROW (row_index, dimensions)
  {
    FOR_EVERY_COLUMN (column_index, dimensions)
    {
      if (IS_CHANNEL_COLUMN(column_index))

        fill_conductances_liquid_cell
        (
#         ifdef PRINT_CONDUCTANCES
          layer_index, row_index, column_index,
#         endif
          dimensions,
          conductances,
          get_cell_length (dimensions, column_index),
          get_cell_width  (dimensions),
          channel->Height,
          channel->CoolantHTCs,
          channel->CoolantVHC,
          channel->CoolantFR
        ) ;

      else

        fill_conductances_wall_cell
        (
#         ifdef PRINT_CONDUCTANCES
          dimensions,
          layer_index, row_index, column_index,
#         endif
          conductances,
          get_cell_length (dimensions, column_index),
          get_cell_width  (dimensions),
          channel->Height,
          channel->WallMaterial->ThermalConductivity
        ) ;

      conductances++ ;

    } // FOR_EVERY_COLUMN
  } // FOR_EVERY_ROW

  return conductances ;
}

/******************************************************************************/

Conductances* update_conductances_channel
(
# ifdef PRINT_CONDUCTANCES
  GridDimension_t layer_index,
# endif
  Channel*        channel,
  Conductances*   conductances,
  Dimensions*     dimensions
)
{
# ifdef PRINT_CONDUCTANCES
  fprintf (stderr, "update_conductances_channel %s\n",
           channel->WallMaterial->Id) ;
# endif

  FOR_EVERY_ROW (row_index, dimensions)
  {
    FOR_EVERY_COLUMN (column_index, dimensions)
    {
      if (IS_CHANNEL_COLUMN(column_index))

        update_conductances_liquid_cell
        (
#         ifdef PRINT_CONDUCTANCES
          layer_index, row_index, column_index,
          get_cell_length (dimensions, column_index),
          get_cell_width  (dimensions),
          channel->Height,
#         endif
          dimensions,
          conductances,
          channel->CoolantVHC,
          channel->CoolantFR
        ) ;

      conductances++ ;

    } // FOR_EVERY_COLUMN
  } // FOR_EVERY_ROW

  return conductances ;
}

/******************************************************************************/

/* ( [um3] . [ J / ( um3 . K ) ]) / [sec]  */
/* = [ J / (K . sec) ]                     */
/* = [ W / K ]                             */

Capacity_t* fill_capacities_channel
(
# ifdef PRINT_CAPACITIES
  GridDimension_t layer_index,
# endif
  Channel*        channel,
  Capacity_t*     capacities,
  Dimensions*     dimensions,
  Time_t          delta_time
)
{
# ifdef PRINT_CAPACITIES
  fprintf (stderr,
    "layer_index = %d\tfill_capacities_channel %s\n",
    layer_index, channel->WallMaterial->Id) ;
# endif

  FOR_EVERY_ROW (row_index, dimensions)
  {
    FOR_EVERY_COLUMN (column_index, dimensions)
    {
      if (IS_CHANNEL_COLUMN(column_index))
      {
        *capacities  = CAPACITY
                       (
                         get_cell_length (dimensions, column_index),
                         get_cell_width  (dimensions),
                         channel->Height,
                         channel->CoolantVHC,
                         delta_time
                       ) ;
#       ifdef PRINT_CAPACITIES
        fprintf (stderr,
          "liquid cell  |  l %2d r %4d c %4d [%6d] |  l %5.2f w %5.2f h %5.2f "\
                      " |  %.5e [capacity] = (l x w x h x %.5e [coolant vhc]) / %.5e [delta] |\n",
          layer_index, row_index, column_index,
          get_cell_offset_in_stack (dimensions, layer_index, row_index, column_index),
          get_cell_length(dimensions, column_index), get_cell_width (dimensions),
          channel->Height, *capacities, channel->CoolantVHC, delta_time) ;
#       endif
      }
      else
      {
        *capacities = CAPACITY
                      (
                        get_cell_length (dimensions, column_index),
                        get_cell_width  (dimensions),
                        channel->Height,
                        channel->WallMaterial->VolumetricHeatCapacity,
                        delta_time
                      ) ;
#       ifdef PRINT_CAPACITIES
        fprintf (stderr,
          "solid cell   |  l %2d r %4d c %4d [%6d] |  l %5.2f w %5.2f h %5.2f "\
                      " |  %.5e [capacity] = (l x w x h x %.5e [wall    vhc]) / %.5e [delta] |\n",
          layer_index, row_index, column_index,
          get_cell_offset_in_stack (dimensions, layer_index, row_index, column_index),
          get_cell_length(dimensions, column_index), get_cell_width (dimensions),
          channel->Height, *capacities, channel->WallMaterial->VolumetricHeatCapacity, delta_time) ;
#       endif
      }

      capacities++ ;

    } // FOR_EVERY_COLUMN
  } // FOR_EVERY_ROW

  return capacities ;
}

/******************************************************************************/

Source_t* fill_sources_channel
(
# ifdef PRINT_SOURCES
  GridDimension_t layer_index,
# endif
  Channel*        channel,
  Source_t*       sources,
  Dimensions*     dimensions
)
{
  Cconv_t C = CCONV(get_number_of_columns (dimensions),
                    channel->CoolantVHC, channel->CoolantFR) ;

# ifdef PRINT_SOURCES
  fprintf (stderr,
    "layer_index = %d\tfill_sources_channel %s\n",
    layer_index, channel->WallMaterial->Id) ;
# endif

  FOR_EVERY_ROW (row_index, dimensions)
  {
    FOR_EVERY_COLUMN (column_index, dimensions)
    {
      if (IS_FIRST_ROW(row_index) && IS_CHANNEL_COLUMN(column_index))
      {
        *sources = 2.0 * C * channel->CoolantTIn ;

#       ifdef PRINT_SOURCES
        fprintf (stderr,
          "liquid cell  | l %2d r %4d c %4d [%6d] "
          "| %.5e [source] = 2 * %.2f [Tin] * %.5e [C]\n",
          layer_index, row_index, column_index,
          get_cell_offset_in_stack (dimensions, layer_index, row_index, column_index),
          *sources, channel->CoolantTIn, C) ;
#       endif
      }

      sources++ ;

    } // FOR_EVERY_COLUMN
  }  // FOR_EVERY_ROW

  return sources ;
}

/******************************************************************************/

SystemMatrix fill_system_matrix_channel
(
# ifdef PRINT_SYSTEM_MATRIX
  Channel*              channel,
# endif
  Dimensions*           dimensions,
  Conductances*         conductances,
  Capacity_t*           capacities,
  GridDimension_t       layer_index,
  SystemMatrix          system_matrix
)
{
# ifdef PRINT_SYSTEM_MATRIX
  fprintf (stderr,
    "(l %2d) fill_system_matrix_channel %s \n",
    layer_index, channel->WallMaterial->Id) ;
# endif

  FOR_EVERY_ROW (row_index, dimensions)
  {
    FOR_EVERY_COLUMN (column_index, dimensions)
    {
       if (IS_CHANNEL_COLUMN(column_index))

         system_matrix = add_liquid_column
                         (
                           dimensions, conductances, capacities,
                           layer_index, row_index, column_index,
                           system_matrix
                         ) ;

       else

         system_matrix = add_solid_column
                         (
                           dimensions, conductances, capacities,
                           NULL,
                           layer_index, row_index, column_index,
                           system_matrix
                         ) ;

      conductances    ++ ;
      capacities      ++ ;

    } // FOR_EVERY_COLUMN
  }  // FOR_EVERY_ROW

  return system_matrix ;
}

/******************************************************************************/
