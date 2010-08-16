
/******************************************************************************
 *                                                                            *
 * Source file "3D-ICe/sources/channel.c"                                     *
 *                                                                            *
 * This file is part of 3D-ICe (http://esl.epfl.ch/3D-ICe), revision 0.1      *
 *                                                                            *
 * 3D-ICe is free software: you can redistribute it and/or modify it under    *
 * the terms of the GNU General Public License as published by the Free       *
 * Software Foundation, either version 3 of the License, or any later         *
 * version.                                                                   *
 *                                                                            *
 * 3D-ICe is distributed in the hope that it will be useful, but WITHOUT      *
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or      *
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for   *
 * more details.                                                              *
 *                                                                            *
 * You should have received a copy of the GNU General Public License along    *
 * with 3D-ICe.  If not, see <http://www.gnu.org/licenses/>.                  *
 *                                                                            *
 *        Copyright (c) 2010, Ecole Polytechnique Fédérale de Lausanne.       *
 *                            All Rights Reserved.                            *
 *                                                                            *
 * Authors: Alessandro Vincenzi, Arvind Sridhar.       threed-ice@esl.epfl.ch *
 *                                                                            *
 * EPFL-STI-IEL-ESL                                                           *
 * Bâtiment ELG, ELG 130                                                      *
 * Station 11                                                                 *
 * 1015 Lausanne, Switzerland                                                 *
 ******************************************************************************/

#include <stdlib.h>

#include "channel.h"
#include "system_matrix.h"

/******************************************************************************/

