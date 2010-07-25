/******************************************************************************
 *                                                                            *
 * Source file "Source/dimensions.c"                                          *
 *                                                                            *
 * EPFL-STI-IEL-ESL                                                           *
 * Bâtiment ELG, ELG 130                                                      *
 * Station 11                                                                 *
 * 1015 Lausanne, Switzerland                    alessandro.vincenzi@epfl.ch  *
 ******************************************************************************/

#include <stdlib.h>

#include "dimensions.h"

/******************************************************************************/

void init_dimensions (Dimensions *dimensions)
{
  dimensions->Cell.FirstLength = 0.0;
  dimensions->Cell.Length      = 0.0;
  dimensions->Cell.LastLength  = 0.0;
  dimensions->Cell.Width       = 0.0;

  dimensions->Grid.NLayers     = 0 ;
  dimensions->Grid.NRows       = 0 ;
  dimensions->Grid.NColumns    = 0 ;
  dimensions->Grid.NCells      = 0 ;
  dimensions->Grid.NNz         = 0 ;

  dimensions->Chip.Length      = 0.0;
  dimensions->Chip.Width       = 0.0;
}

/******************************************************************************/

Dimensions* alloc_and_init_dimensions (void)
{
  Dimensions* dimensions = (Dimensions*) malloc (sizeof (Dimensions));

  if (dimensions != NULL) init_dimensions (dimensions);

  return dimensions;
}

/******************************************************************************/

void print_dimensions (FILE* stream, String_t prefix, Dimensions *dimensions)
{
  fprintf (stream,
    "%sCell dimensions (Fl, l, Ll) x w = (%5.2f, %5.2f, %5.2f) x %5.2f um\n",
    prefix, dimensions->Cell.FirstLength, dimensions->Cell.Length,
            dimensions->Cell.LastLength, dimensions->Cell.Width) ;
  fprintf (stream,
    "%sGrid dimensions     (L x R x C) = (%d x %d x %d) -> %d cells\n",
    prefix, dimensions->Grid.NLayers,  dimensions->Grid.NRows,
            dimensions->Grid.NColumns, dimensions->Grid.NCells);
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

CellDimension_t get_cell_length (Dimensions *dimensions, GridDimension_t column)
{
  if (column == 0)

    return dimensions->Cell.FirstLength;

  else if (column == get_number_of_columns (dimensions) - 1)

    return dimensions->Cell.LastLength;

  return dimensions->Cell.Length;
}

/******************************************************************************/

CellDimension_t get_cell_width (Dimensions *dimensions)
{
  return dimensions->Cell.Width;
}

/******************************************************************************/

GridDimension_t get_number_of_layers (Dimensions *dimensions)
{
  return dimensions->Grid.NLayers ;
}

/******************************************************************************/

GridDimension_t get_number_of_rows (Dimensions *dimensions)
{
  return dimensions->Grid.NRows ;
}

/******************************************************************************/

GridDimension_t get_number_of_columns (Dimensions *dimensions)
{
  return dimensions->Grid.NColumns ;
}

/******************************************************************************/

GridDimension_t get_number_of_cells (Dimensions *dimensions)
{
  return dimensions->Grid.NCells ;
}

/******************************************************************************/

GridDimension_t get_number_of_non_zeroes (Dimensions *dimensions)
{
  return dimensions->Grid.NNz ;
}

/******************************************************************************/

double
get_cell_top_surface
(
  Dimensions *dimensions,
  int        column
)
{
  return get_cell_length (dimensions, column) * get_cell_width (dimensions) ;
}

/******************************************************************************/

double
get_cell_bottom_surface
(
  Dimensions *dimensions,
  int column
)
{
  return get_cell_top_surface (dimensions, column) ;
}

/******************************************************************************/

double
get_cell_east_surface
(
  Dimensions *dimensions,
  double     height
)
{
  return height * get_cell_width (dimensions) ;
}

/******************************************************************************/

double
get_cell_west_surface
(
  Dimensions *dimensions,
  double     height
)
{
  return get_cell_east_surface (dimensions, height) ;
}

/******************************************************************************/

double
get_cell_north_surface
(
  Dimensions *dimensions,
  double     height,
  int        column
)
{
  return get_cell_length (dimensions, column) * height ;
}
/******************************************************************************/

double
get_cell_south_surface
(
  Dimensions *dimensions,
  double height,
  int column
)
{
  return get_cell_north_surface (dimensions, height, column) ;
}

/******************************************************************************/

int
get_layer_area
(
  Dimensions *dimensions
)
{
  return dimensions->Grid.NRows * dimensions->Grid.NColumns ;
}

/******************************************************************************/

int
get_cell_offset_in_layer
(
  Dimensions *dimensions,
  int row,
  int column
)
{
  return row * get_number_of_columns (dimensions) + column ;
}

/******************************************************************************/

int
get_cell_offset_in_stack
(
  Dimensions *dimensions,
  int layer,
  int row,
  int column
)
{
  return layer * get_layer_area (dimensions)
         + get_cell_offset_in_layer (dimensions, row, column) ;
}

/******************************************************************************/

double
get_chip_length
(
  Dimensions *dimensions
)
{
  return dimensions->Chip.Length ;
}

/******************************************************************************/

double
get_chip_width
(
  Dimensions *dimensions
)
{
  return dimensions->Chip.Width ;
}

/******************************************************************************/
