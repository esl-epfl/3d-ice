/******************************************************************************
 *                                                                            *
 * Header file "Include/bicg_thermal_data.h"                                  *
 *                                                                            *
 * EPFL-STI-IEL-ESL                                                           *
 * Bâtiment ELG, ELG 130                                                      *
 * Station 11                                                                 *
 * 1015 Lausanne, Switzerland                    alessandro.vincenzi@epfl.ch  *
 ******************************************************************************/

#ifndef _TL_BICGSTAB_THERMAL_DATA_H_
#define _TL_BICGSTAB_THERMAL_DATA_H_

#include "conductances.h"
#include "stack_description.h"
#include "system_matrix.h"
#include "system_vector.h"

/******************************************************************************
 * "SLUThermalData"                                                           *
 ******************************************************************************/

  struct BICGStabThermalData
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
  bicgstab_init_thermal_data  (
                               struct StackDescription    *stkd,
                               struct BICGStabThermalData *tdata,
                               double                     initial_temperature,
                               double                     delta_time
                              ) ;

  int
  bicgstab_fill_thermal_data (struct StackDescription    *stkd,
                              struct BICGStabThermalData *tdata) ;

  void
  bicgstab_free_thermal_data  (struct BICGStabThermalData *tdata) ;

  int
  bicgstab_solve_system       (struct BICGStabThermalData *tdata,
                               double total_time,
                               double *tolerance,
                               int    *max_iterations) ;

  void
  bicgstab_print_system_matrix (struct BICGStabThermalData *tdata) ;

  void
  bicgstab_print_sources      (struct BICGStabThermalData *tdata) ;

#endif /* _TL_BICGSTAB_THERMAL_DATA_H_ */
