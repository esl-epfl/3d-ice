/******************************************************************************
 * Source file "3D-ICe/sources/dimensions.c"                                  *
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
 * Copyright (C) 2010,                                                        *
 * Embedded Systems Laboratory - Ecole Polytechnique Federale de Lausanne.    *
 * All Rights Reserved.                                                       *
 *                                                                            *
 * Authors: Alessandro Vincenzi, Arvind Sridhar.                              *
 *                                                                            *
 * EPFL-STI-IEL-ESL                                                           *
 * Bâtiment ELG, ELG 130                                                      *
 * Station 11                                                                 *
 * 1015 Lausanne, Switzerland                          threed-ice@esl.epfl.ch *
 ******************************************************************************/

#include <stdlib.h>

#include "dimensions.h"

/******************************************************************************/

void init_dimensions (Dimensions* dimensions)
{
  dimensions->Cell.FirstWallLength = 0.0 ;
  dimensions->Cell.LastWallLength  = 0.0 ;
  dimensions->Cell.WallLength      = 0.0 ;
  dimensions->Cell.ChannelLength   = 0.0 ;

  dimensions->Cell.Width           = 0.0 ;

  dimensions->Grid.NLayers     = 0 ;
  dimensions->Grid.NRows       = 0 ;
  dimensions->Grid.NColumns    = 0 ;
  dimensions->Grid.NCells      = 0 ;
  dimensions->Grid.NNz         = 0 ;

  dimensions->Chip.Length      = 0.0;
  dimensions->Chip.Width       = 0.0;

  dimensions->StackHasChannel = FALSE_V ;
}

/******************************************************************************/

Dimensions* alloc_and_init_dimensions (void)
{
  Dimensions* dimensions = (Dimensions*) malloc (sizeof (Dimensions));

  if (dimensions != NULL) init_dimensions (dimensions);

  return dimensions;
}

/******************************************************************************/

void print_dimensions (FILE* stream, String_t prefix, Dimensions* dimensions)
{
  if (dimensions->StackHasChannel == FALSE_V )

    fprintf (stream,
      "%sCell dimensions         (l x w) = (%5.2f x %5.2f) um\n",
      prefix, dimensions->Cell.WallLength, dimensions->Cell.Width) ;

  else

    fprintf (stream,
      "%sCell dimensions (f, c, w, l) x w = (%5.2f, %5.2f, %5.2f, %5.2f) x %5.2f um\n",
      prefix, dimensions->Cell.FirstWallLength, dimensions->Cell.ChannelLength,
              dimensions->Cell.WallLength, dimensions->Cell.LastWallLength,
              dimensions->Cell.Width) ;

  fprintf (stream,
    "%sGrid dimensions     (L x R x C) = (%d x %d x %d) -> %d nonzeroes\n",
    prefix, dimensions->Grid.NLayers,  dimensions->Grid.NRows,
            dimensions->Grid.NColumns, dimensions->Grid.NNz);
  fprintf (stream,
    "%sChip dimensions         (L x W) = (%5.2f x %5.2f) mm\n",
    prefix, dimensions->Chip.Length / 1000.0, dimensions->Chip.Width / 1000.0);
}

/******************************************************************************/

void free_dimensions (Dimensions* dimensions)
{
  free (dimensions);
}

/******************************************************************************/

CellDimension_t get_cell_length (Dimensions* dimensions, GridDimension_t column)
{
  if (dimensions->StackHasChannel == FALSE_V)

    return dimensions->Cell.WallLength ;

  if (column == 0)

    return dimensions->Cell.FirstWallLength ;

  else if (column == get_number_of_columns (dimensions) - 1)

    return dimensions->Cell.LastWallLength ;

  else

    if (column % 2 == 0) // Even -> wall

      return dimensions->Cell.WallLength ;

    else                 // Odd -> channel

      return dimensions->Cell.ChannelLength ;
}

/******************************************************************************/

CellDimension_t get_cell_width (Dimensions* dimensions)
{
  return dimensions->Cell.Width;
}

/******************************************************************************/

GridDimension_t get_number_of_layers (Dimensions* dimensions)
{
  return dimensions->Grid.NLayers ;
}

/******************************************************************************/

GridDimension_t get_number_of_rows (Dimensions* dimensions)
{
  return dimensions->Grid.NRows ;
}

/******************************************************************************/

GridDimension_t get_number_of_columns (Dimensions* dimensions)
{
  return dimensions->Grid.NColumns ;
}

/******************************************************************************/

GridDimension_t get_number_of_cells (Dimensions* dimensions)
{
  return dimensions->Grid.NCells ;
}

/******************************************************************************/

GridDimension_t get_number_of_non_zeroes (Dimensions* dimensions)
{
  return dimensions->Grid.NNz ;
}

/******************************************************************************/

Quantity_t get_layer_area (Dimensions* dimensions)
{
  return dimensions->Grid.NRows * dimensions->Grid.NColumns ;
}

/******************************************************************************/

Quantity_t get_cell_offset_in_layer
(
  Dimensions*   dimensions,
  RowIndex_t    row,
  ColumnIndex_t column
)
{
  return row * get_number_of_columns (dimensions) + column ;
}

/******************************************************************************/

Quantity_t get_cell_offset_in_stack
(
  Dimensions*   dimensions,
  LayerIndex_t  layer,
  RowIndex_t    row,
  ColumnIndex_t column
)
{
  return layer * get_layer_area (dimensions)
         + get_cell_offset_in_layer (dimensions, row, column) ;
}

/******************************************************************************/

ChipDimension_t get_chip_length (Dimensions* dimensions)
{
  return dimensions->Chip.Length ;
}

/******************************************************************************/

ChipDimension_t get_chip_width (Dimensions* dimensions)
{
  return dimensions->Chip.Width ;
}

/******************************************************************************/
