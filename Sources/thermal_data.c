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

#include "thermal_data.h"

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
alloc_and_init_thermal_data
(
  StackDescription *stkd,
  ThermalData *tdata,
  double initial_temperature,
  double delta_time
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

  if ( (tdata->Resistances
         = (Resistances *) malloc ( sizeof(Resistances)*tdata->Size )) == NULL)

    goto resistances_fail ;

  if ( (tdata->SLU_PermutationMatrixR
         = (int *) malloc ( sizeof(int) * tdata->Size )) == NULL )

    goto slu_perm_r_fail ;

  if ( (tdata->SLU_PermutationMatrixC
    = (int *) malloc ( sizeof(int) * tdata->Size )) == NULL )

    goto slu_perm_c_fail ;

  if ( (tdata->SLU_Etree
         = (int *) malloc ( sizeof(int) * tdata->Size )) == NULL )

    goto slu_etree_fail ;

  if ( (tdata->SLU_RowsScaleFactors
         = (double *) malloc ( sizeof(double) * tdata->Size )) == NULL )

    goto slu_row_scale_fail ;

  if ( (tdata->SLU_ColumnsScaleFactors
         = (double *) malloc ( sizeof(double) * tdata->Size )) == NULL )

    goto slu_column_scale_fail ;

  if ( alloc_system_matrix (&tdata->SM_A, tdata->Size,
                            stkd->Dimensions->Grid.NNz) == 0)
    goto sm_a_fail ;

  if ( alloc_system_vector (&tdata->SV_B, tdata->Size) == 0 )
    goto sv_b_fail ;

  StatInit (&tdata->SLU_Stat) ;

  /* Set initial values */

  init_data (tdata->Temperatures, tdata->Size, initial_temperature) ;

  init_data (tdata->Sources, tdata->Size, 0.0) ;

  set_default_options (&tdata->SLU_Options) ;

  tdata->SLU_Options.PrintStat       = NO ;
  tdata->SLU_Options.Equil           = NO ;
  tdata->SLU_Options.SymmetricMode   = YES ;
  tdata->SLU_Options.ColPerm         = MMD_AT_PLUS_A ;
  tdata->SLU_Options.DiagPivotThresh = 0.01 ;

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

  dCreate_Dense_Matrix  /* Vector X */
  (
    &tdata->SLUMatrix_X, tdata->Size, 1,
    tdata->Temperatures, tdata->Size,
    SLU_DN, SLU_D, SLU_GE
  );

  return 1 ;

  /* Free if malloc errors */

sv_b_fail :
  free_system_matrix (&tdata->SM_A) ;
sm_a_fail :
  free (tdata->SLU_ColumnsScaleFactors) ;
slu_column_scale_fail :
  free (tdata->SLU_RowsScaleFactors) ;
slu_row_scale_fail :
  free (tdata->SLU_Etree) ;
slu_etree_fail :
  free (tdata->SLU_PermutationMatrixR);
slu_perm_c_fail :
  free (tdata->SLU_PermutationMatrixR) ;
slu_perm_r_fail :
  free (tdata->Resistances) ;
resistances_fail :
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
free_thermal_data (ThermalData *tdata)
{
  if (tdata == NULL) return ;

  free (tdata->Sources) ;
  free (tdata->Capacities) ;
  free (tdata->Resistances) ;

  free (tdata->SLU_PermutationMatrixR) ;
  free (tdata->SLU_PermutationMatrixC) ;
  free (tdata->SLU_Etree) ;
  free (tdata->SLU_RowsScaleFactors) ;
  free (tdata->SLU_ColumnsScaleFactors) ;

  StatFree (&tdata->SLU_Stat) ;

  Destroy_SuperMatrix_Store (&tdata->SLUMatrix_A) ;
  free_system_matrix        (&tdata->SM_A) ;

  Destroy_SuperMatrix_Store (&tdata->SLUMatrix_B);
  free_system_vector        (&tdata->SV_B) ;

  Destroy_SuperMatrix_Store (&tdata->SLUMatrix_X);
  free                      (tdata->Temperatures) ;

}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

void
fill_thermal_data
(
  StackDescription *stkd,
  ThermalData *tdata
)
{
  fill_resistances_stack_description
  (
    stkd,
    tdata->Resistances
  ) ;

  fill_capacities_stack_description
  (
    stkd,
    tdata->Capacities,
    tdata->delta_time
  ) ;

  fill_sources_stack_description
  (
    stkd,
    tdata->Sources
  ) ;

  fill_system_matrix
  (
    stkd,
    &tdata->SM_A,
    tdata->Resistances,
    tdata->Capacities
  );

  fill_system_vector
  (
    &tdata->SV_B,
    tdata->Sources,
    tdata->Capacities,
    tdata->Temperatures
  );

//  get_perm_c
//  (
//    tdata->SLU_Options.ColPerm,
//    &tdata->SLUMatrix_A,
//    tdata->SLU_PermutationMatrixC
//  ) ;
//
//  sp_preorder
//  (
//    &tdata->SLU_Options,
//    &tdata->SLUMatrix_A,
//    tdata->SLU_PermutationMatrixC,
//    tdata->SLU_Etree,
//    &tdata->SLUMatrix_A
//  );
//
//  dgstrf
//  (
//    &tdata->SLU_Options,
//    &tdata->SLUMatrix_A,
//    sp_ienv(2), sp_ienv(1), /* relax and panel size */
//    tdata->SLU_Etree,
//    NULL, 0,                /* work and lwork */
//    tdata->SLU_PermutationMatrixC,
//    tdata->SLU_PermutationMatrixR,
//    &tdata->SLUMatrix_L,
//    &tdata->SLUMatrix_U,
//    &tdata->SLU_Stat,
//    &tdata->SLU_Info
//  ) ;
//
//  return tdata->SLU_Info ;
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

int
solve_system
(
  ThermalData *tdata,
  double total_time
)
{
  for ( ; total_time > 0 ; total_time -= tdata->delta_time)
  {
    dgssvx
    (
      &tdata->SLU_Options,
      &tdata->SLUMatrix_A,
      tdata->SLU_PermutationMatrixC,
      tdata->SLU_PermutationMatrixR,
      tdata->SLU_Etree,
      tdata->SLU_equed,
      tdata->SLU_RowsScaleFactors,
      tdata->SLU_ColumnsScaleFactors,
      &tdata->SLUMatrix_L,
      &tdata->SLUMatrix_U,
      NULL, 0,
      &tdata->SLUMatrix_B,
      &tdata->SLUMatrix_X,
      &tdata->SLU_rpg,
      &tdata->SLU_rcond,
      &tdata->SLU_ferr,
      &tdata->SLU_berr,
      &tdata->SLU_MemUsage,
      &tdata->SLU_Stat,
      &tdata->SLU_Info
    );

    if (tdata->SLU_Info != 0)
      break ;

    tdata->SLU_Options.Fact = FACTORED ;

    fill_system_vector
    (
      &tdata->SV_B,
      tdata->Sources,
      tdata->Capacities,
      tdata->Temperatures
    ) ;
  }

//    dgstrs
//    (
//      NOTRANS,
//      &tdata->SLUMatrix_L,
//      &tdata->SLUMatrix_U,
//      tdata->SLU_PermutationMatrixC,
//      tdata->SLU_PermutationMatrixR,
//      &tdata->SLUMatrix_B,
//      &tdata->SLU_Stat,
//      &tdata->SLU_Info
//    ) ;

  return tdata->SLU_Info ;
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

void
update_thermal_data
(
  StackDescription *stkd,
  ThermalData *tdata
)
{
  fill_sources_stack_description
  (
    stkd,
    tdata->Sources
  ) ;

  fill_system_vector
  (
    &tdata->SV_B,
    tdata->Sources,
    tdata->Capacities,
    tdata->Temperatures
  );
}
