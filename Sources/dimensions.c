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

void
init_cells_dimensions (CellDimensions * cd)
{
  if (cd == NULL) return ;

  cd->FirstLength = 0.0;
  cd->Length      = 0.0;
  cd->LastLength  = 0.0;
  cd->Width       = 0.0;
}

/******************************************************************************/

void
print_cells_dimensions (FILE * stream, char *prefix, CellDimensions * cd)
{
  fprintf (stream,
    "%sCell dimensions (Fl, l, Ll) x w = (%5.2f, %5.2f, %5.2f) x %5.2f um\n",
    prefix, cd->FirstLength, cd->Length, cd->LastLength, cd->Width) ;
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

void
init_grid_dimensions (GridDimensions * gd)
{
  if (gd == NULL) return ;

  gd->NLayers  = 0 ;
  gd->NRows    = 0 ;
  gd->NColumns = 0 ;
  gd->NCells   = 0 ;
  gd->NNz      = 0 ;
}

/******************************************************************************/

void
print_grid_dimensions (FILE * stream, char *prefix, GridDimensions * gd)
{
  fprintf (stream,
    "%sGrid dimensions     (L x R x C) = (%d x %d x %d) -> %d cells\n",
    prefix, gd->NLayers, gd->NRows, gd->NColumns, gd->NCells);
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

void
init_chip_dimensions (ChipDimensions * cd)
{
  if (cd == NULL) return ;

  cd->Length = 0.0;
  cd->Width  = 0.0;
}

/******************************************************************************/

void
print_chip_dimensions (FILE * stream, char *prefix, ChipDimensions * cd)
{
  fprintf (stream,
    "%sChip dimensions         (L x W) = (%5.2f x %5.2f) mm\n",
    prefix, cd->Length, cd->Width);
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

void
init_dimensions (Dimensions * dim)
{
  if (dim == NULL) return ;

  init_cells_dimensions (&(dim->Cell));
  init_grid_dimensions  (&(dim->Grid));
  init_chip_dimensions  (&(dim->Chip));
}

/******************************************************************************/

Dimensions *
alloc_and_init_dimensions (void)
{
  Dimensions *dimensions = (Dimensions *) malloc (sizeof (Dimensions));

  init_dimensions (dimensions);

  return dimensions;
}

/******************************************************************************/

void
print_dimensions (FILE * stream, char *prefix, Dimensions * dim)
{
  print_cells_dimensions (stream, prefix, &(dim->Cell));
  print_grid_dimensions  (stream, prefix, &(dim->Grid));
  print_chip_dimensions  (stream, prefix, &(dim->Chip));
}

/******************************************************************************/

void
free_dimensions (Dimensions * dim)
{
  if (dim == NULL) return ;

  free (dim);
}

/******************************************************************************/

double
get_cell_width
(
  Dimensions *dimensions
)
{
  return dimensions->Cell.Width;
}

/******************************************************************************/

double
get_cell_length
(
  Dimensions *dimensions,
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

int
get_number_of_layers
(
  Dimensions *dimensions
)
{
  return dimensions->Grid.NLayers ;
}

/******************************************************************************/

int
get_number_of_rows
(
  Dimensions *dimensions
)
{
  return dimensions->Grid.NRows ;
}

/******************************************************************************/

int
get_number_of_columns
(
  Dimensions *dimensions
)
{
  return dimensions->Grid.NColumns ;
}

/******************************************************************************/

int
get_cell_top_surface
(
  Dimensions *dimensions,
  int        column
)
{
  return get_cell_length (dimensions, column) * get_cell_width (dimensions) ;
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
  return row * dimensions->Grid.NColumns + column ;
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
get_cell_offset_in_stack
(
  Dimensions *dimensions,
  int layer,
  int row,
  int column
)
{
  return layer * get_layer_area (dimensions)
         + row * get_number_of_columns (dimensions)
         + column ;
}

/******************************************************************************/
