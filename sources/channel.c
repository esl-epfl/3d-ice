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
  channel->Height       = CELLDIMENSION_I ;
  channel->CoolantHTCs  = COOLANTHTCS_I ;
  channel->CoolantVHC   = COOLANTVHC_I ;
  channel->CoolantTIn   = TEMPERATURE_I ;
  channel->CoolantFR    = COOLANTFR_I ;
  channel->WallMaterial = NULL ;
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
  free (channel) ;
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
           STRING_F "channel :\n",
           prefix) ;

  fprintf (stream,
           STRING_F "              height " CELLDIMENSION_F " ;\n",
           prefix, channel->Height) ;

  fprintf (stream,
           STRING_F "      channel length " CELLDIMENSION_F " ;\n",
           prefix, dimensions->Cell.ChannelLength) ;

  fprintf (stream,
           STRING_F "         wall length " CELLDIMENSION_F " ;\n",
           prefix, dimensions->Cell.WallLength) ;

  fprintf (stream,
           STRING_F "   first wall length " CELLDIMENSION_F " ;\n",
           prefix, dimensions->Cell.FirstWallLength) ;

  fprintf (stream,
           STRING_F "    last wall length " CELLDIMENSION_F " ;\n",
           prefix, dimensions->Cell.LastWallLength) ;

  fprintf (stream,
           STRING_F "   wall material " STRING_F " ;\n",
           prefix, channel->WallMaterial->Id) ;

  fprintf (stream,
           STRING_F "   coolant flow rate " COOLANTFR_F " ;\n",
           prefix, channel->CoolantFR) ;

  fprintf (stream,
           STRING_F "   coolant heat transfert coefficent side " COOLANTHTC_F " ,\n",
           prefix, channel->CoolantHTCs.Side) ;

  fprintf (stream,
           STRING_F "                                      top " COOLANTHTC_F " ,\n",
           prefix, channel->CoolantHTCs.Top) ;

  fprintf (stream,
           STRING_F "                                   bottom " COOLANTHTC_F " ;\n",
           prefix, channel->CoolantHTCs.Bottom) ;

  fprintf (stream,
           STRING_F "   coolant volumetric heat capacity " COOLANTVHC_F " ;\n",
           prefix, channel->CoolantVHC) ;

  fprintf (stream,
           STRING_F "   coolant incoming temperature " TEMPERATURE_F " ;\n",
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
           STRING_F "channel                     = %p\n",
           prefix, channel) ;

  fprintf (stream,
           STRING_F "channel->Height             = " CELLDIMENSION_F "\n",
           prefix, channel->Height) ;

  fprintf (stream,
           STRING_F "channel->CoolantHTCs.Side   = " COOLANTHTC_F "\n",
           prefix, channel->CoolantHTCs.Side) ;

  fprintf (stream,
           STRING_F "channel->CoolantHTCs.Top    = " COOLANTHTC_F "\n",
           prefix, channel->CoolantHTCs.Top) ;

  fprintf (stream,
           STRING_F "channel->CoolantHTCs.Bottom = " COOLANTHTC_F "\n",
           prefix, channel->CoolantHTCs.Bottom) ;

  fprintf (stream,
           STRING_F "channel->CoolantVHC         = " COOLANTVHC_F "\n",
           prefix, channel->CoolantVHC) ;

  fprintf (stream,
           STRING_F "channel->CoolantTIn         = " TEMPERATURE_F "\n",
           prefix, channel->CoolantTIn) ;

  fprintf (stream,
           STRING_F "channel->CoolantFR          = " COOLANTFR_F "\n",
           prefix, channel->CoolantFR) ;

  fprintf (stream,
           STRING_F "channel->WallMaterial       = %p\n",
           prefix, channel->WallMaterial) ;
}

/******************************************************************************/

void fill_thermal_grid_data_channel
  (
  ThermalGridData* thermalgriddata,
  GridDimension_t  layer_index,
  Channel*         channel
)
{
# ifdef PRINT_THERMAL_GRID_DATA
  fprintf (stderr, "\n#%d\tChannel [%s]\n\n", layer_index, channel->Wall->Id) ;
# endif

  fill_thermal_grid_data
  (
    thermalgriddata,
    layer_index,
    TDICE_LAYER_CHANNEL,
    channel->WallMaterial->ThermalConductivity,
    channel->WallMaterial->VolumetricHeatCapacity,
    channel->Height
  ) ;
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
  ThermalGridData*      thermalgriddata,
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
                           dimensions, thermalgriddata,
                           layer_index, row_index, column_index,
                           system_matrix
                         ) ;

       else

         system_matrix = add_solid_column
                         (
                           dimensions, thermalgriddata,
                           layer_index, row_index, column_index,
                           system_matrix
                         ) ;

    } // FOR_EVERY_COLUMN
  }  // FOR_EVERY_ROW

  return system_matrix ;
}

/******************************************************************************/
