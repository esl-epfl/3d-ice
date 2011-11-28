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

  typedef struct
  {
    /* The following pointers point to memory used to store, for every cell  */
    /* in the 3D grid, the temperature and the source values                 */

    double* Temperatures ;
    double*      Sources ;

    ThermalCell*   ThermalCells ;

    /* The number of cells in the 3D grid to be emulated */

    uint32_t    Size ;

    /* The matrix A representing the linear system */

    SystemMatrix SM_A ;

    /* The right hand vector B is represented by the Temperatures array,     */
    /* since the SuperLU routine used to solve the linear system overwrites  */
    /* the B rhs vector with the result (temperatures).                      */

    /* Data used to interface with SuperLU */

    SuperMatrix SLUMatrix_A ,
                SLUMatrix_A_Permuted ,
                SLUMatrix_B ,
                SLUMatrix_L ,
                SLUMatrix_U ;

    SuperLUStat_t     SLU_Stat ;
    superlu_options_t SLU_Options ;

    int  SLU_Info ;
    int* SLU_PermutationMatrixR ;
    int* SLU_PermutationMatrixC ;
    int* SLU_Etree ;

  } ThermalData ;

/******************************************************************************/

  /* Sets all the fields of tdata to a default value                        */
  /*                                                                        */
  /* tdata               The address of the ThermalData structure to init   */
  /*                                                                        */
  /* initial_temperature The initial temperature [K] for all the thermal    */
  /*                     cells of the 3D grid                               */
  /*                                                                        */
  /* slot_time           the length [seconds] of each time slot, i.e. the   */
  /*                     interval of time during which the power values of  */
  /*                     of the floorplan elements are held constant. The   */
  /*                     end of a time slot (and the beginning of the       */
  /*                     following one) corresponds to the power switching  */
  /*                     activity of a floorplan element                    */
  /*                                                                        */
  /* step_time           time discretization length [seconds] of the time   */
  /*                     domain for the numerical integration of the system */
  /*                     of differential equations                          */

  void init_thermal_data (ThermalData *tdata) ;

/******************************************************************************/

  /* Allocs memory and fills it reading the structure of the 3D-IC from     */
  /* StackDescription structure.                                            */
  /*                                                                        */
  /* At the end of the function, tdata.Temperatures will be filled with the */
  /* initial temperature while tdata.Sources will be filled with zeroes.    */
  /* tdata.Conductances and tdata.Capacities are filled with the            */
  /* corresponding values. In case of success, the matrix tdata.SM_A will   */
  /* be filled and factored (see fields related to the usage of SuperLU).   */
  /* The right hand side vector is not stored since we use the Temperature  */
  /* array for this purpose                                                 */
  /*                                                                        */
  /* Returns                                                                */
  /*                                                                        */
  /*  0 if success,                                                         */
  /* -1 if memory allocation error,                                         */
  /* >0 otherwise (factorization error). Please see the "output" field of   */
  /*     the function "dgstrf" in SuperLU                                   */

  int fill_thermal_data

    (ThermalData* tdata, StackDescription* stkd, Analysis *analysis) ;

/******************************************************************************/

  /* Frees all the memory                                                   */

  void free_thermal_data  (ThermalData* tdata) ;

/******************************************************************************/

  /* Consumes a time step emulating heat injection and dissipation.         */
  /*                                                                        */
  /* Returns                                                                */
  /*                                                                        */
  /*   1 if the time slots are over,                                        */
  /*   0 if the emulation succeeded (step consumed),                        */
  /*  <0 if there have been a problem with the computation of the solution  */
  /*     of the linear system. See the "info" prameter of "dgstrs" in       */
  /*     SuperLU.                                                           */

  int emulate_step (ThermalData* tdata, StackDescription* stkd, Analysis *analysis) ;

/******************************************************************************/

  /* Consumes a time slot emulating heat injection and dissipation.         */
  /*                                                                        */
  /* Returns                                                                */
  /*                                                                        */
  /*   1 if the time slots are over                                         */
  /*   0 if the emulation succeeded (slot consumed),                        */
  /*  <0 if there have been a problem with the computation of the solution  */
  /*     of the linear system. See the "info" prameter of "dgstrs"          */
  /*     in SuperLU.                                                        */

  int emulate_slot (ThermalData* tdata, StackDescription* stkd, Analysis *analysis) ;

