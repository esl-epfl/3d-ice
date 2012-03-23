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

#include "thermal_data.h"
#include "macros.h"

/******************************************************************************/

static void init_data (double* data, uint32_t size, double init_value)
{
    while (size--) *data++ = init_value ;
}

/******************************************************************************/

void init_thermal_data (ThermalData *tdata)
{
    tdata->Size = 0u ;

    tdata->Temperatures = NULL ;
    tdata->Sources      = NULL ;
    tdata->ThermalCells = NULL ;

    init_system_matrix (&tdata->SM_A) ;

    tdata->SLU_PermutationMatrixR = NULL ;
    tdata->SLU_PermutationMatrixC = NULL ;
    tdata->SLU_Etree              = NULL ;

    tdata->SLUMatrix_A.Store          = NULL ;
    tdata->SLUMatrix_A_Permuted.Store = NULL ;
    tdata->SLUMatrix_B.Store          = NULL ;
    tdata->SLUMatrix_L.Store          = NULL ;
    tdata->SLUMatrix_U.Store          = NULL ;

    tdata->SLU_Info = 0 ;

    StatInit (&tdata->SLU_Stat) ;

    set_default_options (&tdata->SLU_Options) ;

    tdata->SLU_Options.Fact            = DOFACT ;
    tdata->SLU_Options.PrintStat       = NO ;
    tdata->SLU_Options.Equil           = NO ;
    tdata->SLU_Options.SymmetricMode   = YES ;
    tdata->SLU_Options.ColPerm         = MMD_AT_PLUS_A ;
    tdata->SLU_Options.RowPerm         = NOROWPERM ;
    tdata->SLU_Options.DiagPivotThresh = 0.001 ;
}

/******************************************************************************/

