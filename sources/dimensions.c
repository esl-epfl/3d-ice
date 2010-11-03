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
#include "macros.h"

/******************************************************************************/

void init_dimensions (Dimensions* dimensions)
{
  dimensions->Cell.FirstWallLength = CELLDIMENSION_I ;
  dimensions->Cell.LastWallLength  = CELLDIMENSION_I ;
  dimensions->Cell.WallLength      = CELLDIMENSION_I ;
  dimensions->Cell.ChannelLength   = CELLDIMENSION_I ;

  dimensions->Cell.Width           = CELLDIMENSION_I ;

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
  Dimensions* dimensions = malloc (sizeof(Dimensions)) ;

  if (dimensions != NULL) init_dimensions (dimensions) ;

  return dimensions ;
}

/******************************************************************************/

void print_formatted_dimensions
(
  FILE*       stream,
  String_t    prefix,
  Dimensions* dimensions
)
{
  fprintf (stream,
           STRING_F "dimensions : \n",
           prefix) ;

  fprintf (stream,
           STRING_F "   chip length " CHIPDIMENSION_F " , width " CHIPDIMENSION_F " ;\n",
           prefix, dimensions->Chip.Length, dimensions->Chip.Width) ;

  fprintf (stream,
           STRING_F "   cell length " CELLDIMENSION_F " , width " CELLDIMENSION_F " ;\n",
           prefix, dimensions->Cell.WallLength, dimensions->Cell.Width) ;
}

/******************************************************************************/

void print_detailed_dimensions
(
  FILE* stream,
  String_t prefix,
  Dimensions* dimensions
)
{
  fprintf (stream,
           STRING_F "dimensions                  = %p\n",
           prefix, dimensions) ;

  fprintf (stream,
           STRING_F "  Cell.FirstWallLength      = " CELLDIMENSION_F "\n",
           prefix, dimensions->Cell.FirstWallLength) ;

  fprintf (stream,
           STRING_F "  Cell.WallLength           = " CELLDIMENSION_F "\n",
           prefix, dimensions->Cell.WallLength) ;

  fprintf (stream,
           STRING_F "  Cell.LastWallLength       = " CELLDIMENSION_F "\n",
           prefix, dimensions->Cell.LastWallLength) ;

  fprintf (stream,
           STRING_F "  Cell.ChannelLength        = " CELLDIMENSION_F "\n",
           prefix, dimensions->Cell.ChannelLength) ;

  fprintf (stream,
           STRING_F "  Cell.Width                = " CELLDIMENSION_F "\n",
           prefix, dimensions->Cell.Width) ;

  fprintf (stream,
           STRING_F "  Grid.NLayers              = " GRIDDIMENSION_F "\n",
           prefix, dimensions->Grid.NLayers) ;

  fprintf (stream,
           STRING_F "  Grid.NRows                = " GRIDDIMENSION_F "\n",
           prefix, dimensions->Grid.NRows) ;

  fprintf (stream,
           STRING_F "  Grid.NColumns             = " GRIDDIMENSION_F "\n",
           prefix, dimensions->Grid.NColumns) ;

  fprintf (stream,
           STRING_F "  Grid.NCells               = " GRIDDIMENSION_F "\n",
           prefix, dimensions->Grid.NCells) ;

  fprintf (stream,
           STRING_F "  Grid.NNz                  = " GRIDDIMENSION_F "\n",
           prefix, dimensions->Grid.NNz) ;

  fprintf (stream,
           STRING_F "  Chip.Length               = " CHIPDIMENSION_F "\n",
           prefix, dimensions->Chip.Length) ;

  fprintf (stream,
           STRING_F "  Chip.Width                = " CHIPDIMENSION_F "\n",
           prefix, dimensions->Chip.Width) ;

  fprintf (stream,
           STRING_F "  StackHasChannel           = " BOOL_F "\n",
           prefix, dimensions->StackHasChannel) ;
}

/******************************************************************************/

void free_dimensions (Dimensions* dimensions)
{
  free (dimensions) ;
}

/******************************************************************************/

CellDimension_t get_cell_length
(
  Dimensions*     dimensions,
  GridDimension_t column_index
)
{
  if (!dimensions->StackHasChannel)

    return dimensions->Cell.WallLength ;

  if (IS_FIRST_COLUMN(column_index))

    return dimensions->Cell.FirstWallLength ;

  else if (IS_LAST_COLUMN(column_index, dimensions))

    return dimensions->Cell.LastWallLength ;

  else

    if (IS_CHANNEL_COLUMN(column_index))

      return dimensions->Cell.ChannelLength ;

    else

      return dimensions->Cell.WallLength ;
}

/******************************************************************************/

CellDimension_t get_cell_width (Dimensions* dimensions)
{
  return dimensions->Cell.Width ;
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
  GridDimension_t row_index,
  GridDimension_t column_index
)
{
  return row_index * get_number_of_columns (dimensions) + column_index ;
}

/******************************************************************************/

GridDimension_t get_cell_offset_in_stack
(
  Dimensions*     dimensions,
  GridDimension_t layer_index,
  GridDimension_t row_index,
  GridDimension_t column_index
)
{
  return layer_index * get_layer_area (dimensions)
         + get_cell_offset_in_layer (dimensions, row_index, column_index) ;
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
