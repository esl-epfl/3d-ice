/******************************************************************************
 *                                                                            *
 * Header file "Include/thermal_data_cgs.h"                                   *
 *                                                                            *
 * EPFL-STI-IEL-ESL                                                           *
 * Bâtiment ELG, ELG 130                                                      *
 * Station 11                                                                 *
 * 1015 Lausanne, Switzerland                    alessandro.vincenzi@epfl.ch  *
 ******************************************************************************/

#ifndef _TL_CGS_THERMAL_DATA_H_
#define _TL_CGS_THERMAL_DATA_H_

#include "conductances.h"
#include "stack_description.h"
#include "system_matrix.h"
#include "system_vector.h"

/******************************************************************************
 * "CGSThermalData"                                                           *
 ******************************************************************************/

  struct CGSThermalData
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
  cgs_init_thermal_data  (
                          struct StackDescription *stkd,
                          struct CGSThermalData  *tdata,
                          double                  initial_temperature,
                          double                  delta_time
                          ) ;

  int
  cgs_fill_thermal_data (struct StackDescription *stkd,
                         struct CGSThermalData *tdata) ;

  void
  cgs_free_thermal_data (struct CGSThermalData *tdata) ;

  int
  cgs_diag_pre_solve_system      (struct CGSThermalData *tdata,
                                  double                 total_time,
                                  double                 *tolerance,
                                  int                    *max_iterations) ;

  int
  cgs_ilu_pre_solve_system       (struct CGSThermalData *tdata,
                                  double                 total_time,
                                  double                 *tolerance,
                                  int                    *max_iterations) ;

  void
  cgs_print_system_matrix (struct CGSThermalData *tdata) ;

  void
  cgs_print_sources      (struct CGSThermalData *tdata) ;

#endif /* _TL_CGS_THERMAL_DATA_H_ */
