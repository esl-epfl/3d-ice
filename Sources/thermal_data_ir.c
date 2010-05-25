/******************************************************************************
 *                                                                            *
 * Source file "Sources/data.c"                                               *
 *                                                                            *
 * EPFL-STI-IEL-ESL                                                           *
 * Bâtiment ELG, ELG 130                                                      *
 * Station 11                                                                 *
 * 1015 Lausanne, Switzerland                    alessandro.vincenzi@epfl.ch  *
 ******************************************************************************/

#include <stdlib.h>
#include <stdio.h>

#include "thermal_data_ir.h"

#include "diagpre_double.h"
#include "compcol_double.h"
#include "mvblasd.h"
#include "ir.h"

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
ir_init_thermal_data
(
  struct StackDescription *stkd,
  struct IRThermalData   *tdata,
  double                  initial_temperature,
  double                  delta_time
)
{
  if (tdata == NULL) return 0 ;

  tdata->Size                = stkd->Dimensions->Grid.NCells ;
  tdata->initial_temperature = initial_temperature ;
  tdata->delta_time          = delta_time ;

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

//  (tdata->BICG_Matrix_A).newsize (tdata->Size,
//                                  tdata->Size,
//                                  stkd->Dimensions->Grid.NNz);
//
//  (tdata->BICG_Vector_B).newsize (tdata->Size) ;
//
//  (tdata->BICG_Vector_X).newsize (tdata->Size) ;

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
ir_free_thermal_data (struct IRThermalData *tdata)
{
  free (tdata->Temperatures) ;
  free (tdata->Sources) ;
  free (tdata->Capacities) ;
  free (tdata->Conductances) ;

  free_system_matrix (&tdata->SM_A) ;
  free_system_vector (&tdata->SV_B) ;
  free_system_vector (&tdata->SV_X) ;
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

int
ir_fill_thermal_data
(
  struct StackDescription *stkd,
  struct IRThermalData   *tdata
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
ir_solve_system
(
  struct IRThermalData  *tdata,
  double                 total_time,
  double                 *tolerance,
  int                    *max_iterations
)
{
  int counter, _max_iterations = *max_iterations ;
  double _tolerance = *tolerance;

  for (counter = 0 ; counter < tdata->SV_X.Size ; counter++)
    tdata->SV_X.Values[counter] = tdata->Temperatures[counter] ;

  for ( ; total_time > 0 ; total_time -= tdata->delta_time)
  {
    CompCol_Mat_double A (
      tdata->SM_A.Size, tdata->SM_A.Size, tdata->SM_A.NNz,
      tdata->SM_A.Values, tdata->SM_A.Rows, tdata->SM_A.Columns
    ) ;

    DiagPreconditioner_double Preconditioner (A) ;

    VECTOR_double B (tdata->SV_B.Values, tdata->SV_B.Size) ;

    VECTOR_double x (tdata->SV_X.Values, tdata->SV_X.Size) ;

    _tolerance      = *tolerance ;
    _max_iterations = *max_iterations ;

    if ( IR (A, x, B, Preconditioner, _max_iterations, _tolerance) == 1)

      return 1 ;

    for (counter = 0 ; counter < tdata->SV_B.Size ; counter++)
      tdata->Temperatures[counter] = x(counter) ;

    fill_system_vector (
      &tdata->SV_B,
      tdata->Sources, tdata->Capacities, tdata->Temperatures
    ) ;

  }

  *max_iterations = _max_iterations ;
  *tolerance      = _tolerance ;

  return 0 ;
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

void
ir_print_system_matrix
(
  struct IRThermalData *tdata
)
{
  if (tdata->SM_A.Storage == TL_CCS_MATRIX)
  {
    print_system_matrix_columns(&tdata->SM_A, "ir_sm_ccs_columns.txt") ;
    print_system_matrix_rows   (&tdata->SM_A, "ir_sm_ccs_rows.txt") ;
    print_system_matrix_values (&tdata->SM_A, "ir_sm_ccs_values.txt") ;
  }
  else if (tdata->SM_A.Storage == TL_CRS_MATRIX)
  {
    print_system_matrix_columns(&tdata->SM_A, "ir_sm_crs_columns.txt") ;
    print_system_matrix_rows   (&tdata->SM_A, "ir_sm_crs_rows.txt") ;
    print_system_matrix_values (&tdata->SM_A, "ir_sm_crs_values.txt") ;
  }
  else
    fprintf (stderr, "Matrix format unknown\n") ;
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

void
ir_print_sources
(
  struct IRThermalData *tdata
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
