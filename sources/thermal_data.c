/******************************************************************************
 * This file is part of 3D-ICE, version 1.0.1 .                               *
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
 * with 3D-ICE. If not, see <http://www.gnu.org/licenses/>.                   *
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
 * For any comment, suggestion or request  about 3D-ICE, please  register and *
 * write to the mailing list (see http://listes.epfl.ch/doc.cgi?liste=3d-ice) *
 *                                                                            *
 * EPFL-STI-IEL-ESL                                                           *
 * Batiment ELG, ELG 130                Mail : 3d-ice@listes.epfl.ch          *
 * Station 11                                  (SUBSCRIPTION IS NECESSARY)    *
 * 1015 Lausanne, Switzerland           Url  : http://esl.epfl.ch/3d-ice.html *
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
  Temperature_t     initial_temperature,
  Time_t            slot_time,
  Time_t            step_time
)
{
  tdata->Size = 0 ;

  tdata->StepTime = step_time ;
  tdata->SlotTime = slot_time ;

  tdata->CurrentTime      = 0.0 ;
  tdata->CurrentSlotLimit = 0.0 ;

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
  tdata->Size = get_number_of_cells(stkd->Dimensions) ;

  /* Alloc and set temperatures */

  if ( (tdata->Temperatures
         = (Temperature_t*) malloc ( sizeof(Temperature_t) * tdata->Size )
       ) == NULL )

    goto temperatures_fail ;

  /* Set Temperatures and SLU vector B */

  init_data (tdata->Temperatures, tdata->Size, tdata->InitialTemperature) ;

  dCreate_Dense_Matrix  /* Vector B */
  (
    &tdata->SLUMatrix_B, tdata->Size, 1,
    tdata->Temperatures, tdata->Size,
    SLU_DN, SLU_D, SLU_GE
  );

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
                                     tdata->StepTime) ;

  /* Alloc and set sources */

  if ( (tdata->Sources
         = (Source_t*) malloc ( sizeof(Source_t) * tdata->Size )
       ) == NULL )

    goto sources_fail ;

  init_data (tdata->Sources, tdata->Size, 0.0) ;

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

  if (tdata->SLU_Info == 0)
    tdata->SLU_Options.Fact = FACTORED ;

  return tdata->SLU_Info ;

  /* Free if malloc errors */

slu_etree_fail :
  free (tdata->SLU_PermutationMatrixC);
slu_perm_c_fail :
  free (tdata->SLU_PermutationMatrixR) ;
slu_perm_r_fail :
  Destroy_SuperMatrix_Store (&tdata->SLUMatrix_A) ;
  free_system_matrix (&tdata->SM_A) ;
sm_a_fail :
  free (tdata->Sources) ;
sources_fail :
  free (tdata->Capacities) ;
capacities_fail :
  free (tdata->Conductances) ;
conductances_fail :
  Destroy_SuperMatrix_Store (&tdata->SLUMatrix_B);
  free (tdata->Temperatures) ;
temperatures_fail :
  return -1 ;
}

/******************************************************************************/

void free_thermal_data (ThermalData* tdata)
{
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

  if (tdata->SLU_Options.Fact != DOFACT )
  {
    Destroy_CompCol_Permuted (&tdata->SLUMatrix_A_Permuted) ;
    Destroy_SuperNode_Matrix (&tdata->SLUMatrix_L) ;
    Destroy_CompCol_Matrix   (&tdata->SLUMatrix_U) ;
  }
}

/******************************************************************************/

Time_t get_current_time(ThermalData* tdata)
{
  return tdata->CurrentTime ;
}

/******************************************************************************/

int emulate_step (ThermalData* tdata, StackDescription* stkd)
{
  int count;

  if (tdata->SLU_Options.Fact != FACTORED)
  {
    fprintf (stderr, "call fill_thermal_data before emulating\n");
    return -1 ;
  }

  if (tdata->CurrentTime >= tdata->CurrentSlotLimit)
  {
    if (get_number_of_remaining_power_values(stkd) == 0)

      return 1 ;

    fill_sources_stack_description (stkd, tdata->Sources, tdata->Conductances) ;

    tdata->CurrentSlotLimit += tdata->SlotTime ;

    if (tdata->CurrentTime != 0.0)

      return 2 ;
  }

  for(count = 0 ; count < tdata->Size ; count++)

    tdata->Temperatures[count]

      = tdata->Sources[count] + tdata->Capacities[count]
                                * tdata->Temperatures[count] ;

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

  if (tdata->SLU_Info < 0)
  {
    fprintf (stderr, "Error while solving linear system\n");
    return tdata->SLU_Info ;
  }

  tdata->CurrentTime += tdata->StepTime ;

  return 0 ;
}

