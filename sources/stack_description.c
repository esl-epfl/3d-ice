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

#include "stack_description.h"
#include "layer.h"
#include "../bison/stack_description_parser.h"
#include "../flex/stack_description_scanner.h"

/******************************************************************************/

extern int  stack_description_parse (StackDescription* stkd, yyscan_t scanner) ;
static int  fill_floorplans         (StackDescription* stkd) ;
static void align_layers_in_die     (StackDescription* stkd) ;
static void align_stack_elements    (StackDescription* stkd) ;

/******************************************************************************/

void init_stack_description (StackDescription* stkd)
{
  stkd->FileName            = NULL ;
  stkd->MaterialsList       = NULL ;
  stkd->Channel             = NULL ;
  stkd->DiesList            = NULL ;
  stkd->EnvironmentHeatSink = NULL ;
  stkd->StackElementsList   = NULL ;
  stkd->Dimensions          = NULL ;
  stkd->RemainingTimeSlots  = 0 ;
}

/******************************************************************************/

int fill_stack_description
(
  StackDescription* stkd,
  String_t          filename
)
{
  FILE*    input;
  int      result;
  yyscan_t scanner;

  input = fopen (filename, "r");
  if(input == NULL)
  {
    perror(filename);
    return 1;
  }

  stkd->FileName = strdup (filename);

  stack_description_lex_init (&scanner);
  stack_description_set_in (input, scanner);

  result = stack_description_parse (stkd, scanner);

  stack_description_lex_destroy (scanner);
  fclose (input);

  if (result == 1) return result ;

  align_stack_elements (stkd) ;
  align_layers_in_die  (stkd) ;

  return fill_floorplans (stkd) ;
}

/******************************************************************************/

void free_stack_description (StackDescription* stkd)
{
  free_materials_list        (stkd->MaterialsList) ;
  free_channel               (stkd->Channel) ;
  free_dies_list             (stkd->DiesList) ;
  free_environment_heat_sink (stkd->EnvironmentHeatSink) ;
  free_stack_elements_list   (stkd->StackElementsList) ;
  free_dimensions            (stkd->Dimensions) ;
  free                       (stkd->FileName) ;
}

/******************************************************************************/

void print_stack_description
(
  FILE*             stream,
  String_t          prefix,
  StackDescription* stkd
)
{
  fprintf(stream, "%sStack read from file %s\n", prefix, stkd->FileName) ;

  print_materials_list (stream, prefix, stkd->MaterialsList) ;

  if (stkd->EnvironmentHeatSink != NULL)

    print_environment_heat_sink (stream, prefix, stkd->EnvironmentHeatSink) ;

  if (stkd->Channel != NULL)

    print_channel (stream, prefix, stkd->Channel);

  print_dies_list (stream, prefix, stkd->DiesList) ;

  print_stack_elements_list (stream, prefix, stkd->StackElementsList) ;

  print_dimensions (stream, prefix, stkd->Dimensions) ;
}

/******************************************************************************/

void print_all_floorplans
(
  FILE*             stream,
  String_t          prefix,
  StackDescription* stkd
)
{
  StackElement* stack_element = stkd->StackElementsList ;

  for ( ; stack_element != NULL ; stack_element = stack_element->Next)

    if (stack_element->Type == TL_STACK_ELEMENT_DIE)

      print_floorplan(stream, prefix, stack_element->Floorplan) ;
}

/******************************************************************************/

static int fill_floorplans (StackDescription* stkd)
{
  int result = 0 ;
  StackElement* stack_element = stkd->StackElementsList ;

  for ( ; stack_element != NULL ; stack_element = stack_element->Next)

    if (stack_element->Type == TL_STACK_ELEMENT_DIE)
    {
      result += fill_floorplan (stack_element->Floorplan, stkd->Dimensions) ;
      stkd->RemainingTimeSlots
        = stack_element->Floorplan->ElementsList->PowerValues->Length ;
    }

  return result ;
 }

/******************************************************************************/

static void align_layers_in_die (StackDescription* stkd)
{
  Layer* layer ;
  LayerIndex_t layer_offset ;

  Die* die = stkd->DiesList ;
  for ( ; die != NULL ; die = die->Next)
  {
    layer_offset = 0 ;
    layer = die->LayersList ;
    for ( ; layer != NULL ; layer = layer->Next)
    {
      layer->LayersOffset = layer_offset++ ;
    }
  }
}

