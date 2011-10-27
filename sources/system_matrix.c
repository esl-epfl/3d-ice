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
    SystemMatrix *matrix,
    Quantity_t    size,
    Quantity_t    nnz
)
{
    matrix->Size = size ;
    matrix->NNz  = nnz ;

    matrix->RowIndices = malloc (sizeof(SystemMatrixRow_t) * nnz) ;

    if (matrix->RowIndices == NULL)

        return 0 ;

    matrix->ColumnPointers = malloc (sizeof(SystemMatrixColumn_t) * (size + 1)) ;

    if (matrix->ColumnPointers == NULL)
    {
        FREE_POINTER (free, matrix->RowIndices) ;
        return 0 ;
    }

    matrix->Values = malloc (sizeof(SystemMatrixValue_t) * nnz) ;

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

            fprintf (file, "%5d\t%5d\t%32.24f\n", matrix.RowIndices[row] + 1, column + 1, matrix.Values[row]) ;

    fclose (file) ;
}

/******************************************************************************/
/******************************************************************************/

SystemMatrix add_solid_column
(
    Dimensions      *dimensions,
    ThermalCell     *thermal_cells,
    GridDimension_t  layer_index,
    GridDimension_t  row_index,
    GridDimension_t  column_index,
    SystemMatrix     system_matrix
)
{
    SystemMatrixValue_t  conductance      = SYSTEMMATRIXVALUE_I ;
    SystemMatrixValue_t  diagonal_value   = SYSTEMMATRIXVALUE_I ;
    SystemMatrixValue_t* diagonal_pointer = NULL ;

    GridDimension_t layer_offset  = get_layer_area (dimensions) ;
    GridDimension_t row_offset    = get_number_of_columns (dimensions) ;
    GridDimension_t column_offset = 1 ;

    GridDimension_t cell_index = get_cell_offset_in_stack

        (dimensions, layer_index, row_index, column_index) ;

    ThermalCell *cell = thermal_cells + cell_index ;

#ifdef PRINT_SYSTEM_MATRIX
    fpos_t diag_fposition, last_fpos ;
    fprintf (stderr,
        "add_solid_column   l %2d r %4d c %4d [%7d]\n",
        layer_index, row_index, column_index, cell_index) ;
#endif

    *system_matrix.ColumnPointers = *(system_matrix.ColumnPointers - 1) ;

    /* BOTTOM */

    if ( ! IS_FIRST_LAYER(layer_index) )
    {
        *system_matrix.RowIndices++ = cell_index - layer_offset ;

        conductance = PARALLEL (cell->Bottom, (cell - layer_offset)->Top) ;

        if (conductance == 0)
        {
            conductance = cell->Bottom ? cell->Bottom : (cell - layer_offset)->Top ;
        }

        *system_matrix.Values++  = -conductance ;
        diagonal_value          +=  conductance ;

        (*system_matrix.ColumnPointers)++ ;

#ifdef PRINT_SYSTEM_MATRIX
        fprintf (stderr,
            "  bottom  \t%d\t% .4e = % .4e (B) || % .4e (T)\n",
            *(system_matrix.RowIndices-1), *(system_matrix.Values-1),
            cell->Bottom, (cell - layer_offset)->Top) ;
#endif
    }

    /* SOUTH */

    if ( ! IS_FIRST_ROW(row_index) )
    {
        *system_matrix.RowIndices++ = cell_index - row_offset ;

        conductance = PARALLEL (cell->South, (cell - row_offset)->North) ;

        *system_matrix.Values++  = -conductance ;
        diagonal_value          +=  conductance ;

        (*system_matrix.ColumnPointers)++ ;

#ifdef PRINT_SYSTEM_MATRIX
        fprintf (stderr,
            "  south   \t%d\t% .4e = % .4e (S) || % .4e (N)\n",
            *(system_matrix.RowIndices-1), *(system_matrix.Values-1),
            cell->South, (cell - row_offset)->North) ;
#endif
    }

    /* WEST */

    if ( ! IS_FIRST_COLUMN(column_index) )
    {
        *system_matrix.RowIndices++ = cell_index - column_offset ;

        conductance = PARALLEL (cell->West, (cell - column_offset)->East) ;

        *system_matrix.Values++  = -conductance ;
        diagonal_value          +=  conductance ;

        (*system_matrix.ColumnPointers)++ ;

#ifdef PRINT_SYSTEM_MATRIX
        fprintf (stderr,
            "  west    \t%d\t% .4e = % .4e (W) || % .4e (E)\n",
            *(system_matrix.RowIndices-1), *(system_matrix.Values-1),
            cell->West, (cell - column_offset)->East) ;
#endif
    }

    /* DIAGONAL */

    *system_matrix.RowIndices++ = cell_index ;
    *system_matrix.Values       = cell->Capacity ;
    diagonal_pointer            = system_matrix.Values++ ;

    (*system_matrix.ColumnPointers)++ ;

#ifdef PRINT_SYSTEM_MATRIX
    fprintf (stderr, "  diagonal\t%d\t", *(system_matrix.RowIndices-1)) ;
    fgetpos (stderr, &diag_fposition) ;
    fprintf (stderr, "            ( + % .4e [capacity] )\n", *(system_matrix.Values-1)) ;
#endif

    /* EAST */

    if ( ! IS_LAST_COLUMN(column_index, dimensions) )
    {
        *system_matrix.RowIndices++ = cell_index + column_offset ;

        conductance = PARALLEL (cell->East, (cell + column_offset)->West) ;

        *system_matrix.Values++  = -conductance ;
        diagonal_value          +=  conductance ;

        (*system_matrix.ColumnPointers)++ ;

#ifdef PRINT_SYSTEM_MATRIX
        fprintf (stderr,
            "  east    \t%d\t% .4e = % .4e (E) || % .4e (W)\n",
            *(system_matrix.RowIndices-1), *(system_matrix.Values-1),
            cell->East, (cell + column_offset)->West) ;
#endif
    }

    /* NORTH */

    if ( ! IS_LAST_ROW(row_index, dimensions) )
    {
        *system_matrix.RowIndices++ = cell_index + row_offset ;

        conductance = PARALLEL (cell->North, (cell + row_offset)->South) ;

        *system_matrix.Values++  = -conductance ;
        diagonal_value          +=  conductance ;

        (*system_matrix.ColumnPointers)++ ;

#ifdef PRINT_SYSTEM_MATRIX
        fprintf (stderr,
            "  north   \t%d\t% .4e = % .4e (N) || % .4e (S)\n",
            *(system_matrix.RowIndices-1), *(system_matrix.Values-1),
            cell->North, (cell + row_offset)->South) ;
#endif
    }

    /* TOP */

    if ( ! IS_LAST_LAYER(layer_index, dimensions) )
    {
        *system_matrix.RowIndices++ = cell_index + layer_offset ;

        conductance = PARALLEL (cell->Top, (cell + layer_offset)->Bottom) ;

        if (conductance == 0)
        {
            conductance = cell->Top ? cell->Top : (cell + layer_offset)->Bottom ;
        }

        *system_matrix.Values++  = -conductance ;
        diagonal_value          +=  conductance ;

        (*system_matrix.ColumnPointers)++ ;

#ifdef PRINT_SYSTEM_MATRIX
        fprintf (stderr,
            "  top     \t%d\t% .4e = % .4e (T) || % .4e (B)\n",
            *(system_matrix.RowIndices-1), *(system_matrix.Values-1),
            cell->Top, (cell + layer_offset)->Bottom) ;
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
/******************************************************************************/

SystemMatrix add_liquid_column_mc_4rm
(
    Dimensions      *dimensions,
    ThermalCell     *thermal_cells,
    GridDimension_t  layer_index,
    GridDimension_t  row_index,
    GridDimension_t  column_index,
    SystemMatrix     system_matrix
)
{
    SystemMatrixValue_t  conductance      = SYSTEMMATRIXVALUE_I ;
    SystemMatrixValue_t  diagonal_value   = SYSTEMMATRIXVALUE_I ;
    SystemMatrixValue_t* diagonal_pointer = NULL ;

    GridDimension_t layer_offset  = get_layer_area (dimensions) ;
    GridDimension_t row_offset    = get_number_of_columns (dimensions) ;
    GridDimension_t column_offset = 1 ;

    GridDimension_t cell_index = get_cell_offset_in_stack

        (dimensions, layer_index, row_index, column_index) ;

    ThermalCell *cell = thermal_cells + cell_index ;

#ifdef PRINT_SYSTEM_MATRIX
    fpos_t diag_fposition, last_fpos ;
    fprintf (stderr,
        "add_liquid_column  l %2d r %4d c %4d [%7d]\n",
        layer_index, row_index, column_index, cell_index) ;
#endif

    *system_matrix.ColumnPointers = *(system_matrix.ColumnPointers - 1) ;

    /* BOTTOM */

    if ( ! IS_FIRST_LAYER(layer_index) )
    {
        *system_matrix.RowIndices++ = cell_index - layer_offset ;

        conductance = PARALLEL (cell->Bottom, (cell - layer_offset)->Top) ;

        *system_matrix.Values++  = -conductance ;
        diagonal_value          +=  conductance ;

        (*system_matrix.ColumnPointers)++ ;

#ifdef PRINT_SYSTEM_MATRIX
        fprintf (stderr,
            "  bottom  \t%d\t% .4e = % .4e (B) || % .4e (T)\n",
            *(system_matrix.RowIndices-1), *(system_matrix.Values-1),
            cell->Bottom, (cell - layer_offset)->Top) ;
#endif
    }

    /* SOUTH */

    if ( ! IS_FIRST_ROW(row_index) )
    {
        *system_matrix.RowIndices++ = cell_index - row_offset ;
        *system_matrix.Values++     = cell->North ; /* == C */

        (*system_matrix.ColumnPointers)++ ;

#ifdef PRINT_SYSTEM_MATRIX
        fprintf (stderr,
            "  south   \t%d\t% .4e (N)\n",
            *(system_matrix.RowIndices-1), *(system_matrix.Values-1)) ;
#endif
    }

    /* WEST */

    if ( ! IS_FIRST_COLUMN(column_index) )
    {
        *system_matrix.RowIndices++ = cell_index - column_offset ;

        conductance = PARALLEL (cell->West, (cell - column_offset)->East) ;

        *system_matrix.Values++  = -conductance ;
        diagonal_value          +=  conductance ;

        (*system_matrix.ColumnPointers)++ ;

#ifdef PRINT_SYSTEM_MATRIX
        fprintf (stderr,
            "  west    \t%d\t% .4e = % .4e (W) || % .4e (E)\n",
            *(system_matrix.RowIndices-1), *(system_matrix.Values-1),
            cell->West, (cell - column_offset)->East) ;
#endif
    }

    /* DIAGONAL */

    *system_matrix.RowIndices++ = cell_index ;
    *system_matrix.Values       = cell->Capacity ;
    diagonal_pointer            = system_matrix.Values++ ;

    (*system_matrix.ColumnPointers)++ ;

#ifdef PRINT_SYSTEM_MATRIX
    fprintf (stderr, "  diagonal\t%d\t", *(system_matrix.RowIndices-1)) ;
    fgetpos (stderr, &diag_fposition) ;
    fprintf (stderr, "            ( + % .4e [capacity] )\n", *(system_matrix.Values-1)) ;
#endif

    /* EAST */

    if ( ! IS_LAST_COLUMN(column_index, dimensions) )
    {
        *system_matrix.RowIndices++ = cell_index + column_offset ;

        conductance = PARALLEL (cell->East, (cell + column_offset)->West) ;

        *system_matrix.Values++  = -conductance ;
        diagonal_value          +=  conductance ;

        (*system_matrix.ColumnPointers)++ ;

#ifdef PRINT_SYSTEM_MATRIX
        fprintf (stderr,
            "  east    \t%d\t% .4e = % .4e (E) || % .4e (W)\n",
            *(system_matrix.RowIndices-1), *(system_matrix.Values-1),
            cell->East, (cell + column_offset)->West) ;
#endif
    }

    /* NORTH */

    if ( ! IS_LAST_ROW(row_index, dimensions) )
    {
        *system_matrix.RowIndices++ = cell_index + row_offset ;
        *system_matrix.Values++     = cell->South ; /* == - C */

        (*system_matrix.ColumnPointers)++ ;

#ifdef PRINT_SYSTEM_MATRIX
        fprintf (stderr,
            "  north   \t%d\t% .4e (S)\n",
            *(system_matrix.RowIndices-1), *(system_matrix.Values-1)) ;
#endif
    }

    /* TOP */

    if ( ! IS_LAST_LAYER(layer_index, dimensions) )
    {
        *system_matrix.RowIndices++ = cell_index + layer_offset ;

        conductance = PARALLEL (cell->Top, (cell + layer_offset)->Bottom) ;

        *system_matrix.Values++  = -conductance ;
        diagonal_value          +=  conductance ;

        (*system_matrix.ColumnPointers)++ ;

#ifdef PRINT_SYSTEM_MATRIX
        fprintf (stderr,
            "  top     \t%d\t% .4e = % .4e (T) || % .4e (B)\n",
            *(system_matrix.RowIndices-1), *(system_matrix.Values-1),
            cell->Top, (cell + layer_offset)->Bottom) ;
#endif
    }

    /* DIAGONAL ELEMENT */

    *diagonal_pointer += diagonal_value ;

    if (IS_FIRST_ROW(row_index) || IS_LAST_ROW(row_index, dimensions))

        *diagonal_pointer += cell->North ; /* == (C) */

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
/******************************************************************************/

SystemMatrix add_liquid_column_mc_2rm
(
    Dimensions      *dimensions,
    ThermalCell     *thermal_cells,
    GridDimension_t  layer_index,
    GridDimension_t  row_index,
    GridDimension_t  column_index,
    SystemMatrix     system_matrix
)
{
    SystemMatrixValue_t  conductance      = SYSTEMMATRIXVALUE_I ;
    SystemMatrixValue_t  diagonal_value   = SYSTEMMATRIXVALUE_I ;
    SystemMatrixValue_t* diagonal_pointer = NULL ;

    GridDimension_t layer_offset = get_layer_area (dimensions) ;
    GridDimension_t row_offset   = get_number_of_columns (dimensions) ;

    GridDimension_t cell_index = get_cell_offset_in_stack

        (dimensions, layer_index, row_index, column_index) ;

    ThermalCell *cell = thermal_cells + cell_index ;

#ifdef PRINT_SYSTEM_MATRIX
    fpos_t diag_fposition, last_fpos ;
    fprintf (stderr,
        "add_liquid_column_2rm  l %2d r %4d c %4d [%7d]\n",
        layer_index, row_index, column_index, cell_index) ;
#endif

    *system_matrix.ColumnPointers = *(system_matrix.ColumnPointers - 1) ;

    /* BOTTOM */

    if ( ! IS_FIRST_LAYER(layer_index) )
    {
        *system_matrix.RowIndices++ = cell_index - layer_offset ;

        conductance = cell->Bottom;

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

    if ( ! IS_FIRST_ROW(row_index) )
    {
        *system_matrix.RowIndices++ = cell_index - row_offset ;
        *system_matrix.Values++     = cell->North ; /* == C */

        (*system_matrix.ColumnPointers)++ ;

#ifdef PRINT_SYSTEM_MATRIX
        fprintf (stderr,
            "  south   \t%d\t% .4e\n",
            *(system_matrix.RowIndices-1), *(system_matrix.Values-1)) ;
#endif
    }

    /* DIAGONAL */

    *system_matrix.RowIndices++ = cell_index ;
    *system_matrix.Values       = cell->Capacity ;
    diagonal_pointer            = system_matrix.Values++ ;

    (*system_matrix.ColumnPointers)++ ;

#ifdef PRINT_SYSTEM_MATRIX
    fprintf (stderr, "  diagonal\t%d\t", *(system_matrix.RowIndices-1)) ;
    fgetpos (stderr, &diag_fposition) ;
    fprintf (stderr, "            ( + % .4e [capacity] )\n", *(system_matrix.Values-1)) ;
#endif

    /* NORTH */

    if ( ! IS_LAST_ROW(row_index, dimensions) )
    {
        *system_matrix.RowIndices++ = cell_index + row_offset ;
        *system_matrix.Values++     = cell->South ; /* == - C */

        (*system_matrix.ColumnPointers)++ ;

#ifdef PRINT_SYSTEM_MATRIX
        fprintf (stderr,
            "  north   \t%d\t% .4e\n",
            *(system_matrix.RowIndices-1), *(system_matrix.Values-1)) ;
#endif
    }

    /* TOP */

    if ( ! IS_LAST_LAYER(layer_index, dimensions) )
    {
        *system_matrix.RowIndices++ = cell_index + 2 * layer_offset ;

        conductance = cell->Top;

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

    if (IS_FIRST_ROW (row_index) || IS_LAST_ROW (row_index, dimensions))

        *diagonal_pointer += cell->North ; /* == (C) */

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

SystemMatrix add_bottom_wall_column_mc_2rm
(
    Dimensions      *dimensions,
    ThermalCell     *thermal_cells,
    GridDimension_t  layer_index,
    GridDimension_t  row_index,
    GridDimension_t  column_index,
    SystemMatrix     system_matrix
)
{
    SystemMatrixValue_t  conductance      = SYSTEMMATRIXVALUE_I ;
    SystemMatrixValue_t  diagonal_value   = SYSTEMMATRIXVALUE_I ;
    SystemMatrixValue_t* diagonal_pointer = NULL ;

    GridDimension_t layer_offset = get_layer_area (dimensions) ;

    GridDimension_t cell_index = get_cell_offset_in_stack

        (dimensions, layer_index, row_index, column_index) ;

    ThermalCell *cell = thermal_cells + cell_index ;

#ifdef PRINT_SYSTEM_MATRIX
    fpos_t diag_fposition, last_fpos ;
    fprintf (stderr,
        "add_bottom_wall_column_2rm   l %2d r %4d c %4d [%7d]\n",
        layer_index, row_index, column_index, cell_index) ;
#endif

    *system_matrix.ColumnPointers = *(system_matrix.ColumnPointers - 1) ;

    /* BOTTOM connected to Silicon */

    if ( ! IS_FIRST_LAYER(layer_index) )
    {
        *system_matrix.RowIndices++ = cell_index - layer_offset ;

        conductance = (cell - layer_offset)->Top ;

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

    *system_matrix.RowIndices++ = cell_index ;
    *system_matrix.Values       = cell->Capacity ;
    diagonal_pointer            = system_matrix.Values++ ;

    (*system_matrix.ColumnPointers)++ ;

#ifdef PRINT_SYSTEM_MATRIX
    fprintf (stderr, "  diagonal\t%d\t", *(system_matrix.RowIndices-1)) ;
    fgetpos (stderr, &diag_fposition) ;
    fprintf (stderr, "            ( + % .4e [capacity] )\n", *(system_matrix.Values-1)) ;
#endif

    /* Top connected to Virtual Wall */

    if ( ! IS_LAST_LAYER(layer_index, dimensions) )
    {
        *system_matrix.RowIndices++ = cell_index + layer_offset ;

        conductance = (cell + layer_offset)->Bottom ;

        *system_matrix.Values++  = -conductance ;
        diagonal_value          +=  conductance ;

        (*system_matrix.ColumnPointers)++ ;

#ifdef PRINT_SYSTEM_MATRIX
        fprintf (stderr,
            "  Top connected to Channel  \t%d\t% .4e\n",
            *(system_matrix.RowIndices-1), *(system_matrix.Values-1)) ;
#endif
    }

    /* Top connected to Channel */

    if ( ! IS_LAST_LAYER(layer_index, dimensions) )
    {
        *system_matrix.RowIndices++ = cell_index + 2 * layer_offset ;

        conductance = (cell + 2 * layer_offset)->Bottom ;

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

SystemMatrix add_top_wall_column_mc_2rm
(
    Dimensions      *dimensions,
    ThermalCell     *thermal_cells,
    GridDimension_t  layer_index,
    GridDimension_t  row_index,
    GridDimension_t  column_index,
    SystemMatrix     system_matrix
)
{
    SystemMatrixValue_t  conductance      = SYSTEMMATRIXVALUE_I ;
    SystemMatrixValue_t  diagonal_value   = SYSTEMMATRIXVALUE_I ;
    SystemMatrixValue_t* diagonal_pointer = NULL ;

    GridDimension_t layer_offset = get_layer_area (dimensions) ;

    GridDimension_t cell_index = get_cell_offset_in_stack

        (dimensions, layer_index, row_index, column_index) ;

    ThermalCell *cell = thermal_cells + cell_index ;

#ifdef PRINT_SYSTEM_MATRIX
    fpos_t diag_fposition, last_fpos ;
    fprintf (stderr,
        "add_top_wall_column_2rm   l %2d r %4d c %4d [%7d]\n",
        layer_index, row_index, column_index, cell_index) ;
#endif

    *system_matrix.ColumnPointers = *(system_matrix.ColumnPointers - 1) ;

    /* BOTTOM connected to Virtual Wall */

    if ( ! IS_FIRST_LAYER(layer_index) )
    {
        *system_matrix.RowIndices++ = cell_index - 2 * layer_offset ;

        conductance = (cell - 2 * layer_offset)->Top ;

        *system_matrix.Values++  = -conductance ;
        diagonal_value          +=  conductance ;

        (*system_matrix.ColumnPointers)++ ;

#ifdef PRINT_SYSTEM_MATRIX
        fprintf (stderr,
           "  Bottom connected to Channel     \t%d\t% .4e\n",
            *(system_matrix.RowIndices-1), *(system_matrix.Values-1)) ;
#endif
    }

    /* BOTTOM connected to Channel */

    if ( ! IS_FIRST_LAYER(layer_index) )
    {
        *system_matrix.RowIndices++ = cell_index - layer_offset ;

        conductance = (cell - layer_offset)->Top ;

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

    *system_matrix.RowIndices++ = cell_index ;
    *system_matrix.Values       = cell->Capacity ;
    diagonal_pointer            = system_matrix.Values++ ;

    (*system_matrix.ColumnPointers)++ ;

#ifdef PRINT_SYSTEM_MATRIX
    fprintf (stderr, "  diagonal\t%d\t", *(system_matrix.RowIndices-1)) ;
    fgetpos (stderr, &diag_fposition) ;
    fprintf (stderr, "            ( + % .4e [capacity] )\n", *(system_matrix.Values-1)) ;
#endif

    /* Top connected to Silicon */

    if ( ! IS_LAST_LAYER(layer_index, dimensions) )
    {
        *system_matrix.RowIndices++ = cell_index + layer_offset ;

        conductance = (cell + layer_offset)->Bottom ;

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

SystemMatrix add_virtual_wall_column_mc_2rm
(
    Dimensions      *dimensions,
    ThermalCell     *thermal_cells,
    GridDimension_t  layer_index,
    GridDimension_t  row_index,
    GridDimension_t  column_index,
    SystemMatrix     system_matrix
)
{
    SystemMatrixValue_t  conductance      = SYSTEMMATRIXVALUE_I ;
    SystemMatrixValue_t  diagonal_value   = SYSTEMMATRIXVALUE_I ;
    SystemMatrixValue_t* diagonal_pointer = NULL ;

    GridDimension_t layer_offset = get_layer_area (dimensions) ;
    GridDimension_t row_offset   = get_number_of_columns (dimensions) ;

    GridDimension_t cell_index = get_cell_offset_in_stack

        (dimensions, layer_index, row_index, column_index) ;

    ThermalCell *cell = thermal_cells + cell_index ;

#ifdef PRINT_SYSTEM_MATRIX
    fpos_t diag_fposition, last_fpos ;
    fprintf (stderr,
        "add_virtual_wall_column   l %2d r %4d c %4d [%7d]\n",
        layer_index, row_index, column_index, cell_index) ;
#endif

    *system_matrix.ColumnPointers = *(system_matrix.ColumnPointers - 1) ;

    /* BOTTOM */

    if ( ! IS_FIRST_LAYER(layer_index) )
    {
        *system_matrix.RowIndices++ = cell_index - 2 * layer_offset ;

        conductance = cell->Bottom;

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

    if ( ! IS_FIRST_ROW(row_index) )
    {
        *system_matrix.RowIndices++ = cell_index - row_offset ;

        conductance = PARALLEL
                    (
                        cell->South,
                        thermal_cells [cell_index - row_offset].North
                    ) ;

        *system_matrix.Values++  = -conductance ;
        diagonal_value          +=  conductance ;

        (*system_matrix.ColumnPointers)++ ;

#ifdef PRINT_SYSTEM_MATRIX
        fprintf (stderr,
        "  south   \t%d\t% .4e\n", *(system_matrix.RowIndices-1), *(system_matrix.Values-1)) ;
#endif
    }

    /* DIAGONAL */

    *system_matrix.RowIndices++ = cell_index ;
    *system_matrix.Values       = cell->Capacity ;
    diagonal_pointer            = system_matrix.Values++ ;

    (*system_matrix.ColumnPointers)++ ;

#ifdef PRINT_SYSTEM_MATRIX
    fprintf (stderr, "  diagonal\t%d\t", *(system_matrix.RowIndices-1)) ;
    fgetpos (stderr, &diag_fposition) ;
    fprintf (stderr, "            ( + % .4e [capacity] )\n", *(system_matrix.Values-1)) ;
#endif

    /* NORTH */

    if ( ! IS_LAST_ROW(row_index, dimensions) )
    {
        *system_matrix.RowIndices++ = cell_index + row_offset ;

        conductance = PARALLEL
                    (
                        cell->North,
                        thermal_cells [cell_index + row_offset].South
                    ) ;

        *system_matrix.Values++  = -conductance ;
        diagonal_value          +=  conductance ;

        (*system_matrix.ColumnPointers)++ ;

#ifdef PRINT_SYSTEM_MATRIX
        fprintf (stderr,
            "  north   \t%d\t% .4e\n",
            *(system_matrix.RowIndices-1), *(system_matrix.Values-1)) ;
#endif
    }

    /* TOP */

    if ( ! IS_LAST_LAYER(layer_index, dimensions) )
    {
        *system_matrix.RowIndices++ = cell_index + layer_offset ;

        conductance = cell->Top;

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
/******************************************************************************/

SystemMatrix add_liquid_column_pf
(
    Dimensions      *dimensions,
    ThermalCell     *thermal_cells,
    GridDimension_t  layer_index,
    GridDimension_t  row_index,
    GridDimension_t  column_index,
    SystemMatrix     system_matrix
)
{
    SystemMatrixValue_t  conductance      = SYSTEMMATRIXVALUE_I ;
    SystemMatrixValue_t  diagonal_value   = SYSTEMMATRIXVALUE_I ;
    SystemMatrixValue_t* diagonal_pointer = NULL ;

    GridDimension_t layer_offset = get_layer_area (dimensions) ;
    GridDimension_t row_offset   = get_number_of_columns (dimensions) ;

    GridDimension_t cell_index = get_cell_offset_in_stack

        (dimensions, layer_index, row_index, column_index) ;

    ThermalCell *cell = thermal_cells + cell_index ;

#ifdef PRINT_SYSTEM_MATRIX
    fpos_t diag_fposition, last_fpos ;
    fprintf (stderr,
        "add_liquid_column_2rm  l %2d r %4d c %4d [%7d]\n",
        layer_index, row_index, column_index, cell_index) ;
#endif

    *system_matrix.ColumnPointers = *(system_matrix.ColumnPointers - 1) ;

    /* BOTTOM */

    if ( ! IS_FIRST_LAYER(layer_index) )
    {
        *system_matrix.RowIndices++ = cell_index - 2 * layer_offset ;

        conductance = cell->Bottom;

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

    if ( ! IS_FIRST_ROW(row_index) )
    {
        *system_matrix.RowIndices++ = cell_index - row_offset ;
        *system_matrix.Values++     = cell->North ;

        (*system_matrix.ColumnPointers)++ ;

#ifdef PRINT_SYSTEM_MATRIX
        fprintf (stderr,
            "  south   \t%d\t% .4e\n",
            *(system_matrix.RowIndices-1), *(system_matrix.Values-1)) ;
#endif
    }

    /* DIAGONAL */

    *system_matrix.RowIndices++ = cell_index ;
    *system_matrix.Values       = cell->Capacity ;
    diagonal_pointer            = system_matrix.Values++ ;

    (*system_matrix.ColumnPointers)++ ;

#ifdef PRINT_SYSTEM_MATRIX
    fprintf (stderr, "  diagonal\t%d\t", *(system_matrix.RowIndices-1)) ;
    fgetpos (stderr, &diag_fposition) ;
    fprintf (stderr, "            ( + % .4e [capacity] )\n", *(system_matrix.Values-1)) ;
#endif

    /* NORTH */

    if ( ! IS_LAST_ROW(row_index, dimensions) )
    {
        *system_matrix.RowIndices++ = cell_index + row_offset ;
        *system_matrix.Values++     = cell->South ;

        (*system_matrix.ColumnPointers)++ ;

#ifdef PRINT_SYSTEM_MATRIX
        fprintf (stderr,
            "  north   \t%d\t% .4e\n",
            *(system_matrix.RowIndices-1), *(system_matrix.Values-1)) ;
#endif
    }

    /* TOP */

    if ( ! IS_LAST_LAYER(layer_index, dimensions) )
    {
        *system_matrix.RowIndices++ = cell_index + layer_offset ;

        conductance = cell->Top;

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

    if (IS_FIRST_ROW (row_index) || IS_LAST_ROW (row_index, dimensions))

        *diagonal_pointer += cell->North ; /* == (C) */

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

SystemMatrix add_bottom_wall_column_pf
(
    Dimensions      *dimensions,
    ThermalCell     *thermal_cells,
    GridDimension_t  layer_index,
    GridDimension_t  row_index,
    GridDimension_t  column_index,
    SystemMatrix     system_matrix
)
{
    SystemMatrixValue_t  conductance      = SYSTEMMATRIXVALUE_I ;
    SystemMatrixValue_t  diagonal_value   = SYSTEMMATRIXVALUE_I ;
    SystemMatrixValue_t* diagonal_pointer = NULL ;

    GridDimension_t layer_offset = get_layer_area (dimensions) ;

    GridDimension_t cell_index = get_cell_offset_in_stack

        (dimensions, layer_index, row_index, column_index) ;

    ThermalCell *cell = thermal_cells + cell_index ;

#ifdef PRINT_SYSTEM_MATRIX
    fpos_t diag_fposition, last_fpos ;
    fprintf (stderr,
        "add_bottom_wall_column_2rm   l %2d r %4d c %4d [%7d]\n",
        layer_index, row_index, column_index, cell_index) ;
#endif

    *system_matrix.ColumnPointers = *(system_matrix.ColumnPointers - 1) ;

    /* BOTTOM connected to Silicon */

    if ( ! IS_FIRST_LAYER(layer_index) )
    {
        *system_matrix.RowIndices++ = cell_index - layer_offset ;

        conductance = (cell - layer_offset)->Top ;

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

    *system_matrix.RowIndices++ = cell_index ;
    *system_matrix.Values       = cell->Capacity ;
    diagonal_pointer            = system_matrix.Values++ ;

    (*system_matrix.ColumnPointers)++ ;

#ifdef PRINT_SYSTEM_MATRIX
    fprintf (stderr, "  diagonal\t%d\t", *(system_matrix.RowIndices-1)) ;
    fgetpos (stderr, &diag_fposition) ;
    fprintf (stderr, "            ( + % .4e [capacity] )\n", *(system_matrix.Values-1)) ;
#endif


    /* Top connected to Virtual Wall */

    if ( ! IS_LAST_LAYER(layer_index, dimensions) )
    {
        *system_matrix.RowIndices++ = cell_index + layer_offset ;

        conductance = (cell + layer_offset)->Bottom ;

        *system_matrix.Values++  = -conductance ;
        diagonal_value          +=  conductance ;

        (*system_matrix.ColumnPointers)++ ;

#ifdef PRINT_SYSTEM_MATRIX
        fprintf (stderr,
            "  Top connected to Channel  \t%d\t% .4e\n",
            *(system_matrix.RowIndices-1), *(system_matrix.Values-1)) ;
#endif
    }

    /* Top connected to Channel */

    if ( ! IS_LAST_LAYER(layer_index, dimensions) )
    {
        *system_matrix.RowIndices++ = cell_index + 2 * layer_offset ;

        conductance = (cell + 2 * layer_offset)->Bottom ;

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

SystemMatrix add_top_wall_column_pf
(
    Dimensions      *dimensions,
    ThermalCell     *thermal_cells,
    GridDimension_t  layer_index,
    GridDimension_t  row_index,
    GridDimension_t  column_index,
    SystemMatrix     system_matrix
)
{
    SystemMatrixValue_t  conductance      = SYSTEMMATRIXVALUE_I ;
    SystemMatrixValue_t  diagonal_value   = SYSTEMMATRIXVALUE_I ;
    SystemMatrixValue_t* diagonal_pointer = NULL ;

    GridDimension_t layer_offset = get_layer_area (dimensions) ;

    GridDimension_t cell_index = get_cell_offset_in_stack

        (dimensions, layer_index, row_index, column_index) ;

    ThermalCell *cell = thermal_cells + cell_index ;

#ifdef PRINT_SYSTEM_MATRIX
    fpos_t diag_fposition, last_fpos ;
    fprintf (stderr,
        "add_top_wall_column_2rm   l %2d r %4d c %4d [%7d]\n",
        layer_index, row_index, column_index, cell_index) ;
#endif

    *system_matrix.ColumnPointers = *(system_matrix.ColumnPointers - 1) ;

    /* BOTTOM connected to Virtual Wall */

    if ( ! IS_FIRST_LAYER(layer_index) )
    {
        *system_matrix.RowIndices++ = cell_index - 2 * layer_offset ;

        conductance = (cell - 2 * layer_offset)->Top ;

        *system_matrix.Values++  = -conductance ;
        diagonal_value          +=  conductance ;

        (*system_matrix.ColumnPointers)++ ;

#ifdef PRINT_SYSTEM_MATRIX
        fprintf (stderr,
            "  Bottom connected to Channel     \t%d\t% .4e\n",
            *(system_matrix.RowIndices-1), *(system_matrix.Values-1)) ;
#endif
    }

    /* BOTTOM connected to Channel */

    if ( ! IS_FIRST_LAYER(layer_index) )
    {
        *system_matrix.RowIndices++ = cell_index - layer_offset ;

        conductance = (cell - layer_offset)->Top ;

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

    *system_matrix.RowIndices++ = cell_index ;
    *system_matrix.Values       = cell->Capacity ;
    diagonal_pointer            = system_matrix.Values++ ;

    (*system_matrix.ColumnPointers)++ ;

#ifdef PRINT_SYSTEM_MATRIX
    fprintf (stderr, "  diagonal\t%d\t", *(system_matrix.RowIndices-1)) ;
    fgetpos (stderr, &diag_fposition) ;
    fprintf (stderr, "            ( + % .4e [capacity] )\n", *(system_matrix.Values-1)) ;
#endif

    /* Top connected to Silicon */

    if ( ! IS_LAST_LAYER(layer_index, dimensions) )
    {
        *system_matrix.RowIndices++ = cell_index + layer_offset ;

        conductance = (cell + layer_offset)->Bottom ;

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

SystemMatrix add_virtual_wall_column_pf
(
    Dimensions      *dimensions,
    ThermalCell     *thermal_cells,
    GridDimension_t  layer_index,
    GridDimension_t  row_index,
    GridDimension_t  column_index,
    SystemMatrix     system_matrix
)
{
    SystemMatrixValue_t  conductance      = SYSTEMMATRIXVALUE_I ;
    SystemMatrixValue_t  diagonal_value   = SYSTEMMATRIXVALUE_I ;
    SystemMatrixValue_t* diagonal_pointer = NULL ;

    GridDimension_t layer_offset = get_layer_area (dimensions) ;

    GridDimension_t cell_index = get_cell_offset_in_stack

        (dimensions, layer_index, row_index, column_index) ;

    ThermalCell *cell = thermal_cells + cell_index ;

#ifdef PRINT_SYSTEM_MATRIX
    fpos_t diag_fposition, last_fpos ;
    fprintf (stderr,
        "add_virtual_wall_column   l %2d r %4d c %4d [%7d]\n",
        layer_index, row_index, column_index, cell_index) ;
#endif

    *system_matrix.ColumnPointers = *(system_matrix.ColumnPointers - 1) ;

    /* BOTTOM */

    if ( ! IS_FIRST_LAYER(layer_index) )
    {
        *system_matrix.RowIndices++ = cell_index - layer_offset ;

        conductance = cell->Bottom;

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

    *system_matrix.RowIndices++ = cell_index ;
    *system_matrix.Values       = cell->Capacity ;
    diagonal_pointer            = system_matrix.Values++ ;

    (*system_matrix.ColumnPointers)++ ;

#ifdef PRINT_SYSTEM_MATRIX
    fprintf (stderr, "  diagonal\t%d\t", *(system_matrix.RowIndices-1)) ;
    fgetpos (stderr, &diag_fposition) ;
    fprintf (stderr, "            ( + % .4e [capacity] )\n", *(system_matrix.Values-1)) ;
#endif

    /* TOP */

    if ( ! IS_LAST_LAYER(layer_index, dimensions) )
    {
        *system_matrix.RowIndices++ = cell_index + 2 * layer_offset ;

        conductance = cell->Top;

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
