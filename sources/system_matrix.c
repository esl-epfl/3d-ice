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
#include <stdio.h>

#include "conductances.h"
#include "macros.h"
#include "system_matrix.h"

/******************************************************************************/

int alloc_system_matrix
(
  SystemMatrix*   matrix,
  Quantity_t      size,
  Quantity_t      nnz
)
{
  matrix->Size = size ;
  matrix->NNz  = nnz ;

  matrix->RowIndices
    = (SystemMatrixRow_t*) malloc (sizeof(SystemMatrixRow_t) * nnz ) ;

  if (matrix->RowIndices == NULL)
    return 0 ;

  matrix->ColumnPointers
    = (SystemMatrixColumn_t*) malloc (sizeof(SystemMatrixColumn_t) * size + 1 ) ;

  if (matrix->ColumnPointers == NULL)
  {
    free (matrix->RowIndices) ;
    return 0 ;
  }

  matrix->Values
    = (SystemMatrixValue_t*) malloc (sizeof(SystemMatrixValue_t) * nnz ) ;

  if (matrix->Values == NULL)
  {
    free (matrix->ColumnPointers) ;
    free (matrix->RowIndices) ;
    return 0 ;
  }

  return 1 ;
}

/******************************************************************************/

void free_system_matrix (SystemMatrix* matrix)
{
  free (matrix->ColumnPointers) ;
  free (matrix->RowIndices) ;
  free (matrix->Values) ;
}

/******************************************************************************/

SystemMatrix add_solid_column
(
  Dimensions*           dimensions,
  Conductances*         conductances,
  Capacity_t*           capacities,
  ConventionalHeatSink* conventionalheatsink,
  GridDimension_t       current_layer,
  GridDimension_t       current_row,
  GridDimension_t       current_column,
  SystemMatrix          system_matrix
)
{
  SystemMatrixValue_t  conductance      = SYSTEMMATRIXVALUE_I ;
  SystemMatrixValue_t  diagonal_value   = SYSTEMMATRIXVALUE_I ;
  SystemMatrixValue_t* diagonal_pointer = NULL ;

  GridDimension_t      current_cell
    = get_cell_offset_in_stack (dimensions, current_layer,
                                            current_row,
                                            current_column) ;

#ifdef PRINT_SYSTEM_MATRIX
  fpos_t diag_fposition, last_fpos ;
  fprintf (stderr,
    "add_solid_column   l %2d r %4d c %4d [%6d]\n",
    current_layer, current_row, current_column, current_cell) ;
#endif

  *system_matrix.ColumnPointers = *(system_matrix.ColumnPointers - 1) ;

  /* BOTTOM */

  if ( ! IS_FIRST_LAYER(current_layer) )
  {
    *system_matrix.RowIndices++ = current_cell - get_layer_area(dimensions) ;

    conductance = PARALLEL (conductances->Bottom,
                           (conductances - get_layer_area(dimensions))->Top) ;

    *system_matrix.Values++  = -conductance ;
    diagonal_value          +=  conductance ;

    (*system_matrix.ColumnPointers)++ ;

#ifdef PRINT_SYSTEM_MATRIX
    fprintf (stderr,
    "  bottom  \t%d\t%.5e = %.5e (B) || %.5e (T)\n",
    *(system_matrix.RowIndices-1), *(system_matrix.Values-1),
    conductances->Bottom, (conductances - get_layer_area(dimensions))->Top) ;
#endif
  }

  /* SOUTH */

  if ( ! IS_FIRST_ROW(current_row) )
  {
    *system_matrix.RowIndices++ = current_cell - get_number_of_columns(dimensions) ;

    conductance = PARALLEL (conductances->South,
                           (conductances - get_number_of_columns(dimensions))->North) ;

    *system_matrix.Values++  = -conductance ;
    diagonal_value          +=  conductance ;

    (*system_matrix.ColumnPointers)++ ;

#ifdef PRINT_SYSTEM_MATRIX
    fprintf (stderr,
      "  south   \t%d\t%.5e = %.5e (S) || %.5e (N)\n",
      *(system_matrix.RowIndices-1), *(system_matrix.Values-1),
      conductances->South, (conductances - get_number_of_columns(dimensions))->North) ;
#endif
  }

  /* WEST */

  if ( ! IS_FIRST_COLUMN(current_column) )
  {
    *system_matrix.RowIndices++ = current_cell - 1 ;

    conductance = PARALLEL (conductances->West,
                           (conductances - 1)->East) ;

    *system_matrix.Values++  = -conductance ;
    diagonal_value          +=  conductance ;

    (*system_matrix.ColumnPointers)++ ;

#ifdef PRINT_SYSTEM_MATRIX
    fprintf (stderr,
      "  west    \t%d\t%.5e = %.5e (W) || %.5e (E)\n",
      *(system_matrix.RowIndices-1), *(system_matrix.Values-1),
      conductances->West, (conductances - 1)->East) ;
#endif
  }

  /* DIAGONAL */

  *system_matrix.RowIndices++ = current_cell ;
  *system_matrix.Values       = *capacities ;
  diagonal_pointer            = system_matrix.Values++ ;

  (*system_matrix.ColumnPointers)++ ;

#ifdef PRINT_SYSTEM_MATRIX
  fprintf (stderr, "  diagonal\t%d\t ", *(system_matrix.RowIndices-1)) ;
  fgetpos (stderr, &diag_fposition) ;
  if (conventionalheatsink != NULL)
    fprintf (stderr, "            ( + %.5e [capacity] + %.5e [ehtc N])\n",
             *capacities, conductances->Top) ;
  else
    fprintf (stderr, "            ( + %.5e [capacity] )\n", *capacities) ;
#endif

  /* EAST */

  if ( ! IS_LAST_COLUMN(current_column, dimensions) )
  {
    *system_matrix.RowIndices++ = current_cell + 1 ;

    conductance = PARALLEL (conductances->East,
                           (conductances + 1)->West) ;

    *system_matrix.Values++  = -conductance ;
    diagonal_value          +=  conductance ;

    (*system_matrix.ColumnPointers)++ ;

#ifdef PRINT_SYSTEM_MATRIX
    fprintf (stderr,
      "  east    \t%d\t%.5e = %.5e (E) || %.5e (W)\n",
      *(system_matrix.RowIndices-1), *(system_matrix.Values-1),
      conductances->East, (conductances + 1)->West) ;
#endif
  }

  /* NORTH */

  if ( ! IS_LAST_ROW(current_row, dimensions) )
  {
    *system_matrix.RowIndices++ = current_cell + get_number_of_columns(dimensions) ;

    conductance = PARALLEL (conductances->North,
                           (conductances + get_number_of_columns(dimensions))->South) ;

    *system_matrix.Values++  = -conductance ;
    diagonal_value          +=  conductance ;

    (*system_matrix.ColumnPointers)++ ;

#ifdef PRINT_SYSTEM_MATRIX
    fprintf (stderr,
      "  north   \t%d\t%.5e = %.5e (N) || %.5e (S)\n",
      *(system_matrix.RowIndices-1), *(system_matrix.Values-1),
      conductances->North, (conductances + get_number_of_columns(dimensions))->South) ;
#endif
  }

  /* TOP */

  if ( ! IS_LAST_LAYER(current_layer, dimensions) )
  {
    *system_matrix.RowIndices++ = current_cell + get_layer_area(dimensions) ;

    conductance = PARALLEL (conductances->Top,
                           (conductances + get_layer_area(dimensions))->Bottom) ;

    *system_matrix.Values++  = -conductance ;
    diagonal_value          +=  conductance ;

    (*system_matrix.ColumnPointers)++ ;

#ifdef PRINT_SYSTEM_MATRIX
    fprintf (stderr,
      "  top     \t%d\t%.5e = %.5e (T) || %.5e (B)\n",
      *(system_matrix.RowIndices-1), *(system_matrix.Values-1),
      conductances->Top, (conductances + get_layer_area(dimensions))->Bottom) ;
#endif
  }
  else
  {
    if (conventionalheatsink != NULL)

      diagonal_value += conductances->Top ;
  }

  /* DIAGONAL ELEMENT */

  *diagonal_pointer += diagonal_value ;

#ifdef PRINT_SYSTEM_MATRIX
  fgetpos (stderr, &last_fpos) ;
  fsetpos (stderr, &diag_fposition) ;
  fprintf (stderr, "%.5e", *diagonal_pointer) ;
  fsetpos (stderr, &last_fpos) ;

  fprintf (stderr, "  %d\n", *system_matrix.ColumnPointers) ;
#endif

  system_matrix.ColumnPointers++ ;

  return system_matrix ;
}

