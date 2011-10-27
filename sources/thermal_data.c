/******************************************************************************
 * This file is part of 3D-ICE, version 1.0.2 .                               *
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
#include "macros.h"

/******************************************************************************/

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
  tdata->Size = QUANTITY_I ;

  tdata->StepTime = step_time ;
  tdata->SlotTime = slot_time ;

  tdata->SlotLength = (Quantity_t) ( slot_time / step_time ) ;

  tdata->CurrentTime      = QUANTITY_I ;

  tdata->InitialTemperature = initial_temperature ;

  tdata->Temperatures = NULL ;
  tdata->Sources      = NULL ;
  tdata->ThermalCells = NULL ;

  init_system_matrix (&tdata->SM_A) ;

  tdata->SLU_PermutationMatrixR = NULL ;
  tdata->SLU_PermutationMatrixC = NULL ;
  tdata->SLU_Etree              = NULL ;

  tdata->SLUMatrix_A.Store          = NULL ;
  tdata->SLUMatrix_A_Permuted.Store = NULL ;
  tdata->SLUMatrix_B.Store          = NULL ;
  tdata->SLUMatrix_L.Store          = NULL ;
  tdata->SLUMatrix_U.Store          = NULL ;

  tdata->SLU_Info = 0 ;

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
  int result ;

  tdata->Size = (Quantity_t) get_number_of_cells(stkd->Dimensions) ;

  /* Alloc and set temperatures */

  tdata->Temperatures
    = malloc (sizeof(Temperature_t) * tdata->Size) ;

  if (tdata->Temperatures == NULL)  goto temperatures_fail ;

  /* Set Temperatures and SLU vector B */

  init_data (tdata->Temperatures, tdata->Size, tdata->InitialTemperature) ;

  dCreate_Dense_Matrix  /* Vector B */
  (
    &tdata->SLUMatrix_B, tdata->Size, 1,
    tdata->Temperatures, tdata->Size,
    SLU_DN, SLU_D, SLU_GE
  ) ;

  /* Alloc and set thermal grid data */

  tdata->ThermalCells
    = malloc (sizeof(ThermalCell) * tdata->Size) ;

  if (tdata->ThermalCells == NULL)  goto thermal_cell_data_fail ;

  fill_thermal_cell_stack_description

      (tdata->ThermalCells, tdata->StepTime, stkd) ;

  /* Alloc and set sources */

  tdata->Sources
    = malloc (sizeof(Source_t) * tdata->Size) ;

  if (tdata->Sources == NULL)  goto sources_fail ;

  init_data (tdata->Sources, tdata->Size, 0.0) ;

  /* Alloc and set system matrix */

  result = alloc_system_matrix
           (
             &tdata->SM_A,
             tdata->Size,
             get_number_of_non_zeroes(stkd->Dimensions)
           ) ;

  if (result == 0)  goto system_matrix_fail ;

  fill_system_matrix_stack_description

    (tdata->SM_A, tdata->ThermalCells, stkd) ;

  dCreate_CompCol_Matrix
  (
    &tdata->SLUMatrix_A, tdata->Size, tdata->Size, tdata->SM_A.NNz,
    tdata->SM_A.Values, tdata->SM_A.RowIndices, tdata->SM_A.ColumnPointers,
    SLU_NC, SLU_D, SLU_GE
  ) ;

  /* Alloc SLU memory */

  tdata->SLU_PermutationMatrixR
    = malloc(sizeof(*tdata->SLU_PermutationMatrixR) * tdata->Size) ;

  if (tdata->SLU_PermutationMatrixR == NULL )  goto slu_perm_r_fail ;

  tdata->SLU_PermutationMatrixC
    = malloc(sizeof(*tdata->SLU_PermutationMatrixC) * tdata->Size) ;

  if (tdata->SLU_PermutationMatrixC == NULL )  goto slu_perm_c_fail ;

  tdata->SLU_Etree
    = malloc (sizeof(*tdata->SLU_Etree) * tdata->Size) ;

  if (tdata->SLU_Etree == NULL)  goto slu_etree_fail ;


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

  if (tdata->SLU_Info == 0)  tdata->SLU_Options.Fact = FACTORED ;

  return tdata->SLU_Info ;

  /* Free if malloc errors */

slu_etree_fail :
  FREE_POINTER (free, tdata->SLU_PermutationMatrixC) ;
slu_perm_c_fail :
  FREE_POINTER (free, tdata->SLU_PermutationMatrixR) ;
