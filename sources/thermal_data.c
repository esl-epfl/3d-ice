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

void init_thermal_data (ThermalData_t *tdata)
{
    tdata->Size = 0u ;

    tdata->Temperatures = NULL ;
    tdata->Sources      = NULL ;

    init_thermal_grid (&tdata->ThermalGrid) ;

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
    ThermalData_t      *tdata,
    StackDescription_t *stkd,
    Analysis_t         *analysis
)
{
    Error_t result ;

    tdata->Size = get_number_of_cells (stkd->Dimensions) ;

    /* Alloc and set temperatures */

    tdata->Temperatures =

        (Temperature_t*) malloc (sizeof(Temperature_t) * tdata->Size) ;

    if (tdata->Temperatures == NULL)

        goto temperatures_fail ;

    /* Set Temperatures to the initial thermal state and builds SLU vector B */

    init_data (tdata->Temperatures, tdata->Size, analysis->InitialTemperature) ;

    dCreate_Dense_Matrix  /* Vector B */

        (&tdata->SLUMatrix_B, tdata->Size, 1,
         tdata->Temperatures, tdata->Size,
         SLU_DN, SLU_D, SLU_GE) ;

    /* Alloc and fill the thermal grid */

    result = alloc_thermal_grid

        (&tdata->ThermalGrid, get_number_of_layers (stkd->Dimensions)) ;

    if (result == TDICE_FAILURE)

        goto thermal_grid_fail ;

    fill_thermal_grid (&tdata->ThermalGrid, stkd, analysis) ;

    /* Alloc and set sources to zero */

    tdata->Sources = (Source_t *) malloc (sizeof(Source_t) * tdata->Size) ;

    if (tdata->Sources == NULL)

        goto sources_fail ;

    init_data (tdata->Sources, tdata->Size, 0.0) ;

    /* Alloc and fill the system matrix and builds the SLU wrapper */

    result = alloc_system_matrix

        (&tdata->SM_A, tdata->Size, get_number_of_connections (stkd->Dimensions)) ;

    if (result == TDICE_FAILURE)

        goto system_matrix_fail ;

    fill_system_matrix (&tdata->SM_A, &tdata->ThermalGrid, stkd->Dimensions) ;

    dCreate_CompCol_Matrix

        (&tdata->SLUMatrix_A, tdata->Size, tdata->Size, tdata->SM_A.NNz,
                tdata->SM_A.Values,
         (int*) tdata->SM_A.RowIndices,
         (int*) tdata->SM_A.ColumnPointers,
         SLU_NC, SLU_D, SLU_GE) ;

    /* Alloc SLU permutation matrices and elimination tree */

    tdata->SLU_PermutationMatrixR = (int *) malloc (sizeof(int) * tdata->Size) ;

    if (tdata->SLU_PermutationMatrixR == NULL )

        goto slu_perm_r_fail ;

    tdata->SLU_PermutationMatrixC = (int *) malloc(sizeof(int) * tdata->Size) ;

    if (tdata->SLU_PermutationMatrixC == NULL )

        goto slu_perm_c_fail ;

    tdata->SLU_Etree = (int *) malloc (sizeof(int) * tdata->Size) ;

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

    free_thermal_grid (&tdata->ThermalGrid) ;

thermal_grid_fail:

    Destroy_SuperMatrix_Store (&tdata->SLUMatrix_B) ;
    FREE_POINTER              (free, tdata->Temperatures) ;

temperatures_fail :

    fprintf (stderr, "Malloc error\n") ;

    return TDICE_FAILURE ;
}

/******************************************************************************/

