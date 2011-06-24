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
#include <stdio.h>

#include "macros.h"
#include "system_matrix.h"

/******************************************************************************/

void init_system_matrix (SystemMatrix* matrix)
{
  matrix->ColumnPointers = NULL ;
  matrix->RowIndices     = NULL;
  matrix->Values         = NULL ;
  matrix->Size           = QUANTITY_I ;
  matrix->NNz            = QUANTITY_I ;
}

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
    = malloc (sizeof(SystemMatrixRow_t) * nnz) ;

  if (matrix->RowIndices == NULL)

    return 0 ;

  matrix->ColumnPointers
    = malloc (sizeof(SystemMatrixColumn_t) * size + 1) ;

  if (matrix->ColumnPointers == NULL)
  {
    FREE_POINTER (free, matrix->RowIndices) ;
    return 0 ;
  }

  matrix->Values
    = malloc (sizeof(SystemMatrixValue_t) * nnz) ;

  if (matrix->Values == NULL)
  {
    FREE_POINTER (free, matrix->ColumnPointers) ;
    FREE_POINTER (free, matrix->RowIndices) ;
    return 0 ;
  }

  return 1 ;
}

/******************************************************************************/

void free_system_matrix (SystemMatrix* matrix)
{
  FREE_POINTER (free, matrix->ColumnPointers) ;
  FREE_POINTER (free, matrix->RowIndices) ;
  FREE_POINTER (free, matrix->Values) ;
}

/******************************************************************************/

void print_system_matrix (String_t filename, SystemMatrix matrix)
{
  FILE* file = fopen (filename, "w") ;

  if (file == NULL)
  {
    fprintf(stderr, "Cannot open file %s\n", filename) ;
    exit (-1) ;
  }

  int row, column ;

  for (column = 0 ; column < matrix.Size ; column ++)

    for (row = matrix.ColumnPointers[column] ; row < matrix.ColumnPointers[column + 1] ; row++)

      // Index starts from 1 not 0
      fprintf (file, "%5d\t%5d\t%32.24f\n", matrix.RowIndices[row] + 1, column + 1, matrix.Values[row]) ;

  fclose (file) ;
}

/******************************************************************************/

SystemMatrix add_solid_column
(
  Dimensions*           dimensions,
  ThermalCell*          thermalcells,
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
    "add_solid_column   l %2d r %4d c %4d [%7d]\n",
    current_layer, current_row, current_column, current_cell) ;
#endif

  *system_matrix.ColumnPointers = *(system_matrix.ColumnPointers - 1) ;

  /* BOTTOM */

  if ( ! IS_FIRST_LAYER(current_layer) )
  {
    *system_matrix.RowIndices++ = current_cell - get_layer_area(dimensions) ;

    conductance = PARALLEL
                  (
                    thermalcells[current_cell].Bottom,
                    thermalcells[current_cell - get_layer_area(dimensions)].Top
                  ) ;
    if (conductance == 0)
      conductance = thermalcells[current_cell].Bottom
                  ? (thermalcells[current_cell].Bottom)
                  : thermalcells[current_cell - get_layer_area(dimensions)].Top ;

    *system_matrix.Values++  = -conductance ;
    diagonal_value          +=  conductance ;

    (*system_matrix.ColumnPointers)++ ;

#ifdef PRINT_SYSTEM_MATRIX
    fprintf (stderr,
    "  bottom  \t%d\t% .4e = % .4e (B) || % .4e (T)\n",
    *(system_matrix.RowIndices-1), *(system_matrix.Values-1),
    thermalcells[current_cell].Bottom,
    thermalcells[current_cell - get_layer_area(dimensions)].Top) ;
#endif
  }

  /* SOUTH */

  if ( ! IS_FIRST_ROW(current_row) )
  {
    *system_matrix.RowIndices++ = current_cell - get_number_of_columns(dimensions) ;

    conductance = PARALLEL
                  (
                    thermalcells[current_cell].South,
                    thermalcells[current_cell - get_number_of_columns(dimensions)].North
                  ) ;

    *system_matrix.Values++  = -conductance ;
    diagonal_value          +=  conductance ;

    (*system_matrix.ColumnPointers)++ ;

#ifdef PRINT_SYSTEM_MATRIX
    fprintf (stderr,
      "  south   \t%d\t% .4e = % .4e (S) || % .4e (N)\n",
      *(system_matrix.RowIndices-1), *(system_matrix.Values-1),
      thermalcells[current_cell].South,
      thermalcells[current_cell - get_number_of_columns(dimensions)].North) ;
#endif
  }

  /* WEST */

  if ( ! IS_FIRST_COLUMN(current_column) )
  {
    *system_matrix.RowIndices++ = current_cell - 1 ;

    conductance = PARALLEL
                  (
                    thermalcells[current_cell].West,
                    thermalcells[current_cell - 1].East
                  ) ;

    *system_matrix.Values++  = -conductance ;
    diagonal_value          +=  conductance ;

    (*system_matrix.ColumnPointers)++ ;

#ifdef PRINT_SYSTEM_MATRIX
    fprintf (stderr,
      "  west    \t%d\t% .4e = % .4e (W) || % .4e (E)\n",
      *(system_matrix.RowIndices-1), *(system_matrix.Values-1),
      thermalcells[current_cell].West,
      thermalcells[current_cell - 1].East) ;
#endif
  }

  /* DIAGONAL */

  *system_matrix.RowIndices++ = current_cell ;
  *system_matrix.Values       = thermalcells[current_cell].Capacity ;
  diagonal_pointer            = system_matrix.Values++ ;

  (*system_matrix.ColumnPointers)++ ;

