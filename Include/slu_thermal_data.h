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

#ifdef __cplusplus
extern "C"
{
#endif

#include "conductances.h"
#include "stack_description.h"
#include "system_matrix.h"
#include "system_vector.h"
#include "slu_ddefs.h"

/******************************************************************************
 * "SLUThermalData"                                                           *
 ******************************************************************************/

  typedef struct
  {
    double       *Temperatures ;
    double       *Sources ;
    double       *Capacities ;
    Conductances *Conductances ;

    int    Size ;
    double initial_temperature ,
           delta_time ;

    SystemMatrix SM_A ;
    SystemVector SV_B ;

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

  } SLUThermalData ;

/******************************************************************************/

  int
  slu_init_thermal_data   (
                           StackDescription *stkd,
                           SLUThermalData   *tdata,
                           MatrixStorage_t  storage,
                           double           initial_temperature,
                           double           delta_time
                          ) ;

  int
  slu_fill_thermal_data  (StackDescription *stkd, SLUThermalData *tdata) ;

  void
  slu_free_thermal_data  (SLUThermalData *tdata) ;

  int
  slu_solve_system       (SLUThermalData *tdata, double total_time) ;

  void
  slu_print_system_matrix (SLUThermalData *tdata) ;

  void
  slu_print_sources       (SLUThermalData *tdata) ;

#ifdef __cplusplus
}
#endif
#endif /* _TL_SLU_THERMAL_DATA_H_ */
