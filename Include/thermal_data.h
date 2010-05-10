/******************************************************************************
 *                                                                            *
 * Header file "Include/data.h"                                               *
 *                                                                            *
 * EPFL-STI-IEL-ESL                                                           *
 * Bâtiment ELG, ELG 130                                                      *
 * Station 11                                                                 *
 * 1015 Lausanne, Switzerland                    alessandro.vincenzi@epfl.ch  *
 ******************************************************************************/

#ifndef _TL_THERMAL_DATA_H_
#define _TL_THERMAL_DATA_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include "resistances.h"
#include "stack_description.h"
#include "system_matrix.h"
#include "system_vector.h"
#include "slu_ddefs.h"

/******************************************************************************
 *                                                                            *
 * "Data" : the collection of temperature, source and capacity.               *
 *                                                                            *
 ******************************************************************************/

  typedef struct
  {
    double      *Temperatures ;
    double      *Sources ;
    double      *Capacities ;
    Resistances *Resistances ;

    int    Size ;
    double initial_temperature ,
           delta_time ;

    SystemMatrix SM_A ;
    SystemVector SV_B ;
    SystemVector SV_X ;

    SuperMatrix SLUMatrix_A ,
                SLUMatrix_B ,
                SLUMatrix_L ,
                SLUMatrix_U ,
                SLUMatrix_X ;

    SuperLUStat_t     SLU_Stat ;
    superlu_options_t SLU_Options ;
    mem_usage_t       SLU_MemUsage ;

    int    SLU_Info ,
           *SLU_PermutationMatrixR ,
           *SLU_PermutationMatrixC ,
           *SLU_Etree ;
    double SLU_rpg ,
           SLU_rcond ,
           SLU_ferr ,
           SLU_berr ,
           *SLU_RowsScaleFactors ,
           *SLU_ColumnsScaleFactors ;

    char   SLU_equed[1] ;

  } ThermalData ;

/******************************************************************************/

  int alloc_and_init_thermal_data
  (
    StackDescription *stkd,
    ThermalData *tdata,
    double initial_temperature,
    double delta_time
  ) ;

  void free_thermal_data (ThermalData *tdata) ;

  void fill_thermal_data (StackDescription *stkd, ThermalData *tdata) ;

  int solve_system (ThermalData *tdata, double total_time) ;

#ifdef __cplusplus
}
#endif
#endif /* _TL_THERMAL_DATA_H_ */
