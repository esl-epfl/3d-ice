/******************************************************************************
 * This file is part of 3D-ICE, version 2.2 .                                 *
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

void system_matrix_init (SystemMatrix_t* sysmatrix)
{
    sysmatrix->ColumnPointers = NULL ;
    sysmatrix->RowIndices     = NULL;
    sysmatrix->Values         = NULL ;
    sysmatrix->Size           = (CellIndex_t) 0u ;
    sysmatrix->NNz            = (CellIndex_t) 0u ;

    sysmatrix->SLU_PermutationMatrixR = NULL ;
    sysmatrix->SLU_PermutationMatrixC = NULL ;
    sysmatrix->SLU_Etree              = NULL ;

    sysmatrix->SLUMatrix_A.Store          = NULL ;
    sysmatrix->SLUMatrix_A_Permuted.Store = NULL ;
    sysmatrix->SLUMatrix_L.Store          = NULL ;
    sysmatrix->SLUMatrix_U.Store          = NULL ;

    sysmatrix->SLU_Info = 0 ;

    sysmatrix->SLU_Stat.panel_histo = NULL ;
    sysmatrix->SLU_Stat.utime       = NULL ;
    sysmatrix->SLU_Stat.ops         = NULL ;
    sysmatrix->SLU_Stat.TinyPivots  = (int) 0 ;
    sysmatrix->SLU_Stat.RefineSteps = (int) 0 ;
    sysmatrix->SLU_Stat.expansions  = (int) 0 ;

    set_default_options (&sysmatrix->SLU_Options) ;

    sysmatrix->SLU_Options.Fact            = DOFACT ;
    sysmatrix->SLU_Options.PrintStat       = NO ;
    sysmatrix->SLU_Options.Equil           = NO ;
    sysmatrix->SLU_Options.SymmetricMode   = YES ;
    sysmatrix->SLU_Options.ColPerm         = MMD_AT_PLUS_A ;
    sysmatrix->SLU_Options.RowPerm         = NOROWPERM ;
    sysmatrix->SLU_Options.DiagPivotThresh = 0.001 ;
}

/******************************************************************************/

Error_t system_matrix_build
(
    SystemMatrix_t *sysmatrix,
    CellIndex_t     size,
    CellIndex_t     nnz
)
{
    sysmatrix->Size = size ;
    sysmatrix->NNz  = nnz ;

    sysmatrix->RowIndices = (CellIndex_t *) malloc (sizeof(CellIndex_t) * nnz) ;

    if (sysmatrix->RowIndices == NULL)

        return TDICE_FAILURE ;

    sysmatrix->ColumnPointers = (CellIndex_t *) malloc (sizeof(CellIndex_t) * (size + 1)) ;

    if (sysmatrix->ColumnPointers == NULL)
    {
        free (sysmatrix->RowIndices) ;
        return TDICE_FAILURE ;
    }

    sysmatrix->Values = (SystemMatrixCoeff_t *) malloc (sizeof(SystemMatrixCoeff_t) * nnz) ;

    if (sysmatrix->Values == NULL)
    {
        free (sysmatrix->ColumnPointers) ;
        free (sysmatrix->RowIndices) ;
        return TDICE_FAILURE ;
    }

    dCreate_CompCol_Matrix

        (&sysmatrix->SLUMatrix_A, sysmatrix->Size, sysmatrix->Size, sysmatrix->NNz,
          sysmatrix->Values, (int*) sysmatrix->RowIndices, (int*) sysmatrix->ColumnPointers,
         SLU_NC, SLU_D, SLU_GE) ;

    /* Alloc SLU permutation matrices and elimination tree */

    sysmatrix->SLU_PermutationMatrixR = (int *) malloc (sizeof(int) * sysmatrix->Size) ;

    if (sysmatrix->SLU_PermutationMatrixR == NULL )
    {
        free (sysmatrix->ColumnPointers) ;
        free (sysmatrix->RowIndices) ;
        free (sysmatrix->Values) ;

        Destroy_SuperMatrix_Store (&sysmatrix->SLUMatrix_A) ;

        return TDICE_FAILURE ;
    }

    sysmatrix->SLU_PermutationMatrixC = (int *) malloc(sizeof(int) * sysmatrix->Size) ;

    if (sysmatrix->SLU_PermutationMatrixC == NULL )
    {
        free (sysmatrix->ColumnPointers) ;
        free (sysmatrix->RowIndices) ;
        free (sysmatrix->Values) ;

        Destroy_SuperMatrix_Store (&sysmatrix->SLUMatrix_A) ;

        free (sysmatrix->SLU_PermutationMatrixR) ;

        return TDICE_FAILURE ;
    }

    sysmatrix->SLU_Etree = (int *) malloc (sizeof(int) * sysmatrix->Size) ;

    if (sysmatrix->SLU_Etree == NULL)
    {
        free (sysmatrix->ColumnPointers) ;
        free (sysmatrix->RowIndices) ;
        free (sysmatrix->Values) ;

        Destroy_SuperMatrix_Store (&sysmatrix->SLUMatrix_A) ;

        free (sysmatrix->SLU_PermutationMatrixR) ;
        free (sysmatrix->SLU_PermutationMatrixC) ;

        return TDICE_FAILURE ;
    }

    StatInit (&sysmatrix->SLU_Stat) ;

    return TDICE_SUCCESS ;
}

/******************************************************************************/

