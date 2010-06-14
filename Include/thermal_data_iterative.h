/******************************************************************************
 *                                                                            *
 * Header file "Include/thermal_data_iterative.h"                             *
 *                                                                            *
 * EPFL-STI-IEL-ESL                                                           *
 * Bâtiment ELG, ELG 130                                                      *
 * Station 11                                                                 *
 * 1015 Lausanne, Switzerland                    alessandro.vincenzi@epfl.ch  *
 ******************************************************************************/

#ifndef _TL_THERMAL_DATA_ITERATIVE_H_
#define _TL_THERMAL_DATA_ITERATIVE_H_

#include "conductances.h"
#include "stack_description.h"
#include "system_matrix.h"

#include "mvvd.h"

/******************************************************************************
 * "ThermalDataIterative"                                                     *
 ******************************************************************************/

  struct ThermalDataIterative
  {
    double              *Temperatures ;
    double              *Sources ;
    double              *Capacities ;
    struct Conductances *Conductances ;

    int    Size ;
    double initial_temperature ,
           delta_time ;

    struct SystemMatrix SM_A ;
    MV_Vector_double I_Vector_B ;

  } ;

/******************************************************************************/

  int
  init_thermal_data_iterative
  (
    struct StackDescription     *stkd,
    struct ThermalDataIterative *tdata,
    double                      initial_temperature,
    double                      delta_time
  ) ;

  int
  fill_thermal_data_iterative
  (
    struct StackDescription     *stkd,
    struct ThermalDataIterative *tdata
  ) ;

  void
  free_thermal_data_iterative
  (
    struct ThermalDataIterative *tdata
  ) ;

  int
  solve_system_iterative
  (
    struct ThermalDataIterative *tdata,
    double                      total_time,
    double                      *tolerance,
    int                         *max_iterations
#if defined TL_GMRES_ITERATIVE_SOLVER
    ,int                        restart
#endif
  ) ;

  void
  print_system_matrix_iterative (struct ThermalDataIterative *tdata) ;

  void
  print_sources_iterative (struct ThermalDataIterative *tdata) ;

#endif /* _TL_THERMAL_DATA_ITERATIVE_H_ */
