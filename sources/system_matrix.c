/******************************************************************************
 * Bison source file "3D-ICE/bison/floorplan_parser.y"                        *
 *                                                                            *
 * This file is part of 3D-ICE 1.0 (beta).                                    *
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
 * with 3D-ICe. If not, see <http://www.gnu.org/licenses/>.                   *
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
 * EPFL-STI-IEL-ESL                                                           *
 * Batiment ELG, ELG 130                                                      *
 * Station 11                                          threed-ice@esl.epfl.ch *
 * 1015 Lausanne, Switzerland                       http://esl.epfl.ch/3D-ICE *
 ******************************************************************************/

#include <stdlib.h>
#include <stdio.h>

#include "conductances.h"
#include "system_matrix.h"

#define PARALLEL(x,y)      ( (x * y) / ( x + y) )
#define LAYER_OFFSET(dim)  (dim->Grid.NRows * dim->Grid.NColumns)
#define ROW_OFFSET(dim)    (dim->Grid.NColumns)
#define COLUMN_OFFSET(dim) (1)

/******************************************************************************/

int alloc_system_matrix
(
  SystemMatrix*   matrix,
  Quantity_t      nvalues,
  Quantity_t      nnz
)
{
  matrix->Size    = nvalues ;
  matrix->NNz     = nnz ;

  matrix->RowOffsets
    = (RowIndex_t*) malloc (sizeof(RowIndex_t) * nvalues + 1 ) ;

  if (matrix->RowOffsets == NULL)
    return 0 ;

  matrix->ColumnIndices
    = (ColumnIndex_t*) malloc (sizeof(ColumnIndex_t) * nnz ) ;

  if (matrix->ColumnIndices == NULL)
  {
    free (matrix->RowOffsets) ;
    return 0 ;
  }

  matrix->Values
    = (SystemMatrixValue_t*) malloc (sizeof(SystemMatrixValue_t) * nnz ) ;

  if (matrix->Values == NULL)
  {
    free (matrix->ColumnIndices) ;
    free (matrix->RowOffsets) ;
    return 0 ;
  }

  return 1 ;
}

/******************************************************************************/

void free_system_matrix (SystemMatrix* matrix)
{
  free (matrix->ColumnIndices) ;
  free (matrix->RowOffsets) ;
  free (matrix->Values) ;
}

/******************************************************************************/