/******************************************************************************/

  /* Consumes the first power value of heat injection and dissipation       */
  /* and does steady state simulation.                                      */
  /* Returns                                                                */
  /*                                                                        */
  /*   1 if the emulation succeeded (power value consumed),                 */
  /*     or if no power values are mentioned at all (error)                 */
  /*  <0 if there have been a problem with the computation of the solution  */
  /*     of the linear system. See the "info" prameter of "dgstrs"          */
  /*     in SuperLU.                                                        */

  int emulate_steady (ThermalData* tdata, StackDescription* stkd, Analysis *analysis) ;

/******************************************************************************/

  /* Update the flow rate.                                                  */
  /*                                                                        */
  /* Sets the new value and then re-fill the system matrix A and update     */
  /* only the (channel) inlet source values. Then, it re-factor the System  */
  /* matrix.                                                                */
  /*                                                                        */
  /* Returns                                                                */
  /*                                                                        */
  /*  0 if success,                                                         */
  /* >0 otherwise (factorization error). Please see the "output" field of   */
  /*     the function "dgstrf" in SuperLU                                   */

  int update_coolant_flow_rate
  (
    ThermalData*      tdata,
    StackDescription* stkd,
    double       new_coolant_fr
  ) ;

/******************************************************************************/

  /* Get the maximum temperature of a given floorplan element                */
  /*                                                                         */
  /* stkd                  the StackDescription structure to query           */
  /* tdata                 the ThermalData structure to query                */
  /* floorplan_id          the id of the floorplan (the stack element id)    */
  /* floorplan_element_id  the id of the floorplan element                   */
  /* max_temperature       the address where the max temperature will be     */
  /*                       written                                           */
  /*                                                                         */
  /* Returns:                                                                */
  /*                                                                        */
  /*   0 if both the ids are correct                                         */
  /*  -1 if the stack element id floorplan_id does not exist                 */
  /*  -2 if floorplan_id exists but it is not the id of a die or it belongs  */
  /*     to a die but the floorplan itself does not exist                    */
  /*  -3 if floorplan_id exists but floorplan_element_id do not              */

  int get_max_temperature_of_floorplan_element
  (
    StackDescription* stkd,
    ThermalData*      tdata,
    char *          floorplan_id,
    char *          floorplan_element_id,
    double*    max_temperature
  ) ;

/******************************************************************************/

  /* Get the minimum temperature of a given floorplan element                */
  /*                                                                         */
  /* stkd                  the StackDescription structure to query           */
  /* floorplan_id          the id of the floorplan (the stack element id)    */
  /* floorplan_element_id  the id of the floorplan element                   */
  /* temperatures          the address of the array temperature to access    */
  /* min_temperature       the address where the min temperature will be     */
  /*                       written                                           */
  /*                                                                         */
  /* Returns:                                                                */
  /*   0 if both the ids are correct                                         */
  /*  -1 if the stack element id floorplan_id does not exist                 */
  /*  -2 if floorplan_id exists but it is not the id of a die or it belongs  */
  /*     to a die but the floorplan itself does not exist                    */
  /*  -3 if floorplan_id exists but floorplan_element_id do not              */

  int get_min_temperature_of_floorplan_element
  (
    StackDescription* stkd,
    ThermalData*      tdata,
    char *          floorplan_id,
    char *          floorplan_element_id,
    double*    min_temperature
  ) ;

/******************************************************************************/

  /* Get the average temperature of a given floorplan element                */
  /*                                                                         */
  /* stkd                  the StackDescription structure to query           */
  /* floorplan_id          the id of the floorplan (the stack element id)    */
  /* floorplan_element_id  the id of the floorplan element                   */
  /* temperatures          the address of the array temperature to access    */
  /* avg_temperature       the address where the avg temperature will be     */
  /*                       written                                           */
  /*                                                                         */
  /* Returns:                                                                */
  /*   0 if both the ids are correct                                         */
  /*  -1 if the stack element id floorplan_id does not exist                 */
  /*  -2 if floorplan_id exists but it is not the id of a die or it belongs  */
  /*     to a die but the floorplan itself does not exist                    */
  /*  -3 if floorplan_id exists but floorplan_element_id do not              */

  int get_avg_temperature_of_floorplan_element
  (
    StackDescription* stkd,
    ThermalData*      tdata,
    char *          floorplan_id,
    char *          floorplan_element_id,
    double*    avg_temperature
  ) ;

