/******************************************************************************
 * Bison source file "3D-ICE/bison/floorplan_parser.y"                        *
 *                                                                            *
 * This file is part of 3D-ICE 1.0 (beta).                                    *
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
 * with 3D-ICe. If not, see <http://www.gnu.org/licenses/>.                   *
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
 * EPFL-STI-IEL-ESL                                                           *
 * Batiment ELG, ELG 130                                                      *
 * Station 11                                           3d-ice@listes.epfl.ch *
 * 1015 Lausanne, Switzerland                  http://esl.epfl.ch/3d-ice.html *
 ******************************************************************************/

#ifndef _3DICE_THERMAL_DATA_H_
#define _3DICE_THERMAL_DATA_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include "conductances.h"
#include "stack_description.h"
#include "system_matrix.h"
#include "slu_ddefs.h"

/******************************************************************************/

  typedef struct
  {
    /* The following pointers point to memory used to store, for every cell  */
    /* in the 3D grid (temperature, source of power, capacity and            */
    /* conductances)                                                         */

    Temperature_t* Temperatures ;
    Source_t*      Sources ;
    Capacity_t*    Capacities ;
    Conductances*  Conductances ;

    Temperature_t InitialTemperature ;

    /* The number of cells in the 3D grid to be emulated */

    Quantity_t    Size ;

    /* Time Values */

    Time_t StepTime ;
    Time_t SlotTime ;

    Time_t CurrentTime ;
    Time_t CurrentSlotLimit ;

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

  /* Given a valid address of a ThermalData structure, sets all its  fields */
  /* using the actual parameters.                                           */
  /*                                                                        */
  /* tdata               The address of the ThermalData structure to init   */
  /*                                                                        */
  /* initial_temperature The initial temperature [K] for all the thermal    */
  /*                     cells of the 3D grid                               */
  /*                                                                        */
  /* step_time           The unity of time [seconds] used for each time     */
  /*                     step during the thermal emulation                  */
  /*                                                                        */
  /* slot_time           the length [seconds] of each time slot, i.e. for   */
  /*                     how long power values read from the floorplans are */
  /*                     injected into the 3D-IC grid of cells. Every slot  */
  /*                     of time is divided into steps (step_time)          */

  void init_thermal_data
  (
    ThermalData*      tdata,
    Temperature_t     initial_temperature,
    Time_t            step_time,
    Time_t            slot_time
  ) ;

/******************************************************************************/

  /* Given the address of a ThermalData structure previously initialized,   */
  /* allocs memory and fills it reading the structure of the 3D-IC from     */
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
  /*   0 if success,                                                        */
  /*  -1 if memory allocation error,                                        */
  /*  >0 otherwise (factorization error). Please see the "output" field of  */
  /*     the function "dgstrf" in SuperLU                                   */

  int fill_thermal_data  (ThermalData* tdata, StackDescription* stkd) ;

/******************************************************************************/

  /* Frees all the memory used by the given ThermalData structure           */

  void free_thermal_data  (ThermalData* tdata) ;

/******************************************************************************/

  /* Consumes a time step emulating heat injection and dissipation.         */
  /*                                                                        */
  /* Returns                                                                */
  /*  -1 if the tdata.SM_A hasn't been factored before                      */
  /*   0 if the emulation succeeded,                                        */
  /*   1 if the time slots are over,                                        */
  /*   2 if the time reached the end of a time slot,                        */
  /*  <0 if there have been a problem with the computation of the solution  */
  /*     of the linear system. See the "info" prameter of "dgstrs" in       */
  /*     SuperLU.                                                           */

  int emulate_step (ThermalData* tdata, StackDescription* stkd) ;

/******************************************************************************/

  /* Consumes a time slot emulating heat injection and dissipation.         */
  /*                                                                        */
  /* Returns                                                                */
  /*  -1 if the tdata.SM_A hasn't been factored before                      */
  /*   0 if the emulation succeeded,                                        */
  /*   1 if the time slots are over                                         */
  /*  <0 if there have been a problem with the computation of the solution  */
  /*     of the linear system. See the "info" prameter of "dgstrs"          */
  /*     in SuperLU.                                                        */

  int emulate_slot (ThermalData* tdata, StackDescription* stkd) ;

/******************************************************************************/

  /* Gives the time, in [seconds], sum of the time slots emulated           */

  Time_t get_current_time (ThermalData* tdata) ;

/******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* _3DICE_THERMAL_DATA_H_ */