Error_t do_factorization (SystemMatrix_t *sysmatrix)
{
    if (sysmatrix->SLU_Options.Fact == DOFACT)
    {
        get_perm_c

            (sysmatrix->SLU_Options.ColPerm, &sysmatrix->SLUMatrix_A,
            sysmatrix->SLU_PermutationMatrixC) ;

        sp_preorder

            (&sysmatrix->SLU_Options, &sysmatrix->SLUMatrix_A,
            sysmatrix->SLU_PermutationMatrixC, sysmatrix->SLU_Etree,
            &sysmatrix->SLUMatrix_A_Permuted) ;
    }
    else if (sysmatrix->SLU_Options.Fact == FACTORED)
    {
         sysmatrix->SLU_Options.Fact = SamePattern_SameRowPerm ;
    }
    else
    {
        fprintf (stderr, "ERROR: wrong factorization status %d\n",
            sysmatrix->SLU_Options.Fact) ;

        return TDICE_FAILURE ;
    }

    dgstrf

        (&sysmatrix->SLU_Options, &sysmatrix->SLUMatrix_A_Permuted,
         sp_ienv(2), sp_ienv(1), /* relax and panel size */
         sysmatrix->SLU_Etree,
         NULL, 0,                /* work and lwork */
         sysmatrix->SLU_PermutationMatrixC, sysmatrix->SLU_PermutationMatrixR,
         &sysmatrix->SLUMatrix_L, &sysmatrix->SLUMatrix_U,
         &sysmatrix->SLU_Stat, &sysmatrix->SLU_Info) ;

    if (sysmatrix->SLU_Info == 0)
    {
        sysmatrix->SLU_Options.Fact = FACTORED ;

        return TDICE_SUCCESS ;
    }
    else
    {
        fprintf (stderr, "SuperLu factorization error %d\n", sysmatrix->SLU_Info) ;

        return TDICE_FAILURE ;
    }
}

/******************************************************************************/

void system_matrix_destroy (SystemMatrix_t *sysmatrix)
{
    free (sysmatrix->ColumnPointers) ;
    free (sysmatrix->RowIndices) ;
    free (sysmatrix->Values) ;

    free (sysmatrix->SLU_PermutationMatrixR) ;
    free (sysmatrix->SLU_PermutationMatrixC) ;
    free (sysmatrix->SLU_Etree) ;

    StatFree (&sysmatrix->SLU_Stat) ;

    Destroy_SuperMatrix_Store (&sysmatrix->SLUMatrix_A) ;

    if (sysmatrix->SLU_Options.Fact != DOFACT )
    {
        Destroy_CompCol_Permuted (&sysmatrix->SLUMatrix_A_Permuted) ;
        Destroy_SuperNode_Matrix (&sysmatrix->SLUMatrix_L) ;
        Destroy_CompCol_Matrix   (&sysmatrix->SLUMatrix_U) ;
    }

    system_matrix_init (sysmatrix) ;
}

/******************************************************************************/