/******************************************************************************/

  /* Get the minimum, average and maximum temperature of a given floorplan   */
  /* element                                                                 */
  /*                                                                         */
  /* stkd                  the StackDescription structure to query           */
  /* floorplan_id          the id of the floorplan (the stack element id)    */
  /* floorplan_element_id  the id of the floorplan element                   */
  /* temperatures          the address of the array temperature to access    */
  /* min_temperature       the address where the min temperature will be     */
  /*                       written                                           */
  /* avg_temperature       the address where the avg temperature will be     */
  /*                       written                                           */
  /* max_temperature       the address where the max temperature will be     */
  /*                       written                                           */
  /*                                                                         */
  /* Returns:                                                                */
  /*   0 if both the ids are correct                                         */
  /*  -1 if the stack element id floorplan_id does not exist                 */
  /*  -2 if floorplan_id exists but it is not the id of a die or it belongs  */
  /*     to a die but the floorplan itself does not exist                    */
  /*  -3 if floorplan_id exists but floorplan_element_id do not              */

  int get_min_avg_max_temperatures_of_floorplan_element
  (
    StackDescription* stkd,
    ThermalData*      tdata,
    char *          floorplan_id,
    char *          floorplan_element_id,
    double*    min_temperature,
    double*    avg_temperature,
    double*    max_temperature
  ) ;

/******************************************************************************/

  /* Get the maximum temperature of all the floorplan element in a floorplan */
  /*                                                                         */
  /* stkd                  the StackDescription structure to query           */
  /* floorplan_id          the id of the floorplan (the stack element id)    */
  /* temperatures          the address of the array temperature to access    */
  /* max_temperature       the address where the max temperatures will be    */
  /*                       written                                           */
  /*                                                                         */
  /* Returns:                                                                */
  /*   0 if the ids is correct                                               */
  /*  -1 if the stack element id floorplan_id does not exist                 */
  /*  -2 if floorplan_id exists but it is not the id of a die or it belongs  */
  /*     to a die but the floorplan itself does not exist                    */
  /*                                                                         */
  /* If the floorplan named floorplan_id has n floorplan element, then the   */
  /* function will access n elements of type double starting from     */
  /* max_temperatures (i.e. the memory must be allocated before calling this */
  /* function). Temperatures will be written following the same order of     */
  /* declaration found in the corresponding .flp file (max_temperature[0] is */
  /* the maximum temperature of the first floorplan element found in the     */
  /* file).                                                                  */

  int get_all_max_temperatures_of_floorplan
  (
    StackDescription* stkd,
    ThermalData*      tdata,
    char *          floorplan_id,
    double*    max_temperature
  ) ;

/******************************************************************************/

  /* Get the minimum temperature of all the floorplan element in a floorplan */
  /*                                                                         */
  /* stkd                  the StackDescription structure to query           */
  /* floorplan_id          the id of the floorplan (the stack element id)    */
  /* temperatures          the address of the array temperature to access    */
  /* min_temperature       the address where the min temperatures will be    */
  /*                       written                                           */
  /*                                                                         */
  /* Returns:                                                                */
  /*   0 if the ids is correct                                               */
  /*  -1 if the stack element id floorplan_id does not exist                 */
  /*  -2 if floorplan_id exists but it is not the id of a die or it belongs  */
  /*     to a die but the Floorplan itself does not exist                    */
  /*                                                                         */
  /* If the floorplan named floorplan_id has n floorplan element, then the   */
  /* function will access n elements of type double starting from     */
  /* min_temperatures (i.e. the memory must be allocated before calling this */
  /* function). Temperatures will be written following the same order of     */
  /* declaration found in the corresponding .flp file (max_temperature[0] is */
  /* the minimum temperature of the first floorplan element found in the     */
  /* file).                                                                  */

  int get_all_min_temperatures_of_floorplan
  (
    StackDescription* stkd,
    ThermalData*      tdata,
    char *          floorplan_id,
    double*    min_temperature
  ) ;

