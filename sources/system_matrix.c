/******************************************************************************
 * This file is part of 3D-ICE, version 2.1 .                                 *
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
 *          Giseong Bak                                                       *
 *          Martino Ruggiero                                                  *
 *          Thomas Brunschwiler                                               *
 *          David Atienza                                                     *
 *                                                                            *
 * For any comment, suggestion or request  about 3D-ICE, please  register and *
 * write to the mailing list (see http://listes.epfl.ch/doc.cgi?liste=3d-ice) *
 * Any usage  of 3D-ICE  for research,  commercial or other  purposes must be *
 * properly acknowledged in the resulting products or publications.           *
 *                                                                            *
 * EPFL-STI-IEL-ESL                                                           *
 * Batiment ELG, ELG 130                Mail : 3d-ice@listes.epfl.ch          *
 * Station 11                                  (SUBSCRIPTION IS NECESSARY)    *
 * 1015 Lausanne, Switzerland           Url  : http://esl.epfl.ch/3d-ice.html *
 ******************************************************************************/

#include <stdlib.h>

#include "system_matrix.h"
#include "macros.h"

/******************************************************************************/

void init_system_matrix (SystemMatrix_t* this)
{
    this->ColumnPointers = NULL ;
    this->RowIndices     = NULL;
    this->Values         = NULL ;
    this->Size           = 0u ;
    this->NNz            = 0u ;
}

/******************************************************************************/

Error_t alloc_system_matrix
(
    SystemMatrix_t *this,
    CellIndex_t     size,
    CellIndex_t     nnz
)
{
    this->Size = size ;
    this->NNz  = nnz ;

    this->RowIndices = (CellIndex_t *) malloc (sizeof(CellIndex_t) * nnz) ;

    if (this->RowIndices == NULL)

        return TDICE_FAILURE ;

    this->ColumnPointers = (CellIndex_t *) malloc (sizeof(CellIndex_t) * (size + 1)) ;

    if (this->ColumnPointers == NULL)
    {
        FREE_POINTER (free, this->RowIndices) ;
        return TDICE_FAILURE ;
    }

    this->Values = (SystemMatrixCoeff_t *) malloc (sizeof(SystemMatrixCoeff_t) * nnz) ;

    if (this->Values == NULL)
    {
        FREE_POINTER (free, this->ColumnPointers) ;
        FREE_POINTER (free, this->RowIndices) ;
        return TDICE_FAILURE ;
    }

    return TDICE_SUCCESS ;
}

/******************************************************************************/

void free_system_matrix (SystemMatrix_t* this)
{
    FREE_POINTER (free, this->ColumnPointers) ;
    FREE_POINTER (free, this->RowIndices) ;
    FREE_POINTER (free, this->Values) ;
}

/******************************************************************************/

