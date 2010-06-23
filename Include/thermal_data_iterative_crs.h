/******************************************************************************
 *                                                                            *
 * Header file "Include/thermal_data_iterative_crs.h"                         *
 *                                                                            *
 * EPFL-STI-IEL-ESL                                                           *
 * Bâtiment ELG, ELG 130                                                      *
 * Station 11                                                                 *
 * 1015 Lausanne, Switzerland                    alessandro.vincenzi@epfl.ch  *
 ******************************************************************************/

#ifndef _TL_THERMAL_DATA_ITERATIVE_CRS_H_
#define _TL_THERMAL_DATA_ITERATIVE_CRS_H_

#include "conductances.h"
#include "stack_description.h"

#include "mvv.h"
#include "mvm.h"
#include "comprow_double.h"
#include "ilupre_double.h"

/******************************************************************************
 * "CRSThermalDataIterative"                                                     *
 ******************************************************************************/

  struct CRSThermalDataIterative
  {
    MV_Vector_double Temperatures ;
    MV_Vector_double Sources ;
    MV_Vector_double Capacities ;

    struct Conductances *Conductances ;

    int    Size ;
    double initial_temperature ,
           delta_time ;

    CompRow_Mat_double I_Matrix_A ;
    MV_Vector_double   I_Vector_B ;

    CompRow_ILUPreconditioner Preconditioner;

#if defined TL_QMR_ITERATIVE_SOLVER
    CompRow_ILUPreconditioner Preconditioner2;
#endif
#if defined TL_GMRES_ITERATIVE_SOLVER
    MV_ColMat_double H;
#endif

  } ;

/******************************************************************************/

  int
  crs_init_thermal_data_iterative
  (
    struct StackDescription     *stkd,
    struct CRSThermalDataIterative *tdata,
    double                      initial_temperature,
    double                      delta_time
#if defined TL_GMRES_ITERATIVE_SOLVER
    ,int                        restert
#endif
  ) ;

  int
  crs_fill_thermal_data_iterative
  (
    struct StackDescription     *stkd,
    struct CRSThermalDataIterative *tdata
  ) ;

  void
  crs_free_thermal_data_iterative
  (
    struct CRSThermalDataIterative *tdata
  ) ;

  int
  crs_solve_system_iterative
  (
    struct CRSThermalDataIterative *tdata,
    double                      total_time,
    double                      *tolerance,
    int                         *max_iterations
#if defined TL_GMRES_ITERATIVE_SOLVER
    ,int                        restart
#endif
  ) ;

#endif /* _TL_THERMAL_DATA_ITERATIVE_H_ */