Error_t update_source_vector
(
    ThermalData_t      *tdata,
    StackDescription_t *stkd
)
{
#ifdef PRINT_SOURCES
    fprintf (stderr,
        "update_source_vector ( l %d r %d c %d )\n",
        get_number_of_layers  (stkd->Dimensions),
        get_number_of_rows    (stkd->Dimensions),
        get_number_of_columns (stkd->Dimensions)) ;
#endif

    // reset all the source vector to 0

    CellIndex_t ccounter ;
    CellIndex_t ncells = get_number_of_cells (stkd->Dimensions) ;

    for (ccounter = 0 ; ccounter != ncells ; ccounter++)

        tdata->Sources [ ccounter ] = (Source_t) 0.0 ;

    // if used, sets the sources due to the heatsink (overwrites all cells in
    // the last layer) as first operation. Then the die will add its sources
    // only in those cells covered by the floorplan.
    // Reverse ordering works as long as every stack element knows where it
    // should access the source vector (offset)

    FOR_EVERY_ELEMENT_IN_LIST_PREV

    (StackElement_t, stack_element, stkd->TopStackElement)
    {
        CellIndex_t layer_index = stack_element->Offset ;

        switch (stack_element->Type)
        {
            case TDICE_STACK_ELEMENT_DIE :
            {
                layer_index += stack_element->Pointer.Die->SourceLayerOffset ;

                Source_t *sources = tdata->Sources +

                    get_cell_offset_in_stack (stkd->Dimensions, layer_index, 0, 0) ;

                Error_t error = fill_sources_floorplan

                    (sources, stkd->Dimensions, stack_element->Floorplan) ;

                if (error == TDICE_FAILURE)

                    return TDICE_FAILURE ;

                break ;
            }
            case TDICE_STACK_ELEMENT_HEATSINK :
            {
                layer_index += stack_element->Pointer.HeatSink->SourceLayerOffset ;

                CellIndex_t cell_index =

                    get_cell_offset_in_stack (stkd->Dimensions, layer_index, 0, 0) ;

                Source_t *sources = tdata->Sources + cell_index ;

                FOR_EVERY_ROW (row, stkd->Dimensions)
                {
                    FOR_EVERY_COLUMN (column, stkd->Dimensions)
                    {
                        *sources = stack_element->Pointer.HeatSink->AmbientTemperature
                                   * get_conductance_top

                                     (&tdata->ThermalGrid, stkd->Dimensions, layer_index, row, column) ;

#ifdef PRINT_SOURCES
                        fprintf (stderr,
                            "solid  cell  |  l %2d r %4d c %4d [%7d] | = %f * %.5e = %.5e\n",
                            layer_index, row, column, cell_index++,
                            stack_element->Pointer.HeatSink->AmbientTemperature,
                            get_conductance_top (&tdata->ThermalGrid, stkd->Dimensions,
                                                 layer_index, row, column),
                            *sources) ;
#endif

                        sources++ ;

                    } // FOR_EVERY_COLUMN
                } // FOR_EVERY_ROW

                break ;
            }
            case TDICE_STACK_ELEMENT_LAYER :

                break ;

            case TDICE_STACK_ELEMENT_CHANNEL :
            {
                layer_index += stack_element->Pointer.Channel->SourceLayerOffset ;

                Source_t *sources = tdata->Sources +

                    get_cell_offset_in_stack (stkd->Dimensions, layer_index, 0, 0) ;

                FOR_EVERY_COLUMN (column_index, stkd->Dimensions)
                {
                    if (IS_CHANNEL_COLUMN (stack_element->Pointer.Channel->ChannelModel, column_index) == true)
                    {
                        *sources =   (Source_t) 2.0
                                   * get_convective_term
                                     (stack_element->Pointer.Channel, stkd->Dimensions, layer_index, 0, column_index)
                                   * stack_element->Pointer.Channel->Coolant.TIn ;

#ifdef PRINT_SOURCES
                        fprintf (stderr,
                            "liquid cell  | r %4d c    0 | l %6.1f w %6.1f "
                            " | %.5e [source] = 2 * %.2f [Tin] * %.5e [C]\n",
                            column_index,
                            get_cell_length (stkd->Dimensions, column_index), get_cell_width (stkd->Dimensions, 0),
                            *sources, stack_element->Pointer.Channel->Coolant.TIn,
                            get_convective_term
                            (stack_element->Pointer.Channel, stkd->Dimensions, layer_index, 0, column_index)) ;
#endif
                    }

                    sources++ ;

                } // FOR_EVERY_COLUMN

                break ;
            }
            case TDICE_STACK_ELEMENT_NONE :

                fprintf (stderr, "Error! Found stack element with unset type\n") ;
                break ;

            default :

                fprintf (stderr, "Error! Unknown stack element type %d\n", stack_element->Type) ;

        } /* switch stack_element->Type */
    }

    return TDICE_SUCCESS ;
}

/******************************************************************************/

