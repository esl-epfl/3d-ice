/******************************************************************************
 * Bison source file "3D-ICE/bison/floorplan_parser.y"                        *
 *                                                                            *
 * This file is part of 3D-ICE 1.0 (beta).                                    *
 *                                                                            *
 * 3D-ICE is free software: you can  redistribute it and/or  modify it  under *
 * the terms of the  GNU General  Public  License as  published by  the  Free *
 * Software  Foundation, either  version  3  of  the License,  or  any  later *
 * version.                                                                   *
 *                                                                            *
 * 3D-ICE is  distributed  in the hope  that it will  be useful, but  WITHOUT *
 * ANY  WARRANTY; without  even the  implied warranty  of MERCHANTABILITY  or *
 * FITNESS  FOR A PARTICULAR  PURPOSE. See the GNU General Public License for *
 * more details.                                                              *
 *                                                                            *
 * You should have  received a copy of  the GNU General  Public License along *
 * with 3D-ICe. If not, see <http://www.gnu.org/licenses/>.                   *
 *                                                                            *
 *                             Copyright (C) 2010                             *
 *   Embedded Systems Laboratory - Ecole Polytechnique Federale de Lausanne   *
 *                            All Rights Reserved.                            *
 *                                                                            *
 * Authors: Arvind Sridhar                                                    *
 *          Alessandro Vincenzi                                               *
 *          Martino Ruggiero                                                  *
 *          Thomas Brunschwiler                                               *
 *          David Atienza                                                     *
 *                                                                            *
 * EPFL-STI-IEL-ESL                                                           *
 * Batiment ELG, ELG 130                                                      *
 * Station 11                                          threed-ice@esl.epfl.ch *
 * 1015 Lausanne, Switzerland                       http://esl.epfl.ch/3D-ICE *
 ******************************************************************************/

#include <stdlib.h>
#include <stdio.h>

#include "thermal_data.h"

/******************************************************************************/

extern void fill_conductances_stack_description
(
  StackDescription* stkd,
  Conductances*     conductances
) ;

extern void fill_capacities_stack_description
(
  StackDescription* stkd,
  Capacity_t*       capacities,
  Time_t            delta_time
) ;

extern void fill_sources_stack_description
(
  StackDescription* stkd,
  Source_t*         sources,
  Conductances*     conductances
) ;

extern void fill_system_matrix_stack_description
(
  StackDescription*    stkd,
  Conductances*        conductances,
  Capacity_t*          capacities,
  RowIndex_t*          row_pointers,
  ColumnIndex_t*       column_indices,
  SystemMatrixValue_t* values
) ;

static void init_data (double* data, Quantity_t size, double init_value)
{
  while (size--) *data++ = init_value ;
}

/******************************************************************************/

void init_thermal_data
(
  ThermalData*      tdata,
  StackDescription* stkd,
  Temperature_t     initial_temperature,
  Time_t            delta_time,
  Time_t            slot_time
)
{
  tdata->Size        = get_number_of_cells(stkd->Dimensions) ;
  tdata->DeltaTime   = delta_time ;
  tdata->SlotTime    = slot_time ;
  tdata->CurrentTime = 0.0 ;

  tdata->InitialTemperature = initial_temperature ;

  tdata->Temperatures = NULL ;
  tdata->Sources      = NULL ;
  tdata->Capacities   = NULL ;
  tdata->Conductances = NULL ;

  tdata->SLU_PermutationMatrixR = NULL ;
  tdata->SLU_PermutationMatrixC = NULL ;
  tdata->SLU_Etree              = NULL ;

  StatInit (&tdata->SLU_Stat) ;

  set_default_options (&tdata->SLU_Options) ;

  tdata->SLU_Options.Fact            = DOFACT ;
  tdata->SLU_Options.PrintStat       = NO ;
  tdata->SLU_Options.Equil           = NO ;
  tdata->SLU_Options.SymmetricMode   = YES ;
  tdata->SLU_Options.ColPerm         = MMD_AT_PLUS_A ;
  tdata->SLU_Options.RowPerm         = NOROWPERM ;
  tdata->SLU_Options.DiagPivotThresh = 0.01 ;
}

/******************************************************************************/

