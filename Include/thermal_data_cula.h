/******************************************************************************
 *                                                                            *
 * Header file "Include/thermal_data_cula.h"                                   *
 *                                                                            *
 * EPFL-STI-IEL-ESL                                                           *
 * Bâtiment ELG, ELG 130                                                      *
 * Station 11                                                                 *
 * 1015 Lausanne, Switzerland                    alessandro.vincenzi@epfl.ch  *
 ******************************************************************************/

#ifndef _TL_CULA_THERMAL_DATA_H_
#define _TL_CULA_THERMAL_DATA_H_

#include "conductances.h"
#include "stack_description.h"
#include "system_matrix.h"
#include "system_vector.h"

#include "culapack.h"

/******************************************************************************
 * "CULAThermalData"                                                           *
 ******************************************************************************/

  struct CULAThermalData
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

    culaFloat* Cula_A ;

  } ;

/******************************************************************************/

  int
  cula_init_thermal_data  (
                           struct StackDescription *stkd,
                           struct CULAThermalData   *tdata,
                           double           initial_temperature,
                           double           delta_time
                          ) ;

  int
  cula_fill_thermal_data  (struct StackDescription *stkd, struct CULAThermalData *tdata) ;

  void
  cula_free_thermal_data  (struct CULAThermalData *tdata) ;

  int
  cula_solve_system       (struct CULAThermalData *tdata, double total_time) ;

  void
  cula_print_system_matrix (struct CULAThermalData *tdata) ;

  void
  cula_print_sources       (struct CULAThermalData *tdata) ;

#endif /* _TL_CULA_THERMAL_DATA_H_ */