#ifdef PRINT_SYSTEM_MATRIX
  fprintf (stderr, "  diagonal\t%d\t", *(system_matrix.RowIndices-1)) ;
  fgetpos (stderr, &diag_fposition) ;
  fprintf (stderr, "            ( + % .4e [capacity] )\n", *(system_matrix.Values-1)) ;
#endif

  /* EAST */

  if ( ! IS_LAST_COLUMN(current_column, dimensions) )
  {
    *system_matrix.RowIndices++ = current_cell + 1 ;

    conductance = PARALLEL
                  (
                    thermalcells[current_cell].East,
                    thermalcells[current_cell + 1].West
                  ) ;

    *system_matrix.Values++  = -conductance ;
    diagonal_value          +=  conductance ;

    (*system_matrix.ColumnPointers)++ ;

#ifdef PRINT_SYSTEM_MATRIX
    fprintf (stderr,
      "  east    \t%d\t% .4e = % .4e (E) || % .4e (W)\n",
      *(system_matrix.RowIndices-1), *(system_matrix.Values-1),
      thermalcells[current_cell].East,
      thermalcells[current_cell + 1].West) ;
#endif
  }

  /* NORTH */

  if ( ! IS_LAST_ROW(current_row, dimensions) )
  {
    *system_matrix.RowIndices++ = current_cell + get_number_of_columns(dimensions) ;

    conductance = PARALLEL
                  (
                    thermalcells[current_cell].North,
                    thermalcells[current_cell + get_number_of_columns(dimensions)].South
                  ) ;

    *system_matrix.Values++  = -conductance ;
    diagonal_value          +=  conductance ;

    (*system_matrix.ColumnPointers)++ ;

#ifdef PRINT_SYSTEM_MATRIX
    fprintf (stderr,
      "  north   \t%d\t% .4e = % .4e (N) || % .4e (S)\n",
      *(system_matrix.RowIndices-1), *(system_matrix.Values-1),
      thermalcells[current_cell].North,
      thermalcells[current_cell + get_number_of_columns(dimensions)].South) ;
#endif
  }

  /* TOP */

  if ( ! IS_LAST_LAYER(current_layer, dimensions) )
  {
    *system_matrix.RowIndices++ = current_cell + get_layer_area(dimensions) ;

    conductance = PARALLEL
                  (
                    thermalcells[current_cell].Top,
                    thermalcells[current_cell + get_layer_area(dimensions)].Bottom
                  ) ;

    if (conductance == 0)
      conductance = thermalcells[current_cell].Top
                  ? thermalcells[current_cell].Top
                  : thermalcells[current_cell + get_layer_area(dimensions)].Bottom ;


    *system_matrix.Values++  = -conductance ;
    diagonal_value          +=  conductance ;

    (*system_matrix.ColumnPointers)++ ;

#ifdef PRINT_SYSTEM_MATRIX
    fprintf (stderr,
      "  top     \t%d\t% .4e = % .4e (T) || % .4e (B)\n",
      *(system_matrix.RowIndices-1), *(system_matrix.Values-1),
      thermalcells[current_cell].Top,
      thermalcells[current_cell + get_layer_area(dimensions)].Bottom) ;
#endif
  }

  /* DIAGONAL ELEMENT */

  *diagonal_pointer += diagonal_value ;