/******************************************************************************/

SystemMatrix add_liquid_column
(
  Dimensions*           dimensions,
  Conductances*         conductances,
  Capacity_t*           capacities,
  GridDimension_t       current_layer,
  GridDimension_t       current_row,
  GridDimension_t       current_column,
  SystemMatrix          system_matrix
)
{
  SystemMatrixValue_t  conductance      = SYSTEMMATRIXVALUE_I ;
  SystemMatrixValue_t  diagonal_value   = SYSTEMMATRIXVALUE_I ;
  SystemMatrixValue_t* diagonal_pointer = NULL ;

  GridDimension_t      current_cell
    = get_cell_offset_in_stack (dimensions, current_layer,
                                            current_row,
                                            current_column) ;

#ifdef PRINT_SYSTEM_MATRIX
  fpos_t diag_fposition, last_fpos ;
  fprintf (stderr,
    "add_liquid_column  l %2d r %4d c %4d [%6d]\n",
    current_layer, current_row, current_column, current_cell) ;
#endif

  *system_matrix.ColumnPointers = *(system_matrix.ColumnPointers - 1) ;

  /* BOTTOM */

  if ( ! IS_FIRST_LAYER(current_layer) )
  {
    *system_matrix.RowIndices++ = current_cell - get_layer_area(dimensions) ;

    conductance = PARALLEL (conductances->Bottom,
                           (conductances - get_layer_area(dimensions))->Top) ;

    *system_matrix.Values++  = -conductance ;
    diagonal_value          +=  conductance ;

    (*system_matrix.ColumnPointers)++ ;

#ifdef PRINT_SYSTEM_MATRIX
    fprintf (stderr,
      "  bottom  \t%d\t%.5e = %.5e (B) || %.5e (T)\n",
      *(system_matrix.RowIndices-1), *(system_matrix.Values-1),
      conductances->Bottom, (conductances - get_layer_area(dimensions))->Top) ;
#endif
  }

  /* SOUTH */

  if ( ! IS_FIRST_ROW(current_row) )
  {
    *system_matrix.RowIndices++ = current_cell - get_number_of_columns(dimensions) ;
    *system_matrix.Values++  = conductances->North ; /* == (C) */

    (*system_matrix.ColumnPointers)++ ;

#ifdef PRINT_SYSTEM_MATRIX
    fprintf (stderr,
      "  south   \t%d\t%.5e (N)\n", *(system_matrix.RowIndices-1), *(system_matrix.Values-1)) ;
#endif
  }

  /* WEST */

  if ( ! IS_FIRST_COLUMN(current_column) )
  {
    *system_matrix.RowIndices++ = current_cell - 1 ;

    conductance = PARALLEL (conductances->West,
                           (conductances - 1)->East) ;

    *system_matrix.Values++  = -conductance ;
    diagonal_value          +=  conductance ;

    (*system_matrix.ColumnPointers)++ ;

#ifdef PRINT_SYSTEM_MATRIX
    fprintf (stderr,
      "  west    \t%d\t%.5e = %.5e (W) || %.5e (E)\n",
      *(system_matrix.RowIndices-1), *(system_matrix.Values-1),
      conductances->West, (conductances - 1)->East) ;
#endif
  }

  /* DIAGONAL */

  *system_matrix.RowIndices++ = current_cell ;
  *system_matrix.Values       = *capacities ;
  diagonal_pointer            = system_matrix.Values++ ;

  (*system_matrix.ColumnPointers)++ ;

#ifdef PRINT_SYSTEM_MATRIX
  fprintf (stderr, "  diagonal\t%d\t", *(system_matrix.RowIndices-1)) ;
  fgetpos (stderr, &diag_fposition) ;
  fprintf (stderr, "            ( + %.5e [capacity] )\n", *capacities) ;
#endif

  /* EAST */

  if ( ! IS_LAST_COLUMN(current_column, dimensions) )
  {
    *system_matrix.RowIndices++ = current_cell + 1 ;

    conductance = PARALLEL (conductances->East,
                           (conductances + 1)->West) ;

    *system_matrix.Values++  = -conductance ;
    diagonal_value          +=  conductance ;

    (*system_matrix.ColumnPointers)++ ;

#ifdef PRINT_SYSTEM_MATRIX
    fprintf (stderr,
      "  east    \t%d\t%.5e = %.5e (E) || %.5e (W)\n",
      *(system_matrix.RowIndices-1), *(system_matrix.Values-1),
      conductances->East, (conductances + 1)->West) ;
#endif
  }

  /* NORTH */

  if ( ! IS_LAST_ROW(current_row, dimensions) )
  {
    *system_matrix.RowIndices++ = current_cell + get_number_of_columns(dimensions) ;
    *system_matrix.Values++  = conductances->South ; /* == -C */

    (*system_matrix.ColumnPointers)++ ;

#ifdef PRINT_SYSTEM_MATRIX
    fprintf (stderr,
      "  north   \t%d\t%.5e (S)\n", *(system_matrix.RowIndices-1), *(system_matrix.Values-1)) ;
#endif
  }

  /* TOP */

  if ( ! IS_LAST_LAYER(current_layer, dimensions) )
  {
    *system_matrix.RowIndices++ = current_cell + get_layer_area(dimensions) ;

    conductance = PARALLEL (conductances->Top,
                           (conductances + get_layer_area(dimensions))->Bottom) ;

    *system_matrix.Values++  = -conductance ;
    diagonal_value          +=  conductance ;

    (*system_matrix.ColumnPointers)++ ;

#ifdef PRINT_SYSTEM_MATRIX
    fprintf (stderr,
      "  top     \t%d\t%.5e = %.5e (T) || %.5e (B)\n",
      *(system_matrix.RowIndices-1), *(system_matrix.Values-1),
      conductances->Top, (conductances + get_layer_area(dimensions))->Bottom) ;
#endif
  }

  /* DIAGONAL ELEMENT */

  *diagonal_pointer += diagonal_value ;

  if (current_row == 0 || current_row == get_number_of_rows(dimensions) - 1)

    *diagonal_pointer += conductances->North ; /* == (C) */

#ifdef PRINT_SYSTEM_MATRIX
  fgetpos (stderr, &last_fpos) ;
  fsetpos (stderr, &diag_fposition) ;
  fprintf (stderr, "%.5e", *diagonal_pointer) ;
  fsetpos (stderr, &last_fpos) ;

  fprintf (stderr, "  %d\n", *system_matrix.ColumnPointers) ;
#endif

  system_matrix.ColumnPointers++ ;

  return system_matrix ;
}

/******************************************************************************/