/******************************************************************************
 * This file is part of 3D-ICE, version 1.0 .                                 *
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
 * For any comment, suggestion or request  about 3D-ICE, please  register and *
 * write to the mailing list (see http://listes.epfl.ch/doc.cgi?liste=3d-ice) *                                                                            *
 *                                                                            *
 * EPFL-STI-IEL-ESL                                                           *
 * Batiment ELG, ELG 130                Mail : 3d-ice@listes.epfl.ch          *
 * Station 11                                  (SUBSCRIPTION IS NECESSARY)    *
 * 1015 Lausanne, Switzerland           Url  : http://esl.epfl.ch/3d-ice.html *
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
  stkd->FileName             = NULL ;
  stkd->MaterialsList        = NULL ;
  stkd->Channel              = NULL ;
  stkd->DiesList             = NULL ;
  stkd->ConventionalHeatSink = NULL ;
  stkd->StackElementsList    = NULL ;
  stkd->Dimensions           = NULL ;
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
    return -1;
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
  free_materials_list         (stkd->MaterialsList) ;
  free_channel                (stkd->Channel) ;
  free_dies_list              (stkd->DiesList) ;
  free_conventional_heat_sink (stkd->ConventionalHeatSink) ;
  free_stack_elements_list    (stkd->StackElementsList) ;
  free_dimensions             (stkd->Dimensions) ;
  free                        (stkd->FileName) ;
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

  if (stkd->ConventionalHeatSink != NULL)

    print_conventional_heat_sink (stream, prefix, stkd->ConventionalHeatSink) ;

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

    if (stack_element->Type == TDICE_STACK_ELEMENT_DIE)

      print_floorplan(stream, prefix, stack_element->Floorplan) ;
}

/******************************************************************************/