/******************************************************************************/

int emulate_slot (ThermalData* tdata, StackDescription* stkd)
{
  int count ;

  if (tdata->SLU_Options.Fact != FACTORED)
  {
    fprintf (stderr, "call fill_thermal_data before emulating\n");
    return -1 ;
  }

  if (tdata->CurrentTime >= tdata->CurrentSlotLimit)
  {
    if ( get_number_of_remaining_power_values(stkd) == 0)

      return 1 ;

    fill_sources_stack_description (stkd, tdata->Sources, tdata->Conductances) ;

    tdata->CurrentSlotLimit += tdata->SlotTime ;
  }

  while ( tdata->CurrentTime < tdata->CurrentSlotLimit )
  {
    for(count = 0 ; count < tdata->Size ; count++)

      tdata->Temperatures[count]

        = tdata->Sources[count] + tdata->Capacities[count]
                                  * tdata->Temperatures[count] ;

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

    if (tdata->SLU_Info < 0)
    {
      fprintf (stderr, "Error while solving linear system\n");
      return tdata->SLU_Info ;
    }

    tdata->CurrentTime += tdata->StepTime ;
  }

  return 0 ;
}

/******************************************************************************/

int get_max_temperature_of_floorplan_element
(
  StackDescription* stkd,
  ThermalData*      tdata,
  String_t          floorplan_id,
  String_t          floorplan_element_id,
  Temperature_t*    max_temperature
)
{
  Quantity_t offset ;
  StackElement* stk_el = find_stack_element_in_list
                         (
                           stkd->StackElementsList,
                           floorplan_id
                         ) ;
  if (stk_el == NULL)

    return -1 ;

  if (   stk_el->Type      != TDICE_STACK_ELEMENT_DIE
      || stk_el->Floorplan == NULL)

    return -2 ;

  offset = get_cell_offset_in_stack
           (
             stkd->Dimensions,
             stk_el->LayersOffset
             + stk_el->Pointer.Die->SourceLayer->LayersOffset,
             0, 0
           ) ;

  return get_max_temperature_floorplan
         (
           stk_el->Floorplan,
           floorplan_element_id,
           stkd->Dimensions,
           tdata->Temperatures + offset,
           max_temperature
         ) ;
}

/******************************************************************************/

int get_min_temperature_of_floorplan_element
(
  StackDescription* stkd,
  ThermalData*      tdata,
  String_t          floorplan_id,
  String_t          floorplan_element_id,
  Temperature_t*    min_temperature
)
{
  Quantity_t offset ;
  StackElement* stk_el = find_stack_element_in_list
                         (
                           stkd->StackElementsList,
                           floorplan_id
                         ) ;
  if (stk_el == NULL)

    return -1 ;

  if (   stk_el->Type      != TDICE_STACK_ELEMENT_DIE
      || stk_el->Floorplan == NULL)

    return -2 ;

  offset = get_cell_offset_in_stack
           (
             stkd->Dimensions,
             stk_el->LayersOffset
             + stk_el->Pointer.Die->SourceLayer->LayersOffset,
             0, 0
           ) ;

  return get_min_temperature_floorplan
         (
           stk_el->Floorplan,
           floorplan_element_id,
           stkd->Dimensions,
           tdata->Temperatures + offset,
           min_temperature
         ) ;
}

/******************************************************************************/

int get_avg_temperature_of_floorplan_element
(
  StackDescription* stkd,
  ThermalData*      tdata,
  String_t          floorplan_id,
  String_t          floorplan_element_id,
  Temperature_t*    avg_temperature
)
{
  Quantity_t offset ;
  StackElement* stk_el = find_stack_element_in_list
                         (
                           stkd->StackElementsList,
                           floorplan_id
                         ) ;
  if (stk_el == NULL)

    return -1 ;

  if (   stk_el->Type      != TDICE_STACK_ELEMENT_DIE
      || stk_el->Floorplan == NULL)

    return -2 ;

  offset = get_cell_offset_in_stack
           (
             stkd->Dimensions,
             stk_el->LayersOffset
             + stk_el->Pointer.Die->SourceLayer->LayersOffset,
             0, 0
           ) ;

  return get_avg_temperature_floorplan
         (
           stk_el->Floorplan,
           floorplan_element_id,
           stkd->Dimensions,
           tdata->Temperatures + offset,
           avg_temperature
         ) ;
}

/******************************************************************************/

