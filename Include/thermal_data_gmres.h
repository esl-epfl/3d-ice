/******************************************************************************
 *                                                                            *
 * Header file "Include/bicg_thermal_data.h"                                  *
 *                                                                            *
 * EPFL-STI-IEL-ESL                                                           *
 * Bâtiment ELG, ELG 130                                                      *
 * Station 11                                                                 *
 * 1015 Lausanne, Switzerland                    alessandro.vincenzi@epfl.ch  *
 ******************************************************************************/

#ifndef _TL_GMRES_THERMAL_DATA_H_
#define _TL_GMRES_THERMAL_DATA_H_

#include "conductances.h"
#include "stack_description.h"
#include "system_matrix.h"
#include "system_vector.h"

/******************************************************************************
 * "GMRESThermalData"                                                         *
 ******************************************************************************/

  struct GMRESThermalData
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
  gmres_init_thermal_data  (
                            struct StackDescription *stkd,
                            struct GMRESThermalData *tdata,
                            double                  initial_temperature,
                            double                  delta_time
                           ) ;

  int
  gmres_fill_thermal_data (struct StackDescription *stkd,
                           struct GMRESThermalData *tdata) ;

  void
  gmres_free_thermal_data (struct GMRESThermalData *tdata) ;

  int
  gmres_solve_system      (struct GMRESThermalData *tdata,
                           double                  total_time,
                           double                  *tolerance,
                           int                     *max_iterations,
                           int                     restart) ;

  void
  gmres_print_system_matrix (struct GMRESThermalData *tdata) ;

  void
  gmres_print_sources       (struct GMRESThermalData *tdata) ;

#endif /* _TL_GMRES_THERMAL_DATA_H_ */