#ifdef PRINT_SYSTEM_MATRIX
  fgetpos (stderr, &last_fpos) ;
  fsetpos (stderr, &diag_fposition) ;
  fprintf (stderr, "% .4e", *diagonal_pointer) ;
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
  ThermalCell*          thermalcells,
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
    "add_liquid_column  l %2d r %4d c %4d [%7d]\n",
    current_layer, current_row, current_column, current_cell) ;
#endif

  *system_matrix.ColumnPointers = *(system_matrix.ColumnPointers - 1) ;

  /* BOTTOM */

  if ( ! IS_FIRST_LAYER(current_layer) )
  {
    *system_matrix.RowIndices++ = current_cell - get_layer_area(dimensions) ;

    conductance = PARALLEL
                  (
                    thermalcells[current_cell].Bottom,
                    thermalcells[current_cell - get_layer_area(dimensions)].Top
                  ) ;

    *system_matrix.Values++  = -conductance ;
    diagonal_value          +=  conductance ;

    (*system_matrix.ColumnPointers)++ ;

#ifdef PRINT_SYSTEM_MATRIX
    fprintf (stderr,
      "  bottom  \t%d\t% .4e = % .4e (B) || % .4e (T)\n",
      *(system_matrix.RowIndices-1), *(system_matrix.Values-1),
      thermalcells[current_cell].Bottom,
      thermalcells[current_cell - get_layer_area(dimensions)].Top) ;
#endif
  }

  /* SOUTH */

  if ( ! IS_FIRST_ROW(current_row) )
  {
    *system_matrix.RowIndices++ = current_cell - get_number_of_columns(dimensions) ;
    *system_matrix.Values++     = thermalcells[current_cell].North ; /* == C */

    (*system_matrix.ColumnPointers)++ ;

#ifdef PRINT_SYSTEM_MATRIX
    fprintf (stderr,
      "  south   \t%d\t% .4e (N)\n", *(system_matrix.RowIndices-1), *(system_matrix.Values-1)) ;
#endif
  }

  /* WEST */

  if ( ! IS_FIRST_COLUMN(current_column) )
  {
    *system_matrix.RowIndices++ = current_cell - 1 ;

    conductance = PARALLEL
                  (
                    thermalcells[current_cell].West,
                    thermalcells[current_cell - 1].East
                  ) ;

    *system_matrix.Values++  = -conductance ;
    diagonal_value          +=  conductance ;

    (*system_matrix.ColumnPointers)++ ;

#ifdef PRINT_SYSTEM_MATRIX
    fprintf (stderr,
      "  west    \t%d\t% .4e = % .4e (W) || % .4e (E)\n",
      *(system_matrix.RowIndices-1), *(system_matrix.Values-1),
      thermalcells[current_cell].West,
      thermalcells[current_cell - 1].East) ;
#endif
  }

  /* DIAGONAL */

  *system_matrix.RowIndices++ = current_cell ;
  *system_matrix.Values       = thermalcells[current_cell].Capacity ;
  diagonal_pointer            = system_matrix.Values++ ;

  (*system_matrix.ColumnPointers)++ ;

#ifdef PRINT_SYSTEM_MATRIX
  fprintf (stderr, "  diagonal\t%d\t", *(system_matrix.RowIndices-1)) ;
  fgetpos (stderr, &diag_fposition) ;
  fprintf (stderr, "            ( + % .4e [capacity] )\n", *(system_matrix.Values-1)) ;