int get_min_avg_max_temperatures_of_floorplan_element
(
  StackDescription* stkd,
  ThermalData*      tdata,
  String_t          floorplan_id,
  String_t          floorplan_element_id,
  Temperature_t*    min_temperature,
  Temperature_t*    avg_temperature,
  Temperature_t*    max_temperature
)
{
  Quantity_t offset ;
  StackElement* stk_el = find_stack_element_in_list
                         (
                           stkd->StackElementsList,
                           floorplan_id
                         ) ;
  if (stk_el == NULL)

    return -1 ;

  if (   stk_el->Type      != TDICE_STACK_ELEMENT_DIE
      || stk_el->Floorplan == NULL)

    return -2 ;

  offset = get_cell_offset_in_stack
           (
             stkd->Dimensions,
             stk_el->LayersOffset
             + stk_el->Pointer.Die->SourceLayer->LayersOffset,
             0, 0
           ) ;

  return get_min_avg_max_temperatures_floorplan
         (
           stk_el->Floorplan,
           floorplan_element_id,
           stkd->Dimensions,
           tdata->Temperatures + offset,
           min_temperature,
           avg_temperature,
           max_temperature
         ) ;
}

/******************************************************************************/

int get_temperature_of_channel_outlet
(
  StackDescription* stkd,
  ThermalData*      tdata,
  String_t          channel_id,
  ColumnIndex_t     outlet_number,
  Temperature_t*    outlet_temperature
)
{
  Quantity_t offset ;
  StackElement* stk_el = find_stack_element_in_list
                         (
                           stkd->StackElementsList,
                           channel_id
                         ) ;
  if (stk_el == NULL)

    return -1 ;

  if (stk_el->Type != TDICE_STACK_ELEMENT_CHANNEL)

    return -2 ;

  if (   outlet_number < 0
      || outlet_number > get_number_of_channel_outlets (stkd) )

    return -3 ;

  offset = get_cell_offset_in_stack
           (
             stkd->Dimensions,
             stk_el->LayersOffset,
             get_number_of_rows(stkd->Dimensions) - 1,
             (outlet_number * 2) + 1
           ) ;

  *outlet_temperature = tdata->Temperatures [ offset ] ;

  return 0 ;
}

/******************************************************************************/

int get_all_max_temperatures_of_floorplan
(
  StackDescription* stkd,
  ThermalData*      tdata,
  String_t          floorplan_id,
  Temperature_t*    max_temperature
)
{
  Quantity_t offset ;
  StackElement* stk_el = find_stack_element_in_list
                         (
                           stkd->StackElementsList,
                           floorplan_id
                         ) ;
  if (stk_el == NULL)

    return -1 ;

  if (   stk_el->Type      != TDICE_STACK_ELEMENT_DIE
      || stk_el->Floorplan == NULL)

    return -2 ;

  offset = get_cell_offset_in_stack
           (
             stkd->Dimensions,
             stk_el->LayersOffset
             + stk_el->Pointer.Die->SourceLayer->LayersOffset,
             0, 0
           ) ;

  get_all_max_temperatures_floorplan
  (
    stk_el->Floorplan,
    stkd->Dimensions,
    tdata->Temperatures + offset,
    max_temperature
  ) ;

  return 0 ;
}

/******************************************************************************/

int get_all_min_temperatures_of_floorplan
(
  StackDescription* stkd,
  ThermalData*      tdata,
  String_t          floorplan_id,
  Temperature_t*    min_temperature
)
{
  Quantity_t offset ;
  StackElement* stk_el = find_stack_element_in_list
                         (
                           stkd->StackElementsList,
                           floorplan_id
                         ) ;
  if (stk_el == NULL)

    return -1 ;

  if (   stk_el->Type      != TDICE_STACK_ELEMENT_DIE
      || stk_el->Floorplan == NULL)

    return -2 ;

  offset = get_cell_offset_in_stack
           (
             stkd->Dimensions,
             stk_el->LayersOffset
             + stk_el->Pointer.Die->SourceLayer->LayersOffset,
             0, 0
           ) ;

  get_all_min_temperatures_floorplan
  (
    stk_el->Floorplan,
    stkd->Dimensions,
    tdata->Temperatures + offset,
    min_temperature
  ) ;

  return 0 ;
}

/******************************************************************************/

int get_all_avg_temperatures_of_floorplan
(
  StackDescription* stkd,
  ThermalData*      tdata,
  String_t          floorplan_id,
  Temperature_t*    avg_temperature
)
{
  Quantity_t offset ;
  StackElement* stk_el = find_stack_element_in_list
                         (
                           stkd->StackElementsList,
                           floorplan_id
                         ) ;
  if (stk_el == NULL)

    return -1 ;

  if (   stk_el->Type      != TDICE_STACK_ELEMENT_DIE
      || stk_el->Floorplan == NULL)

    return -2 ;

  offset = get_cell_offset_in_stack
           (
             stkd->Dimensions,
             stk_el->LayersOffset
             + stk_el->Pointer.Die->SourceLayer->LayersOffset,
             0, 0
           ) ;

  get_all_avg_temperatures_floorplan
  (
    stk_el->Floorplan,
    stkd->Dimensions,
    tdata->Temperatures + offset,
    avg_temperature
  ) ;

  return 0 ;
}

