/******************************************************************************
 * This file is part of 3D-ICE, version 2.2.4 .                               *
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

#include <stdio.h> // For the file type FILE

#include "thermal_data.h"
#include "macros.h"

/******************************************************************************/

static void init_data (double* data, uint32_t size, double init_value)
{
    while (size--) *data++ = init_value ;
}

/******************************************************************************/

void thermal_data_init (ThermalData_t *tdata)
{
    tdata->Size = (CellIndex_t) 0u ;

    tdata->Temperatures = NULL ;

    thermal_grid_init  (&tdata->ThermalGrid) ;
    power_grid_init    (&tdata->PowerGrid) ;
    system_matrix_init (&tdata->SM_A) ;

    tdata->SLUMatrix_B.Store = NULL ;
}

/******************************************************************************/

Error_t thermal_data_build
(
    ThermalData_t      *tdata,
    StackElementList_t *stack_elements_list,
    Dimensions_t       *dimensions,
    Analysis_t         *analysis
)
{
    Error_t result ;

    tdata->Size = get_number_of_cells (dimensions) ;

    /* Alloc and set temperatures */

    tdata->Temperatures =

        (Temperature_t*) malloc (sizeof(Temperature_t) * tdata->Size) ;

    if (tdata->Temperatures == NULL)
    {
        fprintf (stderr, "Cannot malloc temperature array\n") ;

        return TDICE_FAILURE ;
    }

    /* Set Temperatures to the initial thermal state and builds SLU vector B */

    init_data (tdata->Temperatures, tdata->Size, analysis->InitialTemperature) ;

    dCreate_Dense_Matrix  /* Vector B */

        (&tdata->SLUMatrix_B, tdata->Size, 1,
         tdata->Temperatures, tdata->Size,
         SLU_DN, SLU_D, SLU_GE) ;

    /* Alloc and fill the thermal grid */

    result = thermal_grid_build (&tdata->ThermalGrid, dimensions) ;

    if (result == TDICE_FAILURE)
    {
        fprintf (stderr, "Cannot malloc thermal grid\n") ;

        Destroy_SuperMatrix_Store (&tdata->SLUMatrix_B) ;

        free (tdata->Temperatures) ;

        return TDICE_FAILURE ;
    }

    thermal_grid_fill (&tdata->ThermalGrid, stack_elements_list) ;

    /* Alloc and fill the power grid */

    result = power_grid_build (&tdata->PowerGrid, dimensions) ;

    if (result == TDICE_FAILURE)
    {
        fprintf (stderr, "Cannot malloc power grid\n") ;

        Destroy_SuperMatrix_Store (&tdata->SLUMatrix_B) ;

        free (tdata->Temperatures) ;

        thermal_grid_destroy (&tdata->ThermalGrid) ;

        return TDICE_FAILURE ;
    }

    power_grid_fill

        (&tdata->PowerGrid, &tdata->ThermalGrid, stack_elements_list, dimensions) ;

    /* Alloc and fill the system matrix and builds the SLU wrapper */

    result = system_matrix_build

        (&tdata->SM_A, tdata->Size, get_number_of_connections (dimensions)) ;

    if (result == TDICE_FAILURE)
    {
        fprintf (stderr, "Cannot malloc syatem matrix\n") ;

        Destroy_SuperMatrix_Store (&tdata->SLUMatrix_B) ;

        free (tdata->Temperatures) ;

        thermal_grid_destroy (&tdata->ThermalGrid) ;
        power_grid_destroy   (&tdata->PowerGrid) ;

        return TDICE_FAILURE ;
    }

    fill_system_matrix

        (&tdata->SM_A, &tdata->ThermalGrid, analysis, dimensions) ;

    result = do_factorization (&tdata->SM_A) ;

    if (result == TDICE_FAILURE)
    {
        thermal_data_destroy (tdata) ;

        return TDICE_FAILURE ;
    }
    
    /* Set the pluggable heatsing temperatures to initial thermal state */
    
    HeatSink_t *sink = tdata->ThermalGrid.TopHeatSink;
    if(sink && sink->SinkModel == TDICE_HEATSINK_TOP_PLUGGABLE)
    {
        unsigned int size = sink->NColumns * sink->NRows;
        init_data(sink->CurrentTemperatures,  size, analysis->InitialTemperature);
        init_data(sink->PreviousTemperatures, size, analysis->InitialTemperature);
    }

    return TDICE_SUCCESS ;
}

/******************************************************************************/