#endif

  /* EAST */

  if ( ! IS_LAST_COLUMN(current_column, dimensions) )
  {
    *system_matrix.RowIndices++ = current_cell + 1 ;

    conductance = PARALLEL
                  (
                    thermalcells[current_cell].East,
                    thermalcells[current_cell + 1].West
                  ) ;

    *system_matrix.Values++  = -conductance ;
    diagonal_value          +=  conductance ;

    (*system_matrix.ColumnPointers)++ ;

#ifdef PRINT_SYSTEM_MATRIX
    fprintf (stderr,
      "  east    \t%d\t% .4e = % .4e (E) || % .4e (W)\n",
      *(system_matrix.RowIndices-1), *(system_matrix.Values-1),
      thermalcells[current_cell].East,
      thermalcells[current_cell + 1].West) ;
#endif
  }

  /* NORTH */

  if ( ! IS_LAST_ROW(current_row, dimensions) )
  {
    *system_matrix.RowIndices++ = current_cell + get_number_of_columns(dimensions) ;
    *system_matrix.Values++     = thermalcells[current_cell].South ; /* == - C */

    (*system_matrix.ColumnPointers)++ ;

#ifdef PRINT_SYSTEM_MATRIX
    fprintf (stderr,
      "  north   \t%d\t% .4e (S)\n", *(system_matrix.RowIndices-1), *(system_matrix.Values-1)) ;
#endif
  }

  /* TOP */

  if ( ! IS_LAST_LAYER(current_layer, dimensions) )
  {
    *system_matrix.RowIndices++ = current_cell + get_layer_area(dimensions) ;

    conductance = PARALLEL
                  (
                    thermalcells[current_cell].Top,
                    thermalcells[current_cell + get_layer_area(dimensions)].Bottom
                  ) ;

    *system_matrix.Values++  = -conductance ;
    diagonal_value          +=  conductance ;

    (*system_matrix.ColumnPointers)++ ;

#ifdef PRINT_SYSTEM_MATRIX
    fprintf (stderr,
      "  top     \t%d\t% .4e = % .4e (T) || % .4e (B)\n",
      *(system_matrix.RowIndices-1), *(system_matrix.Values-1),
      thermalcells[current_cell].Top,
      thermalcells[current_cell + get_layer_area(dimensions)].Bottom) ;
#endif
  }

  /* DIAGONAL ELEMENT */

  *diagonal_pointer += diagonal_value ;

  if (current_row == 0 || current_row == get_number_of_rows(dimensions) - 1)

    *diagonal_pointer += thermalcells[current_cell].North ; /* == (C) */

#ifdef PRINT_SYSTEM_MATRIX
  fgetpos (stderr, &last_fpos) ;
  fsetpos (stderr, &diag_fposition) ;
  fprintf (stderr, "% .4e", *diagonal_pointer) ;
  fsetpos (stderr, &last_fpos) ;

  fprintf (stderr, "  %d\n", *system_matrix.ColumnPointers) ;
#endif

  system_matrix.ColumnPointers++ ;

  return system_matrix ;
}

/******************************************************************************/

SystemMatrix add_liquid_column_2rm
(
  Dimensions*           dimensions,
  ThermalCell*          thermalcells,
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
    "add_liquid_column_2rm  l %2d r %4d c %4d [%7d]\n",
    current_layer, current_row, current_column, current_cell) ;
#endif

  *system_matrix.ColumnPointers = *(system_matrix.ColumnPointers - 1) ;

  /* BOTTOM */

  if ( ! IS_FIRST_LAYER(current_layer) )
  {
    *system_matrix.RowIndices++ = current_cell - get_layer_area(dimensions) ;

    conductance = thermalcells[current_cell].Bottom;

    *system_matrix.Values++  = -conductance ;
    diagonal_value          +=  conductance ;

    (*system_matrix.ColumnPointers)++ ;

#ifdef PRINT_SYSTEM_MATRIX
    fprintf (stderr,
      "  bottom  \t%d\t% .4e\n",
      *(system_matrix.RowIndices-1), *(system_matrix.Values-1)) ;
#endif
  }

  /* SOUTH */

  if ( ! IS_FIRST_ROW(current_row) )
  {
    *system_matrix.RowIndices++ = current_cell - get_number_of_columns(dimensions) ;
    *system_matrix.Values++     = thermalcells[current_cell].North ; /* == C */

    (*system_matrix.ColumnPointers)++ ;

#ifdef PRINT_SYSTEM_MATRIX
    fprintf (stderr,
      "  south   \t%d\t% .4e\n", *(system_matrix.RowIndices-1), *(system_matrix.Values-1)) ;
#endif
  }

  /* DIAGONAL */

  *system_matrix.RowIndices++ = current_cell ;
  *system_matrix.Values       = thermalcells[current_cell].Capacity ;
  diagonal_pointer            = system_matrix.Values++ ;

  (*system_matrix.ColumnPointers)++ ;

