/******************************************************************************
 *                                                                            *
 * Source file "Sources/thermal_data_direct.c"                                *
 *                                                                            *
 * EPFL-STI-IEL-ESL                                                           *
 * Bâtiment ELG, ELG 130                                                      *
 * Station 11                                                                 *
 * 1015 Lausanne, Switzerland                    alessandro.vincenzi@epfl.ch  *
 ******************************************************************************/

#include <stdlib.h>
#include <stdio.h>

#include "thermal_data_direct.h"

#ifdef TL_NO_CHANNELS
void
insert_extra_sources
(
  struct StackDescription  *stkd,
  struct ThermalDataDirect *tdata
)
{
  int last_layer = get_cell_offset_in_stack(stkd->Dimensions, 3, 0, 0);
  int index;

  for (index = 0; index < get_layer_area(stkd->Dimensions); index++)
  {
    //printf ("Cell %d: %e\n", last_layer + index, tdata->Conductances[last_layer + index].Top );
    tdata->Sources[last_layer + index] +=
      300.0 * tdata->Conductances[last_layer + index].Top;
  }
}
#endif

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

int
init_thermal_data_direct
(
  struct StackDescription  *stkd,
  struct ThermalDataDirect *tdata,
  double                   initial_temperature,
  double                   delta_time
)
{
  if (tdata == NULL) return 0 ;

  tdata->Size                = stkd->Dimensions->Grid.NCells ;
  tdata->initial_temperature = initial_temperature ;
  tdata->delta_time          = delta_time ;

  /* Memory allocation */

  if ( (tdata->Conductances
         = (struct Conductances *) malloc (sizeof(struct Conductances)*tdata->Size)) == NULL)

    return 0 ;

  if ( (tdata->SLU_PermutationMatrixR
         = (int *) malloc ( sizeof(int) * tdata->Size )) == NULL )

    goto slu_perm_r_fail ;

  if ( (tdata->SLU_PermutationMatrixC
    = (int *) malloc ( sizeof(int) * tdata->Size )) == NULL )

    goto slu_perm_c_fail ;

  if ( (tdata->SLU_Etree
         = (int *) malloc ( sizeof(int) * tdata->Size )) == NULL )

    goto slu_etree_fail ;

  tdata->Temperatures.newsize (tdata->Size) ;
  tdata->Sources.newsize      (tdata->Size) ;
  tdata->Capacities.newsize   (tdata->Size) ;
  tdata->D_Matrix_A.newsize   (tdata->Size,
                               tdata->Size,
                               stkd->Dimensions->Grid.NNz) ;

  /* Set initial values */

  tdata->Temperatures = initial_temperature;
  tdata->Sources      = 0.0;

  StatInit (&tdata->SLU_Stat) ;

  set_default_options (&tdata->SLU_Options) ;

  tdata->SLU_Options.Fact            = DOFACT ;
  tdata->SLU_Options.PrintStat       = NO ;
  tdata->SLU_Options.Equil           = NO ;
  tdata->SLU_Options.SymmetricMode   = YES ;
  tdata->SLU_Options.ColPerm         = MMD_AT_PLUS_A ;
  tdata->SLU_Options.DiagPivotThresh = 0.01 ;

  dCreate_CompRow_Matrix                /* Matrix A */
  (
   &tdata->SLUMatrix_A,                 // SLU Matrix Refernce
   tdata->D_Matrix_A.size(0),           // #rows
   tdata->D_Matrix_A.size(1),           // #columns
   tdata->D_Matrix_A.NumNonzeros(),     // #nonzeroes
   &tdata->D_Matrix_A.val(0),           // Coefficients reference
   &tdata->D_Matrix_A.col_ind(0),       // Column indeces reference
   &tdata->D_Matrix_A.row_ptr(0),       // Row pointers reference
   SLU_NR,                              // row-wize, no supernode
   SLU_D,                               // double precision
   SLU_GE                               // general matrix
  ) ;

  dCreate_Dense_Matrix  /* Vector B */
  (
    &tdata->SLUMatrix_B,                // Slu Vector reference
    tdata->Size,                        // #rows
    1,                                  // #columns
    &tdata->Temperatures[0],            // Coefficients reference
    tdata->Size,                        // leading dimension
    SLU_DN,                             // column-wise storage for dense matrix
    SLU_D,                              // double precision
    SLU_GE                              // general matrix
  );

  return 1 ;

  /* Free if malloc errors */

slu_etree_fail :
  free (tdata->SLU_PermutationMatrixR);
slu_perm_c_fail :
  free (tdata->SLU_PermutationMatrixR) ;
slu_perm_r_fail :
  free (tdata->Conductances) ;

  return 0 ;
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

void
free_thermal_data_direct
(
  struct ThermalDataDirect *tdata
)
{
  if (tdata == NULL) return ;

  free (tdata->Conductances) ;

  free (tdata->SLU_PermutationMatrixR) ;
  free (tdata->SLU_PermutationMatrixC) ;
  free (tdata->SLU_Etree) ;

  StatFree (&tdata->SLU_Stat) ;

  Destroy_SuperMatrix_Store (&tdata->SLUMatrix_A) ;
  Destroy_SuperMatrix_Store (&tdata->SLUMatrix_B);

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
fill_thermal_data_direct
(
  struct StackDescription  *stkd,
  struct ThermalDataDirect *tdata
)
{
  //
  // If the flow rate changed then all the conductances and then the
  // capacities are re evaluated. With these new values the A matrix
  // must be re filled and factorized. Since the flow rate is used also
  // for heat injection, we must re evauate also the sources.
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
      &tdata->D_Matrix_A.row_ptr(0),
      &tdata->D_Matrix_A.col_ind(0),
      &tdata->D_Matrix_A.val(0)
    ) ;

    if (tdata->SLU_Options.Fact == FACTORED)
      tdata->SLU_Options.Fact = SamePattern ;
    else
      tdata->SLU_Options.Fact = DOFACT ;

    fill_sources_stack_description (stkd, &tdata->Sources[0]) ;
#ifdef TL_NO_CHANNELS
    insert_extra_sources(stkd, tdata);
#endif
    stkd->PowerValuesChanged = 0 ;
  }

  //
  // If power values are different ne need to re compute the sources array.
  //

  if (stkd->PowerValuesChanged == 1)
  {
    fill_sources_stack_description (stkd, &tdata->Sources[0]) ;
#ifdef TL_NO_CHANNELS
    insert_extra_sources(stkd, tdata);
#endif
    stkd->PowerValuesChanged = 0 ;
  }

  //
  // If the A matrix is not factored, we factorize it .... this must
  // be don at the end of this function since we must return the result
  // of this operation.
  //

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
solve_system_direct
(
  struct ThermalDataDirect *tdata,
  double                   total_time
)
{
  int index;

  if (tdata->SLU_Options.Fact == DOFACT)
    return 1 ;

  for ( ; total_time > 0 ; total_time -= tdata->delta_time)
  {

    for (index = 0 ; index < tdata->Size ; index++)
      tdata->Temperatures[index] = tdata->Sources[index]
                                   + tdata->Capacities[index]
                                     * tdata->Temperatures[index] ;

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
  }

  return tdata->SLU_Info ;
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
