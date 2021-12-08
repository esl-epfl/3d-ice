/******************************************************************************
 * This file is part of 3D-ICE, version 3.1.0 .                               *
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
 *                             Copyright (C) 2021                             *
 *   Embedded Systems Laboratory - Ecole Polytechnique Federale de Lausanne   *
 *                            All Rights Reserved.                            *
 *                                                                            *
 * Authors: Arvind Sridhar              Alessandro Vincenzi                   *
 *          Giseong Bak                 Martino Ruggiero                      *
 *          Thomas Brunschwiler         Eder Zulian                           *
 *          Federico Terraneo           Darong Huang                          *
 *          Luis Costero                Marina Zapater                        *
 *          David Atienza                                                     *
 *                                                                            *
 * For any comment, suggestion or request  about 3D-ICE, please  register and *
 * write to the mailing list (see http://listes.epfl.ch/doc.cgi?liste=3d-ice) *
 * Any usage  of 3D-ICE  for research,  commercial or other  purposes must be *
 * properly acknowledged in the resulting products or publications.           *
 *                                                                            *
 * EPFL-STI-IEL-ESL                     Mail : 3d-ice@listes.epfl.ch          *
 * Batiment ELG, ELG 130                       (SUBSCRIPTION IS NECESSARY)    *
 * Station 11                                                                 *
 * 1015 Lausanne, Switzerland           Url  : http://esl.epfl.ch/3d-ice      *
 ******************************************************************************/

#ifndef _3DICE_THERMAL_DATA_H_
#define _3DICE_THERMAL_DATA_H_

/*! \file thermal_data.h */

