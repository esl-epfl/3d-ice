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
#include "system_matrix.h"

/******************************************************************************/

void init_channel (Channel* channel)
{
  channel->Height             = 0.0 ;
  channel->CoolantHTCs.Side   = 0.0 ;
  channel->CoolantHTCs.Top    = 0.0 ;
  channel->CoolantHTCs.Bottom = 0.0 ;
  channel->CoolantVHC         = 0.0 ;
  channel->CoolantTIn         = 0.0 ;
  channel->CoolantFR          = 0.0 ;
  channel->Wall               = NULL ;
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

  Material* wall = channel->Wall ;

  if (wall != NULL)
  {
    fprintf(stream,
    "%s  Wall thermal conductivity         %.4e (%s)\n",
    prefix, wall->ThermalConductivity, wall->Id) ;
    fprintf(stream,
    "%s  Wall Volum. Heat Capacity         %.4e (%s)\n",
    prefix, wall->VolHeatCapacity, wall->Id) ;
  }
  else
    fprintf(stream,
    "%s  Wall material not specified\n", prefix) ;
}

/******************************************************************************/

Bool_t is_channel_column (Quantity_t column)
{
  // First bit is 0 -> even number -> wall -> FALSE -> 0
  // First bit is 1 -> odd number -> channel -> TRUE -> 1
  return column & 1 ;
}

/******************************************************************************/

Conductances* fill_conductances_channel
(
# ifdef PRINT_CONDUCTANCES
  LayerIndex_t  current_layer,
# endif
  Channel*      channel,
  Conductances* conductances,
  Dimensions*   dimensions
)
{
  RowIndex_t    row ;
  ColumnIndex_t column ;

# ifdef PRINT_CONDUCTANCES
  fprintf (stderr, "fill_conductances_channel %s\n", channel->Wall->Id) ;
# endif

  for
  (
    row = 0 ;
    row < get_number_of_rows (dimensions) ;
    row++
  )

    for
    (
      column = 0 ;
      column < get_number_of_columns (dimensions) ;
      column++, conductances++
    )

      if (is_channel_column(column) == TRUE_V)

        fill_conductances_liquid_cell
        (
#         ifdef PRINT_CONDUCTANCES
          current_layer, row, column,
#         endif
          dimensions,
          conductances,
          get_cell_length (dimensions, column),
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
          current_layer, row, column,
#         endif
          conductances,
          get_cell_length (dimensions, column),
          get_cell_width  (dimensions),
          channel->Height,
          channel->Wall->ThermalConductivity
        ) ;

  return conductances ;
}

/******************************************************************************/

/* ( [um3] . [ J / ( um3 . K ) ]) / [sec]  */
/* = [ J / (K . sec) ]                     */
/* = [ W / K ]                             */

static Capacity_t capacity
(
  CellDimension_t length,
  CellDimension_t width,
  CellDimension_t height,
  CoolantVHC_t    vhc,
  Time_t          time
)
{
  return ((length * width * height) * vhc) / time ;
}

Capacity_t* fill_capacities_channel
(
# ifdef PRINT_CAPACITIES
  LayerIndex_t current_layer,
# endif
  Channel*     channel,
  Capacity_t*  capacities,
  Dimensions*  dimensions,
  Time_t       delta_time
)
{
  RowIndex_t    row ;
  ColumnIndex_t column ;

# ifdef PRINT_CAPACITIES
  fprintf (stderr,
    "current_layer = %d\tfill_capacities_channel %s\n",
    current_layer, channel->Wall->Id) ;
# endif

  for
  (
    row = 0 ;
    row < get_number_of_rows (dimensions) ;
    row++
  )

    for
    (
      column = 0 ;
      column < get_number_of_columns (dimensions) ;
      column++,
      capacities++
    )

      if (is_channel_column(column) == TRUE_V )
      {
        *capacities  = capacity
                       (
                         get_cell_length(dimensions, column),
                         get_cell_width (dimensions),
                         channel->Height,
                         channel->CoolantVHC,
                         delta_time
                       ) ;
#       ifdef PRINT_CAPACITIES
        fprintf (stderr,
          "liquid cell  |  l %2d r %4d c %4d [%6d] |  l %5.2f w %5.2f h %5.2f "\
                      " |  %.5e [capacity] = (l x w x h x %.5e [coolant vhc]) / %.5e [delta] |\n",
          current_layer, row, column,
          get_cell_offset_in_stack (dimensions, current_layer, row, column),
          get_cell_length(dimensions, column), get_cell_width (dimensions),
          channel->Height, *capacities, channel->CoolantVHC, delta_time) ;
#       endif
      }
      else
      {
        *capacities = capacity
                      (
                        get_cell_length (dimensions, column),
                        get_cell_width (dimensions),
                        channel->Height,
                        channel->Wall->VolHeatCapacity,
                        delta_time
                      ) ;
#       ifdef PRINT_CAPACITIES
        fprintf (stderr,
          "solid cell   |  l %2d r %4d c %4d [%6d] |  l %5.2f w %5.2f h %5.2f "\
                      " |  %.5e [capacity] = (l x w x h x %.5e [wall    vhc]) / %.5e [delta] |\n",
          current_layer, row, column,
          get_cell_offset_in_stack (dimensions, current_layer, row, column),
          get_cell_length(dimensions, column), get_cell_width (dimensions),
          channel->Height, *capacities, channel->Wall->VolHeatCapacity, delta_time) ;
#       endif
      }

  return capacities ;
}