int fill_thermal_data
(
  ThermalData*      tdata,
  StackDescription* stkd
)
{
  /* Alloc and set temperatures */

  if ( (tdata->Temperatures
         = (Temperature_t*) malloc ( sizeof(Temperature_t) * tdata->Size )
       ) == NULL )

    goto temperatures_fail ;

  init_data (tdata->Temperatures, tdata->Size, tdata->InitialTemperature) ;

  /* Alloc and set conductances */

  if ( (tdata->Conductances
         = (Conductances*) malloc (sizeof(Conductances)*tdata->Size)
       ) == NULL )

    goto conductances_fail ;

  fill_conductances_stack_description (stkd, tdata->Conductances) ;

  /* Alloc and set capacities */

  if ( (tdata->Capacities
         = (Capacity_t*) malloc ( sizeof(Capacity_t) * tdata->Size )
      ) == NULL )

     goto capacities_fail ;

  fill_capacities_stack_description (stkd, tdata->Capacities,
                                     tdata->DeltaTime) ;

  /* Alloc and set sources */

  if ( (tdata->Sources
         = (Source_t*) malloc ( sizeof(Source_t) * tdata->Size )
       ) == NULL )

    goto sources_fail ;

  init_data (tdata->Sources, tdata->Size, 0.0) ;

  fill_sources_stack_description (stkd, tdata->Sources,
                                  tdata->Conductances) ;

  /* Alloc and set system matrix */

  if ( alloc_system_matrix (&tdata->SM_A, tdata->Size,
                            get_number_of_non_zeroes(stkd->Dimensions)) == 0 )
    goto sm_a_fail ;

  fill_system_matrix_stack_description
  (
    stkd, tdata->Conductances, tdata->Capacities,
    tdata->SM_A.ColumnPointers, tdata->SM_A.RowIndices, tdata->SM_A.Values
  ) ;

  dCreate_CompCol_Matrix
  (
    &tdata->SLUMatrix_A, tdata->Size, tdata->Size, tdata->SM_A.NNz,
    tdata->SM_A.Values, tdata->SM_A.RowIndices, tdata->SM_A.ColumnPointers,
    SLU_NC, SLU_D, SLU_GE
  ) ;

  /* Alloc and set system vector */

  if ( alloc_system_vector (&tdata->SV_B, tdata->Size) == 0 )

    goto sv_b_fail ;

  fill_system_vector (&tdata->SV_B, tdata->Sources,
                                    tdata->Capacities,
                                    tdata->Temperatures) ;

  dCreate_Dense_Matrix  /* Vector B */
  (
    &tdata->SLUMatrix_B, tdata->Size, 1,
    tdata->SV_B.Values, tdata->Size,
    SLU_DN, SLU_D, SLU_GE
  );

  /* Alloc SLU memory */

  if ( (tdata->SLU_PermutationMatrixR
         = (int *) malloc ( sizeof(int) * tdata->Size )) == NULL )

    goto slu_perm_r_fail ;

  if ( (tdata->SLU_PermutationMatrixC
         = (int *) malloc ( sizeof(int) * tdata->Size )) == NULL )

    goto slu_perm_c_fail ;

  if ( (tdata->SLU_Etree
         = (int *) malloc ( sizeof(int) * tdata->Size )) == NULL )

    goto slu_etree_fail ;


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

  /* Free if malloc errors */

slu_etree_fail :
  free (tdata->SLU_PermutationMatrixC);
slu_perm_c_fail :
  free (tdata->SLU_PermutationMatrixR) ;
slu_perm_r_fail :
  Destroy_SuperMatrix_Store (&tdata->SLUMatrix_B);
  free_system_vector        (&tdata->SV_B) ;
sv_b_fail :
  Destroy_SuperMatrix_Store (&tdata->SLUMatrix_A) ;
  free_system_matrix (&tdata->SM_A) ;
sm_a_fail :
  free (tdata->Sources) ;
sources_fail :
  free (tdata->Capacities) ;
capacities_fail :
  free (tdata->Conductances) ;
conductances_fail :
  free (tdata->Temperatures) ;
temperatures_fail :
  return -1 ;
}

/******************************************************************************/

void free_thermal_data (ThermalData* tdata)
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

int emulate_time_slot (ThermalData* tdata, StackDescription* stkd)
{
  Time_t time = tdata->SlotTime ;
  int counter;

 if (tdata->SLU_Options.Fact == DOFACT)
 {
   fprintf (stderr, "System matrix must be factorized\n");
   return 1 ;
 }

  for ( ; time > 0 ; time -= tdata->DeltaTime)
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
    {
      fprintf (stderr, "Error while solving linear system\n");
      return tdata->SLU_Info ;
    }

    for (counter = 0 ; counter < tdata->SV_B.Size ; counter++)
      tdata->Temperatures[counter] = tdata->SV_B.Values[counter] ;

    fill_system_vector // FIXME: both calls aren't redundant ?
    (
      &tdata->SV_B,
      tdata->Sources,
      tdata->Capacities,
      tdata->Temperatures
    ) ;
  }

  tdata->CurrentTime += tdata->SlotTime ;

  if (--stkd->RemainingTimeSlots == 0)
    return 1 ;
  else
  {
    fill_sources_stack_description (stkd, tdata->Sources,
                                          tdata->Conductances) ;

    fill_system_vector // FIXME: both calls aren't redundant ?
    (
      &tdata->SV_B,
      tdata->Sources,
      tdata->Capacities,
      tdata->Temperatures
    ) ;
  }

  return 0 ;
}

/******************************************************************************/

Time_t get_current_time(ThermalData* tdata)
{
  return tdata->CurrentTime ;
}
