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

#include "dimensions.h"

/******************************************************************************/

void init_dimensions (Dimensions* dimensions)
{
  dimensions->Cell.FirstWallLength = CELLDIMENSION_I ;
  dimensions->Cell.LastWallLength  = CELLDIMENSION_I ;
  dimensions->Cell.WallLength      = CELLDIMENSION_I ;
  dimensions->Cell.ChannelLength   = CELLDIMENSION_I ;

  dimensions->Cell.Width           = CELLDIMENSION_I ;
  dimensions->Cell.Length          = CELLDIMENSION_I ;

  dimensions->Grid.NLayers     = GRIDDIMENSION_I ;
  dimensions->Grid.NRows       = GRIDDIMENSION_I ;
  dimensions->Grid.NColumns    = GRIDDIMENSION_I ;
  dimensions->Grid.NCells      = GRIDDIMENSION_I ;
  dimensions->Grid.NNz         = GRIDDIMENSION_I ;

  dimensions->Chip.Length      = CHIPDIMENSION_I ;
  dimensions->Chip.Width       = CHIPDIMENSION_I ;

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
  fprintf (stream,
    "%sGrid dimensions      (L x R x C) = (%d x %d x %d) -> %d nonzeroes\n",
    prefix, get_number_of_layers(dimensions), get_number_of_rows(dimensions),
            get_number_of_columns(dimensions), dimensions->Grid.NNz);

  fprintf (stream,
    "%sChip dimensions          (L x W) = (%.2f x %.2f) um\n",
    prefix, dimensions->Chip.Length, dimensions->Chip.Width);

  if (dimensions->StackHasChannel == FALSE_V )

    fprintf (stream,
      "%sCell dimensions          (l x w) = (%.2f x %.2f) um\n",
      prefix, dimensions->Cell.Length, dimensions->Cell.Width) ;

  else

    fprintf (stream,
      "%sCell dimensions (f, c, w, l) x w = (%.2f, %.2f, %.2f, %.2f) x %.2f um\n",
      prefix, dimensions->Cell.FirstWallLength,
              dimensions->Cell.ChannelLength,
              dimensions->Cell.WallLength,
              dimensions->Cell.LastWallLength,
              dimensions->Cell.Width) ;
}

/******************************************************************************/

void free_dimensions (Dimensions* dimensions)
{
  free (dimensions);
}

/******************************************************************************/

extern Bool_t is_channel_column (GridDimension_t column) ;

CellDimension_t get_cell_length (Dimensions* dimensions, GridDimension_t column)
{
  if (dimensions->StackHasChannel == FALSE_V)

    return dimensions->Cell.Length ;

  if (column == 0)

    return dimensions->Cell.FirstWallLength ;

  else if (column == get_number_of_columns (dimensions) - 1)

    return dimensions->Cell.LastWallLength ;

  else

    if (is_channel_column(column) == TRUE_V)

      return dimensions->Cell.ChannelLength ;

    else

      return dimensions->Cell.WallLength ;
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

Quantity_t get_number_of_non_zeroes (Dimensions* dimensions)
{
  return dimensions->Grid.NNz ;
}

/******************************************************************************/

GridDimension_t get_layer_area (Dimensions* dimensions)
{
  return dimensions->Grid.NRows * dimensions->Grid.NColumns ;
}

/******************************************************************************/

GridDimension_t get_cell_offset_in_layer
(
  Dimensions*     dimensions,
  GridDimension_t row,
  GridDimension_t column
)
{
  return row * get_number_of_columns (dimensions) + column ;
}

/******************************************************************************/

GridDimension_t get_cell_offset_in_stack
(
  Dimensions*     dimensions,
  GridDimension_t layer,
  GridDimension_t row,
  GridDimension_t column
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