Quantity_t add_solid_row
(
  Dimensions*          dimensions,
  Conductances*        conductances,
  Capacity_t*          capacities,
  EnvironmentHeatSink* environmentheatsink,
  LayerIndex_t         current_layer,
  RowIndex_t           current_row,
  ColumnIndex_t        current_column,
  RowIndex_t*          row_offsets,
  ColumnIndex_t*       column_indices,
  SystemMatrixValue_t* values
)
{
  Conductance_t        conductance      = 0.0 ;
  SystemMatrixValue_t  diagonal_value   = 0.0 ;
  SystemMatrixValue_t* diagonal_pointer = NULL ;
  Quantity_t           added            = 0 ;

  int current_cell
    = current_layer * LAYER_OFFSET(dimensions)
      + current_row * ROW_OFFSET(dimensions)
      + current_column ;

#ifdef PRINT_SYSTEM_MATRIX
  fpos_t diag_fposition, last_fpos ;
  fprintf (stderr,
    "add_solid_row   l %2d r %4d c %4d [%6d]\n",
    current_layer, current_row, current_column, current_cell) ;
#endif

  *row_offsets = *(row_offsets - 1) ;

  if ( current_layer > 0 )   /* BOTTOM */
  {
    *column_indices++ = current_cell - LAYER_OFFSET(dimensions) ;

    conductance = PARALLEL (conductances->Bottom,
                           (conductances - LAYER_OFFSET(dimensions))->Top) ;

    *values++       = -conductance ;
    diagonal_value +=  conductance ;

    (*row_offsets)++ ;
    added++;

#ifdef PRINT_SYSTEM_MATRIX
    fprintf (stderr,
    "  bottom  \t%d\t%.5e = %.5e || %.5e\n",
    *(column_indices-1), *(values-1),
    conductances->Bottom, (conductances - LAYER_OFFSET(dimensions))->Top) ;
#endif
  }

  /* SOUTH */

  if ( current_row > 0 )
  {
    *column_indices++ = current_cell - ROW_OFFSET(dimensions) ;

    conductance = PARALLEL (conductances->South,
                           (conductances - ROW_OFFSET(dimensions))->North) ;

    *values++       = -conductance ;
    diagonal_value +=  conductance ;

    (*row_offsets)++ ;
    added++ ;

#ifdef PRINT_SYSTEM_MATRIX
    fprintf (stderr,
      "  south   \t%d\t%.5e = %.5e || %.5e\n",
      *(column_indices-1), *(values-1),
      conductances->South, (conductances - ROW_OFFSET(dimensions))->North) ;
#endif
  }

  /* WEST */

  if ( current_column > 0 )
  {
    *column_indices++ = current_cell - COLUMN_OFFSET(dimensions) ;

    conductance = PARALLEL (conductances->West,
                           (conductances - COLUMN_OFFSET(dimensions))->East) ;

    *values++       = -conductance ;
    diagonal_value +=  conductance ;

    (*row_offsets)++ ;
    added++ ;

#ifdef PRINT_SYSTEM_MATRIX
    fprintf (stderr,
      "  west    \t%d\t%.5e = %.5e || %.5e\n",
      *(column_indices-1), *(values-1),
      conductances->West, (conductances - COLUMN_OFFSET(dimensions))->East) ;
#endif
  }

  /* DIAGONAL */

  *column_indices++ = current_cell ;
  *values           = *capacities ;
  diagonal_pointer  = values++ ;

  (*row_offsets)++ ;
  added ++ ;

#ifdef PRINT_SYSTEM_MATRIX
  fprintf (stderr, "  diagonal\t%d\t ", *(column_indices-1)) ;
  fgetpos (stderr, &diag_fposition) ;
  if (environmentheatsink != NULL)
    fprintf (stderr, "           + ehtc %.5e\n", conductances->Top) ;
  else
    fprintf (stderr, "           \n") ;
#endif

  /* EAST */

  if ( current_column < dimensions->Grid.NColumns - 1 )
  {
    *column_indices++ = current_cell + COLUMN_OFFSET(dimensions) ;

    conductance = PARALLEL (conductances->East,
                           (conductances + COLUMN_OFFSET(dimensions))->West) ;

    *values++       = -conductance ;
    diagonal_value +=  conductance ;

    (*row_offsets)++ ;
    added ++ ;

#ifdef PRINT_SYSTEM_MATRIX
    fprintf (stderr,
      "  east    \t%d\t%.5e = %.5e || %.5e\n",
      *(column_indices-1), *(values-1),
      conductances->East, (conductances + COLUMN_OFFSET(dimensions))->West) ;
#endif
  }

  /* NORTH */

  if ( current_row < dimensions->Grid.NRows - 1 )
  {
    *column_indices++ = current_cell + ROW_OFFSET(dimensions) ;

    conductance = PARALLEL (conductances->North,
                           (conductances + ROW_OFFSET(dimensions))->South) ;

    *values++       = -conductance ;
    diagonal_value +=  conductance ;

    (*row_offsets)++ ;
    added++ ;

#ifdef PRINT_SYSTEM_MATRIX
    fprintf (stderr,
      "  north   \t%d\t%.5e = %.5e || %.5e\n",
      *(column_indices-1), *(values-1),
      conductances->North, (conductances + ROW_OFFSET(dimensions))->South) ;
#endif
  }

  /* TOP */

  if ( current_layer < dimensions->Grid.NLayers - 1)
  {
    *column_indices++ = current_cell + LAYER_OFFSET(dimensions) ;

    conductance = PARALLEL (conductances->Top,
                           (conductances + LAYER_OFFSET(dimensions))->Bottom) ;

    *values++       = -conductance ;
    diagonal_value +=  conductance ;

    (*row_offsets)++ ;
    added++ ;

#ifdef PRINT_SYSTEM_MATRIX
    fprintf (stderr,
      "  top     \t%d\t%.5e = %.5e || %.5e\n",
      *(column_indices-1), *(values-1),
      conductances->Top, (conductances + LAYER_OFFSET(dimensions))->Bottom) ;
#endif
  }
  else
  {
    if (environmentheatsink != NULL)

      diagonal_value += conductances->Top ;
  }

  /* DIAGONAL ELEMENT */

  *diagonal_pointer += diagonal_value ;

#ifdef PRINT_SYSTEM_MATRIX
  fgetpos (stderr, &last_fpos) ;
  fsetpos (stderr, &diag_fposition) ;
  fprintf (stderr, "%.5e", *diagonal_pointer) ;
  fsetpos (stderr, &last_fpos) ;

  fprintf (stderr, "  %d (+%d)\n", *row_offsets, added) ;
#endif

  return added ;
}

/******************************************************************************/