slu_perm_r_fail :
  Destroy_SuperMatrix_Store (&tdata->SLUMatrix_A) ;
  free_system_matrix (&tdata->SM_A) ;
system_matrix_fail :
  FREE_POINTER (free, tdata->Sources) ;
sources_fail :
  FREE_POINTER (free, tdata->ThermalCells) ;
thermal_cell_data_fail :
  Destroy_SuperMatrix_Store (&tdata->SLUMatrix_B) ;
  FREE_POINTER (free, tdata->Temperatures) ;
temperatures_fail :
  return -1 ;
}

/******************************************************************************/

void free_thermal_data (ThermalData* tdata)
{
  FREE_POINTER (free, tdata->Temperatures) ;
  FREE_POINTER (free, tdata->Sources) ;
  FREE_POINTER (free, tdata->ThermalCells) ;

  FREE_POINTER (free, tdata->SLU_PermutationMatrixR) ;
  FREE_POINTER (free, tdata->SLU_PermutationMatrixC) ;
  FREE_POINTER (free, tdata->SLU_Etree) ;

  StatFree (&tdata->SLU_Stat) ;

  Destroy_SuperMatrix_Store (&tdata->SLUMatrix_A) ;
  free_system_matrix        (&tdata->SM_A) ;

  Destroy_SuperMatrix_Store (&tdata->SLUMatrix_B) ;

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
  return tdata->CurrentTime * tdata->StepTime ;
}

/******************************************************************************/

Bool_t is_slot_time(ThermalData* tdata)
{
  return (tdata->CurrentTime % tdata->SlotLength == 0) ? TRUE_V : FALSE_V;
}

/******************************************************************************/

static void
fill_system_vector
(
  Dimensions*          dimensions,
  SystemMatrixValue_t* vector,
  Source_t*            sources,
  ThermalCell*         thermalcells,
  Temperature_t*       temperatures
)
{
# ifdef PRINT_SYSTEM_VECTOR
  Temperature_t old ;
# endif

  FOR_EVERY_LAYER (layer, dimensions)
  {
    FOR_EVERY_ROW (row, dimensions)
    {
      FOR_EVERY_COLUMN (column, dimensions)
      {

#       ifdef PRINT_SYSTEM_VECTOR
        old = *temperatures ;
#       endif

        *vector++ =   *sources++
                    +
                      thermalcells++->Capacity
                    *
                      *temperatures++ ;

#       ifdef PRINT_SYSTEM_VECTOR
        fprintf (stderr,

          " l %2d r %4d c %4d [%7d] | %e [b] = %e [s] + %e [c] * %e [t]\n",

          layer, row, column,
          get_cell_offset_in_stack (dimensions, layer, row, column),
          *(vector-1), *(sources-1), (thermalcells-1)->Capacity, old) ;
#       endif

      } // FOR_EVERY_COLUMN
    } // FOR_EVERY_ROW
  } // FOR_EVERY_LAYER

}

int emulate_step (ThermalData* tdata, StackDescription* stkd)
{
  if (tdata->CurrentTime % tdata->SlotLength == 0)
  {
    if (fill_sources_stack_description (tdata->Sources, tdata->ThermalCells, stkd) != 0)

      return 1 ;
  }

  fill_system_vector
  (
    stkd->Dimensions, tdata->Temperatures,
    tdata->Sources, tdata->ThermalCells, tdata->Temperatures
  ) ;

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
    fprintf (stderr, "Error while solving linear system\n") ;
    return tdata->SLU_Info ;
  }

  tdata->CurrentTime++ ;

  return 0 ;
}

/******************************************************************************/

int emulate_slot (ThermalData* tdata, StackDescription* stkd)
{
  int result = 0 ;

  do
  {
    result = emulate_step(tdata, stkd) ;

    if (result != 0)  break ;

  } while (tdata->CurrentTime % tdata->SlotLength != 0) ;

  return result ;
}

/******************************************************************************/