Error_t fill_thermal_data
(
    ThermalData      *tdata,
    StackDescription *stkd,
    Analysis         *analysis
)
{
    int result ;

    tdata->Size = get_number_of_cells (stkd->Dimensions) ;

    /* Alloc and set temperatures */

    tdata->Temperatures =

        (Temperature_t*) malloc (sizeof(*tdata->Temperatures) * tdata->Size) ;

    if (tdata->Temperatures == NULL)

        goto temperatures_fail ;

    /* Set Temperatures to the initial thermal state and builds SLU vector B */

    init_data (tdata->Temperatures, tdata->Size, analysis->InitialTemperature) ;

    dCreate_Dense_Matrix  /* Vector B */

        (&tdata->SLUMatrix_B, tdata->Size, 1,
         tdata->Temperatures, tdata->Size,
         SLU_DN, SLU_D, SLU_GE) ;

    /* Alloc and fill the grid of thermal cells */

    tdata->ThermalCells = malloc (  sizeof(ThermalCell)
                                  * get_number_of_layers (stkd->Dimensions)
                                  * get_number_of_columns (stkd->Dimensions)) ;

    if (tdata->ThermalCells == NULL)

        goto thermal_cell_data_fail ;

    fill_thermal_cell_stack_description

        (tdata->ThermalCells, analysis, stkd) ;

    /* Alloc and set sources to zero */

    tdata->Sources = malloc (sizeof(*tdata->Sources) * tdata->Size) ;

    if (tdata->Sources == NULL)

        goto sources_fail ;

    init_data (tdata->Sources, tdata->Size, 0.0) ;

    /* Alloc and fill the system matrix and builds the SLU wrapper */

    result = alloc_system_matrix

        (&tdata->SM_A, tdata->Size, get_number_of_connections (stkd->Dimensions)) ;

    if (result == TDICE_FAILURE)

        goto system_matrix_fail ;

    fill_system_matrix_stack_description

        (tdata->SM_A, tdata->ThermalCells, stkd) ;

    dCreate_CompCol_Matrix

        (&tdata->SLUMatrix_A, tdata->Size, tdata->Size, tdata->SM_A.NNz,
                tdata->SM_A.Values,
         (int*) tdata->SM_A.RowIndices,
         (int*) tdata->SM_A.ColumnPointers,
         SLU_NC, SLU_D, SLU_GE) ;

    /* Alloc SLU permutation matrices and elimination tree */

    tdata->SLU_PermutationMatrixR = malloc (sizeof(int) * tdata->Size) ;

    if (tdata->SLU_PermutationMatrixR == NULL )

        goto slu_perm_r_fail ;

    tdata->SLU_PermutationMatrixC = malloc(sizeof(int) * tdata->Size) ;

    if (tdata->SLU_PermutationMatrixC == NULL )

        goto slu_perm_c_fail ;

    tdata->SLU_Etree = malloc (sizeof(int) * tdata->Size) ;

    if (tdata->SLU_Etree == NULL)

        goto slu_etree_fail ;


    /* Execute the RAC=LU factorization of the matrix A */

    get_perm_c

        (tdata->SLU_Options.ColPerm, &tdata->SLUMatrix_A,
         tdata->SLU_PermutationMatrixC) ;

    sp_preorder

        (&tdata->SLU_Options, &tdata->SLUMatrix_A,
         tdata->SLU_PermutationMatrixC, tdata->SLU_Etree,
         &tdata->SLUMatrix_A_Permuted) ;

    dgstrf

        (&tdata->SLU_Options, &tdata->SLUMatrix_A_Permuted,
         sp_ienv(2), sp_ienv(1), /* relax and panel size */
         tdata->SLU_Etree,
         NULL, 0,                /* work and lwork */
         tdata->SLU_PermutationMatrixC, tdata->SLU_PermutationMatrixR,
         &tdata->SLUMatrix_L, &tdata->SLUMatrix_U,
         &tdata->SLU_Stat, &tdata->SLU_Info) ;

    if (tdata->SLU_Info == 0)
    {
        tdata->SLU_Options.Fact = FACTORED ;

        return TDICE_SUCCESS ;
    }
    else
    {
        fprintf (stderr, "SuperLu factorization error %d\n", tdata->SLU_Info) ;

        return TDICE_FAILURE ;
    }

    /* Free if malloc errors */

slu_etree_fail :

    FREE_POINTER (free, tdata->SLU_PermutationMatrixC) ;

slu_perm_c_fail :

    FREE_POINTER (free, tdata->SLU_PermutationMatrixR) ;

slu_perm_r_fail :

    Destroy_SuperMatrix_Store (&tdata->SLUMatrix_A) ;
    free_system_matrix        (&tdata->SM_A) ;

system_matrix_fail :

    FREE_POINTER (free, tdata->Sources) ;

sources_fail :

    FREE_POINTER (free, tdata->ThermalCells) ;

thermal_cell_data_fail :

    Destroy_SuperMatrix_Store (&tdata->SLUMatrix_B) ;
    FREE_POINTER              (free, tdata->Temperatures) ;

temperatures_fail :

    fprintf (stderr, "Malloc error\n") ;

    return TDICE_FAILURE ;
}

/******************************************************************************/

void free_thermal_data (ThermalData* tdata)
{
    FREE_POINTER (free, tdata->Temperatures) ;
    FREE_POINTER (free, tdata->Sources) ;
    FREE_POINTER (free, tdata->ThermalCells) ;

    FREE_POINTER (free, tdata->SLU_PermutationMatrixR) ;
    FREE_POINTER (free, tdata->SLU_PermutationMatrixC) ;
    FREE_POINTER (free, tdata->SLU_Etree) ;

    StatFree (&tdata->SLU_Stat) ;

    Destroy_SuperMatrix_Store (&tdata->SLUMatrix_A) ;
    free_system_matrix        (&tdata->SM_A) ;

    Destroy_SuperMatrix_Store (&tdata->SLUMatrix_B) ;

    if (tdata->SLU_Options.Fact != DOFACT )
    {
        Destroy_CompCol_Permuted (&tdata->SLUMatrix_A_Permuted) ;
        Destroy_SuperNode_Matrix (&tdata->SLUMatrix_L) ;
        Destroy_CompCol_Matrix   (&tdata->SLUMatrix_U) ;
    }
}

/******************************************************************************/

void reset_thermal_state (ThermalData *tdata, Analysis *analysis)
{
    init_data (tdata->Temperatures, tdata->Size, analysis->InitialTemperature) ;
}

/******************************************************************************/

