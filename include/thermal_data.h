/******************************************************************************
 * Header file "3D-ICe/include/termal_data.h"                                 *
 *                                                                            *
 * This file is part of 3D-ICe (http://esl.epfl.ch/3D-ICe), revision 0.1      *
 *                                                                            *
 * 3D-ICe is free software: you can redistribute it and/or modify it under    *
 * the terms of the GNU General Public License as published by the Free       *
 * Software Foundation, either version 3 of the License, or any later         *
 * version.                                                                   *
 *                                                                            *
 * 3D-ICe is distributed in the hope that it will be useful, but WITHOUT      *
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or      *
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for   *
 * more details.                                                              *
 *                                                                            *
 * You should have received a copy of the GNU General Public License along    *
 * with 3D-ICe.  If not, see <http://www.gnu.org/licenses/>.                  *
 *                                                                            *
 * Copyright (C) 2010,                                                        *
 * Embedded Systems Laboratory - Ecole Polytechnique Federale de Lausanne.    *
 * All Rights Reserved.                                                       *
 *                                                                            *
 * Authors: Alessandro Vincenzi, Arvind Sridhar.                              *
 *                                                                            *
 * EPFL-STI-IEL-ESL                                                           *
 * Bâtiment ELG, ELG 130                                                      *
 * Station 11                                                                 *
 * 1015 Lausanne, Switzerland                          threed-ice@esl.epfl.ch *
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

    Time_t        DeltaTime ;
    Time_t        SlotTime ;

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

  int init_thermal_data
  (
    StackDescription* stkd,
    ThermalData*      tdata,
    Temperature_t     initial_temperature,
    Time_t            delta_time,
    Time_t            slot_time
  ) ;

/******************************************************************************/

  int fill_thermal_data  (StackDescription* stkd, ThermalData* tdata) ;

/******************************************************************************/

  void free_thermal_data  (ThermalData* tdata) ;

/******************************************************************************/

  int emulate_power_slot (ThermalData* tdata) ;

/******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* _3DICE_THERMAL_DATA_H_ */