int update_coolant_flow_rate
(
  ThermalData*      tdata,
  StackDescription* stkd,
  CoolantFR_t       new_coolant_fr
)
{
  stkd->Channel->CoolantFR = CONVERT_COOLANT_FLOW_RATE(new_coolant_fr) ;

  fill_system_matrix_stack_description

    (tdata->SM_A, tdata->ThermalCells, stkd) ;  // TODO replace with "update"

  tdata->SLU_Options.Fact = SamePattern_SameRowPerm ;

  dgstrf (&tdata->SLU_Options, &tdata->SLUMatrix_A_Permuted,
          sp_ienv(2), sp_ienv(1), /* relax and panel size */
          tdata->SLU_Etree,
          NULL, 0,                /* work and lwork */
          tdata->SLU_PermutationMatrixC, tdata->SLU_PermutationMatrixR,
          &tdata->SLUMatrix_L, &tdata->SLUMatrix_U,
          &tdata->SLU_Stat, &tdata->SLU_Info) ;

  if (tdata->SLU_Info == 0)

    tdata->SLU_Options.Fact = FACTORED ;

  update_channel_inlet_stack_description (tdata->Sources, stkd) ;

  return tdata->SLU_Info ;
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
  GridDimension_t offset = GRIDDIMENSION_I ;
  StackElement* stk_el   = find_stack_element_in_list
                           (
                             stkd->BottomStackElement,
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
             stk_el->Offset
             + stk_el->Pointer.Die->SourceLayerOffset,
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
  GridDimension_t offset = GRIDDIMENSION_I ;
  StackElement*   stk_el = find_stack_element_in_list
                           (
                             stkd->BottomStackElement,
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
             stk_el->Offset
             + stk_el->Pointer.Die->SourceLayerOffset,
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
  GridDimension_t offset = GRIDDIMENSION_I ;
  StackElement*   stk_el = find_stack_element_in_list
                           (
                             stkd->BottomStackElement,
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
             stk_el->Offset
             + stk_el->Pointer.Die->SourceLayerOffset,
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
  GridDimension_t offset = GRIDDIMENSION_I ;
  StackElement*   stk_el = find_stack_element_in_list
                           (
                             stkd->BottomStackElement,
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
             stk_el->Offset
             + stk_el->Pointer.Die->SourceLayerOffset,
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
  GridDimension_t   outlet_number,
  Temperature_t*    outlet_temperature
)
{
  GridDimension_t offset = GRIDDIMENSION_I ;
  StackElement*   stk_el = find_stack_element_in_list
                           (
                             stkd->BottomStackElement,
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
             stk_el->Offset,
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
  GridDimension_t offset = GRIDDIMENSION_I ;
  StackElement*   stk_el = find_stack_element_in_list
                           (
                             stkd->BottomStackElement,
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
             stk_el->Offset
             + stk_el->Pointer.Die->SourceLayerOffset,
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
  GridDimension_t offset = GRIDDIMENSION_I ;
  StackElement*   stk_el = find_stack_element_in_list
                           (
                             stkd->BottomStackElement,
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
             stk_el->Offset
             + stk_el->Pointer.Die->SourceLayerOffset,
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
  GridDimension_t offset = GRIDDIMENSION_I ;
  StackElement*   stk_el = find_stack_element_in_list
                           (
                             stkd->BottomStackElement,
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
             stk_el->Offset
             + stk_el->Pointer.Die->SourceLayerOffset,
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
  GridDimension_t offset = GRIDDIMENSION_I ;
  StackElement*   stk_el = find_stack_element_in_list
                           (
                             stkd->BottomStackElement,
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
             stk_el->Offset
             + stk_el->Pointer.Die->SourceLayerOffset,
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

int get_cell_temperature
(
  StackDescription* stkd,
  ThermalData*      tdata,
  GridDimension_t   layer_index,
  GridDimension_t   row_index,
  GridDimension_t   column_index,
  Temperature_t*    cell_temperature
)
{
  GridDimension_t id = get_cell_offset_in_stack
                       (
                         stkd->Dimensions,
                         layer_index, row_index, column_index
                       ) ;

  if (id < (GridDimension_t) 0
      || id > get_number_of_cells(stkd->Dimensions))

    return -1 ;

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
  FILE*           output_file ;
  GridDimension_t layer_offset = GRIDDIMENSION_I ;
  StackElement*   stk_el       = find_stack_element_in_list
                                 (
                                   stkd->BottomStackElement,
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

  layer_offset = stk_el->Offset ;

  if (stk_el->Type == TDICE_STACK_ELEMENT_DIE)
    layer_offset += stk_el->Pointer.Die->SourceLayerOffset ;

  Temperature_t* temperature_offset
    = tdata->Temperatures
      + get_cell_offset_in_stack (stkd->Dimensions, layer_offset, 0, 0) ;

  FOR_EVERY_ROW (row_index, stkd->Dimensions)
  {
    FOR_EVERY_COLUMN (column_index, stkd->Dimensions)
    {
      fprintf(output_file, "%7.3f  ", *temperature_offset) ;
      temperature_offset++ ;
    }
    fprintf(output_file, "\n") ;
  }

  fclose (output_file) ;

  return 0 ;
}