/******************************************************************************/

static void align_stack_elements (StackDescription* stkd)
{
  Quantity_t layer_counter = 0 ;
  StackElement* stack_element = stkd->StackElementsList ;

  for ( ; stack_element != NULL ; stack_element = stack_element->Next)
  {
    stack_element->LayersOffset = layer_counter ;

    layer_counter += stack_element->NLayers ;
  }

 }

/******************************************************************************/

void fill_conductances_stack_description
(
  StackDescription* stkd,
  Conductances*     conductances
)
{
  StackElement* stack_element ;

#ifdef PRINT_CONDUCTANCES
  fprintf
  (
    stderr,
    "fill_conductances_stack_description ( l %d r %d c %d )\n",
    get_number_of_layers  (stkd->Dimensions),
    get_number_of_rows    (stkd->Dimensions),
    get_number_of_columns (stkd->Dimensions)
  ) ;
#endif

  for
  (
    stack_element = stkd->StackElementsList ;
    stack_element != NULL ;
    stack_element = stack_element->Next
  )

    switch (stack_element->Type)
    {
      case TL_STACK_ELEMENT_DIE :

        conductances = fill_conductances_die
                       (
                         stack_element->Pointer.Die,
                         conductances,
                         stkd->Dimensions,
                         stkd->EnvironmentHeatSink,
                         stack_element->LayersOffset
                       ) ;
        break ;

      case TL_STACK_ELEMENT_LAYER :

        conductances = fill_conductances_layer
                       (
                         stack_element->Pointer.Layer,
                         conductances,
                         stkd->Dimensions,
                         stkd->EnvironmentHeatSink,
                         stack_element->LayersOffset
                       ) ;
        break ;

      case TL_STACK_ELEMENT_CHANNEL :

        conductances = fill_conductances_channel
                       (
#                        ifdef PRINT_CONDUCTANCES
                         stack_element->LayersOffset,
#                        endif
                         stkd->Channel,
                         conductances, stkd->Dimensions
                       ) ;
        break ;

      case TL_STACK_ELEMENT_NONE :

        fprintf (stderr, "Error! Found stack element with unset type\n") ;
        return ;

      default :

        fprintf
        (
          stderr,
          "Error! Unknown stack element type %d\n",
          stack_element->Type
        ) ;
        return ;

    } /* switch stack_elementy->Type */
}

/******************************************************************************/

void fill_capacities_stack_description
(
  StackDescription* stkd,
  Capacity_t*       capacities,
  Time_t            delta_time
)
{
  StackElement* stack_element ;

#ifdef PRINT_CAPACITIES
  fprintf
  (
    stderr,
    "fill_capacities_stack_description ( l %d r %d c %d )\n",
    get_number_of_layers  (stkd->Dimensions),
    get_number_of_rows    (stkd->Dimensions),
    get_number_of_columns (stkd->Dimensions)
  );
#endif

  for
  (
    stack_element = stkd->StackElementsList ;
    stack_element != NULL ;
    stack_element = stack_element->Next
  )
    switch (stack_element->Type)
    {
      case TL_STACK_ELEMENT_DIE :

        capacities = fill_capacities_die
                     (
#                      ifdef PRINT_CAPACITIES
                       stack_element->LayersOffset,
#                      endif
                       stack_element->Pointer.Die,
                       capacities,
                       stkd->Dimensions,
                       delta_time
                     ) ;
        break ;

      case TL_STACK_ELEMENT_LAYER :

        capacities = fill_capacities_layer
                     (
#                      ifdef PRINT_CAPACITIES
                       stack_element->LayersOffset,
#                      endif
                       stack_element->Pointer.Layer,
                       capacities,
                       stkd->Dimensions,
                       delta_time
                     ) ;
        break ;

      case TL_STACK_ELEMENT_CHANNEL :

        capacities = fill_capacities_channel
                     (
#                      ifdef PRINT_CAPACITIES
                       stack_element->LayersOffset,
#                      endif
                       stkd->Channel,
                       capacities,
                       stkd->Dimensions,
                       delta_time
                     ) ;
        break ;

      case TL_STACK_ELEMENT_NONE :

        fprintf (stderr, "Error! Found stack element with unset type\n") ;
        return ;

      default :

        fprintf
        (
          stderr,
          "Error! Unknown stack element type %d\n",
          stack_element->Type
        ) ;
        return ;

    } /* switch stack_element->Type */
}