static SystemMatrix_t add_solid_column
(
    SystemMatrix_t  this,
    Dimensions_t   *dimensions,
    ThermalGrid_t  *thermal_grid,
    CellIndex_t     layer_index,
    CellIndex_t     row_index,
    CellIndex_t     column_index
)
{
    Conductance_t conductance = 0.0 ;
    SystemMatrixCoeff_t  diagonal_value   = 0.0 ;
    SystemMatrixCoeff_t *diagonal_pointer = NULL ;

#ifdef PRINT_SYSTEM_MATRIX
    fpos_t diag_fposition, last_fpos ;
    fprintf (stderr,
        "add_solid_column   l %2d r %4d c %4d [%7d]\n",
        layer_index, row_index, column_index,
        get_cell_offset_in_stack (dimensions, layer_index, row_index, column_index)) ;
#endif

    *this.ColumnPointers = *(this.ColumnPointers - 1) ;

    /* BOTTOM */

    if ( ! IS_FIRST_LAYER(layer_index) )
    {
        *this.RowIndices++ = get_cell_offset_in_stack

            (dimensions, layer_index - 1, row_index, column_index) ;

        Conductance_t c_bottom = get_conductance_bottom

            (thermal_grid, dimensions, layer_index, row_index, column_index) ;

        Conductance_t c_top = get_conductance_top

            (thermal_grid, dimensions, layer_index - 1, row_index, column_index) ;

        conductance = PARALLEL (c_bottom, c_top) ;

        if (conductance == 0)
        {
            conductance = c_bottom ? c_bottom : c_top ;
        }

        *this.Values++  = -conductance ;
        diagonal_value          +=  conductance ;

        (*this.ColumnPointers)++ ;

#ifdef PRINT_SYSTEM_MATRIX
        fprintf (stderr,
            "  bottom  \t%d\t% .4e = % .4e (B) || % .4e (T)\n",
            *(this.RowIndices-1), *(this.Values-1),
            c_bottom, c_top) ;
#endif
    }

    /* SOUTH */

    if ( ! IS_FIRST_ROW(row_index) )
    {
        *this.RowIndices++ = get_cell_offset_in_stack

            (dimensions, layer_index, row_index - 1, column_index) ;

        conductance = PARALLEL
        (
            get_conductance_south (thermal_grid, dimensions, layer_index, row_index    , column_index),
            get_conductance_north (thermal_grid, dimensions, layer_index, row_index - 1, column_index)
        ) ;

        *this.Values++  = -conductance ;
        diagonal_value          +=  conductance ;

        (*this.ColumnPointers)++ ;

#ifdef PRINT_SYSTEM_MATRIX
        fprintf (stderr,
            "  south   \t%d\t% .4e = % .4e (S) || % .4e (N)\n",
            *(this.RowIndices-1), *(this.Values-1),
            get_conductance_south (thermal_grid, dimensions, layer_index, row_index    , column_index),
            get_conductance_north (thermal_grid, dimensions, layer_index, row_index - 1, column_index)) ;
#endif
    }

    /* WEST */

    if ( ! IS_FIRST_COLUMN(column_index) )
    {
        *this.RowIndices++ = get_cell_offset_in_stack

            (dimensions, layer_index, row_index, column_index - 1) ;

        conductance = PARALLEL
        (
            get_conductance_west (thermal_grid, dimensions, layer_index, row_index, column_index    ),
            get_conductance_east (thermal_grid, dimensions, layer_index, row_index, column_index - 1)
        ) ;

        *this.Values++  = -conductance ;
        diagonal_value          +=  conductance ;

        (*this.ColumnPointers)++ ;

#ifdef PRINT_SYSTEM_MATRIX
        fprintf (stderr,
            "  west    \t%d\t% .4e = % .4e (W) || % .4e (E)\n",
            *(this.RowIndices-1), *(this.Values-1),
            get_conductance_west (thermal_grid, dimensions, layer_index, row_index, column_index    ),
            get_conductance_east (thermal_grid, dimensions, layer_index, row_index, column_index - 1)) ;
#endif
    }

    /* DIAGONAL */

    *this.RowIndices++ = get_cell_offset_in_stack

        (dimensions, layer_index, row_index, column_index) ;

    *this.Values = get_capacity

        (thermal_grid, dimensions, layer_index, row_index, column_index) ;

    diagonal_pointer = this.Values++ ;

    (*this.ColumnPointers)++ ;

#ifdef PRINT_SYSTEM_MATRIX
    fprintf (stderr, "  diagonal\t%d\t", *(this.RowIndices-1)) ;
    fgetpos (stderr, &diag_fposition) ;
    fprintf (stderr, "            ( + % .4e [capacity] )\n", *(this.Values-1)) ;
#endif

    /* EAST */

    if ( ! IS_LAST_COLUMN(column_index, dimensions) )
    {
        *this.RowIndices++ = get_cell_offset_in_stack

            (dimensions, layer_index, row_index, column_index + 1) ;

        conductance = PARALLEL
        (
            get_conductance_east (thermal_grid, dimensions, layer_index, row_index, column_index    ),
            get_conductance_west (thermal_grid, dimensions, layer_index, row_index, column_index + 1)
        ) ;

        *this.Values++  = -conductance ;
        diagonal_value          +=  conductance ;

        (*this.ColumnPointers)++ ;

#ifdef PRINT_SYSTEM_MATRIX
        fprintf (stderr,
            "  east    \t%d\t% .4e = % .4e (E) || % .4e (W)\n",
            *(this.RowIndices-1), *(this.Values-1),
            get_conductance_east (thermal_grid, dimensions, layer_index, row_index, column_index    ),
            get_conductance_west (thermal_grid, dimensions, layer_index, row_index, column_index + 1)) ;
#endif
    }

    /* NORTH */

    if ( ! IS_LAST_ROW(row_index, dimensions) )
    {
        *this.RowIndices++ = get_cell_offset_in_stack

            (dimensions, layer_index, row_index + 1, column_index) ;

        conductance = PARALLEL
        (
            get_conductance_north (thermal_grid, dimensions, layer_index, row_index    , column_index),
            get_conductance_south (thermal_grid, dimensions, layer_index, row_index + 1, column_index)
        ) ;

        *this.Values++  = -conductance ;
        diagonal_value          +=  conductance ;

        (*this.ColumnPointers)++ ;

#ifdef PRINT_SYSTEM_MATRIX
        fprintf (stderr,
            "  north   \t%d\t% .4e = % .4e (N) || % .4e (S)\n",
            *(this.RowIndices-1), *(this.Values-1),
            get_conductance_north (thermal_grid, dimensions, layer_index, row_index    , column_index),
            get_conductance_south (thermal_grid, dimensions, layer_index, row_index + 1, column_index)) ;
#endif
    }

    /* TOP */

    if ( ! IS_LAST_LAYER(layer_index, dimensions) )
    {
        *this.RowIndices++ = get_cell_offset_in_stack

            (dimensions, layer_index + 1, row_index, column_index) ;

        Conductance_t c_top = get_conductance_top

            (thermal_grid, dimensions, layer_index, row_index, column_index) ;

        Conductance_t c_bottom = get_conductance_bottom

            (thermal_grid, dimensions, layer_index + 1, row_index, column_index) ;

        conductance = PARALLEL (c_top, c_bottom) ;

        if (conductance == 0)
        {
            conductance = c_top ? c_top : c_bottom ;
        }

        *this.Values++  = -conductance ;
        diagonal_value          +=  conductance ;

        (*this.ColumnPointers)++ ;

#ifdef PRINT_SYSTEM_MATRIX
        fprintf (stderr,
            "  top     \t%d\t% .4e = % .4e (T) || % .4e (B)\n",
            *(this.RowIndices-1), *(this.Values-1),
            c_top, c_bottom) ;
#endif
    }

    /* DIAGONAL ELEMENT */

    *diagonal_pointer += diagonal_value ;

#ifdef PRINT_SYSTEM_MATRIX
    fgetpos (stderr, &last_fpos) ;
    fsetpos (stderr, &diag_fposition) ;
    fprintf (stderr, "% .4e", *diagonal_pointer) ;
    fsetpos (stderr, &last_fpos) ;

    fprintf (stderr, "  %d\n", *this.ColumnPointers) ;
#endif

    this.ColumnPointers++ ;

    return this ;
}

/******************************************************************************/

static SystemMatrix_t add_spread_column
(
    SystemMatrix_t  this,
    Dimensions_t   *dimensions,
    ThermalGrid_t  *thermal_grid,
    CellIndex_t     layer_index,
    CellIndex_t     row_index,
    CellIndex_t     column_index
)
{
    Conductance_t conductance = 0.0 ;
    SystemMatrixCoeff_t  diagonal_value   = 0.0 ;
    SystemMatrixCoeff_t *diagonal_pointer = NULL ;

#ifdef PRINT_SYSTEM_MATRIX
    fpos_t diag_fposition, last_fpos ;
    fprintf (stderr,
        "add_spread_column   l %2d r %4d c %4d [%7d]\n",
        layer_index, row_index, column_index,
        get_cell_offset_in_stack (dimensions, layer_index, row_index, column_index)) ;
#endif

    *this.ColumnPointers = *(this.ColumnPointers - 1) ;

    /* BOTTOM */

    if ( ! IS_FIRST_LAYER(layer_index) )
    {
        *this.RowIndices++ = get_cell_offset_in_stack

            (dimensions, layer_index - 1, row_index, column_index) ;

        conductance = PARALLEL
        (
            get_conductance_bottom (thermal_grid, dimensions, layer_index    , row_index, column_index),
            get_conductance_top    (thermal_grid, dimensions, layer_index - 1, row_index, column_index)
        ) ;

        *this.Values++  = -conductance ;
        diagonal_value          +=  conductance ;

        (*this.ColumnPointers)++ ;

#ifdef PRINT_SYSTEM_MATRIX
        fprintf (stderr,
            "  bottom  \t%d\t% .4e = % .4e (B) || % .4e (T)\n",
            *(this.RowIndices-1), *(this.Values-1),
            get_conductance_bottom (thermal_grid, dimensions, layer_index    , row_index, column_index),
            get_conductance_top    (thermal_grid, dimensions, layer_index - 1, row_index, column_index)) ;
#endif
    }

    /* DIAGONAL */

    *this.RowIndices++ = get_cell_offset_in_stack

        (dimensions, layer_index, row_index, column_index) ;

    *this.Values = get_capacity

        (thermal_grid, dimensions, layer_index, row_index, column_index) ;

    diagonal_pointer = this.Values++ ;

    (*this.ColumnPointers)++ ;

#ifdef PRINT_SYSTEM_MATRIX
    fprintf (stderr, "  diagonal\t%d\t", *(this.RowIndices-1)) ;
    fgetpos (stderr, &diag_fposition) ;
    fprintf (stderr, "            ( + % .4e [capacity] )\n", *(this.Values-1)) ;
#endif

    /* TOP */

    if ( ! IS_LAST_LAYER(layer_index, dimensions) )
    {
        *this.RowIndices++ = get_cell_offset_in_stack

            (dimensions, layer_index + 1, row_index, column_index) ;

        conductance = PARALLEL
        (
            get_conductance_top    (thermal_grid, dimensions, layer_index    , row_index, column_index),
            get_conductance_bottom (thermal_grid, dimensions, layer_index + 1, row_index, column_index)
        ) ;

        *this.Values++  = -conductance ;
        diagonal_value          +=  conductance ;

        (*this.ColumnPointers)++ ;

#ifdef PRINT_SYSTEM_MATRIX
        fprintf (stderr,
            "  top     \t%d\t% .4e = % .4e (T) || % .4e (B)\n",
            *(this.RowIndices-1), *(this.Values-1),
            get_conductance_top    (thermal_grid, dimensions, layer_index    , row_index, column_index),
            get_conductance_bottom (thermal_grid, dimensions, layer_index + 1, row_index, column_index)) ;
#endif
    }

    /* DIAGONAL ELEMENT */

    *diagonal_pointer += diagonal_value ;

#ifdef PRINT_SYSTEM_MATRIX
    fgetpos (stderr, &last_fpos) ;
    fsetpos (stderr, &diag_fposition) ;
    fprintf (stderr, "% .4e", *diagonal_pointer) ;
    fsetpos (stderr, &last_fpos) ;

    fprintf (stderr, "  %d\n", *this.ColumnPointers) ;
#endif

    this.ColumnPointers++ ;

    return this ;
}

