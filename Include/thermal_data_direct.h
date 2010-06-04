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
#include "system_matrix.h"
#include "system_vector.h"

#include "slu_ddefs.h"

/******************************************************************************
 * "ThermalDataDirect"                                                        *
 ******************************************************************************/

  struct ThermalDataDirect
  {
    double              *Temperatures ;
    double              *Sources ;
    double              *Capacities ;
    struct Conductances *Conductances ;

    int    Size ;
    double initial_temperature ,
           delta_time ;

    struct SystemMatrix SM_A ;
    struct SystemVector SV_B ;

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
    enum MatrixStorage_t     storage,
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

  void
  print_system_matrix_direct (struct ThermalDataDirect *tdata) ;

  void
  print_sources_direct (struct ThermalDataDirect *tdata) ;

#endif /* _TL_THERMAL_DATA_DIRECT_H_ */