Quantity_t add_liquid_row
(
  Dimensions*          dimensions,
  Conductances*        conductances,
  Capacity_t*          capacities,
  LayerIndex_t         current_layer,
  RowIndex_t           current_row,
  ColumnIndex_t        current_column,
  RowIndex_t*          row_offsets,
  ColumnIndex_t*       column_indices,
  SystemMatrixValue_t* values
)
{
  Conductance_t        conductance      = 0.0 ;
  SystemMatrixValue_t  diagonal_value   = 0.0 ;
  SystemMatrixValue_t* diagonal_pointer = NULL ;
  Quantity_t           added            = 0 ;

  int current_cell
    = current_layer * LAYER_OFFSET(dimensions)
      + current_row * ROW_OFFSET(dimensions)
      + current_column ;

#ifdef PRINT_SYSTEM_MATRIX
  fpos_t diag_fposition, last_fpos ;
  fprintf (stderr,
    "add_liquid_row  l %2d r %4d c %4d [%6d]\n",
    current_layer, current_row, current_column, current_cell) ;
#endif

  *row_offsets = *(row_offsets - 1) ;

  /* BOTTOM */

  if ( current_layer > 0 )
  {
    *column_indices++ = current_cell - LAYER_OFFSET(dimensions) ;

    conductance = PARALLEL (conductances->Bottom,
                           (conductances - LAYER_OFFSET(dimensions))->Top) ;

    *values++       = -conductance ;
    diagonal_value +=  conductance ;

    (*row_offsets)++ ;
    added++ ;

#ifdef PRINT_SYSTEM_MATRIX
    fprintf (stderr,
      "  bottom  \t%d\t%.5e = %.5e || %.5e\n",
      *(column_indices-1), *(values-1),
      conductances->Bottom, (conductances - LAYER_OFFSET(dimensions))->Top) ;
#endif
  }

  /* SOUTH */

  if ( current_row > 0 )
  {
    *column_indices++ = current_cell - ROW_OFFSET(dimensions) ;
    *values++  = conductances->South ; /* == (C) */

    (*row_offsets)++ ;
    added++ ;

#ifdef PRINT_SYSTEM_MATRIX
    fprintf (stderr,
      "  south   \t%d\t%.5e\n", *(column_indices-1), *(values-1)) ;
#endif
  }

  /* WEST */

  if ( current_column > 0 )
  {
    *column_indices++ = current_cell - COLUMN_OFFSET(dimensions) ;

    conductance = PARALLEL (conductances->West,
                           (conductances - COLUMN_OFFSET(dimensions))->East) ;

    *values++        = -conductance ;
    diagonal_value  +=  conductance ;

    (*row_offsets)++ ;
    added++;

#ifdef PRINT_SYSTEM_MATRIX
    fprintf (stderr,
      "  west    \t%d\t%.5e = %.5e || %.5e\n",
      *(column_indices-1), *(values-1),
      conductances->West, (conductances - COLUMN_OFFSET(dimensions))->East) ;
#endif
  }

  /* DIAGONAL */

  *column_indices++ = current_cell ;
  *values           = *capacities ;
  diagonal_pointer  = values++ ;

  (*row_offsets)++ ;
  added++;

#ifdef PRINT_SYSTEM_MATRIX
  fprintf (stderr, "  diagonal\t%d\t", *(column_indices-1)) ;
  fgetpos (stderr, &diag_fposition) ;
  fprintf (stderr, "           \n") ;
#endif

  /* EAST */

  if ( current_column < dimensions->Grid.NColumns - 1 )
  {
    *column_indices++ = current_cell + COLUMN_OFFSET(dimensions) ;

    conductance = PARALLEL (conductances->East,
                           (conductances + COLUMN_OFFSET(dimensions))->West) ;

    *values++       = -conductance ;
    diagonal_value +=  conductance ;

    (*row_offsets)++ ;
    added++;

#ifdef PRINT_SYSTEM_MATRIX
    fprintf (stderr,
      "  east    \t%d\t%.5e = %.5e || %.5e\n",
      *(column_indices-1), *(values-1),
      conductances->East, (conductances + COLUMN_OFFSET(dimensions))->West) ;
#endif
  }

  /* NORTH */

  if ( current_row < dimensions->Grid.NRows - 1 )
  {
    *column_indices++ = current_cell + ROW_OFFSET(dimensions) ;
    *values++  = conductances->North ; /* == -C */

    (*row_offsets)++ ;
    added ++ ;

#ifdef PRINT_SYSTEM_MATRIX
    fprintf (stderr,
      "  north   \t%d\t%.5e\n", *(column_indices-1), *(values-1)) ;
#endif
  }

  /* TOP */

  if ( current_layer < dimensions->Grid.NLayers - 1)
  {
    *column_indices++ = current_cell + LAYER_OFFSET(dimensions) ;

    conductance = PARALLEL (conductances->Top,
                           (conductances + LAYER_OFFSET(dimensions))->Bottom) ;

    *values++       = -conductance ;
    diagonal_value +=  conductance ;

    (*row_offsets)++ ;
    added++;

#ifdef PRINT_SYSTEM_MATRIX
    fprintf (stderr,
      "  top     \t%d\t%.5e = %.5e || %.5e\n",
      *(column_indices-1), *(values-1),
      conductances->Top, (conductances + LAYER_OFFSET(dimensions))->Bottom) ;
#endif
  }

  /* DIAGONAL ELEMENT */

  *diagonal_pointer += diagonal_value ;

  if (current_row == 0 || current_row == dimensions->Grid.NRows - 1)

    *diagonal_pointer += conductances->North ; /* == (C) */

#ifdef PRINT_SYSTEM_MATRIX
  fgetpos (stderr, &last_fpos) ;
  fsetpos (stderr, &diag_fposition) ;
  fprintf (stderr, "%.5e", *diagonal_pointer) ;
  fsetpos (stderr, &last_fpos) ;

  fprintf (stderr, "  %d (+%d)\n", *row_offsets, added) ;
#endif

  return added ;
}

/******************************************************************************/