/******************************************************************************/

static SystemMatrix_t add_sink_column
(
    SystemMatrix_t  this,
    Dimensions_t   *dimensions,
    ThermalGrid_t  *thermal_grid,
    CellIndex_t     layer_index,
    CellIndex_t     row_index,
    CellIndex_t     column_index
)
{
    Conductance_t conductance = 0.0 ;
    SystemMatrixCoeff_t  diagonal_value   = 0.0 ;
    SystemMatrixCoeff_t *diagonal_pointer = NULL ;

#ifdef PRINT_SYSTEM_MATRIX
    fpos_t diag_fposition, last_fpos ;
    fprintf (stderr,
        "add_spread_column   l %2d r %4d c %4d [%7d]\n",
        layer_index, row_index, column_index,
        get_cell_offset_in_stack (dimensions, layer_index, row_index, column_index)) ;
#endif

    *this.ColumnPointers = *(this.ColumnPointers - 1) ;

    /* BOTTOM */

    if ( ! IS_FIRST_LAYER(layer_index) )
    {
        *this.RowIndices++ = get_cell_offset_in_stack

            (dimensions, layer_index - 1, row_index, column_index) ;

        conductance = PARALLEL
        (
            get_conductance_bottom (thermal_grid, dimensions, layer_index    , row_index, column_index),
            get_conductance_top    (thermal_grid, dimensions, layer_index - 1, row_index, column_index)
        ) ;

        *this.Values++  = -conductance ;
        diagonal_value          +=  conductance ;

        (*this.ColumnPointers)++ ;

#ifdef PRINT_SYSTEM_MATRIX
        fprintf (stderr,
            "  bottom  \t%d\t% .4e = % .4e (B) || % .4e (T)\n",
            *(this.RowIndices-1), *(this.Values-1),
            get_conductance_bottom (thermal_grid, dimensions, layer_index    , row_index, column_index),
            get_conductance_top    (thermal_grid, dimensions, layer_index - 1, row_index, column_index)) ;
#endif
    }

    /* DIAGONAL */

    *this.RowIndices++ = get_cell_offset_in_stack

        (dimensions, layer_index, row_index, column_index) ;

    *this.Values = get_capacity

        (thermal_grid, dimensions, layer_index, row_index, column_index) ;

    diagonal_pointer = this.Values++ ;

    (*this.ColumnPointers)++ ;

#ifdef PRINT_SYSTEM_MATRIX
    fprintf (stderr, "  diagonal\t%d\t", *(this.RowIndices-1)) ;
    fgetpos (stderr, &diag_fposition) ;
    fprintf (stderr, "            ( + % .4e [capacity] )\n", *(this.Values-1)) ;
#endif

    /* TOP */

    if ( ! IS_LAST_LAYER(layer_index, dimensions) )
    {
        *this.RowIndices++ = get_cell_offset_in_stack

            (dimensions, layer_index + 1, row_index, column_index) ;

        conductance = PARALLEL
        (
            get_conductance_top    (thermal_grid, dimensions, layer_index    , row_index, column_index),
            get_conductance_bottom (thermal_grid, dimensions, layer_index + 1, row_index, column_index)
        ) ;

        *this.Values++  = -conductance ;
        diagonal_value          +=  conductance ;

        (*this.ColumnPointers)++ ;

#ifdef PRINT_SYSTEM_MATRIX
        fprintf (stderr,
            "  top     \t%d\t% .4e = % .4e (T) || % .4e (B)\n",
            *(this.RowIndices-1), *(this.Values-1),
            get_conductance_top    (thermal_grid, dimensions, layer_index    , row_index, column_index),
            get_conductance_bottom (thermal_grid, dimensions, layer_index + 1, row_index, column_index)) ;
#endif
    }

    /* DIAGONAL ELEMENT */

    *diagonal_pointer += diagonal_value ;

#ifdef PRINT_SYSTEM_MATRIX
    fgetpos (stderr, &last_fpos) ;
    fsetpos (stderr, &diag_fposition) ;
    fprintf (stderr, "% .4e", *diagonal_pointer) ;
    fsetpos (stderr, &last_fpos) ;

    fprintf (stderr, "  %d\n", *this.ColumnPointers) ;
#endif

    this.ColumnPointers++ ;

    return this ;
}

/******************************************************************************/