/******************************************************************************/

void fill_sources_stack_description
(
  StackDescription* stkd,
  Source_t*         sources,
  Conductances*     conductances
)
{
  StackElement* stack_element ;
  StackElement* last_stack_element ;
  Source_t*     tmp_sources = sources ;

#ifdef PRINT_SOURCES
  fprintf (stderr,
    "fill_sources_stack_description ( l %d r %d c %d )\n",
    get_number_of_layers  (stkd->Dimensions),
    get_number_of_rows    (stkd->Dimensions),
    get_number_of_columns (stkd->Dimensions));
#endif

  for
  (
    stack_element = stkd->StackElementsList ;
    stack_element != NULL ;
    last_stack_element = stack_element,
    stack_element      = stack_element->Next
  )

    switch (stack_element->Type)
    {
      case TL_STACK_ELEMENT_DIE :

        tmp_sources = fill_sources_die
                      (
#                       ifdef PRINT_SOURCES
                        stack_element->LayersOffset,
#                       endif
                        stack_element->Pointer.Die,
                        stack_element->Floorplan,
                        tmp_sources,
                        stkd->Dimensions
                      ) ;

        break ;

      case TL_STACK_ELEMENT_LAYER :

        tmp_sources += get_layer_area (stkd->Dimensions) ;
        break ;

      case TL_STACK_ELEMENT_CHANNEL :

        tmp_sources = fill_sources_channel
                      (
#                       ifdef PRINT_SOURCES
                        stack_element->LayersOffset,
#                       endif
                        stkd->Channel,
                        tmp_sources,
                        stkd->Dimensions
                      ) ;
        break ;

      case TL_STACK_ELEMENT_NONE :

        fprintf (stderr,  "Error! Found stack element with unset type\n") ;
        return ;

      default :

        fprintf (stderr, "Error! Unknown stack element type %d\n",
          stack_element->Type) ;
        return ;

    } /* switch stack_element->Type */

  if (stkd->EnvironmentHeatSink != NULL)
  {
    if (last_stack_element->Type == TL_STACK_ELEMENT_DIE)

        add_sources_enviroment_heat_sink
        (
          stkd->EnvironmentHeatSink,
          stkd->Dimensions,
          sources,
          conductances,
          last_stack_element->LayersOffset
          + last_stack_element->Pointer.Die->NLayers
        ) ;

    else if (last_stack_element->Type == TL_STACK_ELEMENT_LAYER)

        add_sources_enviroment_heat_sink
        (
          stkd->EnvironmentHeatSink,
          stkd->Dimensions,
          sources,
          conductances,
          last_stack_element->LayersOffset
        ) ;
  }
}

/******************************************************************************/