static void fill_system_vector
(
    Dimensions    *dimensions,
    double        *vector,
    Source_t      *sources,
    ThermalCell   *thermal_cells,
    Temperature_t *temperatures
)
{
#ifdef PRINT_SYSTEM_VECTOR
    Temperature_t old ;
#endif

    CellIndex_t ncolumns = get_number_of_columns (dimensions) ;

    FOR_EVERY_LAYER (layer, dimensions)
    {
        FOR_EVERY_ROW (row, dimensions)
        {
            ThermalCell *tmp = thermal_cells ;

            FOR_EVERY_COLUMN (column, dimensions)
            {

#ifdef PRINT_SYSTEM_VECTOR
                old = *temperatures ;
#endif

                *vector++ = *sources++ + tmp++->Capacity
                                         * *temperatures++ ;

#ifdef PRINT_SYSTEM_VECTOR
                fprintf (stderr,
                    " l %2d r %4d c %4d [%7d] | %e [b] = %e [s] + %e [c] * %e [t]\n",
                    layer, row, column,
                    get_cell_offset_in_stack (dimensions, layer, row, column),
                    *(vector-1), *(sources-1), (tmp-1)->Capacity, old) ;
#endif

            } // FOR_EVERY_COLUMN
        } // FOR_EVERY_ROW

        thermal_cells += ncolumns ;

    } // FOR_EVERY_LAYER
}

/******************************************************************************/

static void fill_system_vector_steady
(
    Dimensions *dimensions,
    double     *vector,
    Source_t   *sources
)
{
    FOR_EVERY_LAYER (layer, dimensions)
    {
        FOR_EVERY_ROW (row, dimensions)
        {
            FOR_EVERY_COLUMN (column, dimensions)
            {
                *vector++ =   *sources++ ;

#ifdef PRINT_SYSTEM_VECTOR
                fprintf (stderr,
                    " l %2d r %4d c %4d [%7d] | %e [b] = %e [s]\n",
                    layer, row, column,
                    get_cell_offset_in_stack (dimensions, layer, row, column),
                    *(vector-1), *(sources-1)) ;
#endif

            } // FOR_EVERY_COLUMN
        } // FOR_EVERY_ROW
  } // FOR_EVERY_LAYER
}

/******************************************************************************/

SimResult_t emulate_step
(
    ThermalData      *tdata,
    StackDescription *stkd,
    Analysis         *analysis
)
{
    if (analysis->AnalysisType != TDICE_ANALYSIS_TYPE_TRANSIENT)

        return TDICE_WRONG_CONFIG ;

    if (slot_completed (analysis) == true)
    {
        Error_t result = fill_sources_stack_description

            (tdata->Sources, tdata->ThermalCells, stkd) ;

        if (result == TDICE_FAILURE)

            return TDICE_END_OF_SIMULATION ;
    }

    fill_system_vector

        (stkd->Dimensions, tdata->Temperatures,
         tdata->Sources, tdata->ThermalCells, tdata->Temperatures) ;

    dgstrs

        (NOTRANS, &tdata->SLUMatrix_L, &tdata->SLUMatrix_U,
         tdata->SLU_PermutationMatrixC, tdata->SLU_PermutationMatrixR,
         &tdata->SLUMatrix_B, &tdata->SLU_Stat, &tdata->SLU_Info) ;

    if (tdata->SLU_Info < 0)
    {
        fprintf (stderr,
            "Error (%d) while solving linear system\n", tdata->SLU_Info) ;

        return TDICE_SOLVER_ERROR ;
    }

    analysis->CurrentTime++ ;

    if (slot_completed (analysis) == false)

        return TDICE_STEP_DONE ;

    else

        return TDICE_SLOT_DONE ;
}

/******************************************************************************/

SimResult_t emulate_slot
(
    ThermalData      *tdata,
    StackDescription *stkd,
    Analysis         *analysis
)
{
    SimResult_t result ;

    do
    {

        result = emulate_step (tdata, stkd, analysis) ;

    }   while (result == TDICE_STEP_DONE) ;

    return result ;
}

/******************************************************************************/