static SystemMatrix_t add_liquid_column_4rm
(
    SystemMatrix_t  this,
    Dimensions_t   *dimensions,
    ThermalGrid_t  *thermal_grid,
    CellIndex_t     layer_index,
    CellIndex_t     row_index,
    CellIndex_t     column_index
)
{
    Conductance_t conductance = 0.0 ;
    SystemMatrixCoeff_t  diagonal_value   = 0.0 ;
    SystemMatrixCoeff_t *diagonal_pointer = NULL ;

#ifdef PRINT_SYSTEM_MATRIX
    fpos_t diag_fposition, last_fpos ;
    fprintf (stderr,
        "add_liquid_column_4rm  l %2d r %4d c %4d [%7d]\n",
        layer_index, row_index, column_index,
        get_cell_offset_in_stack (dimensions, layer_index, row_index, column_index)) ;
#endif

    *this.ColumnPointers = *(this.ColumnPointers - 1) ;

    /* BOTTOM */

    if ( ! IS_FIRST_LAYER(layer_index) )
    {
        *this.RowIndices++ = get_cell_offset_in_stack

            (dimensions, layer_index - 1, row_index, column_index) ;

        conductance = PARALLEL
        (
            get_conductance_bottom (thermal_grid, dimensions, layer_index    , row_index, column_index),
            get_conductance_top    (thermal_grid, dimensions, layer_index - 1, row_index, column_index)
        ) ;

        *this.Values++  = -conductance ;
        diagonal_value          +=  conductance ;

        (*this.ColumnPointers)++ ;

#ifdef PRINT_SYSTEM_MATRIX
        fprintf (stderr,
            "  bottom  \t%d\t% .4e = % .4e (B) || % .4e (T)\n",
            *(this.RowIndices-1), *(this.Values-1),
            get_conductance_bottom (thermal_grid, dimensions, layer_index    , row_index, column_index),
            get_conductance_top    (thermal_grid, dimensions, layer_index - 1, row_index, column_index)) ;
#endif
    }

    /* SOUTH */

    if ( ! IS_FIRST_ROW(row_index) )
    {
        *this.RowIndices++ = get_cell_offset_in_stack

            (dimensions, layer_index, row_index - 1, column_index) ;

        *this.Values++ = get_conductance_north /* == C */

            (thermal_grid, dimensions, layer_index, row_index, column_index) ;

        (*this.ColumnPointers)++ ;

#ifdef PRINT_SYSTEM_MATRIX
        fprintf (stderr,
            "  south   \t%d\t% .4e (N)\n",
            *(this.RowIndices-1), *(this.Values-1)) ;
#endif
    }

    /* WEST */

    if ( ! IS_FIRST_COLUMN(column_index) )
    {
        *this.RowIndices++ = get_cell_offset_in_stack

            (dimensions, layer_index, row_index, column_index - 1) ;

        conductance = PARALLEL
        (
            get_conductance_west (thermal_grid, dimensions, layer_index, row_index, column_index    ),
            get_conductance_east (thermal_grid, dimensions, layer_index, row_index, column_index - 1)
        ) ;

        *this.Values++  = -conductance ;
        diagonal_value          +=  conductance ;

        (*this.ColumnPointers)++ ;

#ifdef PRINT_SYSTEM_MATRIX
        fprintf (stderr,
            "  west    \t%d\t% .4e = % .4e (W) || % .4e (E)\n",
            *(this.RowIndices-1), *(this.Values-1),
            get_conductance_west (thermal_grid, dimensions, layer_index, row_index, column_index    ),
            get_conductance_east (thermal_grid, dimensions, layer_index, row_index, column_index - 1));
#endif
    }

    /* DIAGONAL */

    *this.RowIndices++ = get_cell_offset_in_stack

        (dimensions, layer_index, row_index, column_index) ;

    *this.Values = get_capacity

        (thermal_grid, dimensions, layer_index, row_index, column_index) ;

    diagonal_pointer = this.Values++ ;

    (*this.ColumnPointers)++ ;

#ifdef PRINT_SYSTEM_MATRIX
    fprintf (stderr, "  diagonal\t%d\t", *(this.RowIndices-1)) ;
    fgetpos (stderr, &diag_fposition) ;
    fprintf (stderr, "            ( + % .4e [capacity] )\n", *(this.Values-1)) ;
#endif

    /* EAST */

    if ( ! IS_LAST_COLUMN(column_index, dimensions) )
    {
        *this.RowIndices++ = get_cell_offset_in_stack

            (dimensions, layer_index, row_index, column_index + 1) ;

        conductance = PARALLEL
        (
            get_conductance_east (thermal_grid, dimensions, layer_index, row_index, column_index    ),
            get_conductance_west (thermal_grid, dimensions, layer_index, row_index, column_index + 1)
        ) ;

        *this.Values++  = -conductance ;
        diagonal_value          +=  conductance ;

        (*this.ColumnPointers)++ ;

#ifdef PRINT_SYSTEM_MATRIX
        fprintf (stderr,
            "  east    \t%d\t% .4e = % .4e (E) || % .4e (W)\n",
            *(this.RowIndices-1), *(this.Values-1),
            get_conductance_east (thermal_grid, dimensions, layer_index, row_index, column_index    ),
            get_conductance_west (thermal_grid, dimensions, layer_index, row_index, column_index + 1));
#endif
    }

    /* NORTH */

    if ( ! IS_LAST_ROW(row_index, dimensions) )
    {
        *this.RowIndices++ = get_cell_offset_in_stack

            (dimensions, layer_index, row_index + 1, column_index) ;

        *this.Values++ = get_conductance_south /* == - C */

            (thermal_grid, dimensions, layer_index, row_index, column_index) ;

        (*this.ColumnPointers)++ ;

#ifdef PRINT_SYSTEM_MATRIX
        fprintf (stderr,
            "  north   \t%d\t% .4e (S)\n",
            *(this.RowIndices-1), *(this.Values-1)) ;
#endif
    }

    /* TOP */

    if ( ! IS_LAST_LAYER(layer_index, dimensions) )
    {
        *this.RowIndices++ = get_cell_offset_in_stack

            (dimensions, layer_index + 1, row_index, column_index) ;

        conductance = PARALLEL
        (
            get_conductance_top    (thermal_grid, dimensions, layer_index    , row_index, column_index),
            get_conductance_bottom (thermal_grid, dimensions, layer_index + 1, row_index, column_index)
        ) ;

        *this.Values++  = -conductance ;
        diagonal_value          +=  conductance ;

        (*this.ColumnPointers)++ ;

#ifdef PRINT_SYSTEM_MATRIX
        fprintf (stderr,
            "  top     \t%d\t% .4e = % .4e (T) || % .4e (B)\n",
            *(this.RowIndices-1), *(this.Values-1),
            get_conductance_top    (thermal_grid, dimensions, layer_index    , row_index, column_index),
            get_conductance_bottom (thermal_grid, dimensions, layer_index + 1, row_index, column_index)) ;
#endif
    }

    /* DIAGONAL ELEMENT */

    *diagonal_pointer += diagonal_value ;

    if (IS_FIRST_ROW(row_index) || IS_LAST_ROW(row_index, dimensions))

        *diagonal_pointer += get_conductance_north /* == C */

            (thermal_grid, dimensions, layer_index, row_index, column_index) ;

#ifdef PRINT_SYSTEM_MATRIX
    fgetpos (stderr, &last_fpos) ;
    fsetpos (stderr, &diag_fposition) ;
    fprintf (stderr, "% .4e", *diagonal_pointer) ;
    fsetpos (stderr, &last_fpos) ;

    fprintf (stderr, "  %d\n", *this.ColumnPointers) ;
#endif

    this.ColumnPointers++ ;

    return this ;
}

