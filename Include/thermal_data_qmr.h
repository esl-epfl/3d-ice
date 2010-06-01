/******************************************************************************
 *                                                                            *
 * Header file "Include/thermal_data_qmr.h"                                   *
 *                                                                            *
 * EPFL-STI-IEL-ESL                                                           *
 * Bâtiment ELG, ELG 130                                                      *
 * Station 11                                                                 *
 * 1015 Lausanne, Switzerland                    alessandro.vincenzi@epfl.ch  *
 ******************************************************************************/

#ifndef _TL_QMR_THERMAL_DATA_H_
#define _TL_QMR_THERMAL_DATA_H_

#include "conductances.h"
#include "stack_description.h"
#include "system_matrix.h"
#include "system_vector.h"

/******************************************************************************
 * "QMRThermalData"                                                           *
 ******************************************************************************/

  struct QMRThermalData
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
    struct SystemVector SV_X ;

  } ;

/******************************************************************************/

  int
  qmr_init_thermal_data   (
                          struct StackDescription *stkd,
                          struct QMRThermalData    *tdata,
                          double                  initial_temperature,
                          double                  delta_time
                          ) ;

  int
  qmr_fill_thermal_data  (struct StackDescription *stkd,
                         struct QMRThermalData *tdata) ;

  void
  qmr_free_thermal_data  (struct QMRThermalData *tdata) ;

  int
  qmr_solve_system (struct QMRThermalData *tdata,
                    double               total_time,
                    double               *tolerance,
                    int                  *max_iterations) ;

  void
  qmr_print_system_matrix (struct QMRThermalData *tdata) ;

  void
  qmr_print_sources       (struct QMRThermalData *tdata) ;

#endif /* _TL_QMR_THERMAL_DATA_H_ */