/******************************************************************************/

Source_t* fill_sources_channel
(
# ifdef PRINT_SOURCES
  LayerIndex_t current_layer,
# endif
  Channel*     channel,
  Source_t*    sources,
  Dimensions*  dimensions
)
{
  RowIndex_t    row ;
  ColumnIndex_t column ;

  Cconv_t C = get_c_conv(get_number_of_columns (dimensions),
                         channel->CoolantVHC, channel->CoolantFR) ;

# ifdef PRINT_SOURCES
  fprintf (stderr,
    "current_layer = %d\tfill_sources_channel %s\n",
    current_layer, channel->Wall->Id) ;
# endif

  for
  (
    row = 0 ;
    row < get_number_of_rows (dimensions) ;
    row++
  )

    for
    (
      column = 0 ;
      column < get_number_of_columns (dimensions) ;
      column++,
      sources++
    )

      if (row == 0 && is_channel_column(column) == TRUE_V)
      {
        *sources = 2.0 * C * channel->CoolantTIn ;

#       ifdef PRINT_SOURCES
        fprintf (stderr,
          "liquid cell  | l %2d r %4d c %4d [%6d] "
          "| %.5e [source] = 2 * %.2f [Tin] * %.5e [C]\n",
          current_layer, row, column,
          get_cell_offset_in_stack (dimensions, current_layer, row, column),
          *sources, channel->CoolantTIn, C) ;
#       endif
      }

  return sources ;
}

/******************************************************************************/

Quantity_t fill_system_matrix_channel
(
# ifdef PRINT_SYSTEM_MATRIX
  Channel*             channel,
# endif
  Dimensions*          dimensions,
  Conductances*        conductances,
  Capacity_t*          capacities,
  LayerIndex_t         current_layer,
  ColumnIndex_t*       column_pointers,
  RowIndex_t*          row_indices,
  SystemMatrixValue_t* values
)
{
  RowIndex_t row;
  ColumnIndex_t column;
  Quantity_t added, tot_added ;

# ifdef PRINT_SYSTEM_MATRIX
  fprintf (stderr,
    "(l %2d) fill_system_matrix_channel %s \n",
    current_layer, channel->Wall->Id) ;
# endif

  for
  (
    tot_added = 0 ,
    row       = 0 ;
    row < get_number_of_rows (dimensions) ;
    row++
  )

    for
    (
      column = 0 ;
      column < get_number_of_columns (dimensions) ;
      conductances    ++ ,
      capacities      ++ ,
      column_pointers ++ ,
      row_indices     += added ,
      values          += added ,
      tot_added       += added ,
      column          ++
    )

       if (is_channel_column(column) == TRUE_V )

         added = add_liquid_column
                 (
                   dimensions, conductances, capacities,
                   current_layer, row, column,
                   column_pointers, row_indices, values
                 ) ;

       else

         added = add_solid_column
                 (
                   dimensions, conductances, capacities,
                   NULL,
                   current_layer, row, column,
                   column_pointers, row_indices, values
                 ) ;

  return tot_added ;
}

/******************************************************************************/