/******************************************************************************/

int get_all_min_avg_max_temperatures_of_floorplan
(
  StackDescription* stkd,
  ThermalData*      tdata,
  String_t          floorplan_id,
  Temperature_t*    min_temperature,
  Temperature_t*    avg_temperature,
  Temperature_t*    max_temperature
)
{
  Quantity_t offset ;
  StackElement* stk_el = find_stack_element_in_list
                         (
                           stkd->StackElementsList,
                           floorplan_id
                         ) ;

  if (stk_el == NULL)

    return -1 ;

  if (   stk_el->Type      != TDICE_STACK_ELEMENT_DIE
      || stk_el->Floorplan == NULL)

    return -2 ;

  offset = get_cell_offset_in_stack
           (
             stkd->Dimensions,
             stk_el->LayersOffset
             + stk_el->Pointer.Die->SourceLayer->LayersOffset,
             0, 0
           ) ;

  get_all_min_avg_max_temperatures_floorplan
  (
    stk_el->Floorplan,
    stkd->Dimensions,
    tdata->Temperatures + offset,
    min_temperature,
    avg_temperature,
    max_temperature
  ) ;

 return 0 ;
}

/******************************************************************************/

int get_all_temperatures_of_channel_outlets
(
  StackDescription* stkd,
  ThermalData*      tdata,
  String_t          channel_id,
  Temperature_t*    outlet_temperature
)
{
  ColumnIndex_t column ;
  StackElement* stk_el = find_stack_element_in_list
                         (
                           stkd->StackElementsList,
                           channel_id
                         ) ;
  if (stk_el == NULL)

    return -1 ;

  if (stk_el->Type != TDICE_STACK_ELEMENT_CHANNEL)

    return -2 ;

  Temperature_t* temperature_offset
    = tdata->Temperatures
      + get_cell_offset_in_stack
        (
          stkd->Dimensions, stk_el->LayersOffset,
          get_number_of_rows(stkd->Dimensions) - 1, 0
        ) ;

  for
  (
    column = 0 ;
    column < get_number_of_columns (stkd->Dimensions) ;
    column++
  )
    if (is_channel_column(column) == TRUE_V)

    *outlet_temperature++ = *temperature_offset++ ;

  return 0 ;
}

/******************************************************************************/

int get_cell_temperature
(
  StackDescription* stkd,
  ThermalData*      tdata,
  LayerIndex_t      layer,
  RowIndex_t        row,
  ColumnIndex_t     column,
  Temperature_t*    cell_temperature
)
{
  GridDimension_t id = get_cell_offset_in_stack
                       (
                         stkd->Dimensions,
                         layer, row, column
                       );

  if (id < 0 || id > get_number_of_cells(stkd->Dimensions))

    return -1;

  *cell_temperature = tdata->Temperatures [ id ] ;

  return 0 ;
}

/******************************************************************************/

int print_thermal_map
(
  StackDescription* stkd,
  ThermalData*      tdata,
  String_t          stack_element_id,
  String_t          file_name
)
{
  FILE* output_file ;
  RowIndex_t row;
  ColumnIndex_t column ;
  Quantity_t layer_offset ;
  StackElement* stk_el = find_stack_element_in_list
                         (
                           stkd->StackElementsList,
                           stack_element_id
                         ) ;
  if (stk_el == NULL)
    return -1 ;

  output_file = fopen (file_name, "w") ;
  if (output_file == NULL)
  {
    perror (file_name) ;
    return -2 ;
  }

  layer_offset = stk_el->LayersOffset ;

  if (stk_el->Type == TDICE_STACK_ELEMENT_DIE)
    layer_offset += stk_el->Pointer.Die->SourceLayer->LayersOffset ;

  Temperature_t* temperature_offset
    = tdata->Temperatures
      + get_cell_offset_in_stack (stkd->Dimensions, layer_offset, 0, 0) ;

  for
  (
    row = 0 ;
    row < get_number_of_rows (stkd->Dimensions) ;
    row++
  )
  {
    for
    (
      column = 0 ;
      column < get_number_of_columns (stkd->Dimensions) ;
      column++, temperature_offset++
    )
      fprintf(output_file, "%7.3f  ", *temperature_offset) ;

    fprintf(output_file, "\n") ;
  }

  fclose (output_file) ;

  return 0 ;
}