#ifdef PRINT_SYSTEM_MATRIX
  fprintf (stderr, "  diagonal\t%d\t", *(system_matrix.RowIndices-1)) ;
  fgetpos (stderr, &diag_fposition) ;
  fprintf (stderr, "            ( + % .4e [capacity] )\n", *(system_matrix.Values-1)) ;
#endif

  /* NORTH */

  if ( ! IS_LAST_ROW(current_row, dimensions) )
  {
    *system_matrix.RowIndices++ = current_cell + get_number_of_columns(dimensions) ;
    *system_matrix.Values++     = thermalcells[current_cell].South ; /* == - C */

    (*system_matrix.ColumnPointers)++ ;

#ifdef PRINT_SYSTEM_MATRIX
    fprintf (stderr,
      "  north   \t%d\t% .4e\n", *(system_matrix.RowIndices-1), *(system_matrix.Values-1)) ;
#endif
  }

  /* TOP */

  if ( ! IS_LAST_LAYER(current_layer, dimensions) )
  {
    *system_matrix.RowIndices++ = current_cell + 2 * get_layer_area(dimensions) ;

    conductance = thermalcells[current_cell].Top;

    *system_matrix.Values++  = -conductance ;
    diagonal_value          +=  conductance ;

    (*system_matrix.ColumnPointers)++ ;

#ifdef PRINT_SYSTEM_MATRIX
    fprintf (stderr,
      "  top     \t%d\t% .4e\n",
      *(system_matrix.RowIndices-1), *(system_matrix.Values-1)) ;
#endif
  }

  /* DIAGONAL ELEMENT */

  *diagonal_pointer += diagonal_value ;

  if (current_row == 0 || current_row == get_number_of_rows(dimensions) - 1)

    *diagonal_pointer += thermalcells[current_cell].North ; /* == (C) */

#ifdef PRINT_SYSTEM_MATRIX
  fgetpos (stderr, &last_fpos) ;
  fsetpos (stderr, &diag_fposition) ;
  fprintf (stderr, "% .4e", *diagonal_pointer) ;
  fsetpos (stderr, &last_fpos) ;

  fprintf (stderr, "  %d\n", *system_matrix.ColumnPointers) ;
#endif

  system_matrix.ColumnPointers++ ;

  return system_matrix ;
}

/******************************************************************************/

SystemMatrix add_bottom_wall_column_2rm
(
  Dimensions*           dimensions,
  ThermalCell*          thermalcells,
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
    "add_bottom_wall_column_2rm   l %2d r %4d c %4d [%7d]\n",
    current_layer, current_row, current_column, current_cell) ;
#endif

  *system_matrix.ColumnPointers = *(system_matrix.ColumnPointers - 1) ;

  /* BOTTOM connected to Silicon */

  if ( ! IS_FIRST_LAYER(current_layer) )
  {
    *system_matrix.RowIndices++ = current_cell - get_layer_area(dimensions) ;

    conductance = thermalcells[current_cell - get_layer_area(dimensions)].Top ;

    *system_matrix.Values++  = -conductance ;
    diagonal_value          +=  conductance ;

    (*system_matrix.ColumnPointers)++ ;

#ifdef PRINT_SYSTEM_MATRIX
    fprintf (stderr,
      "  Bottom connected to Silicon     \t%d\t% .4e\n",
      *(system_matrix.RowIndices-1), *(system_matrix.Values-1)) ;
#endif
  }

  /* DIAGONAL */

  *system_matrix.RowIndices++ = current_cell ;
  *system_matrix.Values       = thermalcells[current_cell].Capacity ;
  diagonal_pointer            = system_matrix.Values++ ;

  (*system_matrix.ColumnPointers)++ ;

#ifdef PRINT_SYSTEM_MATRIX
  fprintf (stderr, "  diagonal\t%d\t", *(system_matrix.RowIndices-1)) ;
  fgetpos (stderr, &diag_fposition) ;
  fprintf (stderr, "            ( + % .4e [capacity] )\n", *(system_matrix.Values-1)) ;
