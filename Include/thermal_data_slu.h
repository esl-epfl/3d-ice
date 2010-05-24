/******************************************************************************
 *                                                                            *
 * Header file "Include/slu_thermal_data.h"                                   *
 *                                                                            *
 * EPFL-STI-IEL-ESL                                                           *
 * Bâtiment ELG, ELG 130                                                      *
 * Station 11                                                                 *
 * 1015 Lausanne, Switzerland                    alessandro.vincenzi@epfl.ch  *
 ******************************************************************************/

#ifndef _TL_SLU_THERMAL_DATA_H_
#define _TL_SLU_THERMAL_DATA_H_

#include "conductances.h"
#include "stack_description.h"
#include "system_matrix.h"
#include "system_vector.h"
#include "slu_ddefs.h"

/******************************************************************************
 * "SLUThermalData"                                                           *
 ******************************************************************************/

  struct SLUThermalData
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
  slu_init_thermal_data   (
                           struct StackDescription *stkd,
                           struct SLUThermalData   *tdata,
                           enum MatrixStorage_t  storage,
                           double           initial_temperature,
                           double           delta_time
                          ) ;

  int
  slu_fill_thermal_data  (struct StackDescription *stkd, struct SLUThermalData *tdata) ;

  void
  slu_free_thermal_data  (struct SLUThermalData *tdata) ;

  int
  slu_solve_system       (struct SLUThermalData *tdata, double total_time) ;

  void
  slu_print_system_matrix (struct SLUThermalData *tdata) ;

  void
  slu_print_sources       (struct SLUThermalData *tdata) ;

#endif /* _TL_SLU_THERMAL_DATA_H_ */