void free_thermal_data (ThermalData_t *tdata)
{
    FREE_POINTER (free, tdata->Temperatures) ;
    FREE_POINTER (free, tdata->Sources) ;

    FREE_POINTER (free, tdata->SLU_PermutationMatrixR) ;
    FREE_POINTER (free, tdata->SLU_PermutationMatrixC) ;
    FREE_POINTER (free, tdata->SLU_Etree) ;

    free_thermal_grid (&tdata->ThermalGrid) ;

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

void reset_thermal_state (ThermalData_t *tdata, Analysis_t *analysis)
{
    init_data (tdata->Temperatures, tdata->Size, analysis->InitialTemperature) ;
}

/******************************************************************************/

static void fill_system_vector
(
    Dimensions_t  *dimensions,
    double        *vector,
    Source_t      *sources,
    ThermalGrid_t *thermal_grid,
    Temperature_t *temperatures
)
{
#ifdef PRINT_SYSTEM_VECTOR
    Temperature_t old ;
#endif

    FOR_EVERY_LAYER (layer, dimensions)
    {
        FOR_EVERY_ROW (row, dimensions)
        {
            FOR_EVERY_COLUMN (column, dimensions)
            {

#ifdef PRINT_SYSTEM_VECTOR
                old = *temperatures ;
#endif

                *vector++ =   *sources++
                            + get_capacity (thermal_grid, dimensions, layer, row, column)
                            * *temperatures++ ;

#ifdef PRINT_SYSTEM_VECTOR
                fprintf (stderr,
                    " l %2d r %4d c %4d [%7d] | %e [b] = %e [s] + %e [c] * %e [t]\n",
                    layer, row, column,
                    get_cell_offset_in_stack (dimensions, layer, row, column),
                    *(vector-1), *(sources-1),
                    get_capacity (thermal_grid, dimensions, layer, row, column), old) ;
#endif

            } // FOR_EVERY_COLUMN
        } // FOR_EVERY_ROW
    } // FOR_EVERY_LAYER
}

/******************************************************************************/

static void fill_system_vector_steady
(
    Dimensions_t *dimensions,
    double       *vector,
    Source_t     *sources
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
    ThermalData_t      *tdata,
    StackDescription_t *stkd,
    Analysis_t         *analysis
)
{
    if (analysis->AnalysisType != TDICE_ANALYSIS_TYPE_TRANSIENT)

        return TDICE_WRONG_CONFIG ;

    if (slot_completed (analysis) == true)
    {
        Error_t result = update_source_vector (tdata, stkd) ;

        if (result == TDICE_FAILURE)

            return TDICE_END_OF_SIMULATION ;
    }

    fill_system_vector

        (stkd->Dimensions, tdata->Temperatures,
         tdata->Sources, &tdata->ThermalGrid, tdata->Temperatures) ;

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
    ThermalData_t      *tdata,
    StackDescription_t *stkd,
    Analysis_t         *analysis
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
    ThermalData_t      *tdata,
    StackDescription_t *stkd,
    Analysis_t         *analysis
)
{
    if (analysis->AnalysisType != TDICE_ANALYSIS_TYPE_STEADY)

        return TDICE_WRONG_CONFIG ;

    Error_t result = update_source_vector (tdata, stkd) ;

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
  ThermalData_t      *tdata,
  StackDescription_t *stkd,
  CoolantFR_t         new_flow_rate
)
{
    stkd->Channel->Coolant.FlowRate = FLOW_RATE_FROM_MLMIN_TO_UM3SEC(new_flow_rate) ;

    // TODO replace with "update"

    fill_system_matrix (&tdata->SM_A, &tdata->ThermalGrid, stkd->Dimensions) ;

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

            (StackElement_t, stack_element, stkd->BottomStackElement)

            if (stack_element->Type == TDICE_STACK_ELEMENT_CHANNEL)
            {
                CellIndex_t layer_index = stack_element->Pointer.Channel->SourceLayerOffset ;

                Source_t *sources = tdata->Sources +

                    get_cell_offset_in_stack (stkd->Dimensions, layer_index, 0, 0) ;

                FOR_EVERY_COLUMN (column_index, stkd->Dimensions)
                {
                    if (IS_CHANNEL_COLUMN (stack_element->Pointer.Channel->ChannelModel, column_index) == true)
                    {
                        *sources =   (Source_t) 2.0
                                   * get_convective_term
                                     (stack_element->Pointer.Channel, stkd->Dimensions, layer_index, 0, column_index)
                                   * stack_element->Pointer.Channel->Coolant.TIn ;

#ifdef PRINT_SOURCES
                        fprintf (stderr,
                            "liquid cell  | r %4d c    0 | l %6.1f w %6.1f "
                            " | %.5e [source] = 2 * %.2f [Tin] * %.5e [C]\n",
                            column_index,
                            get_cell_length (stkd->Dimensions, column_index), get_cell_width (stkd->Dimensions, 0),
                            *sources, stack_element->Pointer.Channel->Coolant.TIn,
                            get_convective_term
                            (stack_element->Pointer.Channel, stkd->Dimensions, layer_index, 0, column_index)) ;
#endif
                    }

                    sources++ ;

                } // FOR_EVERY_COLUMN
            }

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
    ThermalData_t      *tdata,
    StackDescription_t *stkd,
    CellIndex_t         layer_index,
    CellIndex_t         row_index,
    CellIndex_t         column_index
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
    ThermalData_t      *tdata,
    StackDescription_t *stkd,
    String_t            stack_element_id,
    String_t            file_name
)
{
    StackElement_t *stack_element = find_stack_element_in_list

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