#ifdef __cplusplus
extern "C"
{
#endif

/******************************************************************************/

#include "types.h"
#include "string_t.h"

#include "analysis.h"
#include "stack_element_list.h"
#include "system_matrix.h"
#include "thermal_grid.h"
#include "power_grid.h"
#include "dimensions.h"
#include "connection_list.h"

#include "slu_ddefs.h"

/******************************************************************************/

    /*! \struct ThermalData_t
     *
     * \brief Structure to collect data to run thermal simulations
     *
     */

    struct ThermalData_t
    {
        /*! Array containing the temperature of each thermal cell */

        Temperature_t *Temperatures ;

        /*! Structure storing the Thermal Grid */

        ThermalGrid_t ThermalGrid ;

        /*! Structure storing the Power Grid */

        PowerGrid_t PowerGrid ;

        /*! The number of cells in the 3D grid */

        CellIndex_t Size ;

        /*! The matrix A representing the linear system for thermal simulation
         *
         * The right hand vector B is represented by the Temperatures array,
         * since the SuperLU routine dgstrs overwrites the B rhs vector with
         * the computed result (temperatures).
         */

        SystemMatrix_t SM_A ;

        /*! SuperLU vector B (wrapper around the Temperatures array) */

        SuperMatrix SLUMatrix_B ;
    } ;


    /*! Definition of the type ThermalData_t */

    typedef struct ThermalData_t ThermalData_t ;



/******************************************************************************/



    /*! Inits the fields of the \a tdata structure with default values
     *
     * \param tdata the address of the structure to initalize
     */

    void thermal_data_init (ThermalData_t *tdata) ;

    /*! Update the number of cells in the non-uniform grid scenario
     *
     * \param dimensions the dimensions of the IC
     * \param stack_elements_list the list of stack element (bottom first)
     */

    void update_number_of_cells (Dimensions_t *dimensions, StackElementList_t *stack_elements_list);
    
    /*! Get cell position for each cell and sace info to arrays position_info and layer_cell_record
     * \param position_info position info contains "left_x, left_y, right_x, right_y" for each thermal cell
     * \param layer_cell_record the vector contains the end index of each layer in the position_info
     * \param layer_type_record the vector contains the layer type information of each layer in the position_info
     * \param stack_elements_list the list of stack element (bottom first)
     * \param dimensions the dimensions of the IC
     */
    void get_cell_position(ChipDimension_t (*position_info)[4], CellIndex_t *layer_cell_record, CellIndex_t *layer_type_record, StackElementList_t *stack_elements_list, Dimensions_t* dimensions);

    /*! Get the Minkowski difference between two cells
     * \param minkowski_diff the vector contains the minkowski difference information
     * \param position_info position info contains "left_x, left_y, right_x, right_y" for each thermal cell
     * \param i_x the index of the one cell
     * \param i_y the index of the other cell
     */
    void get_minkowski_difference(ChipDimension_t *minkowski_diff, ChipDimension_t (*position_info)[4], int i_x, int i_y);

    /*! get connections of each grid in the same layer
     * \param layer_cell_record the vector contains the end index of each layer in the position_info
     * \param layer_type_record the vector contains the layer type information of each layer in the position_info
     * \param position_info_ptr position info contains "left_x, left_y, right_x, right_y" for each thermal cell
     * \param dimensions the dimensions of the IC
     */
    void get_connections_in_layer
    (
        CellIndex_t* layer_cell_record, 
        CellIndex_t* layer_type_record,
        ChipDimension_t (*position_info_ptr)[4],
        Dimensions_t* dimensions
        
    );

    /*! Get connections between layers (from bottom to top)
     * \param layer_cell_record the vector contains the end index of each layer in the position_info
     * \param layer_type_record the vector contains the layer type information of each layer in the position_info
     * \param position_info_ptr position info contains "left_x, left_y, right_x, right_y" for each thermal cell
     * \param dimensions the dimensions of the IC
     */
    void get_connections_between_layer
    (
        CellIndex_t* layer_cell_record, 
        CellIndex_t* layer_type_record,
        ChipDimension_t (*position_info_ptr)[4], 
        Dimensions_t* dimensions
    );

    /*! Allocs and initialize memory and prepares the LU factorization
     *
     * \param tdata       the address of the ThermalData to fill
     * \param list       the list of stack element (bottom first)
     * \param dimensions the dimensions of the IC
     * \param analysis   the address of the Analysis structure
     *
     * \return \c TDICE_FAILURE if the memory allocation fails or the syatem
     *              matrix cannot be split in A=LU.
     * \return \c TDICE_SUCCESS otherwise
     */

    Error_t thermal_data_build
    (
        ThermalData_t      *tdata,
        StackElementList_t *list,
        Dimensions_t       *dimensions,
        Analysis_t         *analysis
    ) ;



    /*! Destroys the content of the fields of the structure \a tdata
     *
     * The function releases any dynamic memory used by the structure and
     * resets its state calling \a thermal_data_init .
     *
     * \param tdata the address of the structure to destroy
     */

    void thermal_data_destroy  (ThermalData_t *tdata) ;



    /*! Reset the thermal state to the initial temperature
     *
     * \param tdata     the address of the ThermalData structure to reset
     * \param analysis the address of the Analysis structure related to \a tdata
     */

    void reset_thermal_state (ThermalData_t *tdata, Analysis_t *analysis) ;



    /*! Simulates a time step
     *
     * \param tdata           the address of the ThermalData to fill
     * \param dimensions     the dimensions of the IC
     * \param analysis       the address of the Analysis structure
     *
     * \return \c TDICE_WRONG_CONFIG if the parameters refers to a steady
     *                               state simulation
     * \return \c TDICE_SOLVER_ERROR if the SLU functions report an error in
     *                               the structure of the system matrix.
     * \return \c TDICE_END_OF_SIMULATION if power values are over.
     * \return \c TDICE_STEP_DONE    if the time step has been simulated
     *                               correclty
     * \return \c TDICE_SLOT_DONE    if the time step has been simulated
     *                               correclty and the slot has been completed
     */

    SimResult_t emulate_step
    (
        ThermalData_t  *tdata,
        Dimensions_t   *dimensions,
        Analysis_t     *analysis
    ) ;



    /*! Simulates a time slot
     *
     * \param tdata           the address of the ThermalData to fill
     * \param dimensions     the dimensions of the IC
     * \param analysis       the address of the Analysis structure
     *
     * \return \c TDICE_WRONG_CONFIG if the parameters refers to a steady
     *                               state simulation
     * \return \c TDICE_SOLVER_ERROR if the SLU functions report an error in
     *                               the structure of the system matrix.
     * \return \c TDICE_END_OF_SIMULATION if power values are over.
     * \return \c TDICE_SLOT_DONE    the slot has been simulated correclty
     */

    SimResult_t emulate_slot
    (
        ThermalData_t  *tdata,
        Dimensions_t   *dimensions,
        Analysis_t     *analysis
    ) ;



    /*! Execute steady state simulation
     *
     * \param tdata           the address of the ThermalData to fill
     * \param dimensions     the dimensions of the IC
     * \param analysis       the address of the Analysis structure
     *
     * \return \c TDICE_WRONG_CONFIG if the parameters refers to a transient
     *                               simulation
     * \return \c TDICE_SOLVER_ERROR if the SLU functions report an error in
     *                               the structure of the system matrix.
     * \return \c TDICE_END_OF_SIMULATION if no power values are given or if
     *                                    the simulation suceeded
     */

    SimResult_t emulate_steady
    (
        ThermalData_t  *tdata,
        Dimensions_t   *dimensions,
        Analysis_t     *analysis
    ) ;



    /*! Update the flow rate
     *
     * Sets the new value in the Channel structure, re-fill the system
     * matrix A and then execute the factorization A=LU again. If tdata
     * succeeds then the source vector will be upadted with the new inlet
     * source value.
     *
     * \param tdata address of the ThermalData structure
     * \param dimensions the dimensions of the IC
     * \param analysis address of the Analisys structure
     * \param new_flow_rate the new flow rate (in ml/min)
     *
     * \return \c TDICE_FAILURE if the syatem matrix cannot be split in A=LU.
     * \return \c TDICE_SUCCESS otherwise
     */

    Error_t update_coolant_flow_rate
    (
        ThermalData_t  *tdata,
        Dimensions_t   *dimensions,
        Analysis_t     *analysis,
        CoolantFR_t     new_flow_rate
    ) ;



    /*! Returns the temperature of a thermal cell (\c L, \c R, \c C )
     *
     * Coordinates of the cell must be within the ranges
     *
     * \param tdata address of the ThermalData structure
     * \param dimensions the dimensions of the IC
     * \param layer_index  the index \c L of the thermal cell
     * \param row_index    the index \c R of the thermal cell
     * \param column_index the index \c C of the thermal cell
     *
     * \return \c 0 if the thermal cell (\c L, \c R, \c C ) does not exist
     * \return the temperature of the thermal cell (\c L, \c R, \c C )
     */

    Temperature_t get_cell_temperature
    (
        ThermalData_t *tdata,
        Dimensions_t  *dimensions,
        CellIndex_t    layer_index,
        CellIndex_t    row_index,
        CellIndex_t    column_index
    ) ;



    /*! Generate a text file with the thermal map of a stack element
     *
     * \param tdata address of the ThermalData structure
     * \param list  the list of stack element (bottom first)
     * \param dimensions the dimensions of the IC
     * \param stack_element_id the id of the stack element as it appears in the
                        stack file
     * \param file_name the path of the file to be generated
     *
     * \return \c TDICE_FAILURE if the file cannot be opened or if the stack
     *                  element \a stack_element_id does not exist
     * \return \c TDICE_SUCCESS otherwise
     */

    Error_t print_thermal_map
    (
        ThermalData_t      *tdata,
        StackElementList_t *list,
        Dimensions_t       *dimensions,
        String_t            stack_element_id,
        String_t            file_name
    ) ;

/******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* _3DICE_THERMAL_DATA_H_ */