/******************************************************************************/

static SystemMatrix_t add_liquid_column_2rm
(
    SystemMatrix_t  this,
    Dimensions_t   *dimensions,
    ThermalGrid_t  *thermal_grid,
    CellIndex_t     layer_index,
    CellIndex_t     row_index,
    CellIndex_t     column_index
)
{
    Conductance_t conductance = 0.0 ;
    SystemMatrixCoeff_t  diagonal_value   = 0.0 ;
    SystemMatrixCoeff_t *diagonal_pointer = NULL ;

#ifdef PRINT_SYSTEM_MATRIX
    fpos_t diag_fposition, last_fpos ;
    fprintf (stderr,
        "add_liquid_column_2rm  l %2d r %4d c %4d [%7d]\n",
        layer_index, row_index, column_index,
        get_cell_offset_in_stack (dimensions, layer_index, row_index, column_index)) ;
#endif

    *this.ColumnPointers = *(this.ColumnPointers - 1) ;

    /* BOTTOM */

    if ( ! IS_FIRST_LAYER(layer_index) )  // FIXME
    {
        *this.RowIndices++ = get_cell_offset_in_stack

            (dimensions, layer_index - 2, row_index, column_index) ;

        conductance = get_conductance_bottom

            (thermal_grid, dimensions, layer_index, row_index, column_index) ;

        *this.Values++  = -conductance ;
        diagonal_value          +=  conductance ;

        (*this.ColumnPointers)++ ;

#ifdef PRINT_SYSTEM_MATRIX
        fprintf (stderr,
            "  bottom  \t%d\t% .4e (B)\n",
            *(this.RowIndices-1), *(this.Values-1)) ;
#endif
    }

    /* SOUTH */

    if ( ! IS_FIRST_ROW(row_index) )
    {
        *this.RowIndices++ = get_cell_offset_in_stack

            (dimensions, layer_index, row_index - 1, column_index) ;

        *this.Values++ = get_conductance_north /* == C */

            (thermal_grid, dimensions, layer_index, row_index, column_index) ;

        (*this.ColumnPointers)++ ;

#ifdef PRINT_SYSTEM_MATRIX
        fprintf (stderr,
            "  south   \t%d\t% .4e (N)\n",
            *(this.RowIndices-1), *(this.Values-1)) ;
#endif
    }

    /* DIAGONAL */

    *this.RowIndices++ = get_cell_offset_in_stack

        (dimensions, layer_index, row_index, column_index) ;

    *this.Values = get_capacity

        (thermal_grid, dimensions, layer_index, row_index, column_index) ;

    diagonal_pointer = this.Values++ ;

    (*this.ColumnPointers)++ ;

#ifdef PRINT_SYSTEM_MATRIX
    fprintf (stderr, "  diagonal\t%d\t", *(this.RowIndices-1)) ;
    fgetpos (stderr, &diag_fposition) ;
    fprintf (stderr, "            ( + % .4e [capacity] )\n", *(this.Values-1)) ;
#endif

    /* NORTH */

    if ( ! IS_LAST_ROW(row_index, dimensions) )
    {
        *this.RowIndices++ = get_cell_offset_in_stack

            (dimensions, layer_index, row_index + 1, column_index) ;

        *this.Values++ = get_conductance_south /* == -C */

            (thermal_grid, dimensions, layer_index, row_index, column_index) ;

        (*this.ColumnPointers)++ ;

#ifdef PRINT_SYSTEM_MATRIX
        fprintf (stderr,
            "  north   \t%d\t% .4e (S)\n",
            *(this.RowIndices-1), *(this.Values-1)) ;
#endif
    }

    /* TOP */

    if ( ! IS_LAST_LAYER(layer_index, dimensions) )
    {
        *this.RowIndices++ = get_cell_offset_in_stack

            (dimensions, layer_index + 1, row_index, column_index) ;

        conductance = get_conductance_top

            (thermal_grid, dimensions, layer_index, row_index, column_index) ;

        *this.Values++  = -conductance ;
        diagonal_value          +=  conductance ;

        (*this.ColumnPointers)++ ;

#ifdef PRINT_SYSTEM_MATRIX
        fprintf (stderr,
            "  top     \t%d\t% .4e (T)\n",
            *(this.RowIndices-1), *(this.Values-1)) ;
#endif
    }

    /* DIAGONAL ELEMENT */

    *diagonal_pointer += diagonal_value ;

    if (IS_FIRST_ROW (row_index) || IS_LAST_ROW (row_index, dimensions))

        *diagonal_pointer += get_conductance_north /* == C */

            (thermal_grid, dimensions, layer_index, row_index, column_index) ;

#ifdef PRINT_SYSTEM_MATRIX
    fgetpos (stderr, &last_fpos) ;
    fsetpos (stderr, &diag_fposition) ;
    fprintf (stderr, "% .4e", *diagonal_pointer) ;
    fsetpos (stderr, &last_fpos) ;

    fprintf (stderr, "  %d\n", *this.ColumnPointers) ;
#endif

    this.ColumnPointers++ ;

    return this ;
}

/******************************************************************************/

