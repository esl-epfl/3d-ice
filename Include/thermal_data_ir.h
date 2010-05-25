/******************************************************************************
 *                                                                            *
 * Header file "Include/thermal_data_ir.h"                                    *
 *                                                                            *
 * EPFL-STI-IEL-ESL                                                           *
 * Bâtiment ELG, ELG 130                                                      *
 * Station 11                                                                 *
 * 1015 Lausanne, Switzerland                    alessandro.vincenzi@epfl.ch  *
 ******************************************************************************/

#ifndef _TL_IR_THERMAL_DATA_H_
#define _TL_IR_THERMAL_DATA_H_

#include "conductances.h"
#include "stack_description.h"
#include "system_matrix.h"
#include "system_vector.h"

/******************************************************************************
 * "CGSThermalData"                                                           *
 ******************************************************************************/

  struct IRThermalData
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
  ir_init_thermal_data   (
                          struct StackDescription *stkd,
                          struct IRThermalData    *tdata,
                          double                  initial_temperature,
                          double                  delta_time
                          ) ;

  int
  ir_fill_thermal_data  (struct StackDescription *stkd,
                         struct IRThermalData *tdata) ;

  void
  ir_free_thermal_data  (struct IRThermalData *tdata) ;

  int
  ir_solve_system        (struct IRThermalData *tdata,
                          double               total_time,
                          double               *tolerance,
                          int                  *max_iterations) ;

  void
  ir_print_system_matrix (struct IRThermalData *tdata) ;

  void
  ir_print_sources       (struct IRThermalData *tdata) ;

#endif /* _TL_IR_THERMAL_DATA_H_ */
