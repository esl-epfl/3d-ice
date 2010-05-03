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
  if (cd != NULL)
  {
    cd->FirstLength = 0.0;
    cd->Length      = 0.0;
    cd->LastLength  = 0.0;
    cd->Width       = 0.0;
  }
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
  if (gd != NULL)
  {
    gd->NLayers  = 0 ;
    gd->NRows    = 0 ;
    gd->NColumns = 0 ;
    gd->NCells   = 0 ;
    gd->NNz      = 0 ;
  }
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
  if (cd != NULL)
  {
    cd->Length = 0.0;
    cd->Width  = 0.0;
  }
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
  if (dim != NULL)
  {
    init_cells_dimensions (&(dim->Cell));
    init_grid_dimensions  (&(dim->Grid));
    init_chip_dimensions  (&(dim->Chip));
  }
}

/******************************************************************************/

Dimensions *
alloc_dimensions (void)
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
  free (dim);
}

/******************************************************************************/

double
get_cell_length (Dimensions * dim, int column)
{
  if (column == 0)
  {
    return dim->Cell.FirstLength;
  }
  else if (column == dim->Grid.NColumns - 1)
  {
    return dim->Cell.LastLength;
  }
  else
  {
    return dim->Cell.Length;
  }
}