static SystemMatrix_t add_bottom_wall_column_2rm
(
    SystemMatrix_t  this,
    Dimensions_t   *dimensions,
    ThermalGrid_t  *thermal_grid,
    CellIndex_t     layer_index,
    CellIndex_t     row_index,
    CellIndex_t     column_index
)
{
    Conductance_t conductance = 0.0 ;
    SystemMatrixCoeff_t  diagonal_value   = 0.0 ;
    SystemMatrixCoeff_t *diagonal_pointer = NULL ;

#ifdef PRINT_SYSTEM_MATRIX
    fpos_t diag_fposition, last_fpos ;
    fprintf (stderr,
        "add_bottom_wall_column_2rm   l %2d r %4d c %4d [%7d]\n",
        layer_index, row_index, column_index,
        get_cell_offset_in_stack (dimensions, layer_index, row_index, column_index)) ;
#endif

    *this.ColumnPointers = *(this.ColumnPointers - 1) ;

    /* BOTTOM connected to Silicon */

    if ( ! IS_FIRST_LAYER(layer_index) )
    {
        *this.RowIndices++ = get_cell_offset_in_stack

            (dimensions, layer_index - 1, row_index, column_index) ;

        conductance = get_conductance_top

            (thermal_grid, dimensions, layer_index - 1, row_index, column_index) ;

        *this.Values++  = -conductance ;
        diagonal_value          +=  conductance ;

        (*this.ColumnPointers)++ ;

#ifdef PRINT_SYSTEM_MATRIX
        fprintf (stderr,
            "  Bottom connected to Silicon     \t%d\t% .4e\n",
            *(this.RowIndices-1), *(this.Values-1)) ;
#endif
    }

    /* DIAGONAL */

    *this.RowIndices++ = get_cell_offset_in_stack

        (dimensions, layer_index, row_index, column_index) ;

    *this.Values = get_capacity

        (thermal_grid, dimensions, layer_index, row_index, column_index) ;

    diagonal_pointer = this.Values++ ;

    (*this.ColumnPointers)++ ;

#ifdef PRINT_SYSTEM_MATRIX
    fprintf (stderr, "  diagonal\t%d\t", *(this.RowIndices-1)) ;
    fgetpos (stderr, &diag_fposition) ;
    fprintf (stderr, "            ( + % .4e [capacity] )\n", *(this.Values-1)) ;
#endif

    /* Top connected to Virtual Wall */

    if ( ! IS_LAST_LAYER(layer_index, dimensions) )
    {
        *this.RowIndices++ = get_cell_offset_in_stack

            (dimensions, layer_index + 1, row_index, column_index) ;

        conductance = get_conductance_top

            (thermal_grid, dimensions, layer_index + 1, row_index, column_index) ;

        *this.Values++  = -conductance ;
        diagonal_value          +=  conductance ;

        (*this.ColumnPointers)++ ;

#ifdef PRINT_SYSTEM_MATRIX
        fprintf (stderr,
            "  Top connected to Channel  \t%d\t% .4e\n",
            *(this.RowIndices-1), *(this.Values-1)) ;
#endif
    }

    /* Top connected to Channel */

    if ( ! IS_LAST_LAYER(layer_index, dimensions) )
    {
        *this.RowIndices++ = get_cell_offset_in_stack

            (dimensions, layer_index + 2, row_index, column_index) ;

        conductance = get_conductance_bottom

            (thermal_grid, dimensions, layer_index + 2, row_index, column_index) ;

        *this.Values++  = -conductance ;
        diagonal_value          +=  conductance ;

        (*this.ColumnPointers)++ ;

#ifdef PRINT_SYSTEM_MATRIX
        fprintf (stderr,
            "  Top connceted to Virtual Wall  \t%d\t% .4e\n",
            *(this.RowIndices-1), *(this.Values-1)) ;
#endif
    }

    /* DIAGONAL ELEMENT */

    *diagonal_pointer += diagonal_value ;

#ifdef PRINT_SYSTEM_MATRIX
    fgetpos (stderr, &last_fpos) ;
    fsetpos (stderr, &diag_fposition) ;
    fprintf (stderr, "% .4e", *diagonal_pointer) ;
    fsetpos (stderr, &last_fpos) ;

    fprintf (stderr, "  %d\n", *this.ColumnPointers) ;
#endif

    this.ColumnPointers++ ;

    return this ;
}

/******************************************************************************/

static SystemMatrix_t add_top_wall_column_2rm
(
    SystemMatrix_t  this,
    Dimensions_t   *dimensions,
    ThermalGrid_t  *thermal_grid,
    CellIndex_t     layer_index,
    CellIndex_t     row_index,
    CellIndex_t     column_index
)
{
    Conductance_t conductance = 0.0 ;
    SystemMatrixCoeff_t  diagonal_value   = 0.0 ;
    SystemMatrixCoeff_t *diagonal_pointer = NULL ;

#ifdef PRINT_SYSTEM_MATRIX
    fpos_t diag_fposition, last_fpos ;
    fprintf (stderr,
        "add_top_wall_column_2rm   l %2d r %4d c %4d [%7d]\n",
        layer_index, row_index, column_index,
        get_cell_offset_in_stack (dimensions, layer_index, row_index, column_index)) ;
#endif

    *this.ColumnPointers = *(this.ColumnPointers - 1) ;

    /* BOTTOM connected to Virtual Wall */

    if ( ! IS_FIRST_LAYER(layer_index) )
    {
        *this.RowIndices++ = get_cell_offset_in_stack

            (dimensions, layer_index - 2, row_index, column_index) ;

        conductance = get_conductance_top

            (thermal_grid, dimensions, layer_index - 2, row_index, column_index) ;

        *this.Values++  = -conductance ;
        diagonal_value          +=  conductance ;

        (*this.ColumnPointers)++ ;

#ifdef PRINT_SYSTEM_MATRIX
        fprintf (stderr,
           "  Bottom connected to Channel     \t%d\t% .4e\n",
            *(this.RowIndices-1), *(this.Values-1)) ;
#endif
    }

    /* BOTTOM connected to Channel */

    if ( ! IS_FIRST_LAYER(layer_index) )
    {
        *this.RowIndices++ = get_cell_offset_in_stack

            (dimensions, layer_index - 1, row_index, column_index) ;

        conductance = get_conductance_top

            (thermal_grid, dimensions, layer_index - 1, row_index, column_index) ;

        *this.Values++  = -conductance ;
        diagonal_value          +=  conductance ;

        (*this.ColumnPointers)++ ;

#ifdef PRINT_SYSTEM_MATRIX
        fprintf (stderr,
            "  Bottom connected to Virtual Wall     \t%d\t% .4e\n",
            *(this.RowIndices-1), *(this.Values-1)) ;
#endif
    }

    /* DIAGONAL */

    *this.RowIndices++ = get_cell_offset_in_stack

        (dimensions, layer_index, row_index, column_index) ;

    *this.Values = get_capacity

        (thermal_grid, dimensions, layer_index, row_index, column_index) ;

    diagonal_pointer = this.Values++ ;

    (*this.ColumnPointers)++ ;

#ifdef PRINT_SYSTEM_MATRIX
    fprintf (stderr, "  diagonal\t%d\t", *(this.RowIndices-1)) ;
    fgetpos (stderr, &diag_fposition) ;
    fprintf (stderr, "            ( + % .4e [capacity] )\n", *(this.Values-1)) ;
#endif

    /* Top connected to Silicon */

    if ( ! IS_LAST_LAYER(layer_index, dimensions) )
    {
        *this.RowIndices++ = get_cell_offset_in_stack

            (dimensions, layer_index + 1, row_index, column_index) ;

        conductance = get_conductance_bottom

            (thermal_grid, dimensions, layer_index + 1, row_index, column_index) ;

        *this.Values++  = -conductance ;
        diagonal_value          +=  conductance ;

        (*this.ColumnPointers)++ ;

#ifdef PRINT_SYSTEM_MATRIX
        fprintf (stderr,
            "  Top connected to Silicon  \t%d\t% .4e\n",
            *(this.RowIndices-1), *(this.Values-1)) ;
#endif
    }

    /* DIAGONAL ELEMENT */

    *diagonal_pointer += diagonal_value ;

#ifdef PRINT_SYSTEM_MATRIX
    fgetpos (stderr, &last_fpos) ;
    fsetpos (stderr, &diag_fposition) ;
    fprintf (stderr, "% .4e", *diagonal_pointer) ;
    fsetpos (stderr, &last_fpos) ;

    fprintf (stderr, "  %d\n", *this.ColumnPointers) ;
#endif

    this.ColumnPointers++ ;

    return this ;
}