void fill_system_matrix_stack_description
(
  StackDescription*    stkd,
  Conductances*        conductances,
  Capacity_t*          capacities,
  RowIndex_t*          row_offsets,
  ColumnIndex_t*       column_indices,
  SystemMatrixValue_t* values
)
{
  StackElement* stack_element ;
  Quantity_t added, area ;

#ifdef PRINT_SYSTEM_MATRIX
  fprintf
  (
    stderr,
    "fill_system_matrix_stack_description ( l %d r %d c %d )\n",
    get_number_of_layers  (stkd->Dimensions),
    get_number_of_rows    (stkd->Dimensions),
    get_number_of_columns (stkd->Dimensions));
#endif

  *row_offsets++ = 0 ;

  for
  (
    added           = 0,
    area            = get_layer_area (stkd->Dimensions),
    stack_element   = stkd->StackElementsList ;
    stack_element  != NULL ;
    conductances   += area * stack_element->NLayers,
    capacities     += area * stack_element->NLayers,
    row_offsets    += area * stack_element->NLayers,
    column_indices += added,
    values         += added,
    stack_element   = stack_element->Next
  )

    switch (stack_element->Type)
    {
      case TL_STACK_ELEMENT_DIE :

        added = fill_system_matrix_die
                (
                  stack_element->Pointer.Die, stkd->Dimensions,
                  conductances, capacities,
                  stkd->EnvironmentHeatSink,
                  stack_element->LayersOffset,
                  row_offsets, column_indices, values
                ) ;
        break ;

      case TL_STACK_ELEMENT_LAYER :

        added = fill_system_matrix_layer
                (
#                 ifdef PRINT_SYSTEM_MATRIX
                  stack_element->Pointer.Layer,
#                 endif
                  stkd->Dimensions, conductances, capacities,
                  stkd->EnvironmentHeatSink,
                  stack_element->LayersOffset,
                  row_offsets, column_indices, values
                ) ;
        break ;

      case TL_STACK_ELEMENT_CHANNEL :

        added = fill_system_matrix_channel
                (
#                 ifdef PRINT_SYSTEM_MATRIX
                  stkd->Channel,
#                 endif
                  stkd->Dimensions, conductances, capacities,
                  stack_element->LayersOffset,
                  row_offsets, column_indices, values
                ) ;
        break ;

      case TL_STACK_ELEMENT_NONE :

        fprintf (stderr,  "Error! Found stack element with unset type\n") ;
        return ;

      default :

        fprintf (stderr, "Error! Unknown stack element type %d\n",
          stack_element->Type) ;
        return ;

    } /* stack_element->Type */
}

/******************************************************************************/

Quantity_t get_total_number_of_floorplan_elements (StackDescription* stkd)
{
  Quantity_t total = 0 ;
  StackElement* stk_el = stkd->StackElementsList;

  for ( ; stk_el != NULL ; stk_el = stk_el->Next)

    if (stk_el->Type == TL_STACK_ELEMENT_DIE && stk_el->Floorplan != NULL)

      total += stk_el->Floorplan->NElements ;

  return total ;
}

/******************************************************************************/

Quantity_t get_number_of_floorplan_elements_in_floorplan
(
  StackDescription* stkd,
  String_t          stack_element_id
)
{
  StackElement* stk_el = find_stack_element_in_list
                         (
                           stkd->StackElementsList,
                           stack_element_id
                         ) ;
  if (stk_el == NULL)

    return -1 ;

  if (stk_el->Type != TL_STACK_ELEMENT_DIE || stk_el->Floorplan == NULL)

    return -2 ;

  return stk_el->Floorplan->NElements ;
}

/******************************************************************************/

int get_max_temperature_in_floorplan_element
(
  StackDescription* stkd,
  String_t          stack_element_id,
  String_t          floorplan_element_id,
  Temperature_t*    temperatures,
  Temperature_t*    max_temperature
)
{
  Quantity_t offset ;
  StackElement* stk_el = find_stack_element_in_list
                         (
                           stkd->StackElementsList,
                           stack_element_id
                         ) ;
  if (stk_el == NULL)

    return -1 ;

  if (   stk_el->Type      != TL_STACK_ELEMENT_DIE
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
           temperatures + offset,
           max_temperature
         ) ;
}

/******************************************************************************/

int get_min_temperature_in_floorplan_element
(
  StackDescription* stkd,
  String_t          stack_element_id,
  String_t          floorplan_element_id,
  Temperature_t*    temperatures,
  Temperature_t*    min_temperature
)
{
  Quantity_t offset ;
  StackElement* stk_el = find_stack_element_in_list
                         (
                           stkd->StackElementsList,
                           stack_element_id
                         ) ;
  if (stk_el == NULL)

    return -1 ;

  if (   stk_el->Type      != TL_STACK_ELEMENT_DIE
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
           temperatures + offset,
           min_temperature
         ) ;
}

/******************************************************************************/

int get_avg_temperature_in_floorplan_element
(
  StackDescription* stkd,
  String_t          stack_element_id,
  String_t          floorplan_element_id,
  Temperature_t*    temperatures,
  Temperature_t*    avg_temperature
)
{
  Quantity_t offset ;
  StackElement* stk_el = find_stack_element_in_list
                         (
                           stkd->StackElementsList,
                           stack_element_id
                         ) ;
  if (stk_el == NULL)

    return -1 ;

  if (   stk_el->Type      != TL_STACK_ELEMENT_DIE
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
           temperatures + offset,
           avg_temperature
         ) ;
}