#endif


  /* Top connected to Channel */

  if ( ! IS_LAST_LAYER(current_layer, dimensions) )
  {
    *system_matrix.RowIndices++ = current_cell + get_layer_area(dimensions) ;

    conductance = thermalcells[current_cell + get_layer_area(dimensions)].Bottom ;

    *system_matrix.Values++  = -conductance ;
    diagonal_value          +=  conductance ;

    (*system_matrix.ColumnPointers)++ ;

#ifdef PRINT_SYSTEM_MATRIX
    fprintf (stderr,
    "  Top connected to Channel  \t%d\t% .4e\n",
    *(system_matrix.RowIndices-1), *(system_matrix.Values-1)) ;
#endif
  }

  /* Top connected to Virtual Wall */

  if ( ! IS_LAST_LAYER(current_layer, dimensions) )
  {
    *system_matrix.RowIndices++ = current_cell + 2 * get_layer_area(dimensions) ;

    conductance = thermalcells[current_cell + 2 * get_layer_area(dimensions)].Bottom ;

    *system_matrix.Values++  = -conductance ;
    diagonal_value          +=  conductance ;

    (*system_matrix.ColumnPointers)++ ;

#ifdef PRINT_SYSTEM_MATRIX
    fprintf (stderr,
    "  Top connceted to Virtual Wall  \t%d\t% .4e\n",
    *(system_matrix.RowIndices-1), *(system_matrix.Values-1)) ;
#endif
  }

  /* DIAGONAL ELEMENT */

  *diagonal_pointer += diagonal_value ;

#ifdef PRINT_SYSTEM_MATRIX
  fgetpos (stderr, &last_fpos) ;
  fsetpos (stderr, &diag_fposition) ;
  fprintf (stderr, "% .4e", *diagonal_pointer) ;
  fsetpos (stderr, &last_fpos) ;

  fprintf (stderr, "  %d\n", *system_matrix.ColumnPointers) ;
#endif

  system_matrix.ColumnPointers++ ;

  return system_matrix ;
}

/******************************************************************************/

SystemMatrix add_top_wall_column_2rm
(
  Dimensions*           dimensions,
  ThermalCell*          thermalcells,
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
    "add_top_wall_column_2rm   l %2d r %4d c %4d [%7d]\n",
    current_layer, current_row, current_column, current_cell) ;
#endif

  *system_matrix.ColumnPointers = *(system_matrix.ColumnPointers - 1) ;

  /* BOTTOM connected to Channel */

  if ( ! IS_FIRST_LAYER(current_layer) )
  {
    *system_matrix.RowIndices++ = current_cell - 2 * get_layer_area(dimensions) ;

    conductance = thermalcells[current_cell - 2 * get_layer_area(dimensions)].Top ;

    *system_matrix.Values++  = -conductance ;
    diagonal_value          +=  conductance ;

    (*system_matrix.ColumnPointers)++ ;

#ifdef PRINT_SYSTEM_MATRIX
    fprintf (stderr,
      "  Bottom connected to Channel     \t%d\t% .4e\n",
      *(system_matrix.RowIndices-1), *(system_matrix.Values-1)) ;
#endif
  }

  /* BOTTOM connected to Virtual Wall */

  if ( ! IS_FIRST_LAYER(current_layer) )
  {
    *system_matrix.RowIndices++ = current_cell - get_layer_area(dimensions) ;

    conductance = thermalcells[current_cell - get_layer_area(dimensions)].Top ;

    *system_matrix.Values++  = -conductance ;
    diagonal_value          +=  conductance ;

    (*system_matrix.ColumnPointers)++ ;

#ifdef PRINT_SYSTEM_MATRIX
    fprintf (stderr,
      "  Bottom connected to Virtual Wall     \t%d\t% .4e\n",
      *(system_matrix.RowIndices-1), *(system_matrix.Values-1)) ;
#endif
  }

  /* DIAGONAL */

  *system_matrix.RowIndices++ = current_cell ;
  *system_matrix.Values       = thermalcells[current_cell].Capacity ;
  diagonal_pointer            = system_matrix.Values++ ;

  (*system_matrix.ColumnPointers)++ ;

#ifdef PRINT_SYSTEM_MATRIX
  fprintf (stderr, "  diagonal\t%d\t", *(system_matrix.RowIndices-1)) ;
  fgetpos (stderr, &diag_fposition) ;
  fprintf (stderr, "            ( + % .4e [capacity] )\n", *(system_matrix.Values-1)) ;