void init_channel (Channel* channel)
{
  channel->Height             = 0.0  ;
  channel->CoolantHTCs.Side   = 0.0  ;
  channel->CoolantHTCs.Top    = 0.0  ;
  channel->CoolantHTCs.Bottom = 0.0  ;
  channel->CoolantVHC         = 0.0  ;
  channel->CoolantTIn         = 0.0  ;
  channel->CoolantFR          = 0.0  ;
  channel->Wall               = NULL ;
  channel->FlowRateChanged    = 0    ;
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

  Material *wall = channel->Wall ;

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

Conductances*   fill_conductances_channel
(
  #ifdef PRINT_CAPACITIES
  LayerIndex_t  current_layer
  #endif
  Channel*      channel,
  Conductances* conductances,
  Dimensions*   dimensions
)
{
  RowIndex_t    row ;
  ColumnIndex_t column ;

#ifdef PRINT_CONDUCTANCES
  fprintf (stderr, "fill_conductances_channel %s\n", channel->Wall->Id) ;
#endif

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

      if (column % 2 == 0) // Even -> wall

        fill_conductances_wall_cell
        (
          #ifdef PRINT_CONDUCTANCES
          dimensions,
          current_layer, row, column,
          #endif
          conductances,
          get_cell_length (dimensions, column),
          get_cell_width  (dimensions),
          channel->Height,
          channel->Wall->ThermalConductivity
        ) ;

      else                 // Odd -> channel

        fill_conductances_liquid_cell
        (
          #ifdef PRINT_CONDUCTANCES
          current_layer, row, column,
          #endif
          dimensions,
          conductances,
          get_cell_length (dimensions, column),
          get_cell_width  (dimensions),
          channel->Height,
          channel->CoolantHTCs,
          channel->CoolantVHC,
          channel->CoolantFR
        ) ;

  return conductances ;
}

/******************************************************************************/

/* ( [um3] . [ J / ( um3 . K ) ]) / [sec]  */
/* = [ J / (K . sec) ]                     */
/* = [ W / K ]                             */

static double capacity
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

Capacity_t*    fill_capacities_channel
(
  #ifdef PRINT_CAPACITIES
  LayerIndex_t current_layer,
  #endif
  Channel*     channel,
  Capacity_t*  capacities,
  Dimensions*  dimensions,
  Time_t       delta_time
)
{
  RowIndex_t    row ;
  ColumnIndex_t column ;

#ifdef PRINT_CAPACITIES
  fprintf (stderr,
    "current_layer = %d\tfill_capacities_channel %s\n",
    current_layer, channel->Wall->Id) ;
#endif

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

      if (column % 2 == 0 ) /* Even -> Wall */
      {
        *capacities = capacity
                      (
                        get_cell_length (dimensions, column),
                        get_cell_width (dimensions),
                        channel->Height,
                        channel->Wall->VolHeatCapacity,
                        delta_time
                      ) ;
#ifdef PRINT_CAPACITIES
        fprintf (stderr,
          "solid cell   |  l %2d r %4d c %4d [%6d] |  l %5.2f w %5.2f h %5.2f "\
                      " |  vhc %.5e --> %.5e\n",
          current_layer, row, column,
          get_cell_offset_in_stack (dimensions, current_layer, row, column),
          get_cell_length(dimensions, column), get_cell_width (dimensions),
          channel->Height,
          channel->Wall->VolHeatCapacity,
          *capacities) ;
#endif
      }
      else                 /* Odd -> liquid */
      {
        *capacities  = capacity
                       (
                         get_cell_length(dimensions, column),
                         get_cell_width (dimensions),
                         channel->Height,
                         channel->CoolantVHC,
                         delta_time
                       ) ;
#ifdef PRINT_CAPACITIES
        fprintf (stderr,
          "liquid cell  |  l %2d r %4d c %4d [%6d] |  l %5.2f w %5.2f h %5.2f "\
                      " |  vhc %.5e --> %.5e\n",
          current_layer, row, column,
          get_cell_offset_in_stack (dimensions, current_layer, row, column),
          get_cell_length(dimensions, column), get_cell_width (dimensions),
          channel->Height,
          channel->CoolantVHC, *capacities) ;
#endif
      }

  return capacities ;
}

/******************************************************************************/

Source_t*      fill_sources_channel
(
  #ifdef PRINT_SOURCES
  LayerIndex_t current_layer,
  #endif
  Channel*     channel,
  Source_t*    sources,
  Dimensions*  dimensions
)
{
  RowIndex_t    row ;
  ColumnIndex_t column ;

  double C = (channel->CoolantVHC * channel->CoolantFR)
             / (double) ( get_number_of_columns (dimensions) - 1 ) ;

#ifdef PRINT_SOURCES
  fprintf (stderr,
    "current_layer = %d\tfill_sources_channel %s " \
    " C = %.5e = (%.5e * %.5e) / %d \n",
    current_layer, channel->Wall->Id,
    C, channel->CoolantVHC, channel->CoolantFR,
    get_number_of_columns (dimensions) - 1) ;
#endif

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

      if (row == 0 && column % 2 != 0) /* Only first row and odd columns */
      {
        *sources = 2.0 * C * channel->CoolantTIn ;

#ifdef PRINT_SOURCES
        fprintf (stderr,
          "liquid cell  |  l %2d r %4d c %4d [%6d] | %.5e  (Tin = %.2f)\n",
          current_layer, row, column,
          get_cell_offset_in_stack (dimensions, current_layer, row, column),
          *sources,
          channel->CoolantTIn) ;
#endif
      }

  return sources ;
}

/******************************************************************************/

Quantity_t      fill_ccs_system_matrix_channel
(
  #ifdef PRINT_SYSTEM_MATRIX
  Channel*      channel,
  #endif
  Dimensions*   dimensions,
  Conductances* conductances,
  Capacity_t*   capacities,
  LayerIndex_t  current_layer,
  int*          columns,
  int*          rows,
  double*       values
)
{
  RowIndex_t row;
  ColumnIndex_t column;
  Quantity_t added, tot_added ;

#ifdef PRINT_SYSTEM_MATRIX
  fprintf (stderr,
    "(l %2d) fill_ccs_system_matrix_channel %s \n",
    current_layer, channel->Wall->Id) ;
#endif

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
      conductances ++ ,
      capacities   ++ ,
      columns      ++ ,
      rows         += added ,
      values       += added ,
      tot_added    += added ,
      column       ++
    )

       if (column % 2 == 0 ) /* Even -> Wall */

         added = add_ccs_solid_column
                 (
                   dimensions, conductances, capacities,
                   current_layer, row, column,
                   columns, rows, values
                 ) ;

       else                  /* Odd -> liquid */

         added = add_ccs_liquid_column
                 (
                   dimensions, conductances, capacities,
                   current_layer, row, column,
                   columns, rows, values
                 ) ;

  return tot_added ;
}

/******************************************************************************/

Quantity_t      fill_crs_system_matrix_channel
(
  #ifdef PRINT_SYSTEM_MATRIX
  Channel*      channel,
  #endif
  Dimensions*   dimensions,
  Conductances* conductances,
  Capacity_t*   capacities,
  LayerIndex_t  current_layer,
  int*          rows,
  int*          columns,
  double*       values
)
{
  RowIndex_t row;
  ColumnIndex_t column;
  Quantity_t added, tot_added ;

#ifdef PRINT_SYSTEM_MATRIX
  fprintf (stderr,
    "(l %2d) fill_crs_system_matrix_channel %s \n",
    current_layer, channel->Wall->Id) ;
#endif

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

      conductances ++ ,
      capacities   ++ ,
      rows         ++ ,
      columns      += added ,
      values       += added ,
      tot_added    += added ,
      column       ++
    )

       if (column % 2 == 0 ) /* Even -> Wall */

         added = add_crs_solid_column
                 (
                   dimensions, conductances, capacities,
                   NULL,
                   current_layer, row, column,
                   rows, columns, values
                 ) ;

       else                  /* Odd -> liquid */

         added = add_crs_liquid_column
                 (
                   dimensions, conductances, capacities,
                   current_layer, row, column,
                   rows, columns, values
                 ) ;

  return tot_added ;
}

/******************************************************************************/
