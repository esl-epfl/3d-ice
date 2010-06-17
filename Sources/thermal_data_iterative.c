/******************************************************************************
 *                                                                            *
 * Source file "Sources/thermal_data_iterative.c"                             *
 *                                                                            *
 * EPFL-STI-IEL-ESL                                                           *
 * Bâtiment ELG, ELG 130                                                      *
 * Station 11                                                                 *
 * 1015 Lausanne, Switzerland                    alessandro.vincenzi@epfl.ch  *
 ******************************************************************************/

#include <stdlib.h>
#include <stdio.h>

#include "thermal_data_iterative.h"

#include "ilupre_double.h"
#include "compcol_double.h"

#include "mvblasd.h"

#if defined TL_BICG_ITERATIVE_SOLVER

#  include "bicg.h"
#  define iterative_solver BiCG

#elif defined TL_BICGSTAB_ITERATIVE_SOLVER

#  include "bicgstab.h"
#  define iterative_solver BiCGSTAB

#elif defined TL_CGS_ITERATIVE_SOLVER

#  include "cgs.h"
#  define iterative_solver CGS

#elif defined TL_GMRES_ITERATIVE_SOLVER

#  include "gmres.h"
#  include "mvmd.h"
#  define iterative_solver GMRES

#elif TL_IR_ITERATIVE_SOLVER

#  include "ir.h"
#  define iterative_solver IR

#elif TL_QMR_ITERATIVE_SOLVER

#  include "qmr.h"
#  define iterative_solver QMR

#endif

#ifdef SUPPORT_CUBLAS
#  include "cublas.h"
#endif

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

int
init_thermal_data_iterative
(
  struct StackDescription     *stkd,
  struct ThermalDataIterative *tdata,
  double                      initial_temperature,
  double                      delta_time
)
{
  if (tdata == NULL) return 0 ;

  tdata->Size                = stkd->Dimensions->Grid.NCells ;
  tdata->initial_temperature = initial_temperature ;
  tdata->delta_time          = delta_time ;

#ifdef SUPPORT_CUBLAS
  cublasStatus stat;

  cublasInit();

  stat = cublasGetError();
  if (stat != CUBLAS_STATUS_SUCCESS)
  {
    printf ("CUBLAS: cublasInit() error %d\n", stat);
    return 0;
  }
#endif

  /* Memory allocation */

  if ( (tdata->Conductances
         = (struct Conductances *) malloc (sizeof(struct Conductances)*tdata->Size)) == NULL)

    return 0 ;

  tdata->Temperatures.newsize(tdata->Size) ;
  tdata->Temperatures = initial_temperature ;

  tdata->Sources.newsize(tdata->Size) ;
  tdata->Sources = 0.0 ;

  tdata->Capacities.newsize(tdata->Size) ;

  tdata->I_Vector_B.newsize(tdata->Size) ;

  tdata->I_Matrix_A.newsize(tdata->Size, tdata->Size,
                            stkd->Dimensions->Grid.NNz) ;

  return 1 ;
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

void
free_thermal_data_iterative
(
  struct ThermalDataIterative *tdata
)
{
  free (tdata->Conductances) ;

#ifdef SUPPORT_CUBLAS
  cublasStatus stat;

  cublasShutdown();

  stat = cublasGetError();
  if (stat != CUBLAS_STATUS_SUCCESS)
    printf ("CUBLAS: cublasShutdown() error %d\n", stat);
#endif

}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

int
fill_thermal_data_iterative
(
  struct StackDescription     *stkd,
  struct ThermalDataIterative *tdata
)
{
  //
  // If the flow rate changed then all the conductances and then the
  // capacities are re evaluated. With these new values the A matrix
  // must be re filled and the preconditioner must be rebuilt.
  // Since the flow rate is used also for heat injection, we must
  // re evauate also the sources.
  //

  if (stkd->Channel->FlowRateChanged == 1)
  {
    fill_conductances_stack_description (stkd, tdata->Conductances) ;

    fill_capacities_stack_description (stkd, &tdata->Capacities[0],
                                             tdata->delta_time) ;

    stkd->Channel->FlowRateChanged = 0 ;

    fill_ccs_system_matrix_stack_description
    (
      stkd,
      tdata->Conductances,
      &tdata->Capacities[0],
      &tdata->I_Matrix_A.col_ptr(0),
      &tdata->I_Matrix_A.row_ind(0),
      &tdata->I_Matrix_A.val(0)
    ) ;

    tdata->Preconditioner.reset(tdata->I_Matrix_A);
#if defined TL_QMR_ITERATIVE_SOLVER
    tdata->Preconditioner2.reset(tdata->I_Matrix_A);
#endif

    fill_sources_stack_description (stkd, &tdata->Sources[0]) ;
    stkd->PowerValuesChanged = 0 ;
  }

  //
  // If power values are different ne need to re compute the sources array.
  //

  if (stkd->PowerValuesChanged == 1)
  {
    fill_sources_stack_description (stkd, &tdata->Sources[0]) ;
    stkd->PowerValuesChanged = 0 ;
  }

  return 0 ;
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

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
)
{
  int result, counter, local_max_iterations = *max_iterations ;
  double local_tolerance = *tolerance;

#if defined TL_GMRES_ITERATIVE_SOLVER
  MATRIX_double H(restart+1, restart, 0.0);
#endif

  for ( ; total_time > 0 ; total_time -= tdata->delta_time)
  {
    local_tolerance      = *tolerance ;
    local_max_iterations = *max_iterations ;

    for (counter = 0 ; counter < tdata->Size ; counter++)
    {
      tdata->I_Vector_B[counter] = tdata->Sources[counter]
                                   + tdata->Capacities[counter]
                                     * tdata->Temperatures[counter] ;
    }

    result = iterative_solver
             (
               tdata->I_Matrix_A, tdata->Temperatures, tdata->I_Vector_B,
               tdata->Preconditioner,
#if defined   TL_QMR_ITERATIVE_SOLVER
               tdata->Preconditioner2,
#elif defined TL_GMRES_ITERATIVE_SOLVER
               H, restart,
#endif
               local_max_iterations, local_tolerance
             ) ;

    if ( result != 0)
      return result ;
  }

  *max_iterations = local_max_iterations ;
  *tolerance      = local_tolerance ;

  return 0 ;
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