static SystemMatrix_t add_solid_column
(
    SystemMatrix_t  sysmatrix,
    ThermalGrid_t  *thermal_grid,
    Analysis_t     *analysis,
    Dimensions_t   *dimensions,
    CellIndex_t     layer_index,
    CellIndex_t     row_index,
    CellIndex_t     column_index
)
{
    Conductance_t conductance = 0.0 ;
    SystemMatrixCoeff_t  diagonal_value   = 0.0 ;
    SystemMatrixCoeff_t *diagonal_pointer = NULL ;

    Conductance_t g_bottom, g_top, g_north, g_south, g_east, g_west ;

#ifdef PRINT_SYSTEM_MATRIX
    fpos_t diag_fposition, last_fpos ;

    fprintf (stderr,
        "add_solid_column   l %2d r %4d c %4d [%7d]\n",
        layer_index, row_index, column_index,
        get_cell_offset_in_stack (dimensions, layer_index, row_index, column_index)) ;
#endif

    *sysmatrix.ColumnPointers = *(sysmatrix.ColumnPointers - 1) ;

    /********************************* BOTTOM *********************************/

    if (IS_FIRST_LAYER(layer_index))

        goto skip_bottom ;

    *sysmatrix.RowIndices++ = get_cell_offset_in_stack

        (dimensions, layer_index - 1, row_index, column_index) ;

    g_bottom = get_conductance_bottom

        (thermal_grid, dimensions, layer_index    , row_index, column_index) ;

    g_top = get_conductance_top

        (thermal_grid, dimensions, layer_index - 1, row_index, column_index) ;

    if (g_top == 0)

        conductance = g_bottom ;

    else

        conductance = PARALLEL (g_bottom, g_top) ;

    *sysmatrix.Values++  = -conductance ;
    diagonal_value +=  conductance ;

    (*sysmatrix.ColumnPointers)++ ;

#ifdef PRINT_SYSTEM_MATRIX
    fprintf (stderr,
        "  bottom  \t%d\t% .4e = % .4e (B) || % .4e (T)\n",
        *(sysmatrix.RowIndices-1), *(sysmatrix.Values-1), g_bottom, g_top) ;
#endif

skip_bottom :

    /********************************* SOUTH **********************************/

    if (   IS_FIRST_ROW(row_index)
        || thermal_grid->LayersProfile [layer_index] == TDICE_LAYER_SPREADER
        || thermal_grid->LayersProfile [layer_index] == TDICE_LAYER_SINK)

        goto skip_south ;

    *sysmatrix.RowIndices++ = get_cell_offset_in_stack

        (dimensions, layer_index, row_index - 1, column_index) ;

    g_south = get_conductance_south

        (thermal_grid, dimensions, layer_index, row_index    , column_index) ;

    g_north = get_conductance_north

        (thermal_grid, dimensions, layer_index, row_index - 1, column_index) ;

    conductance = PARALLEL (g_south, g_north) ;

    *sysmatrix.Values++  = -conductance ;
    diagonal_value +=  conductance ;

    (*sysmatrix.ColumnPointers)++ ;

#ifdef PRINT_SYSTEM_MATRIX
    fprintf (stderr,
        "  south   \t%d\t% .4e = % .4e (S) || % .4e (N)\n",
        *(sysmatrix.RowIndices-1), *(sysmatrix.Values-1), g_south, g_north) ;
#endif

skip_south :

    /********************************* WEST ***********************************/

    if (   IS_FIRST_COLUMN(column_index)
        || thermal_grid->LayersProfile [layer_index] == TDICE_LAYER_SPREADER
        || thermal_grid->LayersProfile [layer_index] == TDICE_LAYER_SINK)

        goto skip_west ;

    *sysmatrix.RowIndices++ = get_cell_offset_in_stack

        (dimensions, layer_index, row_index, column_index - 1) ;

    g_west = get_conductance_west

        (thermal_grid, dimensions, layer_index, row_index, column_index    ) ;

    g_east = get_conductance_east

        (thermal_grid, dimensions, layer_index, row_index, column_index - 1) ;

    conductance = PARALLEL (g_west, g_east) ;

    *sysmatrix.Values++  = -conductance ;
    diagonal_value +=  conductance ;

    (*sysmatrix.ColumnPointers)++ ;

#ifdef PRINT_SYSTEM_MATRIX
    fprintf (stderr,
        "  west    \t%d\t% .4e = % .4e (W) || % .4e (E)\n",
        *(sysmatrix.RowIndices-1), *(sysmatrix.Values-1), g_west, g_east) ;
#endif

skip_west :

    /********************************* DIAGONAL *******************************/

    *sysmatrix.RowIndices++ = get_cell_offset_in_stack

        (dimensions, layer_index, row_index, column_index) ;

    *sysmatrix.Values = 0.0 ;

    if (analysis->AnalysisType == TDICE_ANALYSIS_TYPE_TRANSIENT)
    {
        *sysmatrix.Values = get_capacity

            (thermal_grid, dimensions, layer_index, row_index, column_index) ;

        *sysmatrix.Values /= analysis->StepTime ;
    }

    if (   thermal_grid->LayersProfile [layer_index] == TDICE_LAYER_SINK
        || thermal_grid->LayersProfile [layer_index] == TDICE_LAYER_SOLID_CONNECTED_TO_AMBIENT
        || thermal_grid->LayersProfile [layer_index] == TDICE_LAYER_SOURCE_CONNECTED_TO_AMBIENT)

        *sysmatrix.Values += get_conductance_top

            (thermal_grid, dimensions, layer_index, row_index, column_index) ;

    diagonal_pointer = sysmatrix.Values++ ;

    (*sysmatrix.ColumnPointers)++ ;

#ifdef PRINT_SYSTEM_MATRIX
    fprintf (stderr, "  diagonal\t%d\t", *(sysmatrix.RowIndices-1)) ;
    fgetpos (stderr, &diag_fposition) ;
    fprintf (stderr, "            ( + % .4e [capacity] )\n", *(sysmatrix.Values-1)) ;
#endif

    /********************************* EAST ***********************************/

    if (   IS_LAST_COLUMN(column_index, dimensions)
        || thermal_grid->LayersProfile [layer_index] == TDICE_LAYER_SPREADER
        || thermal_grid->LayersProfile [layer_index] == TDICE_LAYER_SINK)

        goto skip_east ;

    *sysmatrix.RowIndices++ = get_cell_offset_in_stack

        (dimensions, layer_index, row_index, column_index + 1) ;

    g_east = get_conductance_east

        (thermal_grid, dimensions, layer_index, row_index, column_index    ) ;

    g_west = get_conductance_west

        (thermal_grid, dimensions, layer_index, row_index, column_index + 1) ;

    conductance = PARALLEL (g_east, g_west) ;

    *sysmatrix.Values++  = -conductance ;
    diagonal_value +=  conductance ;

    (*sysmatrix.ColumnPointers)++ ;

#ifdef PRINT_SYSTEM_MATRIX
    fprintf (stderr,
        "  east    \t%d\t% .4e = % .4e (E) || % .4e (W)\n",
        *(sysmatrix.RowIndices-1), *(sysmatrix.Values-1), g_east, g_west) ;
#endif

skip_east :

    /********************************* NORTH **********************************/

    if (   IS_LAST_ROW(row_index, dimensions)
        || thermal_grid->LayersProfile [layer_index] == TDICE_LAYER_SPREADER
        || thermal_grid->LayersProfile [layer_index] == TDICE_LAYER_SINK)

        goto skip_north ;

    *sysmatrix.RowIndices++ = get_cell_offset_in_stack

        (dimensions, layer_index, row_index + 1, column_index) ;

    g_north = get_conductance_north

        (thermal_grid, dimensions, layer_index, row_index    , column_index) ;

    g_south = get_conductance_south

        (thermal_grid, dimensions, layer_index, row_index + 1, column_index) ;

    conductance = PARALLEL (g_north, g_south) ;

    *sysmatrix.Values++  = -conductance ;
    diagonal_value +=  conductance ;

    (*sysmatrix.ColumnPointers)++ ;

#ifdef PRINT_SYSTEM_MATRIX
    fprintf (stderr,
        "  north   \t%d\t% .4e = % .4e (N) || % .4e (S)\n",
        *(sysmatrix.RowIndices-1), *(sysmatrix.Values-1), g_north, g_south) ;
#endif

skip_north :

    /********************************* TOP ************************************/

    if (IS_LAST_LAYER(layer_index, dimensions))

        goto skip_top ;

    *sysmatrix.RowIndices++ = get_cell_offset_in_stack

        (dimensions, layer_index + 1, row_index, column_index) ;

    g_top = get_conductance_top

        (thermal_grid, dimensions, layer_index, row_index, column_index) ;

    g_bottom = get_conductance_bottom

        (thermal_grid, dimensions, layer_index + 1, row_index, column_index) ;

    if (g_bottom == 0)

        conductance = g_top ;

    else

        conductance = PARALLEL (g_top, g_bottom) ;

    *sysmatrix.Values++  = -conductance ;
    diagonal_value +=  conductance ;

    (*sysmatrix.ColumnPointers)++ ;

#ifdef PRINT_SYSTEM_MATRIX
    fprintf (stderr,
        "  top     \t%d\t% .4e = % .4e (T) || % .4e (B)\n",
        *(sysmatrix.RowIndices-1), *(sysmatrix.Values-1),
        g_top, g_bottom) ;
#endif

skip_top :

    /************************** DIAGONAL ELEMENT ******************************/

    *diagonal_pointer += diagonal_value ;

#ifdef PRINT_SYSTEM_MATRIX
    fgetpos (stderr, &last_fpos) ;
    fsetpos (stderr, &diag_fposition) ;
    fprintf (stderr, "% .4e", *diagonal_pointer) ;
    fsetpos (stderr, &last_fpos) ;

    fprintf (stderr, "  %d\n", *sysmatrix.ColumnPointers) ;
#endif

    sysmatrix.ColumnPointers++ ;

    return sysmatrix ;
}

