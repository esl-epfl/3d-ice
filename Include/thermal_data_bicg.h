/******************************************************************************
 *                                                                            *
 * Header file "Include/thermal_data_bicg.h"                                  *
 *                                                                            *
 * EPFL-STI-IEL-ESL                                                           *
 * Bâtiment ELG, ELG 130                                                      *
 * Station 11                                                                 *
 * 1015 Lausanne, Switzerland                    alessandro.vincenzi@epfl.ch  *
 ******************************************************************************/

#ifndef _TL_BICG_THERMAL_DATA_H_
#define _TL_BICG_THERMAL_DATA_H_

#include "conductances.h"
#include "stack_description.h"
#include "system_matrix.h"
#include "system_vector.h"

/******************************************************************************
 * "BICGThermalData"                                                          *
 ******************************************************************************/

  struct BICGThermalData
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
  bicg_init_thermal_data  (
                           struct StackDescription *stkd,
                           struct BICGThermalData  *tdata,
                           double                  initial_temperature,
                           double                  delta_time
                          ) ;

  int
  bicg_fill_thermal_data (struct StackDescription *stkd,
                          struct BICGThermalData *tdata) ;

  void
  bicg_free_thermal_data  (struct BICGThermalData *tdata) ;

  int
  bicg_solve_system
  (
    struct BICGThermalData *tdata,
    double                 total_time,
    double                 *tolerance,
    int                    *max_iterations
  ) ;

  void
  bicg_print_system_matrix (struct BICGThermalData *tdata) ;

  void
  bicg_print_sources      (struct BICGThermalData *tdata) ;

#endif /* _TL_BICG_THERMAL_DATA_H_ */