void thermal_data_destroy (ThermalData_t *tdata)
{
    free (tdata->Temperatures) ;

    thermal_grid_destroy (&tdata->ThermalGrid) ;
    power_grid_destroy   (&tdata->PowerGrid) ;

    system_matrix_destroy (&tdata->SM_A) ;

    Destroy_SuperMatrix_Store (&tdata->SLUMatrix_B) ;

    thermal_data_init (tdata) ;
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
    HeatSink_t    *topSink,
    double        *vector,
    Source_t      *sources,
    Capacity_t    *capacities,
    Temperature_t *temperatures,
    Time_t         step_time
)
{
#ifdef PRINT_SYSTEM_VECTOR
    Temperature_t old ;
#endif

    CellIndex_t layer ;
    CellIndex_t row ;
    CellIndex_t column ;

    for (layer = first_layer (dimensions) ; layer <= last_layer (dimensions) ; layer++)
    {
        for (row = first_row (dimensions) ; row <= last_row (dimensions) ; row++)
        {
            for (column = first_column (dimensions) ; column <= last_column (dimensions) ; column++)
            {

#ifdef PRINT_SYSTEM_VECTOR
                old = *temperatures ;
#endif

                *vector++ = *sources++
                            + (*capacities++ / step_time)
                              * *temperatures++ ;

#ifdef PRINT_SYSTEM_VECTOR
                fprintf (stderr,
                    " l %2d r %4d c %4d [%7d] | %e [b] = %e [s] + %e [c] * %e [t]\n",
                    layer, row, column,
                    get_cell_offset_in_stack (dimensions, layer, row, column),
                    *(vector-1), *(sources-1), *(capacities-1), old) ;
#endif

            } // FOR_EVERY_COLUMN
        } // FOR_EVERY_ROW
    } // FOR_EVERY_LAYER
    
    // Copy the rest of the vector
    if(topSink && topSink->SinkModel == TDICE_HEATSINK_TOP_PLUGGABLE)
    {
        for(row = 0; row < topSink->NRows; row++)
        {
            for(column = 0; column < topSink->NColumns; column++)
            {
#ifdef PRINT_SYSTEM_VECTOR
                old = *temperatures ;
#endif

                *vector++ = *sources++
                            + (*capacities++ / step_time)
                              * *temperatures++ ;

#ifdef PRINT_SYSTEM_VECTOR
                fprintf (stderr,
                    "      r %4d c %4d [%7d] | %e [b] = %e [s] + %e [c] * %e [t]\n",
                    row, column,
                    get_spreader_cell_offset (dimensions, topSink, row, column),
                    *(vector-1), *(sources-1), *(capacities-1), old) ;
#endif
            }
        }
    }
}

/******************************************************************************/