/******************************************************************************/

static SystemMatrix_t add_liquid_column_4rm
(
    SystemMatrix_t  sysmatrix,
    ThermalGrid_t  *thermal_grid,
    Analysis_t     *analysis,
    Dimensions_t   *dimensions,
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

    *sysmatrix.ColumnPointers = *(sysmatrix.ColumnPointers - 1) ;

    /* BOTTOM */

    if ( ! IS_FIRST_LAYER(layer_index) )
    {
        *sysmatrix.RowIndices++ = get_cell_offset_in_stack

            (dimensions, layer_index - 1, row_index, column_index) ;

        conductance = PARALLEL
        (
            get_conductance_bottom (thermal_grid, dimensions, layer_index    , row_index, column_index),
            get_conductance_top    (thermal_grid, dimensions, layer_index - 1, row_index, column_index)
        ) ;

        *sysmatrix.Values++  = -conductance ;
        diagonal_value +=  conductance ;

        (*sysmatrix.ColumnPointers)++ ;

#ifdef PRINT_SYSTEM_MATRIX
        fprintf (stderr,
            "  bottom  \t%d\t% .4e = % .4e (B) || % .4e (T)\n",
            *(sysmatrix.RowIndices-1), *(sysmatrix.Values-1),
            get_conductance_bottom (thermal_grid, dimensions, layer_index    , row_index, column_index),
            get_conductance_top    (thermal_grid, dimensions, layer_index - 1, row_index, column_index)) ;
#endif
    }

    /* SOUTH */

    if ( ! IS_FIRST_ROW(row_index) )
    {
        *sysmatrix.RowIndices++ = get_cell_offset_in_stack

            (dimensions, layer_index, row_index - 1, column_index) ;

        *sysmatrix.Values++ = get_conductance_north /* == C */

            (thermal_grid, dimensions, layer_index, row_index, column_index) ;

        (*sysmatrix.ColumnPointers)++ ;

#ifdef PRINT_SYSTEM_MATRIX
        fprintf (stderr,
            "  south   \t%d\t% .4e (N)\n",
            *(sysmatrix.RowIndices-1), *(sysmatrix.Values-1)) ;
#endif
    }

    /* WEST */

    if ( ! IS_FIRST_COLUMN(column_index) )
    {
        *sysmatrix.RowIndices++ = get_cell_offset_in_stack

            (dimensions, layer_index, row_index, column_index - 1) ;

        conductance = PARALLEL
        (
            get_conductance_west (thermal_grid, dimensions, layer_index, row_index, column_index    ),
            get_conductance_east (thermal_grid, dimensions, layer_index, row_index, column_index - 1)
        ) ;

        *sysmatrix.Values++  = -conductance ;
        diagonal_value +=  conductance ;

        (*sysmatrix.ColumnPointers)++ ;

#ifdef PRINT_SYSTEM_MATRIX
        fprintf (stderr,
            "  west    \t%d\t% .4e = % .4e (W) || % .4e (E)\n",
            *(sysmatrix.RowIndices-1), *(sysmatrix.Values-1),
            get_conductance_west (thermal_grid, dimensions, layer_index, row_index, column_index    ),
            get_conductance_east (thermal_grid, dimensions, layer_index, row_index, column_index - 1));
#endif
    }

    /* DIAGONAL */

    *sysmatrix.RowIndices++ = get_cell_offset_in_stack

        (dimensions, layer_index, row_index, column_index) ;

    *sysmatrix.Values = 0.0 ;

    if (analysis->AnalysisType == TDICE_ANALYSIS_TYPE_TRANSIENT)
    {
        *sysmatrix.Values = get_capacity

            (thermal_grid, dimensions, layer_index, row_index, column_index) ;

        *sysmatrix.Values /= analysis->StepTime ;
    }

    diagonal_pointer = sysmatrix.Values++ ;

    (*sysmatrix.ColumnPointers)++ ;

#ifdef PRINT_SYSTEM_MATRIX
    fprintf (stderr, "  diagonal\t%d\t", *(sysmatrix.RowIndices-1)) ;
    fgetpos (stderr, &diag_fposition) ;
    fprintf (stderr, "            ( + % .4e [capacity] )\n", *(sysmatrix.Values-1)) ;
#endif

    /* EAST */

    if ( ! IS_LAST_COLUMN(column_index, dimensions) )
    {
        *sysmatrix.RowIndices++ = get_cell_offset_in_stack

            (dimensions, layer_index, row_index, column_index + 1) ;

        conductance = PARALLEL
        (
            get_conductance_east (thermal_grid, dimensions, layer_index, row_index, column_index    ),
            get_conductance_west (thermal_grid, dimensions, layer_index, row_index, column_index + 1)
        ) ;

        *sysmatrix.Values++  = -conductance ;
        diagonal_value +=  conductance ;

        (*sysmatrix.ColumnPointers)++ ;

#ifdef PRINT_SYSTEM_MATRIX
        fprintf (stderr,
            "  east    \t%d\t% .4e = % .4e (E) || % .4e (W)\n",
            *(sysmatrix.RowIndices-1), *(sysmatrix.Values-1),
            get_conductance_east (thermal_grid, dimensions, layer_index, row_index, column_index    ),
            get_conductance_west (thermal_grid, dimensions, layer_index, row_index, column_index + 1));
#endif
    }

    /* NORTH */

    if ( ! IS_LAST_ROW(row_index, dimensions) )
    {
        *sysmatrix.RowIndices++ = get_cell_offset_in_stack

            (dimensions, layer_index, row_index + 1, column_index) ;

        *sysmatrix.Values++ = get_conductance_south /* == - C */

            (thermal_grid, dimensions, layer_index, row_index, column_index) ;

        (*sysmatrix.ColumnPointers)++ ;

#ifdef PRINT_SYSTEM_MATRIX
        fprintf (stderr,
            "  north   \t%d\t% .4e (S)\n",
            *(sysmatrix.RowIndices-1), *(sysmatrix.Values-1)) ;
#endif
    }

    /* TOP */

    if ( ! IS_LAST_LAYER(layer_index, dimensions) )
    {
        *sysmatrix.RowIndices++ = get_cell_offset_in_stack

            (dimensions, layer_index + 1, row_index, column_index) ;

        conductance = PARALLEL
        (
            get_conductance_top    (thermal_grid, dimensions, layer_index    , row_index, column_index),
            get_conductance_bottom (thermal_grid, dimensions, layer_index + 1, row_index, column_index)
        ) ;

        *sysmatrix.Values++  = -conductance ;
        diagonal_value +=  conductance ;

        (*sysmatrix.ColumnPointers)++ ;

#ifdef PRINT_SYSTEM_MATRIX
        fprintf (stderr,
            "  top     \t%d\t% .4e = % .4e (T) || % .4e (B)\n",
            *(sysmatrix.RowIndices-1), *(sysmatrix.Values-1),
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

    fprintf (stderr, "  %d\n", *sysmatrix.ColumnPointers) ;
#endif

    sysmatrix.ColumnPointers++ ;

    return sysmatrix ;
}

/******************************************************************************/

static SystemMatrix_t add_liquid_column_2rm
(
    SystemMatrix_t  sysmatrix,
    ThermalGrid_t  *thermal_grid,
    Analysis_t     *analysis,
    Dimensions_t   *dimensions,
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

    *sysmatrix.ColumnPointers = *(sysmatrix.ColumnPointers - 1) ;

    /* BOTTOM */

    if ( ! IS_FIRST_LAYER(layer_index) )  // FIXME
    {
        *sysmatrix.RowIndices++ = get_cell_offset_in_stack

            (dimensions, layer_index - 2, row_index, column_index) ;

        conductance = get_conductance_bottom

            (thermal_grid, dimensions, layer_index, row_index, column_index) ;

        *sysmatrix.Values++  = -conductance ;
        diagonal_value +=  conductance ;

        (*sysmatrix.ColumnPointers)++ ;

#ifdef PRINT_SYSTEM_MATRIX
        fprintf (stderr,
            "  bottom  \t%d\t% .4e (B)\n",
            *(sysmatrix.RowIndices-1), *(sysmatrix.Values-1)) ;
#endif
    }

    /* SOUTH */

    if ( ! IS_FIRST_ROW(row_index) )
    {
        *sysmatrix.RowIndices++ = get_cell_offset_in_stack

            (dimensions, layer_index, row_index - 1, column_index) ;

        *sysmatrix.Values++ = get_conductance_north /* == C */

            (thermal_grid, dimensions, layer_index, row_index, column_index) ;

        (*sysmatrix.ColumnPointers)++ ;

#ifdef PRINT_SYSTEM_MATRIX
        fprintf (stderr,
            "  south   \t%d\t% .4e (N)\n",
            *(sysmatrix.RowIndices-1), *(sysmatrix.Values-1)) ;
#endif
    }

    /* DIAGONAL */

    *sysmatrix.RowIndices++ = get_cell_offset_in_stack

        (dimensions, layer_index, row_index, column_index) ;

    *sysmatrix.Values = 0.0 ;

    if (analysis->AnalysisType == TDICE_ANALYSIS_TYPE_TRANSIENT)
    {
        *sysmatrix.Values = get_capacity

            (thermal_grid, dimensions, layer_index, row_index, column_index) ;

        *sysmatrix.Values /= analysis->StepTime ;
    }

    diagonal_pointer = sysmatrix.Values++ ;

    (*sysmatrix.ColumnPointers)++ ;

#ifdef PRINT_SYSTEM_MATRIX
    fprintf (stderr, "  diagonal\t%d\t", *(sysmatrix.RowIndices-1)) ;
    fgetpos (stderr, &diag_fposition) ;
    fprintf (stderr, "            ( + % .4e [capacity] )\n", *(sysmatrix.Values-1)) ;
#endif

    /* NORTH */

    if ( ! IS_LAST_ROW(row_index, dimensions) )
    {
        *sysmatrix.RowIndices++ = get_cell_offset_in_stack

            (dimensions, layer_index, row_index + 1, column_index) ;

        *sysmatrix.Values++ = get_conductance_south /* == -C */

            (thermal_grid, dimensions, layer_index, row_index, column_index) ;

        (*sysmatrix.ColumnPointers)++ ;

#ifdef PRINT_SYSTEM_MATRIX
        fprintf (stderr,
            "  north   \t%d\t% .4e (S)\n",
            *(sysmatrix.RowIndices-1), *(sysmatrix.Values-1)) ;
#endif
    }

    /* TOP */

    if ( ! IS_LAST_LAYER(layer_index, dimensions) )
    {
        *sysmatrix.RowIndices++ = get_cell_offset_in_stack

            (dimensions, layer_index + 1, row_index, column_index) ;

        conductance = get_conductance_top

            (thermal_grid, dimensions, layer_index, row_index, column_index) ;

        *sysmatrix.Values++  = -conductance ;
        diagonal_value +=  conductance ;

        (*sysmatrix.ColumnPointers)++ ;

#ifdef PRINT_SYSTEM_MATRIX
        fprintf (stderr,
            "  top     \t%d\t% .4e (T)\n",
            *(sysmatrix.RowIndices-1), *(sysmatrix.Values-1)) ;
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

    fprintf (stderr, "  %d\n", *sysmatrix.ColumnPointers) ;
#endif

    sysmatrix.ColumnPointers++ ;

    return sysmatrix ;
}

/******************************************************************************/

static SystemMatrix_t add_bottom_wall_column_2rm
(
    SystemMatrix_t  sysmatrix,
    ThermalGrid_t  *thermal_grid,
    Analysis_t     *analysis,
    Dimensions_t   *dimensions,
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

    *sysmatrix.ColumnPointers = *(sysmatrix.ColumnPointers - 1) ;

    /* BOTTOM connected to Silicon */

    if ( ! IS_FIRST_LAYER(layer_index) )
    {
        *sysmatrix.RowIndices++ = get_cell_offset_in_stack

            (dimensions, layer_index - 1, row_index, column_index) ;

        conductance = get_conductance_top

            (thermal_grid, dimensions, layer_index - 1, row_index, column_index) ;

        *sysmatrix.Values++  = -conductance ;
        diagonal_value +=  conductance ;

        (*sysmatrix.ColumnPointers)++ ;

#ifdef PRINT_SYSTEM_MATRIX
        fprintf (stderr,
            "  Bottom connected to Silicon     \t%d\t% .4e\n",
            *(sysmatrix.RowIndices-1), *(sysmatrix.Values-1)) ;
#endif
    }

    /* DIAGONAL */

    *sysmatrix.RowIndices++ = get_cell_offset_in_stack

        (dimensions, layer_index, row_index, column_index) ;

    *sysmatrix.Values = 0.0 ;

    if (analysis->AnalysisType == TDICE_ANALYSIS_TYPE_TRANSIENT)
    {
        *sysmatrix.Values = get_capacity

            (thermal_grid, dimensions, layer_index, row_index, column_index) ;

        *sysmatrix.Values /= analysis->StepTime ;
    }

    diagonal_pointer = sysmatrix.Values++ ;

    (*sysmatrix.ColumnPointers)++ ;

#ifdef PRINT_SYSTEM_MATRIX
    fprintf (stderr, "  diagonal\t%d\t", *(sysmatrix.RowIndices-1)) ;
    fgetpos (stderr, &diag_fposition) ;
    fprintf (stderr, "            ( + % .4e [capacity] )\n", *(sysmatrix.Values-1)) ;
#endif

    /* Top connected to Virtual Wall */

    if ( ! IS_LAST_LAYER(layer_index, dimensions) )
    {
        *sysmatrix.RowIndices++ = get_cell_offset_in_stack

            (dimensions, layer_index + 1, row_index, column_index) ;

        conductance = get_conductance_top

            (thermal_grid, dimensions, layer_index + 1, row_index, column_index) ;

        *sysmatrix.Values++  = -conductance ;
        diagonal_value +=  conductance ;

        (*sysmatrix.ColumnPointers)++ ;

#ifdef PRINT_SYSTEM_MATRIX
        fprintf (stderr,
            "  Top connected to Channel  \t%d\t% .4e\n",
            *(sysmatrix.RowIndices-1), *(sysmatrix.Values-1)) ;
#endif
    }

    /* Top connected to Channel */

    if ( ! IS_LAST_LAYER(layer_index, dimensions) )
    {
        *sysmatrix.RowIndices++ = get_cell_offset_in_stack

            (dimensions, layer_index + 2, row_index, column_index) ;

        conductance = get_conductance_bottom

            (thermal_grid, dimensions, layer_index + 2, row_index, column_index) ;

        *sysmatrix.Values++  = -conductance ;
        diagonal_value +=  conductance ;

        (*sysmatrix.ColumnPointers)++ ;

#ifdef PRINT_SYSTEM_MATRIX
        fprintf (stderr,
            "  Top connceted to Virtual Wall  \t%d\t% .4e\n",
            *(sysmatrix.RowIndices-1), *(sysmatrix.Values-1)) ;
#endif
    }

    /* DIAGONAL ELEMENT */

    *diagonal_pointer += diagonal_value ;

#ifdef PRINT_SYSTEM_MATRIX
    fgetpos (stderr, &last_fpos) ;
    fsetpos (stderr, &diag_fposition) ;
    fprintf (stderr, "% .4e", *diagonal_pointer) ;
    fsetpos (stderr, &last_fpos) ;

    fprintf (stderr, "  %d\n", *sysmatrix.ColumnPointers) ;
#endif

    sysmatrix.ColumnPointers++ ;

    return sysmatrix ;
}

/******************************************************************************/

static SystemMatrix_t add_top_wall_column_2rm
(
    SystemMatrix_t  sysmatrix,
    ThermalGrid_t  *thermal_grid,
    Analysis_t     *analysis,
    Dimensions_t   *dimensions,
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

    *sysmatrix.ColumnPointers = *(sysmatrix.ColumnPointers - 1) ;

    /* BOTTOM connected to Virtual Wall */

    if ( ! IS_FIRST_LAYER(layer_index) )
    {
        *sysmatrix.RowIndices++ = get_cell_offset_in_stack

            (dimensions, layer_index - 2, row_index, column_index) ;

        conductance = get_conductance_top

            (thermal_grid, dimensions, layer_index - 2, row_index, column_index) ;

        *sysmatrix.Values++  = -conductance ;
        diagonal_value +=  conductance ;

        (*sysmatrix.ColumnPointers)++ ;

#ifdef PRINT_SYSTEM_MATRIX
        fprintf (stderr,
           "  Bottom connected to Channel     \t%d\t% .4e\n",
            *(sysmatrix.RowIndices-1), *(sysmatrix.Values-1)) ;
#endif
    }

    /* BOTTOM connected to Channel */

    if ( ! IS_FIRST_LAYER(layer_index) )
    {
        *sysmatrix.RowIndices++ = get_cell_offset_in_stack

            (dimensions, layer_index - 1, row_index, column_index) ;

        conductance = get_conductance_top

            (thermal_grid, dimensions, layer_index - 1, row_index, column_index) ;

        *sysmatrix.Values++  = -conductance ;
        diagonal_value +=  conductance ;

        (*sysmatrix.ColumnPointers)++ ;

#ifdef PRINT_SYSTEM_MATRIX
        fprintf (stderr,
            "  Bottom connected to Virtual Wall     \t%d\t% .4e\n",
            *(sysmatrix.RowIndices-1), *(sysmatrix.Values-1)) ;
#endif
    }

    /* DIAGONAL */

    *sysmatrix.RowIndices++ = get_cell_offset_in_stack

        (dimensions, layer_index, row_index, column_index) ;

    *sysmatrix.Values = 0.0 ;

    if (analysis->AnalysisType == TDICE_ANALYSIS_TYPE_TRANSIENT)
    {
        *sysmatrix.Values = get_capacity

            (thermal_grid, dimensions, layer_index, row_index, column_index) ;

        *sysmatrix.Values /= analysis->StepTime ;
    }

    diagonal_pointer = sysmatrix.Values++ ;

    (*sysmatrix.ColumnPointers)++ ;

#ifdef PRINT_SYSTEM_MATRIX
    fprintf (stderr, "  diagonal\t%d\t", *(sysmatrix.RowIndices-1)) ;
    fgetpos (stderr, &diag_fposition) ;
    fprintf (stderr, "            ( + % .4e [capacity] )\n", *(sysmatrix.Values-1)) ;
#endif

    /* Top connected to Silicon */

    if ( ! IS_LAST_LAYER(layer_index, dimensions) )
    {
        *sysmatrix.RowIndices++ = get_cell_offset_in_stack

            (dimensions, layer_index + 1, row_index, column_index) ;

        conductance = get_conductance_bottom

            (thermal_grid, dimensions, layer_index + 1, row_index, column_index) ;

        *sysmatrix.Values++  = -conductance ;
        diagonal_value +=  conductance ;

        (*sysmatrix.ColumnPointers)++ ;

#ifdef PRINT_SYSTEM_MATRIX
        fprintf (stderr,
            "  Top connected to Silicon  \t%d\t% .4e\n",
            *(sysmatrix.RowIndices-1), *(sysmatrix.Values-1)) ;
#endif
    }

    /* DIAGONAL ELEMENT */

    *diagonal_pointer += diagonal_value ;

#ifdef PRINT_SYSTEM_MATRIX
    fgetpos (stderr, &last_fpos) ;
    fsetpos (stderr, &diag_fposition) ;
    fprintf (stderr, "% .4e", *diagonal_pointer) ;
    fsetpos (stderr, &last_fpos) ;

    fprintf (stderr, "  %d\n", *sysmatrix.ColumnPointers) ;
#endif

    sysmatrix.ColumnPointers++ ;

    return sysmatrix ;
}


/******************************************************************************/

static SystemMatrix_t add_virtual_wall_column_2rm
(
    SystemMatrix_t  sysmatrix,
    ThermalGrid_t  *thermal_grid,
    Analysis_t     *analysis,
    Dimensions_t   *dimensions,
    ChannelModel_t  channel_model,
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
        "add_virtual_wall_column   l %2d r %4d c %4d [%7d]\n",
        layer_index, row_index, column_index,
        get_cell_offset_in_stack (dimensions, layer_index, row_index, column_index)) ;
#endif

    *sysmatrix.ColumnPointers = *(sysmatrix.ColumnPointers - 1) ;

    /* BOTTOM */

    if ( ! IS_FIRST_LAYER(layer_index) )
    {
        *sysmatrix.RowIndices++ = get_cell_offset_in_stack

            (dimensions, layer_index - 1, row_index, column_index) ;

        conductance = get_conductance_bottom

            (thermal_grid, dimensions, layer_index, row_index, column_index) ;

        *sysmatrix.Values++  = -conductance ;
        diagonal_value +=  conductance ;

        (*sysmatrix.ColumnPointers)++ ;

#ifdef PRINT_SYSTEM_MATRIX
        fprintf (stderr,
        "  bottom  \t%d\t% .4e\n",
        *(sysmatrix.RowIndices-1), *(sysmatrix.Values-1)) ;
#endif
    }

    /* SOUTH */

    if (   channel_model == TDICE_CHANNEL_MODEL_MC_2RM
        && ! IS_FIRST_ROW(row_index) )
    {
        *sysmatrix.RowIndices++ = get_cell_offset_in_stack

            (dimensions, layer_index, row_index - 1, column_index) ;

        conductance = PARALLEL
        (
            get_conductance_south (thermal_grid, dimensions, layer_index, row_index, column_index),
            get_conductance_north (thermal_grid, dimensions, layer_index, row_index, column_index)
        ) ;

        *sysmatrix.Values++  = -conductance ;
        diagonal_value +=  conductance ;

        (*sysmatrix.ColumnPointers)++ ;

#ifdef PRINT_SYSTEM_MATRIX
        fprintf (stderr,
        "  south   \t%d\t% .4e\n", *(sysmatrix.RowIndices-1), *(sysmatrix.Values-1)) ;
#endif
    }

    /* DIAGONAL */

    *sysmatrix.RowIndices++ = get_cell_offset_in_stack

        (dimensions, layer_index, row_index, column_index) ;

    *sysmatrix.Values = 0.0 ;

    if (analysis->AnalysisType == TDICE_ANALYSIS_TYPE_TRANSIENT)
    {
        *sysmatrix.Values = get_capacity

            (thermal_grid, dimensions, layer_index, row_index, column_index) ;

        *sysmatrix.Values /= analysis->StepTime ;
    }

    diagonal_pointer = sysmatrix.Values++ ;

    (*sysmatrix.ColumnPointers)++ ;

#ifdef PRINT_SYSTEM_MATRIX
    fprintf (stderr, "  diagonal\t%d\t", *(sysmatrix.RowIndices-1)) ;
    fgetpos (stderr, &diag_fposition) ;
    fprintf (stderr, "            ( + % .4e [capacity] )\n", *(sysmatrix.Values-1)) ;
#endif

    /* NORTH */

    if (     channel_model == TDICE_CHANNEL_MODEL_MC_2RM
        && ! IS_LAST_ROW(row_index, dimensions) )
    {
        *sysmatrix.RowIndices++ = get_cell_offset_in_stack

            (dimensions, layer_index, row_index + 1, column_index) ;

        conductance = PARALLEL
        (
            get_conductance_north (thermal_grid, dimensions, layer_index, row_index, column_index),
            get_conductance_south (thermal_grid, dimensions, layer_index, row_index, column_index)
        ) ;

        *sysmatrix.Values++  = -conductance ;
        diagonal_value +=  conductance ;

        (*sysmatrix.ColumnPointers)++ ;

#ifdef PRINT_SYSTEM_MATRIX
        fprintf (stderr,
            "  north   \t%d\t% .4e\n",
            *(sysmatrix.RowIndices-1), *(sysmatrix.Values-1)) ;
#endif
    }

    /* TOP */

    if ( ! IS_LAST_LAYER(layer_index, dimensions) )
    {
        *sysmatrix.RowIndices++ = get_cell_offset_in_stack

            (dimensions, layer_index + 2, row_index, column_index) ;

        conductance = get_conductance_top

            (thermal_grid, dimensions, layer_index, row_index, column_index) ;

        *sysmatrix.Values++  = -conductance ;
        diagonal_value +=  conductance ;

        (*sysmatrix.ColumnPointers)++ ;

#ifdef PRINT_SYSTEM_MATRIX
        fprintf (stderr,
            "  top     \t%d\t% .4e\n",
            *(sysmatrix.RowIndices-1), *(sysmatrix.Values-1)) ;
#endif
    }

    /* DIAGONAL ELEMENT */

    *diagonal_pointer += diagonal_value ;

#ifdef PRINT_SYSTEM_MATRIX
    fgetpos (stderr, &last_fpos) ;
    fsetpos (stderr, &diag_fposition) ;
    fprintf (stderr, "% .4e", *diagonal_pointer) ;
    fsetpos (stderr, &last_fpos) ;

    fprintf (stderr, "  %d\n", *sysmatrix.ColumnPointers) ;
#endif

    sysmatrix.ColumnPointers++ ;

    return sysmatrix ;
}

/******************************************************************************/

void fill_system_matrix
(
    SystemMatrix_t *sysmatrix,
    ThermalGrid_t  *thermal_grid,
    Analysis_t     *analysis,
    Dimensions_t   *dimensions
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

    tmp_matrix.Size = sysmatrix->Size ;
    tmp_matrix.NNz  = sysmatrix->NNz ;

    tmp_matrix.ColumnPointers = sysmatrix->ColumnPointers ;
    tmp_matrix.RowIndices     = sysmatrix->RowIndices ;
    tmp_matrix.Values         = sysmatrix->Values ;

    tmp_matrix.ColumnPointers[0] = 0u ;

    tmp_matrix.ColumnPointers++ ;

    CellIndex_t lindex ;

    for (lindex = 0u ; lindex != thermal_grid->Size ; lindex++)
    {
        switch (thermal_grid->LayersProfile [lindex])
        {
            case TDICE_LAYER_SOLID :
            case TDICE_LAYER_SOURCE :
            case TDICE_LAYER_SOLID_CONNECTED_TO_AMBIENT :
            case TDICE_LAYER_SOURCE_CONNECTED_TO_AMBIENT :
            case TDICE_LAYER_SPREADER :
            case TDICE_LAYER_SINK :
            {

                FOR_EVERY_ROW (row_index, dimensions)
                {
                    FOR_EVERY_COLUMN (column_index, dimensions)
                    {
                        tmp_matrix = add_solid_column

                            (tmp_matrix, thermal_grid, analysis, dimensions,
                             lindex, row_index, column_index) ;

                    } // FOR_EVERY_COLUMN
                } // FOR_EVERY_ROW


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

                                (tmp_matrix, thermal_grid, analysis, dimensions,
                                 lindex, row_index, column_index) ;

                        else

                            tmp_matrix = add_solid_column

                                (tmp_matrix, thermal_grid, analysis, dimensions,
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

                            (tmp_matrix, thermal_grid, analysis, dimensions,
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

                            (tmp_matrix, thermal_grid, analysis, dimensions,
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

                            (tmp_matrix, thermal_grid, analysis, dimensions,
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

                            (tmp_matrix, thermal_grid, analysis, dimensions,
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

Error_t solve_sparse_linear_system (SystemMatrix_t *sysmatrix, SuperMatrix *b)
{
    dgstrs

        (NOTRANS, &sysmatrix->SLUMatrix_L, &sysmatrix->SLUMatrix_U,
         sysmatrix->SLU_PermutationMatrixC, sysmatrix->SLU_PermutationMatrixR,
         b, &sysmatrix->SLU_Stat, &sysmatrix->SLU_Info) ;

    if (sysmatrix->SLU_Info < 0)
    {
        fprintf (stderr,
            "Error (%d) while solving linear system\n", sysmatrix->SLU_Info) ;

        return TDICE_FAILURE ;
    }

    return TDICE_SUCCESS ;
}

/******************************************************************************/

void system_matrix_print (SystemMatrix_t sysmatrix, String_t file_name)
{
    FILE* file = fopen (file_name, "w") ;

    if (file == NULL)
    {
        fprintf(stderr, "Cannot open file %s\n", file_name) ;
        return ;
    }

    CellIndex_t row, column ;

    for (column = 0 ; column < sysmatrix.Size ; column++)

        for (row = sysmatrix.ColumnPointers[column] ;
             row < sysmatrix.ColumnPointers[column + 1] ;
             row++)

            fprintf (file, "%9d\t%9d\t%32.24f\n",
                sysmatrix.RowIndices[row] + 1, column + 1,
                sysmatrix.Values[row]) ;

    fclose (file) ;
}

/******************************************************************************/
