/******************************************************************************
 *                                                                            *
 * Source file "Sources/thermal_data_slu.c"                                   *
 *                                                                            *
 * EPFL-STI-IEL-ESL                                                           *
 * Bâtiment ELG, ELG 130                                                      *
 * Station 11                                                                 *
 * 1015 Lausanne, Switzerland                    alessandro.vincenzi@epfl.ch  *
 ******************************************************************************/

#include <stdlib.h>
#include <stdio.h>

#include "thermal_data_slu.h"

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
slu_init_thermal_data
(
  struct StackDescription *stkd,
  struct SLUThermalData   *tdata,
  enum MatrixStorage_t  storage,
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

  if ( (tdata->SLU_PermutationMatrixR
         = (int *) malloc ( sizeof(int) * tdata->Size )) == NULL )

    goto slu_perm_r_fail ;

  if ( (tdata->SLU_PermutationMatrixC
    = (int *) malloc ( sizeof(int) * tdata->Size )) == NULL )

    goto slu_perm_c_fail ;

  if ( (tdata->SLU_Etree
         = (int *) malloc ( sizeof(int) * tdata->Size )) == NULL )

    goto slu_etree_fail ;

  if ( alloc_system_matrix (&tdata->SM_A, storage,
                            tdata->Size, stkd->Dimensions->Grid.NNz) == 0)
    goto sm_a_fail ;

  if ( alloc_system_vector (&tdata->SV_B, tdata->Size) == 0 )
    goto sv_b_fail ;

  StatInit (&tdata->SLU_Stat) ;

  /* Set initial values */

  init_data (tdata->Temperatures, tdata->Size, initial_temperature) ;

  init_data (tdata->Sources, tdata->Size, 0.0) ;

  set_default_options (&tdata->SLU_Options) ;

  tdata->SLU_Options.Fact            = DOFACT ;
  tdata->SLU_Options.PrintStat       = NO ;
  tdata->SLU_Options.Equil           = NO ;
  tdata->SLU_Options.SymmetricMode   = YES ;
  tdata->SLU_Options.ColPerm         = MMD_AT_PLUS_A ;
  tdata->SLU_Options.DiagPivotThresh = 0.01 ;

  if (storage == TL_CRS_MATRIX)

    dCreate_CompRow_Matrix  /* Matrix A */
    (
      &tdata->SLUMatrix_A, tdata->Size, tdata->Size, tdata->SM_A.NNz,
      tdata->SM_A.Values, tdata->SM_A.Columns, tdata->SM_A.Rows,
      SLU_NR, SLU_D, SLU_GE
    ) ;

  else

    dCreate_CompCol_Matrix  /* Matrix A */
    (
      &tdata->SLUMatrix_A, tdata->Size, tdata->Size, tdata->SM_A.NNz,
      tdata->SM_A.Values, tdata->SM_A.Rows, tdata->SM_A.Columns,
      SLU_NC, SLU_D, SLU_GE
    ) ;

  dCreate_Dense_Matrix  /* Vector B */
  (
    &tdata->SLUMatrix_B, tdata->Size, 1,
    tdata->SV_B.Values, tdata->Size,
    SLU_DN, SLU_D, SLU_GE
  );

  return 1 ;

  /* Free if malloc errors */

sv_b_fail :
  free_system_matrix (&tdata->SM_A) ;
sm_a_fail :
  free (tdata->SLU_Etree) ;
slu_etree_fail :
  free (tdata->SLU_PermutationMatrixR);
slu_perm_c_fail :
  free (tdata->SLU_PermutationMatrixR) ;
slu_perm_r_fail :
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
slu_free_thermal_data (struct SLUThermalData *tdata)
{
  if (tdata == NULL) return ;

  free (tdata->Temperatures) ;
  free (tdata->Sources) ;
  free (tdata->Capacities) ;
  free (tdata->Conductances) ;

  free (tdata->SLU_PermutationMatrixR) ;
  free (tdata->SLU_PermutationMatrixC) ;
  free (tdata->SLU_Etree) ;

  StatFree (&tdata->SLU_Stat) ;

  Destroy_SuperMatrix_Store (&tdata->SLUMatrix_A) ;
  free_system_matrix        (&tdata->SM_A) ;

  Destroy_SuperMatrix_Store (&tdata->SLUMatrix_B);
  free_system_vector        (&tdata->SV_B) ;

  if (tdata->SLU_Options.Fact != DOFACT )
  {
    Destroy_CompCol_Permuted (&tdata->SLUMatrix_A_Permuted) ;
    Destroy_SuperNode_Matrix (&tdata->SLUMatrix_L) ;
    Destroy_CompCol_Matrix   (&tdata->SLUMatrix_U) ;
  }
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

int
slu_fill_thermal_data
(
  struct StackDescription *stkd,
  struct SLUThermalData *tdata
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

    if (tdata->SLU_Options.Fact == FACTORED)

      tdata->SLU_Options.Fact = SamePattern ;

    else

      tdata->SLU_Options.Fact = DOFACT ;


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
  }

  if (tdata->SLU_Options.Fact != FACTORED )
  {
    get_perm_c (tdata->SLU_Options.ColPerm,
                &tdata->SLUMatrix_A,
                tdata->SLU_PermutationMatrixC) ;

    sp_preorder (&tdata->SLU_Options, &tdata->SLUMatrix_A,
                 tdata->SLU_PermutationMatrixC, tdata->SLU_Etree,
                 &tdata->SLUMatrix_A_Permuted) ;

    dgstrf (&tdata->SLU_Options, &tdata->SLUMatrix_A_Permuted,
            sp_ienv(2), sp_ienv(1), /* relax and panel size */
            tdata->SLU_Etree,
            NULL, 0,                /* work and lwork */
            tdata->SLU_PermutationMatrixC, tdata->SLU_PermutationMatrixR,
            &tdata->SLUMatrix_L, &tdata->SLUMatrix_U,
            &tdata->SLU_Stat, &tdata->SLU_Info) ;

    tdata->SLU_Options.Fact = FACTORED ;

    return tdata->SLU_Info ;
  }

  return 0 ;
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

int
slu_solve_system
(
  struct SLUThermalData *tdata,
  double         total_time
)
{
  int counter;

  if (tdata->SLU_Options.Fact == DOFACT)
    return 1 ;

  for ( ; total_time > 0 ; total_time -= tdata->delta_time)
  {

    dgstrs
    (
      NOTRANS,
      &tdata->SLUMatrix_L,
      &tdata->SLUMatrix_U,
      tdata->SLU_PermutationMatrixC,
      tdata->SLU_PermutationMatrixR,
      &tdata->SLUMatrix_B,
      &tdata->SLU_Stat,
      &tdata->SLU_Info
    ) ;

    if (tdata->SLU_Info != 0)
      break ;

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

  return tdata->SLU_Info ;
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

void
slu_print_system_matrix
(
  struct SLUThermalData *tdata
)
{
  if (tdata->SM_A.Storage == TL_CCS_MATRIX)
  {
    print_system_matrix_columns(&tdata->SM_A, "slu_sm_ccs_columns.txt") ;
    print_system_matrix_rows   (&tdata->SM_A, "slu_sm_ccs_rows.txt") ;
    print_system_matrix_values (&tdata->SM_A, "slu_sm_ccs_values.txt") ;
  }
  else if (tdata->SM_A.Storage == TL_CRS_MATRIX)
  {
    print_system_matrix_columns(&tdata->SM_A, "slu_sm_crs_columns.txt") ;
    print_system_matrix_rows   (&tdata->SM_A, "slu_sm_crs_rows.txt") ;
    print_system_matrix_values (&tdata->SM_A, "slu_sm_crs_values.txt") ;
  }
  else
    fprintf (stderr, "Matrix format unknown\n") ;
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

void
slu_print_sources
(
  struct SLUThermalData *tdata
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