SimResult_t emulate_steady
(
    ThermalData      *tdata,
    StackDescription *stkd,
    Analysis         *analysis
)
{
    if (analysis->AnalysisType != TDICE_ANALYSIS_TYPE_STEADY)

        return TDICE_WRONG_CONFIG ;

    Error_t result = fill_sources_stack_description

        (tdata->Sources, tdata->ThermalCells, stkd) ;

    if (result == TDICE_FAILURE)
    {
        fprintf (stderr,

            "Warning: no power trace given for steady state simulation\n") ;

        return TDICE_END_OF_SIMULATION ;
    }

    fill_system_vector_steady

        (stkd->Dimensions, tdata->Temperatures, tdata->Sources) ;

    dgstrs

        (NOTRANS, &tdata->SLUMatrix_L, &tdata->SLUMatrix_U,
         tdata->SLU_PermutationMatrixC, tdata->SLU_PermutationMatrixR,
         &tdata->SLUMatrix_B, &tdata->SLU_Stat, &tdata->SLU_Info) ;

    if (tdata->SLU_Info < 0)
    {
        fprintf (stderr,

            "Error (%d) while solving linear system\n", tdata->SLU_Info) ;

        return TDICE_SOLVER_ERROR ;
    }

    return TDICE_END_OF_SIMULATION ;
}

/******************************************************************************/

Error_t update_coolant_flow_rate
(
  ThermalData      *tdata,
  StackDescription *stkd,
  CoolantFR_t       new_flow_rate
)
{
    stkd->Channel->Coolant.FlowRate = FLOW_RATE_FROM_MLMIN_TO_UM3SEC(new_flow_rate) ;

    fill_system_matrix_stack_description

        (tdata->SM_A, tdata->ThermalCells, stkd) ;  // TODO replace with "update"

    tdata->SLU_Options.Fact = SamePattern_SameRowPerm ;

    dgstrf

        (&tdata->SLU_Options, &tdata->SLUMatrix_A_Permuted,
         sp_ienv(2), sp_ienv(1), /* relax and panel size */
         tdata->SLU_Etree,
         NULL, 0,                /* work and lwork */
         tdata->SLU_PermutationMatrixC, tdata->SLU_PermutationMatrixR,
         &tdata->SLUMatrix_L, &tdata->SLUMatrix_U,
         &tdata->SLU_Stat, &tdata->SLU_Info) ;

    if (tdata->SLU_Info == 0)
    {
        tdata->SLU_Options.Fact = FACTORED ;

        FOR_EVERY_ELEMENT_IN_LIST_NEXT

            (StackElement, stack_element, stkd->BottomStackElement)

            if (stack_element->Type == TDICE_STACK_ELEMENT_CHANNEL)

                fill_sources_channel

                    (tdata->Sources, stkd->Dimensions,
                    stack_element->Offset, stack_element->Pointer.Channel) ;

        return TDICE_SUCCESS ;
    }
    else
    {
        fprintf (stderr, "SuperLu factorization error %d\n", tdata->SLU_Info) ;

        return TDICE_FAILURE ;
    }
}

/******************************************************************************/

Temperature_t get_cell_temperature
(
    ThermalData      *tdata,
    StackDescription *stkd,
    CellIndex_t       layer_index,
    CellIndex_t       row_index,
    CellIndex_t       column_index
)
{
    CellIndex_t id = get_cell_offset_in_stack

                     (stkd->Dimensions, layer_index, row_index, column_index) ;

    if (id >= get_number_of_cells (stkd->Dimensions))

        return 0.0 ;

    else

        return *(tdata->Temperatures + id) ;
}

/******************************************************************************/

Error_t print_thermal_map
(
    ThermalData      *tdata,
    StackDescription *stkd,
    String_t          stack_element_id,
    String_t          file_name
)
{
    StackElement *stack_element = find_stack_element_in_list

         (stkd->BottomStackElement, stack_element_id) ;

    if (stack_element == NULL)

        return TDICE_FAILURE ;

    FILE *output_file = fopen (file_name, "w") ;

    if (output_file == NULL)
    {
        fprintf (stderr, "Unable to open output file %s\n", file_name) ;

        return TDICE_FAILURE ;
    }

    print_thermal_map_stack_element

        (stack_element, stkd->Dimensions, tdata->Temperatures, output_file) ;

    fclose (output_file) ;

    return TDICE_SUCCESS ;
}