int fill_floorplans (StackDescription* stkd)
{
  StackElement* stack_element = stkd->StackElementsList ;

  for ( ; stack_element != NULL ; stack_element = stack_element->Next)

    if (stack_element->Type == TDICE_STACK_ELEMENT_DIE)

      if (fill_floorplan (stack_element->Floorplan, stkd->Dimensions) == 1)

        return 1 ;

  return 0 ;
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
      case TDICE_STACK_ELEMENT_DIE :

        conductances = fill_conductances_die
                       (
                         stack_element->Pointer.Die,
                         conductances,
                         stkd->Dimensions,
                         stkd->ConventionalHeatSink,
                         stack_element->LayersOffset
                       ) ;
        break ;

      case TDICE_STACK_ELEMENT_LAYER :

        conductances = fill_conductances_layer
                       (
                         stack_element->Pointer.Layer,
                         conductances,
                         stkd->Dimensions,
                         stkd->ConventionalHeatSink,
                         stack_element->LayersOffset
                       ) ;
        break ;

      case TDICE_STACK_ELEMENT_CHANNEL :

        conductances = fill_conductances_channel
                       (
#                        ifdef PRINT_CONDUCTANCES
                         stack_element->LayersOffset,
#                        endif
                         stkd->Channel,
                         conductances, stkd->Dimensions
                       ) ;
        break ;

      case TDICE_STACK_ELEMENT_NONE :

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
      case TDICE_STACK_ELEMENT_DIE :

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

      case TDICE_STACK_ELEMENT_LAYER :

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

      case TDICE_STACK_ELEMENT_CHANNEL :

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

      case TDICE_STACK_ELEMENT_NONE :

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
  StackElement* stack_element      = NULL ;
  StackElement* last_stack_element = NULL;
  Source_t*     tmp_sources        = sources ;

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
      case TDICE_STACK_ELEMENT_DIE :

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

      case TDICE_STACK_ELEMENT_LAYER :

        tmp_sources += get_layer_area (stkd->Dimensions) ;
        break ;

      case TDICE_STACK_ELEMENT_CHANNEL :

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

      case TDICE_STACK_ELEMENT_NONE :

        fprintf (stderr,  "Error! Found stack element with unset type\n") ;
        return ;

      default :

        fprintf (stderr, "Error! Unknown stack element type %d\n",
          stack_element->Type) ;
        return ;

    } /* switch stack_element->Type */

  if (stkd->ConventionalHeatSink != NULL)
  {
    if (last_stack_element->Type == TDICE_STACK_ELEMENT_DIE)

        add_sources_conventional_heat_sink
        (
          stkd->ConventionalHeatSink,
          stkd->Dimensions,
          sources,
          conductances,
          last_stack_element->LayersOffset
          + last_stack_element->Pointer.Die->NLayers
        ) ;

    else if (last_stack_element->Type == TDICE_STACK_ELEMENT_LAYER)

        add_sources_conventional_heat_sink
        (
          stkd->ConventionalHeatSink,
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
  ColumnIndex_t*       column_pointers,
  RowIndex_t*          row_indices,
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

  *column_pointers++ = 0 ;

  for
  (
    added            = 0,
    area             = get_layer_area (stkd->Dimensions),
    stack_element    = stkd->StackElementsList ;
    stack_element   != NULL ;
    conductances    += area * stack_element->NLayers,
    capacities      += area * stack_element->NLayers,
    column_pointers += area * stack_element->NLayers,
    row_indices     += added,
    values          += added,
    stack_element    = stack_element->Next
  )

    switch (stack_element->Type)
    {
      case TDICE_STACK_ELEMENT_DIE :

        added = fill_system_matrix_die
                (
                  stack_element->Pointer.Die, stkd->Dimensions,
                  conductances, capacities,
                  stkd->ConventionalHeatSink,
                  stack_element->LayersOffset,
                  column_pointers, row_indices, values
                ) ;
        break ;

      case TDICE_STACK_ELEMENT_LAYER :

        added = fill_system_matrix_layer
                (
#                 ifdef PRINT_SYSTEM_MATRIX
                  stack_element->Pointer.Layer,
#                 endif
                  stkd->Dimensions, conductances, capacities,
                  stkd->ConventionalHeatSink,
                  stack_element->LayersOffset,
                  column_pointers, row_indices, values
                ) ;
        break ;

      case TDICE_STACK_ELEMENT_CHANNEL :

        added = fill_system_matrix_channel
                (
#                 ifdef PRINT_SYSTEM_MATRIX
                  stkd->Channel,
#                 endif
                  stkd->Dimensions, conductances, capacities,
                  stack_element->LayersOffset,
                  column_pointers, row_indices, values
                ) ;
        break ;

      case TDICE_STACK_ELEMENT_NONE :

        fprintf (stderr,  "Error! Found stack element with unset type\n") ;
        return ;

      default :

        fprintf (stderr, "Error! Unknown stack element type %d\n",
          stack_element->Type) ;
        return ;

    } /* stack_element->Type */
}

/******************************************************************************/

Quantity_t get_number_of_remaining_power_values (StackDescription* stkd)
{
  StackElement* stk_el = stkd->StackElementsList ;
  while (stk_el != NULL && stk_el->Type != TDICE_STACK_ELEMENT_DIE)
    stk_el = stk_el->Next;

  // if stk_el == NULL then BUG !!!

  return stk_el->Floorplan->ElementsList->PowerValues->Length ;
}

/******************************************************************************/

Quantity_t get_number_of_floorplan_elements
(
  StackDescription* stkd,
  String_t          floorplan_id
)
{
  StackElement* stk_el = find_stack_element_in_list
                         (
                           stkd->StackElementsList,
                           floorplan_id
                         ) ;
  if (stk_el == NULL)

    return -1 ;

  if (stk_el->Type != TDICE_STACK_ELEMENT_DIE || stk_el->Floorplan == NULL)

    return -2 ;

  return stk_el->Floorplan->NElements ;
}

/******************************************************************************/

Quantity_t get_number_of_channel_outlets (StackDescription* stkd)
{
  return (get_number_of_columns(stkd->Dimensions) - 2) / 2 ;
}

/******************************************************************************/