/******************************************************************************/

int get_min_avg_max_temperatures_in_floorplan_element
(
  StackDescription* stkd,
  String_t          stack_element_id,
  String_t          floorplan_element_id,
  Temperature_t*    temperatures,
  Temperature_t*    min_temperature,
  Temperature_t*    avg_temperature,
  Temperature_t*    max_temperature
)
{
  Quantity_t offset ;
  StackElement* stk_el = find_stack_element_in_list
                         (
                           stkd->StackElementsList,
                           stack_element_id
                         ) ;
  if (stk_el == NULL)

    return -1 ;

  if (   stk_el->Type      != TL_STACK_ELEMENT_DIE
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
           temperatures + offset,
           min_temperature,
           avg_temperature,
           max_temperature
         ) ;
}

/******************************************************************************/

int get_all_max_temperatures_in_floorplan
(
  StackDescription* stkd,
  String_t          stack_element_id,
  Temperature_t*    temperatures,
  Temperature_t*    max_temperature
)
{
  Quantity_t offset ;
  StackElement* stk_el = find_stack_element_in_list
                         (
                           stkd->StackElementsList,
                           stack_element_id
                         ) ;
  if (stk_el == NULL)

    return -1 ;

  if (   stk_el->Type      != TL_STACK_ELEMENT_DIE
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
    temperatures + offset,
    max_temperature
  ) ;

  return 0 ;
}

/******************************************************************************/

int get_all_min_temperatures_in_floorplan
(
  StackDescription* stkd,
  String_t          stack_element_id,
  Temperature_t*    temperatures,
  Temperature_t*    min_temperature
)
{
  Quantity_t offset ;
  StackElement* stk_el = find_stack_element_in_list
                         (
                           stkd->StackElementsList,
                           stack_element_id
                         ) ;
  if (stk_el == NULL)

    return -1 ;

  if (   stk_el->Type      != TL_STACK_ELEMENT_DIE
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
    temperatures + offset,
    min_temperature
  ) ;

  return 0 ;
}

/******************************************************************************/

int get_all_avg_temperatures_in_floorplan
(
  StackDescription* stkd,
  String_t          stack_element_id,
  Temperature_t*    temperatures,
  Temperature_t*    avg_temperature
)
{
  Quantity_t offset ;
  StackElement* stk_el = find_stack_element_in_list
                         (
                           stkd->StackElementsList,
                           stack_element_id
                         ) ;
  if (stk_el == NULL)

    return -1 ;

  if (   stk_el->Type      != TL_STACK_ELEMENT_DIE
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
    temperatures + offset,
    avg_temperature
  ) ;

  return 0 ;
}

/******************************************************************************/

int get_all_min_avg_max_temperatures_in_floorplan
(
  StackDescription* stkd,
  String_t          stack_element_id,
  Temperature_t*    temperatures,
  Temperature_t*    min_temperature,
  Temperature_t*    avg_temperature,
  Temperature_t*    max_temperature
)
{
  Quantity_t offset ;
  StackElement* stk_el = find_stack_element_in_list
                         (
                           stkd->StackElementsList,
                           stack_element_id
                         ) ;

  if (stk_el == NULL)

    return -1 ;

  if (   stk_el->Type      != TL_STACK_ELEMENT_DIE
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
    temperatures + offset,
    min_temperature,
    avg_temperature,
    max_temperature
  ) ;

 return 0 ;
}

/******************************************************************************/

int print_thermal_map
(
  StackDescription* stkd,
  Temperature_t*    temperatures,
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

  if (stk_el->Type == TL_STACK_ELEMENT_DIE)
    layer_offset += stk_el->Pointer.Die->SourceLayer->LayersOffset ;

  temperatures
    += get_cell_offset_in_stack (stkd->Dimensions, layer_offset, 0, 0) ;

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
      column++, temperatures++
    )
      fprintf(output_file, "%7.3f  ", *temperatures) ;

    fprintf(output_file, "\n") ;
  }

  fclose (output_file) ;

  return 0 ;
}