static void fill_system_vector_steady
(
    Dimensions_t *dimensions,
    double       *vector,
    Source_t     *sources
)
{
    CellIndex_t layer ;
    CellIndex_t row ;
    CellIndex_t column ;

    for (layer = first_layer (dimensions) ; layer <= last_layer (dimensions) ; layer++)
    {
        for (row = first_row (dimensions) ; row <= last_row (dimensions) ; row++)
        {
            for (column = first_column (dimensions) ; column <= last_column (dimensions) ; column++)
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

Error_t pluggable_heatsink(ThermalData_t *tdata, Dimensions_t *dimensions,
        Analysis_t *analysis)
{
    // In this function we compute the heat flow using the heatsink temperatures
    // at the previous time step, then we compute the current temperatures.
    // If the previous temperatures differ too much from the current ones,
    // the simulation may provide incorrect results
    const double threshold = 1.0;
    
    // We have something to do only if the we're using the pluggable heatsink model
    HeatSink_t *sink = tdata->ThermalGrid.TopHeatSink;
    if(sink == NULL || sink->SinkModel != TDICE_HEATSINK_TOP_PLUGGABLE)
            return TDICE_SUCCESS;
    
    // Compute heat flow from spreader to sink
    double *temp = sink->CurrentTemperatures;
    double *flow = sink->CurrentHeatFlow;
    Conductance_t conductance = get_spreader_conductance(sink);
    CellIndex_t row, col;
    for(row = 0; row < sink->NRows; row++)
        for(col = 0; col < sink->NColumns; col++)
            *flow++= (tdata->Temperatures[
                get_spreader_cell_offset(dimensions,sink,row,col)] - *temp++)
                * conductance;
    
    // Call the pluggable heat sink function to compute the temperatures
    // at the interface between the spreader and sink
    unsigned int size = sink->NColumns * sink->NRows;
    if(sink->PluggableHeatsink(
        sink->CurrentHeatFlow,
        sink->CurrentTemperatures,
        sink->NRows,
        sink->NColumns,
        size,
        analysis->StepTime) == false)
    {
        fprintf(stderr, "Error: pluggable heatsink callback failed\n");
        return TDICE_FAILURE;
    }
    
    // Compare the computed temperatures against the previous ones
    unsigned int i;
    for(i = 0; i < size; i++)
    {
        if(abs(sink->CurrentTemperatures[i] - sink->PreviousTemperatures[i]) <= threshold)
            continue;
        fprintf(stderr, "Warning: the integration time step may be too small\n");
        break;
    }
    
    // Update the previous temperatures
    memcpy(sink->PreviousTemperatures, sink->CurrentTemperatures, size * sizeof(double));
    
    // Update the sources vector using the temperatures at the heatsink interface
    Source_t *sources = tdata->PowerGrid.Sources;
    sources += get_spreader_cell_offset(dimensions,sink,0,0);
    for(i = 0; i < size; i++) sources[i] = conductance * sink->CurrentTemperatures[i];
    
    return TDICE_SUCCESS;
}

/******************************************************************************/

SimResult_t emulate_step
(
    ThermalData_t  *tdata,
    Dimensions_t   *dimensions,
    Analysis_t     *analysis
)
{
    if (analysis->AnalysisType != TDICE_ANALYSIS_TYPE_TRANSIENT)

        return TDICE_WRONG_CONFIG ;

    if (slot_completed (analysis) == true)
    {
        Error_t result = update_source_vector (&tdata->PowerGrid, dimensions) ;

        if (result == TDICE_FAILURE)

            return TDICE_END_OF_SIMULATION ;
    }
    
    if(pluggable_heatsink(tdata, dimensions, analysis) == TDICE_FAILURE)
        return TDICE_SOLVER_ERROR ;

    fill_system_vector

        (dimensions, tdata->ThermalGrid.TopHeatSink, tdata->Temperatures, tdata->PowerGrid.Sources,
         tdata->PowerGrid.CellsCapacities, tdata->Temperatures, analysis->StepTime) ;

    Error_t res = solve_sparse_linear_system (&tdata->SM_A, &tdata->SLUMatrix_B) ;

    if (res != TDICE_SUCCESS)

        return TDICE_SOLVER_ERROR ;

    increase_by_step_time (analysis) ;

    if (slot_completed (analysis) == false)

        return TDICE_STEP_DONE ;

    else

        return TDICE_SLOT_DONE ;
}

/******************************************************************************/

SimResult_t emulate_slot
(
    ThermalData_t  *tdata,
    Dimensions_t   *dimensions,
    Analysis_t     *analysis
)
{
    SimResult_t result ;

    do
    {

        result = emulate_step (tdata, dimensions, analysis) ;

    }   while (result == TDICE_STEP_DONE) ;

    return result ;
}

/******************************************************************************/

SimResult_t emulate_steady
(
    ThermalData_t  *tdata,
    Dimensions_t   *dimensions,
    Analysis_t     *analysis
)
{
    if (analysis->AnalysisType != TDICE_ANALYSIS_TYPE_STEADY)

        return TDICE_WRONG_CONFIG ;

    Error_t result = update_source_vector (&tdata->PowerGrid, dimensions) ;

    if (result == TDICE_FAILURE)
    {
        fprintf (stderr,

            "Warning: no power trace given for steady state simulation\n") ;

        return TDICE_END_OF_SIMULATION ;
    }

    fill_system_vector_steady (dimensions, tdata->Temperatures, tdata->PowerGrid.Sources) ;

    Error_t res = solve_sparse_linear_system (&tdata->SM_A, &tdata->SLUMatrix_B) ;

    if (res != TDICE_SUCCESS)

        return TDICE_SOLVER_ERROR ;

    return TDICE_END_OF_SIMULATION ;
}

/******************************************************************************/

Error_t update_coolant_flow_rate
(
    ThermalData_t  *tdata,
    Dimensions_t   *dimensions,
    Analysis_t     *analysis,
    CoolantFR_t     new_flow_rate
)
{
    tdata->ThermalGrid.Channel->Coolant.FlowRate =

        FLOW_RATE_FROM_MLMIN_TO_UM3SEC(new_flow_rate) ;

    // TODO replace with "update"

    fill_system_matrix (&tdata->SM_A, &tdata->ThermalGrid, analysis, dimensions) ;

    if (do_factorization (&tdata->SM_A) == TDICE_FAILURE)

        return TDICE_FAILURE ;

    update_channel_sources (&tdata->PowerGrid, dimensions) ;

    return TDICE_SUCCESS ;
}

/******************************************************************************/

Temperature_t get_cell_temperature
(
    ThermalData_t *tdata,
    Dimensions_t  *dimensions,
    CellIndex_t    layer_index,
    CellIndex_t    row_index,
    CellIndex_t    column_index
)
{
    CellIndex_t id = get_cell_offset_in_stack

                     (dimensions, layer_index, row_index, column_index) ;

    if (id >= get_number_of_cells (dimensions))

        return 0.0 ;

    else

        return *(tdata->Temperatures + id) ;
}

/******************************************************************************/

Error_t print_thermal_map
(
    ThermalData_t      *tdata,
    StackElementList_t *list,
    Dimensions_t       *dimensions,
    String_t            stack_element_id,
    String_t            file_name
)
{
    StackElement_t stkel ;

    stack_element_init (&stkel) ;

    string_copy (&stkel.Id, &stack_element_id) ;

    StackElement_t *tmp = stack_element_list_find (list, &stkel) ;

    if (tmp == NULL)
    {
        stack_element_destroy (&stkel) ;

        return TDICE_FAILURE ;
    }

    stack_element_destroy (&stkel) ;

    FILE *output_file = fopen (file_name, "w") ;

    if (output_file == NULL)
    {
        fprintf (stderr, "Unable to open output file %s\n", file_name) ;

        return TDICE_FAILURE ;
    }

    stack_element_print_thermal_map

        (tmp, dimensions, tdata->Temperatures, output_file) ;

    fclose (output_file) ;

    return TDICE_SUCCESS ;
}
