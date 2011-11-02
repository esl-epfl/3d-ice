/******************************************************************************
 * This file is part of 3D-ICE, version 1.0.2 .                               *
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

void init_dimensions (Dimensions *dimensions)
{
    dimensions->Cell.FirstWallLength = 0.0 ;
    dimensions->Cell.LastWallLength  = 0.0 ;
    dimensions->Cell.WallLength      = 0.0 ;
    dimensions->Cell.ChannelLength   = 0.0 ;

    dimensions->Cell.Width = 0.0 ;

    dimensions->Grid.NLayers      = 0u ;
    dimensions->Grid.NRows        = 0u ;
    dimensions->Grid.NColumns     = 0u ;
    dimensions->Grid.NCells       = 0u ;
    dimensions->Grid.NConnections = 0u ;

    dimensions->Chip.Length = 0.0 ;
    dimensions->Chip.Width  = 0.0 ;
}

/******************************************************************************/

Dimensions *alloc_and_init_dimensions (void)
{
  Dimensions *dimensions = malloc (sizeof(Dimensions)) ;

  if (dimensions != NULL) init_dimensions (dimensions) ;

  return dimensions ;
}

/******************************************************************************/

void print_formatted_dimensions
(
  FILE       *stream,
  char       *prefix,
  Dimensions *dimensions
)
{
  fprintf (stream,
           "%sdimensions : \n",
           prefix) ;

  fprintf (stream,
           "%s   chip length %7.1f , width %7.1f ;\n",
           prefix, dimensions->Chip.Length, dimensions->Chip.Width) ;

  fprintf (stream,
           "%s   cell length %7.1f , width %7.1f ;\n",
           prefix, dimensions->Cell.WallLength, dimensions->Cell.Width) ;
}

/******************************************************************************/

void print_detailed_dimensions
(
  FILE       *stream,
  char       *prefix,
  Dimensions *dimensions
)
{
  fprintf (stream,
           "%sdimensions                  = %p\n",
           prefix, dimensions) ;

  fprintf (stream,
           "%s  Cell.FirstWallLength      = %.1f\n",
           prefix, dimensions->Cell.FirstWallLength) ;

  fprintf (stream,
           "%s  Cell.WallLength           = %.1f\n",
           prefix, dimensions->Cell.WallLength) ;

  fprintf (stream,
           "%s  Cell.LastWallLength       = %.1f\n",
           prefix, dimensions->Cell.LastWallLength) ;

  fprintf (stream,
           "%s  Cell.ChannelLength        = %.1f\n",
           prefix, dimensions->Cell.ChannelLength) ;

  fprintf (stream,
           "%s  Cell.Width                = %.1f\n",
           prefix, dimensions->Cell.Width) ;

  fprintf (stream,
           "%s  Grid.NLayers              = %d\n",
           prefix, dimensions->Grid.NLayers) ;

  fprintf (stream,
           "%s  Grid.NRows                = %d\n",
           prefix, dimensions->Grid.NRows) ;

  fprintf (stream,
           "%s  Grid.NColumns             = %d\n",
           prefix, dimensions->Grid.NColumns) ;

  fprintf (stream,
           "%s  Grid.NCells               = %d\n",
           prefix, dimensions->Grid.NCells) ;

  fprintf (stream,
           "%s  Grid.Nconnections         = %d\n",
           prefix, dimensions->Grid.NConnections) ;

  fprintf (stream,
           "%s  Chip.Length               = %.1f\n",
           prefix, dimensions->Chip.Length) ;

  fprintf (stream,
           "%s  Chip.Width                = %.1f\n",
           prefix, dimensions->Chip.Width) ;
}

/******************************************************************************/

void free_dimensions (Dimensions *dimensions)
{
  FREE_POINTER (free, dimensions) ;
}

/******************************************************************************/

double get_cell_length
(
  Dimensions *dimensions,
  uint32_t    column_index
)
{
  if (IS_FIRST_COLUMN (column_index))

    return dimensions->Cell.FirstWallLength ;

  else if (IS_LAST_COLUMN (column_index, dimensions))

    return dimensions->Cell.LastWallLength ;

  else

    if (column_index & 1)

      return dimensions->Cell.ChannelLength ;

    else

      return dimensions->Cell.WallLength ;
}

/******************************************************************************/

double get_cell_width
(
    Dimensions *dimensions,
    uint32_t    __attribute__ ((unused)) row_index
)
{
  return dimensions->Cell.Width ;
}

/******************************************************************************/

uint32_t get_number_of_layers (Dimensions *dimensions)
{
  return dimensions->Grid.NLayers ;
}

/******************************************************************************/

uint32_t get_number_of_rows (Dimensions *dimensions)
{
  return dimensions->Grid.NRows ;
}

/******************************************************************************/

uint32_t get_number_of_columns (Dimensions *dimensions)
{
  return dimensions->Grid.NColumns ;
}

/******************************************************************************/

uint32_t get_number_of_cells (Dimensions *dimensions)
{
  return dimensions->Grid.NCells ;
}

/******************************************************************************/

uint32_t get_number_of_connections (Dimensions *dimensions)
{
  return dimensions->Grid.NConnections ;
}

/******************************************************************************/

uint32_t get_layer_area (Dimensions *dimensions)
{
  return dimensions->Grid.NRows * dimensions->Grid.NColumns ;
}

/******************************************************************************/

uint32_t get_cell_offset_in_layer
(
  Dimensions *dimensions,
  uint32_t    row_index,
  uint32_t    column_index
)
{
  return row_index * get_number_of_columns (dimensions) + column_index ;
}

/******************************************************************************/

uint32_t get_cell_offset_in_stack
(
  Dimensions *dimensions,
  uint32_t    layer_index,
  uint32_t    row_index,
  uint32_t    column_index
)
{
  return layer_index * get_layer_area (dimensions)
         + get_cell_offset_in_layer (dimensions, row_index, column_index) ;
}

/******************************************************************************/

double get_chip_length (Dimensions *dimensions)
{
  return dimensions->Chip.Length ;
}

/******************************************************************************/

double get_chip_width (Dimensions *dimensions)
{
  return dimensions->Chip.Width ;
}

/******************************************************************************/