/******************************************************************************/

static SystemMatrix_t add_virtual_wall_column_2rm
(
    SystemMatrix_t   this,
    Dimensions_t    *dimensions,
    ThermalGrid_t   *thermal_grid,
    ChannelModel_t   channel_model,
    CellIndex_t      layer_index,
    CellIndex_t      row_index,
    CellIndex_t      column_index
)
{
    Conductance_t conductance = 0.0 ;
    SystemMatrixCoeff_t  diagonal_value   = 0.0 ;
    SystemMatrixCoeff_t *diagonal_pointer = NULL ;

#ifdef PRINT_SYSTEM_MATRIX
    fpos_t diag_fposition, last_fpos ;
    fprintf (stderr,
        "add_virtual_wall_column   l %2d r %4d c %4d [%7d]\n",
        layer_index, row_index, column_index,
        get_cell_offset_in_stack (dimensions, layer_index, row_index, column_index)) ;
#endif

    *this.ColumnPointers = *(this.ColumnPointers - 1) ;

    /* BOTTOM */

    if ( ! IS_FIRST_LAYER(layer_index) )
    {
        *this.RowIndices++ = get_cell_offset_in_stack

            (dimensions, layer_index - 1, row_index, column_index) ;

        conductance = get_conductance_bottom

            (thermal_grid, dimensions, layer_index, row_index, column_index) ;

        *this.Values++  = -conductance ;
        diagonal_value          +=  conductance ;

        (*this.ColumnPointers)++ ;

#ifdef PRINT_SYSTEM_MATRIX
        fprintf (stderr,
        "  bottom  \t%d\t% .4e\n",
        *(this.RowIndices-1), *(this.Values-1)) ;
#endif
    }

    /* SOUTH */

    if (   channel_model == TDICE_CHANNEL_MODEL_MC_2RM
        && ! IS_FIRST_ROW(row_index) )
    {
        *this.RowIndices++ = get_cell_offset_in_stack

            (dimensions, layer_index, row_index - 1, column_index) ;

        conductance = PARALLEL
        (
            get_conductance_south (thermal_grid, dimensions, layer_index, row_index, column_index),
            get_conductance_north (thermal_grid, dimensions, layer_index, row_index, column_index)
        ) ;

        *this.Values++  = -conductance ;
        diagonal_value          +=  conductance ;

        (*this.ColumnPointers)++ ;

#ifdef PRINT_SYSTEM_MATRIX
        fprintf (stderr,
        "  south   \t%d\t% .4e\n", *(this.RowIndices-1), *(this.Values-1)) ;
#endif
    }

    /* DIAGONAL */

    *this.RowIndices++ = get_cell_offset_in_stack

        (dimensions, layer_index, row_index, column_index) ;

    *this.Values = get_capacity

        (thermal_grid, dimensions, layer_index, row_index, column_index) ;

    diagonal_pointer = this.Values++ ;

    (*this.ColumnPointers)++ ;

#ifdef PRINT_SYSTEM_MATRIX
    fprintf (stderr, "  diagonal\t%d\t", *(this.RowIndices-1)) ;
    fgetpos (stderr, &diag_fposition) ;
    fprintf (stderr, "            ( + % .4e [capacity] )\n", *(this.Values-1)) ;
#endif

    /* NORTH */

    if (     channel_model == TDICE_CHANNEL_MODEL_MC_2RM
        && ! IS_LAST_ROW(row_index, dimensions) )
    {
        *this.RowIndices++ = get_cell_offset_in_stack

            (dimensions, layer_index, row_index + 1, column_index) ;

        conductance = PARALLEL
        (
            get_conductance_north (thermal_grid, dimensions, layer_index, row_index, column_index),
            get_conductance_south (thermal_grid, dimensions, layer_index, row_index, column_index)
        ) ;

        *this.Values++  = -conductance ;
        diagonal_value          +=  conductance ;

        (*this.ColumnPointers)++ ;

#ifdef PRINT_SYSTEM_MATRIX
        fprintf (stderr,
            "  north   \t%d\t% .4e\n",
            *(this.RowIndices-1), *(this.Values-1)) ;
#endif
    }

    /* TOP */

    if ( ! IS_LAST_LAYER(layer_index, dimensions) )
    {
        *this.RowIndices++ = get_cell_offset_in_stack

            (dimensions, layer_index + 2, row_index, column_index) ;

        conductance = get_conductance_top

            (thermal_grid, dimensions, layer_index, row_index, column_index) ;

        *this.Values++  = -conductance ;
        diagonal_value          +=  conductance ;

        (*this.ColumnPointers)++ ;

#ifdef PRINT_SYSTEM_MATRIX
        fprintf (stderr,
            "  top     \t%d\t% .4e\n",
            *(this.RowIndices-1), *(this.Values-1)) ;
#endif
    }

    /* DIAGONAL ELEMENT */

    *diagonal_pointer += diagonal_value ;

#ifdef PRINT_SYSTEM_MATRIX
    fgetpos (stderr, &last_fpos) ;
    fsetpos (stderr, &diag_fposition) ;
    fprintf (stderr, "% .4e", *diagonal_pointer) ;
    fsetpos (stderr, &last_fpos) ;

    fprintf (stderr, "  %d\n", *this.ColumnPointers) ;
#endif

    this.ColumnPointers++ ;

    return this ;
}

/******************************************************************************/

