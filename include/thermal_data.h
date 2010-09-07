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
 * Station 11                                          threed-ice@esl.epfl.ch *
 * 1015 Lausanne, Switzerland                       http://esl.epfl.ch/3D-ICE *
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
#include "system_vector.h"
#include "slu_ddefs.h"

/******************************************************************************/

  typedef struct
  {
    Temperature_t* Temperatures ;
    Source_t*      Sources ;
    Capacity_t*    Capacities ;
    Conductances*  Conductances ;

    Quantity_t    Size ;
    Temperature_t InitialTemperature ;

    Time_t CurrentTime ;
    Time_t DeltaTime ;
    Time_t SlotTime ;

    SystemMatrix SM_A ;
    SystemVector SV_B ;

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
  /* using the actual parameters and allocs memory.                         */
  /*                                                                        */
  /* tdata               The address of the ThermalData structure to init   */
  /*                                                                        */
  /* stkd                The address of a StackDescription structure        */
  /*                     already filled with data from a .stk file          */
  /*                                                                        */
  /* initial_temperature The initial temperature [K] for all the thermal    */
  /*                     cells of the 3D grid                               */
  /*                                                                        */
  /* delta_time          The unity of time [seconds] used for each time     */
  /*                     step during the thermal emulation                  */
  /*                                                                        */
  /* slot_time           the length [seconds] of each time slot, i.e. for   */
  /*                     how long power values read from the floorplans are */
  /*                     injected into the 3D-IC grid of cells              */
  /*                                                                        */
  /* Returns                                                                */
  /* 0 if there have been any error in memory allocation and a message will */
  /*   be printed into stderr,                                              */
  /* 1 otherwise, i.e. tdata has been initialized with success              */

  int init_thermal_data
  (
    ThermalData*      tdata,
    StackDescription* stkd,
    Temperature_t     initial_temperature,
    Time_t            delta_time,
    Time_t            slot_time
  ) ;

/******************************************************************************/

  /* Given the address of a ThermalData structure previously initialized,   */
  /* fills all the memory reading the structure of the 3D-IC from a         */
  /* StackDescription structure. Also performs the factorization of the     */
  /* matrix used to emulate heat dissipation.                               */
  /*                                                                        */
  /* After using this function, the 3DIC represented by the data stored in  */
  /* tdata is ready be emulated. It means that the linear system (the       */
  /* matrix and the right hand side vector) is ready to be solved. The RHS  */
  /* B has been filled using the first power value for each floorplan       */
  /* element.                                                               */
  /*                                                                        */
  /* Returns 0 if success, !0 otherwise (factorization error). Please see   */
  /* the "output" field of the function "dgstrf" in SuperLU                 */

  int fill_thermal_data  (ThermalData* tdata, StackDescription* stkd) ;

/******************************************************************************/

  /* Frees all the memory used by the given ThermalData structure           */

  void free_thermal_data  (ThermalData* tdata) ;

/******************************************************************************/


  /* Iteratively consume a time slot emulating heat injection and           */
  /* dissipation. At the end of the call, the 3DIC has the thermal state    */
  /* (temperature array) corresponding to the end of the time slot while    */
  /*  the RHS vector B is already filled with the power values read, if     */
  /* any, from the following time slot.                                     */
  /*                                                                        */
  /* Returns 0 if the emulation succedes, 1 if the time slots are over or   */
  /* <0 if there have been a problem with the computation of the solution   */
  /* of the linear system. See the "info" prameter of "dgstrs" in SuperLU.  */

  int emulate_time_slot (ThermalData* tdata, StackDescription* stkd) ;

/******************************************************************************/

  /* Gives the time, in [seconds], sum of the time slots emulated */

  Time_t get_current_time (ThermalData* tdata) ;

/******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* _3DICE_THERMAL_DATA_H_ */
