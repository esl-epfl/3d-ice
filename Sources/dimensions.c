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
/******************************************************************************/
/******************************************************************************/

void
init_dimensions
(
  struct Dimensions *dimensions
)
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
/******************************************************************************/
/******************************************************************************/

struct Dimensions *
alloc_and_init_dimensions
(
  void
)
{
  struct Dimensions *dimensions
    = (struct Dimensions *) malloc (sizeof (struct Dimensions));

  if (dimensions != NULL) init_dimensions (dimensions);

  return dimensions;
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

void
print_dimensions
(
  FILE *stream,
  char *prefix,
  struct Dimensions *dimensions
)
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
/******************************************************************************/
/******************************************************************************/

void
free_dimensions
(
  struct Dimensions * dimensions
)
{
  free (dimensions);
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

double
get_cell_length
(
  struct Dimensions *dimensions,
  int        column
)
{
  if (column == 0)

    return dimensions->Cell.FirstLength;

  else if (column == get_number_of_columns (dimensions) - 1)

    return dimensions->Cell.LastLength;

  return dimensions->Cell.Length;
}

/******************************************************************************/

double
get_cell_width
(
  struct Dimensions *dimensions
)
{
  return dimensions->Cell.Width;
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

int
get_number_of_layers
(
  struct Dimensions *dimensions
)
{
  return dimensions->Grid.NLayers ;
}

/******************************************************************************/

int
get_number_of_rows
(
  struct Dimensions *dimensions
)
{
  return dimensions->Grid.NRows ;
}

/******************************************************************************/

int
get_number_of_columns
(
  struct Dimensions *dimensions
)
{
  return dimensions->Grid.NColumns ;
}

/******************************************************************************/

int
get_number_of_cells
(
  struct Dimensions *dimensions
)
{
  return dimensions->Grid.NCells ;
}

/******************************************************************************/

int
get_number_of_non_zeroes
(
  struct Dimensions *dimensions
)
{
  return dimensions->Grid.NNz ;
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

double
get_cell_top_surface
(
  struct Dimensions *dimensions,
  int        column
)
{
  return get_cell_length (dimensions, column) * get_cell_width (dimensions) ;
}

/******************************************************************************/

double
get_cell_bottom_surface
(
  struct Dimensions *dimensions,
  int column
)
{
  return get_cell_top_surface (dimensions, column) ;
}

/******************************************************************************/

double
get_cell_east_surface
(
  struct Dimensions *dimensions,
  double     height
)
{
  return height * get_cell_width (dimensions) ;
}

/******************************************************************************/

double
get_cell_west_surface
(
  struct Dimensions *dimensions,
  double     height
)
{
  return get_cell_east_surface (dimensions, height) ;
}

/******************************************************************************/

double
get_cell_north_surface
(
  struct Dimensions *dimensions,
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
  struct Dimensions *dimensions,
  double height,
  int column
)
{
  return get_cell_north_surface (dimensions, height, column) ;
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

int
get_layer_area
(
  struct Dimensions *dimensions
)
{
  return dimensions->Grid.NRows * dimensions->Grid.NColumns ;
}

/******************************************************************************/

int
get_cell_offset_in_layer
(
  struct Dimensions *dimensions,
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
  struct Dimensions *dimensions,
  int layer,
  int row,
  int column
)
{
  return layer * get_layer_area (dimensions)
         + get_cell_offset_in_layer (dimensions, row, column) ;
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

double
get_chip_length
(
  struct Dimensions *dimensions
)
{
  return dimensions->Chip.Length ;
}

/******************************************************************************/

double
get_chip_width
(
  struct Dimensions *dimensions
)
{
  return dimensions->Chip.Width ;
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