void fill_system_matrix
(
    SystemMatrix_t     *this,
    ThermalGrid_t      *thermal_grid,
    Dimensions_t       *dimensions
)
{
#ifdef PRINT_SYSTEM_MATRIX
    fprintf (stderr,
        "fill_system_matrix ( l %d r %d c %d )\n",
        get_number_of_layers  (dimensions),
        get_number_of_rows    (dimensions),
        get_number_of_columns (dimensions)) ;
#endif

    SystemMatrix_t tmp_matrix ;

    tmp_matrix.Size = this->Size ;
    tmp_matrix.NNz  = this->NNz ;

    tmp_matrix.ColumnPointers = this->ColumnPointers ;
    tmp_matrix.RowIndices     = this->RowIndices ;
    tmp_matrix.Values         = this->Values ;

    tmp_matrix.ColumnPointers[0] = 0u ;

    tmp_matrix.ColumnPointers++ ;

    CellIndex_t lindex ;

    for (lindex = 0u ; lindex != thermal_grid->Size ; lindex++)
    {
        switch (thermal_grid->LayersProfile [lindex])
        {
            case TDICE_LAYER_SOLID :
            {

                FOR_EVERY_ROW (row_index, dimensions)
                {
                    FOR_EVERY_COLUMN (column_index, dimensions)
                    {
                        tmp_matrix = add_solid_column

                            (tmp_matrix, dimensions, thermal_grid,
                             lindex, row_index, column_index) ;

                    } // FOR_EVERY_COLUMN
                } // FOR_EVERY_ROW


                break ;
            }
            case TDICE_LAYER_SOLID_CONNECTED_TO_AMBIENT :
            {
                FOR_EVERY_ROW (_row_, dimensions)
                {
                    FOR_EVERY_COLUMN (_column_, dimensions)
                    {
                        tmp_matrix = add_solid_column

                            (tmp_matrix, dimensions, thermal_grid,
                             lindex, _row_, _column_) ;

                    } // FOR_EVERY_COLUMN
                } // FOR_EVERY_ROW

                CellIndex_t cell  = get_cell_offset_in_stack (dimensions, lindex, 0 ,0) ;

                FOR_EVERY_ROW (row, dimensions)
                {
                    FOR_EVERY_COLUMN (column, dimensions)
                    {
                        CellIndex_t row_index ;

                        for (row_index = this->ColumnPointers [ cell ] ;
                            row_index < (CellIndex_t) this->ColumnPointers [ cell + 1 ] ;
                            row_index ++)

                            if ((CellIndex_t) this->RowIndices [ row_index ] == cell)

                                this->Values [ row_index ] += get_conductance_top

                                    (thermal_grid, dimensions, lindex, row, column) ;

                        cell++ ;
                    }
                }

                break ;
            }
            case TDICE_LAYER_SPREADER :
            {
                FOR_EVERY_ROW (row_index, dimensions)
                {
                    FOR_EVERY_COLUMN (column_index, dimensions)
                    {
                        tmp_matrix = add_spread_column

                            (tmp_matrix, dimensions, thermal_grid,
                             lindex, row_index, column_index) ;

                    } // FOR_EVERY_COLUMN
                } // FOR_EVERY_ROW

                break ;
            }
            case TDICE_LAYER_SINK :
            {
                FOR_EVERY_ROW (_row_, dimensions)
                {
                    FOR_EVERY_COLUMN (_column_, dimensions)
                    {
                        tmp_matrix = add_sink_column

                            (tmp_matrix, dimensions, thermal_grid,
                             lindex, _row_, _column_) ;

                    } // FOR_EVERY_COLUMN
                } // FOR_EVERY_ROW

                CellIndex_t cell  = get_cell_offset_in_stack (dimensions, lindex, 0 ,0) ;

                FOR_EVERY_ROW (row, dimensions)
                {
                    FOR_EVERY_COLUMN (column, dimensions)
                    {
                        CellIndex_t row_index ;

                        for (row_index = this->ColumnPointers [ cell ] ;
                            row_index < (CellIndex_t) this->ColumnPointers [ cell + 1 ] ;
                            row_index ++)

                            if ((CellIndex_t) this->RowIndices [ row_index ] == cell)

                                this->Values [ row_index ] += get_conductance_top

                                    (thermal_grid, dimensions, lindex, row, column) ;

                        cell++ ;
                    }
                }

                break ;
            }
            case TDICE_LAYER_CHANNEL_4RM :
            {
                FOR_EVERY_ROW (row_index, dimensions)
                {
                    FOR_EVERY_COLUMN (column_index, dimensions)
                    {
                        if (IS_CHANNEL_COLUMN (thermal_grid->Channel->ChannelModel, column_index) == true)

                            tmp_matrix = add_liquid_column_4rm

                                (tmp_matrix, dimensions, thermal_grid,
                                 lindex, row_index, column_index) ;

                        else

                            tmp_matrix = add_solid_column

                                (tmp_matrix, dimensions, thermal_grid,
                                 lindex, row_index, column_index) ;

                    } // FOR_EVERY_COLUMN
                }  // FOR_EVERY_ROW

                break ;
            }
            case TDICE_LAYER_CHANNEL_2RM :
            case TDICE_LAYER_PINFINS_INLINE :
            case TDICE_LAYER_PINFINS_STAGGERED :
            {
                FOR_EVERY_ROW (row_index, dimensions)
                {
                    FOR_EVERY_COLUMN (column_index, dimensions)
                    {
                        tmp_matrix = add_liquid_column_2rm

                            (tmp_matrix, dimensions, thermal_grid,
                             lindex, row_index, column_index) ;

                    } // FOR_EVERY_COLUMN
                }  // FOR_EVERY_ROW

                break ;
            }
            case TDICE_LAYER_VWALL_CHANNEL :
            case TDICE_LAYER_VWALL_PINFINS :
            {
                FOR_EVERY_ROW (row_index, dimensions)
                {
                    FOR_EVERY_COLUMN (column_index, dimensions)
                    {
                        tmp_matrix = add_virtual_wall_column_2rm

                            (tmp_matrix, dimensions, thermal_grid,
                             thermal_grid->Channel->ChannelModel,
                             lindex, row_index, column_index) ;

                    } // FOR_EVERY_COLUMN
                }  // FOR_EVERY_ROW

                break ;
            }
            case TDICE_LAYER_TOP_WALL :
            {
                FOR_EVERY_ROW (row_index, dimensions)
                {
                    FOR_EVERY_COLUMN (column_index, dimensions)
                    {
                        tmp_matrix = add_top_wall_column_2rm

                            (tmp_matrix, dimensions, thermal_grid,
                             lindex, row_index, column_index) ;

                    } // FOR_EVERY_COLUMN
                }  // FOR_EVERY_ROW

                break ;
            }
            case TDICE_LAYER_BOTTOM_WALL :
            {
                FOR_EVERY_ROW (row_index, dimensions)
                {
                    FOR_EVERY_COLUMN (column_index, dimensions)
                    {
                        tmp_matrix = add_bottom_wall_column_2rm

                            (tmp_matrix, dimensions, thermal_grid,
                             lindex, row_index, column_index) ;

                    } // FOR_EVERY_COLUMN
                }  // FOR_EVERY_ROW
                break ;

            }
            case TDICE_LAYER_NONE :
            {
                fprintf (stderr, "ERROR: unset layer type\n") ;

                break ;
            }
            default :

                fprintf (stderr, "ERROR: unknown layer type %d\n", thermal_grid->LayersProfile [lindex]) ;
        }

    }
}

/******************************************************************************/

void print_system_matrix (SystemMatrix_t this, String_t file_name)
{
    FILE* file = fopen (file_name, "w") ;

    if (file == NULL)
    {
        fprintf(stderr, "Cannot open file %s\n", file_name) ;
        return ;
    }

    CellIndex_t row, column ;

    for (column = 0 ; column < this.Size ; column++)

        for (row = this.ColumnPointers[column] ;
             row < this.ColumnPointers[column + 1] ;
             row++)

            fprintf (file, "%9d\t%9d\t%32.24f\n",
                this.RowIndices[row] + 1, column + 1,
                this.Values[row]) ;

    fclose (file) ;
}

/******************************************************************************/
