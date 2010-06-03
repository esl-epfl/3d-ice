/******************************************************************************
 *                                                                            *
 * Source file "Sources/thermal_data_cula.c"                                   *
 *                                                                            *
 * EPFL-STI-IEL-ESL                                                           *
 * Bâtiment ELG, ELG 130                                                      *
 * Station 11                                                                 *
 * 1015 Lausanne, Switzerland                    alessandro.vincenzi@epfl.ch  *
 ******************************************************************************/

#include <stdlib.h>
#include <stdio.h>

#include "thermal_data_cula.h"

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
cula_init_thermal_data
(
  struct StackDescription *stkd,
  struct CULAThermalData   *tdata,
  double           initial_temperature,
  double           delta_time
)
{
  if (tdata == NULL) return 0 ;

  tdata->Size                = stkd->Dimensions->Grid.NCells ;
  tdata->initial_temperature = initial_temperature ;
  tdata->delta_time          = delta_time ;

  /* Memory allocation */

  if ( (tdata->Temperatures
         = (double *) malloc ( sizeof(double) * tdata->Size )) == NULL )

    goto temperatures_fail ;

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

  if ( (tdata->Cula_A
         = (culaFloat*) malloc (sizeof(culaFloat)*tdata->Size*tdata->Size)) == NULL)
    goto cula_a_fail ;

  /* Set initial values */

  init_data (tdata->Temperatures, tdata->Size, initial_temperature) ;

  init_data (tdata->Sources, tdata->Size, 0.0) ;

  int index ;
  for (index = 0; index < tdata->Size*tdata->Size; index++)
    tdata->Cula_A[index] = 0.0 ;

  return 1 ;

  /* Free if malloc errors */

cula_a_fail:
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
temperatures_fail :

  return 0 ;
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

void
cula_free_thermal_data (struct CULAThermalData *tdata)
{
  if (tdata == NULL) return ;

  free (tdata->Temperatures) ;
  free (tdata->Sources) ;
  free (tdata->Capacities) ;
  free (tdata->Conductances) ;

  free_system_matrix        (&tdata->SM_A) ;
  free_system_vector        (&tdata->SV_B) ;

  free (&tdata->Cula_A) ;
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

int
cula_fill_thermal_data
(
  struct StackDescription *stkd,
  struct CULAThermalData *tdata
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

    fill_sources_stack_description (stkd, tdata->Sources) ;

    fill_system_vector (&tdata->SV_B, tdata->Sources,
                                      tdata->Capacities,
                                      tdata->Temperatures) ;

    stkd->Channel->FlowRateChanged = 0 ;
    stkd->PowerValuesChanged = 0 ;
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
cula_solve_system
(
  struct CULAThermalData *tdata,
  double         total_time
)
{
  int counter;
  culaStatus status;

  status = culaInitialize();
  if(status != culaNoError)
  {
    printf("%s\n", culaGetStatusString(status));
    return 1 ;
  }

  for ( ; total_time > 0 ; total_time -= tdata->delta_time)
  {

//    status = culaCgesv(N, NRHS, A, N, IPIV, X, N);
//
//      if(status != culaNoError)
//        break ;

    for (counter = 0 ; counter < tdata->SV_B.Size ; counter++)

      tdata->Temperatures[counter] = tdata->SV_B.Values[counter] ;

    fill_system_vector
    (
      &tdata->SV_B,
      tdata->Sources,
      tdata->Capacities,
      tdata->Temperatures
    ) ;

  }

  culaShutdown();

  return 0 ;
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

void
cula_print_system_matrix
(
  struct CULAThermalData *tdata
)
{
  if (tdata->SM_A.Storage == TL_CCS_MATRIX)
  {
    print_system_matrix_columns(&tdata->SM_A, "cula_sm_ccs_columns.txt") ;
    print_system_matrix_rows   (&tdata->SM_A, "cula_sm_ccs_rows.txt") ;
    print_system_matrix_values (&tdata->SM_A, "cula_sm_ccs_values.txt") ;
  }
  else if (tdata->SM_A.Storage == TL_CRS_MATRIX)
  {
    print_system_matrix_columns(&tdata->SM_A, "cula_sm_crs_columns.txt") ;
    print_system_matrix_rows   (&tdata->SM_A, "cula_sm_crs_rows.txt") ;
    print_system_matrix_values (&tdata->SM_A, "cula_sm_crs_values.txt") ;
  }
  else
    fprintf (stderr, "Matrix format unknown\n") ;
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

void
cula_print_sources
(
  struct CULAThermalData *tdata
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
