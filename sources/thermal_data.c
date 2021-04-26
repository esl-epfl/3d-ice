/******************************************************************************
 * This file is part of 3D-ICE, version 3.0.0 .                               *
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
 *          Federico Terraneo                                                 *
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

void update_number_of_cells (Dimensions_t *dimensions, StackElementList_t *stack_elements_list)
{
    int cell_num_non_uniform = 0;
    int cell_num_die;
    int dis_x_element = 0;
    int dis_y_element = 0;
    // enumerate all the stack elements (dies)
    StackElementListNode_t *stkeln ;
    for (stkeln  = stack_elements_list->First ;
        stkeln != NULL ;
        stkeln  = stkeln->Next)
    {
        StackElement_t *stkel = &stkeln->Data ;
        int dis_x_die = stkel->Pointer.Die->Dis_X;
        int dis_y_die = stkel->Pointer.Die->Dis_Y;

        // enumerate all the floorplan elements
        FloorplanElementListNode_t *ele_flp ;
        cell_num_die = 0;
        for (ele_flp  = stkel->Pointer.Die->Floorplan.ElementsList.First ;
            ele_flp != NULL ;
            ele_flp  = ele_flp->Next)
        {
            FloorplanElement_t *ele_flpi = &ele_flp->Data ;
            dis_x_element = ele_flpi->ICElements.First->Data.Dis_X;
            dis_y_element = ele_flpi->ICElements.First->Data.Dis_Y;
            if (dis_x_element != 0 && dis_y_element != 0)
            {
                cell_num_die += dis_x_element*dis_y_element;
            }
            else
            {
                cell_num_die += dis_x_die*dis_y_die;
            }
        }
        // Total cell number in all the layers the die has
        cell_num_die = cell_num_die*stkel->NLayers;
        cell_num_non_uniform += cell_num_die;
    }
    dimensions->Grid.NCells = cell_num_non_uniform;
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
    
    // TODO_Darong: re-evaluate the thermal grids, interconect when enable non-uniform thermal grids
    if (dimensions->NonUniform == 1)
    {
        // First re-evaluate the thermal grids for each elements
        update_number_of_cells (dimensions, stack_elements_list);
    }

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

    result = thermal_grid_fill (&tdata->ThermalGrid, stack_elements_list) ;
    
    if (result == TDICE_FAILURE)
    {
        Destroy_SuperMatrix_Store (&tdata->SLUMatrix_B) ;

        free (tdata->Temperatures) ;

        return TDICE_FAILURE ;
    }

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

Error_t pluggable_heatsink(ThermalData_t *tdata, Dimensions_t *dimensions)
{
    // We have something to do only if we're using the pluggable heatsink model
    HeatSink_t *sink = tdata->ThermalGrid.TopHeatSink;
    if(sink == NULL || sink->SinkModel != TDICE_HEATSINK_TOP_PLUGGABLE)
            return TDICE_SUCCESS;
    
    //Get a pointer to the spreader temperatures
    double *SpreaderTemperatures = tdata->Temperatures;
    SpreaderTemperatures += get_spreader_cell_offset(dimensions,sink,0,0);
    
    Source_t *sources = tdata->PowerGrid.Sources;
    sources += get_spreader_cell_offset(dimensions,sink,0,0);
    
    // Call the pluggable heat sink function to compute the heat flows to
    // the heatsink
    if(sink->PluggableHeatsink(SpreaderTemperatures,sources))
    {
        fprintf(stderr, "Error: pluggable heatsink callback failed\n");
        return TDICE_FAILURE;
    }
    
    // Both 3D-ICE and plugin use passive sign convention
    unsigned int size = sink->NColumns * sink->NRows;
    unsigned int i;
    for(i = 0; i < size; i++)
        sources[i] = - sources[i];

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
    
    if(pluggable_heatsink(tdata, dimensions) == TDICE_FAILURE)
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

    if(tdata->ThermalGrid.TopHeatSink &&
       tdata->ThermalGrid.TopHeatSink->SinkModel == TDICE_HEATSINK_TOP_PLUGGABLE)

        return TDICE_SOLVER_ERROR ; //TODO: support steady state pluggable sink

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
