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

#ifndef _3DICE_THERMAL_DATA_H_
#define _3DICE_THERMAL_DATA_H_

/*! \file thermal_data.h */

#ifdef __cplusplus
extern "C"
{
#endif

/******************************************************************************/

#include <stdio.h>

#include "types.h"

#include "analysis.h"
#include "stack_description.h"
#include "system_matrix.h"
#include "thermal_cell.h"

#include "slu_ddefs.h"

/******************************************************************************/

    /*! \struct ThermalData
     *
     * \brief Structure to collect data to run thermal simulations
     *
     */

    struct ThermalData
    {
        /*! Array containing the temperature of each thermal cell */

        Temperature_t *Temperatures ;

        /*! Array containing the source value of each thermal cell */

        Source_t *Sources ;

        /*! Array of Thermal cells */

        ThermalCell *ThermalCells ;

        /*! The number of cells in the 3D grid */

        CellIndex_t Size ;

        /*! The matrix A representing the linear system for thermal simulation
         *
         * The right hand vector B is represented by the Temperatures array,
         * since the SuperLU routine dgstrs overwrites the B rhs vector with
         * the computed result (temperatures).
         */

        SystemMatrix SM_A ;

        /*! SuperLU matrix A (wrapper arount our SystemMatrix SM_A )*/

        SuperMatrix SLUMatrix_A ;

        /*! SuperLU matrix A after the permutation */

        SuperMatrix SLUMatrix_A_Permuted ;

        /*! SuperLU vector B (wrapper around the Temperatures array) */

        SuperMatrix SLUMatrix_B ;

        /*! SuperLU matrix L after the A=LU factorization */

        SuperMatrix SLUMatrix_L ;

        /*! SuperLU matrix U after the A=LU factorization */

        SuperMatrix SLUMatrix_U ;

        /*! SuperLU structure for statistics */

        SuperLUStat_t     SLU_Stat ;

        /*! SuperLU structure for factorization options */

        superlu_options_t SLU_Options ;

        /*! SuperLU integer to code the result of the SLU routines */

        int  SLU_Info ;

        /*! SuperLU matrix R for permutation RAC = LU. */

        int* SLU_PermutationMatrixR ;

        /*! SuperLU matrix C for permutation RAC = LU. */

        int* SLU_PermutationMatrixC ;

        /*! SuperLU elimination tree */

        int* SLU_Etree ;

    } ;


    /*! Definition of the type ThermalData */

    typedef struct ThermalData ThermalData ;

/******************************************************************************/

    /*! Sets all the fields of \a tdata to a default value (zero or \c NULL )
     *  and configure the SLU fields to run a factorization
     *
     * \param tdata the address of the thermal data to initialize
     */

    void init_thermal_data (ThermalData *tdata) ;



    /*! Allocs and initialize memory and prepares the LU factorization
     *
     * \param tdata the address of the ThermalData to fill
     * \param stkd  the address of the StackDescription previously filled
     *              through the parsing of the stack file
     * \param analysis the address of the Analysis previously filled trough
     *                  the parsing of the stack file
     *
     * \return \c TDICE_FAILURE if the memory allocation fails or the syatem
     *              matrix cannot be split in A=LU.
     * \return \c TDICE_SUCCESS otherwise
     */

    Error_t fill_thermal_data

        (ThermalData* tdata, StackDescription* stkd, Analysis *analysis) ;



    /*! Frees the memory related to \a tdata
     *
     * The parametrer \a tdata must be the address of a static variable
     *
     * \param tdata the address of the ThermalData structure to free
     */

    void free_thermal_data  (ThermalData* tdata) ;



    /*! Reset the thermal state to the initial temperature
     *
     * \param tdata    the address of the ThermalData structure to reset
     * \param analysis the address of the Analysis structure related to \a tdata
     */

    void reset_thermal_state (ThermalData *tdata, Analysis *analysis) ;



    /*! Simulates a time step
     *
     * \param tdata     address of the ThermalData structure
     * \param stkd      address of the StackDescription structure used to
     *                  fill the content of \a tdata and \a analysys
     * \param analysis  address of the Analysis structure
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

        (ThermalData *tdata, StackDescription *stkd, Analysis *analysis) ;



    /*! Simulates a time slot
     *
     * \param tdata     address of the ThermalData structure
     * \param stkd      address of the StackDescription structure used to
     *                  fill the content of \a tdata and \a analysys
     * \param analysis  address of the Analysis structure
     *
     * \return \c TDICE_WRONG_CONFIG if the parameters refers to a steady
     *                               state simulation
     * \return \c TDICE_SOLVER_ERROR if the SLU functions report an error in
     *                               the structure of the system matrix.
     * \return \c TDICE_END_OF_SIMULATION if power values are over.
     * \return \c TDICE_SLOT_DONE    the slot has been simulated correclty
     */

    SimResult_t emulate_slot

        (ThermalData *tdata, StackDescription *stkd, Analysis *analysis) ;



    /*! Execute steady state simulation
     *
     * \param tdata     address of the ThermalData structure
     * \param stkd      address of the StackDescription structure used to
     *                  fill the content of \a tdata and \a analysys
     * \param analysis  address of the Analysis structure
     *
     * \return \c TDICE_WRONG_CONFIG if the parameters refers to a transient
     *                               simulation
     * \return \c TDICE_SOLVER_ERROR if the SLU functions report an error in
     *                               the structure of the system matrix.
     * \return \c TDICE_END_OF_SIMULATION if no power values are given or if
     *                                    the simulation suceeded
     */

    SimResult_t emulate_steady

        (ThermalData *tdata, StackDescription *stkd, Analysis *analysis) ;



    /*! Update the flow rate
     *
     * Sets the new value in the Channel structure, re-fill the system
     * matrix A and then execute the factorization A=LU again. If this
     * succeeds then the source vector will be upadted with the new inlet
     * source value.
     *
     * \param tdata address of the ThermalData structure
     * \param stkd  address of the StackDescription structure
     * \param new_flow_rate the new flow rate (in ml/min)
     *
     * \return \c TDICE_FAILURE if the syatem matrix cannot be split in A=LU.
     * \return \c TDICE_SUCCESS otherwise
     */

    Error_t update_coolant_flow_rate

        (ThermalData *tdata, StackDescription *stkd, CoolantFR_t new_flow_rate) ;



    /*! Returns the temperature of a thermal cell (\c L, \c R, \c C )
     *
     * Coordinates of the cell must be within the ranges
     *
     * \param tdata        address of the ThermalData structure
     * \param stkd         address of the StackDescription structure
     * \param layer_index  the index \c L of the thermal cell
     * \param row_index    the index \c R of the thermal cell
     * \param column_index the index \c C of the thermal cell
     *
     * \return \c 0 if the thermal cell (\c L, \c R, \c C ) does not exist
     * \return the temperature of the thermal cell (\c L, \c R, \c C )
     */

    Temperature_t get_cell_temperature
    (
        ThermalData      *tdata,
        StackDescription *stkd,
        CellIndex_t       layer_index,
        CellIndex_t       row_index,
        CellIndex_t       column_index
    ) ;



    /*! Generate a text file with the thermal map of a stack element
     *
     * \param tdata     address of the ThermalData structure
     * \param stkd      address of the StackDescription structure
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
        ThermalData      *tdata,
        StackDescription *stkd,
        String_t          stack_element_id,
        String_t          file_name
    ) ;

/******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* _3DICE_THERMAL_DATA_H_ */
