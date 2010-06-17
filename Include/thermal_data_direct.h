/******************************************************************************
 *                                                                            *
 * Header file "Include/thermal_data_direct.h"                                *
 *                                                                            *
 * EPFL-STI-IEL-ESL                                                           *
 * Bâtiment ELG, ELG 130                                                      *
 * Station 11                                                                 *
 * 1015 Lausanne, Switzerland                    alessandro.vincenzi@epfl.ch  *
 ******************************************************************************/

#ifndef _TL_THERMAL_DATA_DIRECT_H_
#define _TL_THERMAL_DATA_DIRECT_H_

#include "conductances.h"
#include "stack_description.h"

#include "mvvd.h"
#include "comprow_double.h"

#include "slu_ddefs.h"

/******************************************************************************
 * "ThermalDataDirect"                                                        *
 ******************************************************************************/

  struct ThermalDataDirect
  {
    MV_Vector_double Temperatures ;
    MV_Vector_double Sources ;
    MV_Vector_double Capacities ;

    struct Conductances *Conductances ;

    int    Size ;
    double initial_temperature ,
           delta_time ;

    CompRow_Mat_double D_Matrix_A ;

    SuperMatrix SLUMatrix_A ,
                SLUMatrix_A_Permuted ,
                SLUMatrix_B ,
                SLUMatrix_L ,
                SLUMatrix_U ;

    SuperLUStat_t     SLU_Stat ;
    superlu_options_t SLU_Options ;

    int    SLU_Info ,
           *SLU_PermutationMatrixR ,
           *SLU_PermutationMatrixC ,
           *SLU_Etree ;

  } ;

/******************************************************************************/

  int
  init_thermal_data_direct
  (
    struct StackDescription  *stkd,
    struct ThermalDataDirect *tdata,
    double                   initial_temperature,
    double                   delta_time
  ) ;

  int
  fill_thermal_data_direct
  (
    struct StackDescription  *stkd,
    struct ThermalDataDirect *tdata
  ) ;

  void
  free_thermal_data_direct
  (
    struct ThermalDataDirect *tdata
  ) ;

  int
  solve_system_direct
  (
    struct ThermalDataDirect *tdata,
    double total_time
  ) ;

#endif /* _TL_THERMAL_DATA_DIRECT_H_ */