/******************************************************************************/

  /* Get the average temperature of all the floorplan element in a floorplan */
  /*                                                                         */
  /* stkd                  the StackDescription structure to query           */
  /* floorplan_id          the id of the floorplan (the stack element id)    */
  /* temperatures          the address of the array temperature to access    */
  /* avg_temperature       the address where the avg temperatures will be    */
  /*                       written                                           */
  /*                                                                         */
  /* Returns:                                                                */
  /*   0 if the ids is correct                                               */
  /*  -1 if the stack element id floorplan_id does not exist                 */
  /*  -2 if floorplan_id exists but it is not the id of a die or it belongs  */
  /*     to a die but the Floorplan itself does not exist                    */
  /*                                                                         */
  /* If the floorplan named floorplan_id has n floorplan element, then the   */
  /* function will access n elements of type double starting from     */
  /* avg_temperatures (i.e. the memory must be allocated before calling this */
  /* function). Temperatures will be written following the same order of     */
  /* declaration found in the corresponding .flp file (max_temperature[0] is */
  /* the average temperature of the first floorplan element found in the     */
  /* file).                                                                  */

  int get_all_avg_temperatures_of_floorplan
  (
    StackDescription* stkd,
    ThermalData*      tdata,
    char *          floorplan_id,
    double*    avg_temperature
  ) ;

/******************************************************************************/

  /* Get the minimum, average and maximum temperature of all the floorplan   */
  /* element in a floorplan                                                  */
  /*                                                                         */
  /* stkd                  the StackDescription structure to query           */
  /* floorplan_id          the id of the floorplan (the stack element id)    */
  /* temperatures          the address of the array temperature to access    */
  /* min_temperature       the address where the min temperatures will be    */
  /*                       written                                           */
  /* avg_temperature       the address where the avg temperatures will be    */
  /*                       written                                           */
  /* max_temperature       the address where the max temperatures will be    */
  /*                       written                                           */
  /*                                                                         */
  /* Returns:                                                                */
  /*   0 if the ids is correct                                               */
  /*  -1 if the stack element id floorplan_id does not exist                 */
  /*  -2 if floorplan_id exists but it is not the id of a die or it belongs  */
  /*     to a die but the Floorplan itself does not exist                    */
  /*                                                                         */
  /* If the floorplan named floorplan_id has n floorplan element, then the   */
  /* function will access n elements of type double starting from     */
  /* min_temperatures, avg_temperatures, etc. (i.e. the memory must be       */
  /* allocated before calling this function). Temperatures will be written   */
  /* following the same order of declaration found in the corresponding .flp */
  /* file (min_temperature[0] is the minimum temperature of the first        */
  /* floorplan element found in the file, etc...).                           */

  int get_all_min_avg_max_temperatures_of_floorplan
  (
    StackDescription* stkd,
    ThermalData*      tdata,
    char *          floorplan_id,
    double*    min_temperature,
    double*    avg_temperature,
    double*    max_temperature
  ) ;

/******************************************************************************/

  /* Get the themperature of a given thermal cell                            */
  /*                                                                         */
  /* Returns:                                                                */
  /*   0 if the cell exists                                                  */
  /*  -1 if the cell does not exist                                          */
  /*  -2 if there has been an error when opening the file file_name          */

  int get_cell_temperature
  (
    StackDescription* stkd,
    ThermalData*      tdata,
    uint32_t   layer_index,
    uint32_t   row_index,
    uint32_t   column_index,
    double*    cell_temperature
  ) ;

/******************************************************************************/

  /* Given the address of a StackDescrption structure and the array of       */
  /* temperatures, print to file_name the thermal map of stack_element_id.   */
  /* stack_element_id is the id given to one of the stack element composing  */
  /* the 3D stack. If it refers to a die, the active source layer will be    */
  /* printed.                                                                */
  /*                                                                         */
  /* Returns:                                                                */
  /*   0 if the ids is correct                                               */
  /*  -1 if the stack element id floorplan_id does not exist                 */
  /*  -2 if there has been an error when opening the file file_name          */
  /*                                                                         */
  /* The thermal map is printex as a matrix with get_number_of_columns ()    */
  /* columns and get_number_of_rows () rows.                                 */

  int print_thermal_map
  (
    StackDescription* stkd,
    ThermalData*      tdata,
    char *          stack_element_id,
    char *          file_name
  ) ;

/******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* _3DICE_THERMAL_DATA_H_ */
