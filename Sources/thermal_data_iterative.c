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


static
void
init_data (double *data, int size, double init_value)
{
  while (size--) *data++ = init_value ;
}

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

  if ( (tdata->Temperatures
         = (double *) malloc ( sizeof(double) * tdata->Size )) == NULL )

    return 0 ;

  if ( (tdata->Sources
         = (double *) malloc ( sizeof(double) * tdata->Size )) == NULL )

    goto sources_fail ;

  if ( (tdata->Capacities
        = (double *) malloc ( sizeof(double) * tdata->Size )) == NULL )

    goto capacities_fail ;

  if ( (tdata->Conductances
         = (struct Conductances *) malloc (sizeof(struct Conductances)*tdata->Size)) == NULL)

    goto conductances_fail ;

  if ( alloc_system_matrix (&tdata->SM_A, TL_CCS_MATRIX,
                            tdata->Size, stkd->Dimensions->Grid.NNz) == 0)
    goto sm_a_fail ;

  if ( alloc_system_vector (&tdata->SV_B, tdata->Size) == 0 )
    goto sv_b_fail ;

  if ( alloc_system_vector (&tdata->SV_X, tdata->Size) == 0 )
    goto sv_x_fail ;

  /* Set initial values */

  init_data (tdata->Temperatures, tdata->Size, initial_temperature) ;
  init_data (tdata->Sources, tdata->Size, 0.0) ;

  return 1 ;

  /* Free if malloc errors */

sv_x_fail:
  free_system_vector (&tdata->SV_B) ;
sv_b_fail :
  free_system_matrix (&tdata->SM_A) ;
sm_a_fail :
  free (tdata->Conductances) ;
conductances_fail :
  free (tdata->Capacities) ;
capacities_fail :
  free (tdata->Sources) ;
sources_fail :
  free (tdata->Temperatures) ;

  return 0 ;
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
  free (tdata->Temperatures) ;
  free (tdata->Sources) ;
  free (tdata->Capacities) ;
  free (tdata->Conductances) ;

  free_system_matrix (&tdata->SM_A) ;
  free_system_vector (&tdata->SV_B) ;
  free_system_vector (&tdata->SV_X) ;

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
  if (stkd->Channel->FlowRateChanged == 1)
  {
    fill_conductances_stack_description (stkd, tdata->Conductances) ;

    fill_capacities_stack_description (stkd, tdata->Capacities,
                                             tdata->delta_time) ;

    fill_system_matrix
    (
      stkd,
      &tdata->SM_A,
      tdata->Conductances,
      tdata->Capacities
    ) ;

    stkd->Channel->FlowRateChanged = 0 ;
    stkd->PowerValuesChanged = 1 ;
  }

  if (stkd->PowerValuesChanged == 1)
  {
    fill_sources_stack_description (stkd, tdata->Sources) ;

    fill_system_vector (&tdata->SV_B, tdata->Sources,
                                      tdata->Capacities,
                                      tdata->Temperatures) ;

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

  CompCol_Mat_double A (
    tdata->SM_A.Size, tdata->SM_A.Size, tdata->SM_A.NNz,
    tdata->SM_A.Values, tdata->SM_A.Rows, tdata->SM_A.Columns
  ) ;

  CompCol_ILUPreconditioner_double Preconditioner (A) ;

#if defined   TL_QMR_ITERATIVE_SOLVER
  CompCol_ILUPreconditioner_double Preconditioner2 (A) ;
#elif defined TL_GMRES_ITERATIVE_SOLVER
  MATRIX_double H(restart+1, restart, 0.0);
#endif

  VECTOR_double B (tdata->SV_B.Values, tdata->SV_B.Size) ;

  VECTOR_double x (tdata->SV_B.Size) ;

  for (counter = 0 ; counter < tdata->SV_B.Size ; counter++)
    x(counter) = tdata->Temperatures[counter] ;

  for ( ; total_time > 0 ; total_time -= tdata->delta_time)
  {
    local_tolerance      = *tolerance ;
    local_max_iterations = *max_iterations ;

    result = iterative_solver
             (
               A, x, B, Preconditioner,
#if defined   TL_QMR_ITERATIVE_SOLVER
               Preconditioner2,
#elif defined TL_GMRES_ITERATIVE_SOLVER
               H, restart,
#endif
               local_max_iterations, local_tolerance
             ) ;

    if ( result != 0)
      return result ;

    for (counter = 0 ; counter < tdata->SV_B.Size ; counter++)
    {
      tdata->Temperatures[counter] = x(counter) ;

      B(counter) = tdata->Sources[counter]
                   + tdata->Capacities[counter] * x(counter) ;
    }
  }

  for (counter = 0 ; counter < tdata->SV_B.Size ; counter++)
    tdata->SV_B.Values[counter] = B(counter) ;

  *max_iterations = local_max_iterations ;
  *tolerance      = local_tolerance ;

  return 0 ;
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
void
print_system_matrix_iterative
(
  struct ThermalDataIterative *tdata
)
{
  if (tdata->SM_A.Storage == TL_CCS_MATRIX)
  {
    print_system_matrix_columns(&tdata->SM_A, "bicg_sm_ccs_columns.txt") ;
    print_system_matrix_rows   (&tdata->SM_A, "bicg_sm_ccs_rows.txt") ;
    print_system_matrix_values (&tdata->SM_A, "bicg_sm_ccs_values.txt") ;
  }
  else if (tdata->SM_A.Storage == TL_CRS_MATRIX)
  {
    print_system_matrix_columns(&tdata->SM_A, "bicg_sm_crs_columns.txt") ;
    print_system_matrix_rows   (&tdata->SM_A, "bicg_sm_crs_rows.txt") ;
    print_system_matrix_values (&tdata->SM_A, "bicg_sm_crs_values.txt") ;
  }
  else
    fprintf (stderr, "Matrix format unknown\n") ;
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

void
print_sources_iterative
(
  struct ThermalDataIterative *tdata
)
{
  int counter ;
  FILE *file = fopen("source_values.txt", "w") ;

  if (file == NULL) return ;

  for (counter = 0 ; counter < tdata->Size ; counter++ )

    if (tdata->Sources[counter] != 0)

      fprintf (file, "%d\t %.6e\n", counter + 1, tdata->Sources[counter]) ;

  fclose (file) ;
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