#endif


  /* Top connected to Silicon */

  if ( ! IS_LAST_LAYER(current_layer, dimensions) )
  {
    *system_matrix.RowIndices++ = current_cell + get_layer_area(dimensions) ;

    conductance = thermalcells[current_cell + get_layer_area(dimensions)].Bottom ;

    *system_matrix.Values++  = -conductance ;
    diagonal_value          +=  conductance ;

    (*system_matrix.ColumnPointers)++ ;

#ifdef PRINT_SYSTEM_MATRIX
    fprintf (stderr,
    "  Top connected to Silicon  \t%d\t% .4e\n",
    *(system_matrix.RowIndices-1), *(system_matrix.Values-1)) ;
#endif
  }

  /* DIAGONAL ELEMENT */

  *diagonal_pointer += diagonal_value ;

#ifdef PRINT_SYSTEM_MATRIX
  fgetpos (stderr, &last_fpos) ;
  fsetpos (stderr, &diag_fposition) ;
  fprintf (stderr, "% .4e", *diagonal_pointer) ;
  fsetpos (stderr, &last_fpos) ;

  fprintf (stderr, "  %d\n", *system_matrix.ColumnPointers) ;
#endif

  system_matrix.ColumnPointers++ ;

  return system_matrix ;
}

/******************************************************************************/
SystemMatrix add_virtual_wall_column_2rm
(
  Dimensions*           dimensions,
  ThermalCell*          thermalcells,
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
    "add_virtual_wall_column   l %2d r %4d c %4d [%7d]\n",
    current_layer, current_row, current_column, current_cell) ;
#endif

  *system_matrix.ColumnPointers = *(system_matrix.ColumnPointers - 1) ;

  /* BOTTOM */

  if ( ! IS_FIRST_LAYER(current_layer) )
  {
    *system_matrix.RowIndices++ = current_cell - 2 * get_layer_area(dimensions) ;

    conductance = thermalcells[current_cell].Bottom;

    *system_matrix.Values++  = -conductance ;
    diagonal_value          +=  conductance ;

    (*system_matrix.ColumnPointers)++ ;

#ifdef PRINT_SYSTEM_MATRIX
    fprintf (stderr,
    "  bottom  \t%d\t% .4e\n",
    *(system_matrix.RowIndices-1), *(system_matrix.Values-1)) ;
#endif
  }

  /* DIAGONAL */

  *system_matrix.RowIndices++ = current_cell ;
  *system_matrix.Values       = thermalcells[current_cell].Capacity ;
  diagonal_pointer            = system_matrix.Values++ ;

  (*system_matrix.ColumnPointers)++ ;

#ifdef PRINT_SYSTEM_MATRIX
  fprintf (stderr, "  diagonal\t%d\t", *(system_matrix.RowIndices-1)) ;
  fgetpos (stderr, &diag_fposition) ;
  fprintf (stderr, "            ( + % .4e [capacity] )\n", *(system_matrix.Values-1)) ;
#endif

  /* TOP */

  if ( ! IS_LAST_LAYER(current_layer, dimensions) )
  {
    *system_matrix.RowIndices++ = current_cell + get_layer_area(dimensions) ;

    conductance = thermalcells[current_cell].Top;

    *system_matrix.Values++  = -conductance ;
    diagonal_value          +=  conductance ;

    (*system_matrix.ColumnPointers)++ ;

#ifdef PRINT_SYSTEM_MATRIX
    fprintf (stderr,
      "  top     \t%d\t% .4e\n",
      *(system_matrix.RowIndices-1), *(system_matrix.Values-1)) ;
#endif
  }

  /* DIAGONAL ELEMENT */

  *diagonal_pointer += diagonal_value ;

#ifdef PRINT_SYSTEM_MATRIX
  fgetpos (stderr, &last_fpos) ;
  fsetpos (stderr, &diag_fposition) ;
  fprintf (stderr, "% .4e", *diagonal_pointer) ;
  fsetpos (stderr, &last_fpos) ;

  fprintf (stderr, "  %d\n", *system_matrix.ColumnPointers) ;
#endif

  system_matrix.ColumnPointers++ ;

  return system_matrix ;
}

/******************************************************************************/

