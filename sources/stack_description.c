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

#include "stack_description.h"
#include "thermal_cell.h"
#include "macros.h"
#include "layer.h"
#include "system_matrix.h"

#include "../bison/stack_description_parser.h"
#include "../flex/stack_description_scanner.h"

/******************************************************************************/

extern int  stack_description_parse (StackDescription* stkd, yyscan_t scanner) ;
static int  fill_floorplans         (StackDescription* stkd) ;

/******************************************************************************/

void init_stack_description (StackDescription* stkd)
{
  stkd->FileName             = STRING_I ;
  stkd->MaterialsList        = NULL ;
  stkd->ConventionalHeatSink = NULL ;
  stkd->Channel              = NULL ;
  stkd->DiesList             = NULL ;
  stkd->TopStackElement      = NULL ;
  stkd->BottomStackElement   = NULL ;
  stkd->Dimensions           = NULL ;
}

/******************************************************************************/

int fill_stack_description
(
  StackDescription* stkd,
  String_t          filename
)
{
  FILE*    input ;
  int      result ;
  yyscan_t scanner ;

  input = fopen (filename, "r") ;
  if(input == NULL)
  {
    perror(filename) ;
    return -1 ;
  }

  stkd->FileName = strdup (filename) ;

  stack_description_lex_init (&scanner) ;
  stack_description_set_in (input, scanner) ;

  result = stack_description_parse (stkd, scanner) ;

  stack_description_lex_destroy (scanner) ;
  fclose (input) ;

  if (result == 1) return result ;

  return fill_floorplans (stkd) ;
}

/******************************************************************************/

void free_stack_description (StackDescription* stkd)
{
  FREE_POINTER (free_materials_list,         stkd->MaterialsList) ;
  FREE_POINTER (free_channel,                stkd->Channel) ;
  FREE_POINTER (free_dies_list,              stkd->DiesList) ;
  FREE_POINTER (free_conventional_heat_sink, stkd->ConventionalHeatSink) ;
  FREE_POINTER (free_stack_elements_list,    stkd->BottomStackElement) ;
  FREE_POINTER (free_dimensions,             stkd->Dimensions) ;
  FREE_POINTER (free,                        stkd->FileName) ;

  stkd->TopStackElement = NULL ;
}

/******************************************************************************/

void print_formatted_stack_description
(
  FILE*             stream,
  String_t          prefix,
  StackDescription* stkd
)
{
  print_formatted_materials_list (stream, prefix, stkd->MaterialsList) ;
  fprintf (stream, "%s\n", prefix) ;

  if (stkd->ConventionalHeatSink != NULL)
  {
    print_formatted_conventional_heat_sink (stream, prefix,
                                            stkd->ConventionalHeatSink) ;
    fprintf (stream, "%s\n", prefix) ;
  }

  if (stkd->Channel != NULL)
  {
    print_formatted_channel (stream, prefix, stkd->Channel, stkd->Dimensions) ;
    fprintf (stream, "%s\n", prefix) ;
  }

  print_formatted_dies_list (stream, prefix, stkd->DiesList) ;
  fprintf (stream, "%s\n", prefix) ;

  print_formatted_stack_elements_list (stream, prefix,
                                       stkd->TopStackElement) ;
  fprintf (stream, "%s\n", prefix) ;

  print_formatted_dimensions (stream, prefix, stkd->Dimensions) ;
}

/******************************************************************************/

void print_detailed_stack_description
(
  FILE*             stream,
  String_t          prefix,
  StackDescription* stkd
)
{
    String_t new_prefix = malloc (sizeof(*new_prefix) * (5 + strlen(prefix))) ;

    if (new_prefix == NULL) return ;

    sprintf (new_prefix, "%s    ", prefix) ;

    fprintf (stream,
             "%sstkd                            = %p\n",
             prefix, stkd) ;

    fprintf (stream,
             "%s  FileName                      = " "%s\n",
             prefix, stkd->FileName) ;

    fprintf (stream,
             "%s  MaterialsList                 = %p\n",
             prefix, stkd->MaterialsList) ;

    if (stkd->MaterialsList != NULL)
    {
        fprintf (stream, "%s\n", prefix) ;
        print_detailed_materials_list (stream, new_prefix, stkd->MaterialsList) ;
        fprintf (stream, "%s\n", prefix) ;
    }

    fprintf (stream,
             "%s  ConventionalHeatSink          = %p\n",
             prefix, stkd->ConventionalHeatSink) ;

    if (stkd->ConventionalHeatSink != NULL)
    {
        fprintf (stream, "%s\n", prefix) ;
        print_detailed_conventional_heat_sink (stream, new_prefix,
                                           stkd->ConventionalHeatSink) ;
        fprintf (stream, "%s\n", prefix) ;
    }

    fprintf (stream,
             "%s  Channel                       = %p\n",
             prefix, stkd->Channel) ;

    if (stkd->Channel != NULL)
    {
        fprintf (stream, "%s\n", prefix) ;
        print_detailed_channel (stream, new_prefix, stkd->Channel) ;
        fprintf (stream, "%s\n", prefix) ;
    }

    fprintf (stream,
             "%s  DiesList                      = %p\n",
             prefix, stkd->DiesList) ;

    if (stkd->DiesList != NULL)
    {
        fprintf (stream, "%s\n", prefix) ;
        print_detailed_dies_list (stream, new_prefix, stkd->DiesList) ;
        fprintf (stream, "%s\n", prefix) ;
    }

    fprintf (stream,
             "%s  TopStackElement               = %p\n",
             prefix, stkd->TopStackElement) ;

    fprintf (stream,
             "%s  BottomStackElement            = %p\n",
             prefix, stkd->BottomStackElement) ;

    if (stkd->TopStackElement != NULL)
    {
        fprintf (stream, "%s\n", prefix) ;
        print_detailed_stack_elements_list (stream, new_prefix, stkd->TopStackElement) ;
        fprintf (stream, "%s\n", prefix) ;
    }

    fprintf (stream,
             "%s  Dimensions                    = %p\n",
             prefix, stkd->Dimensions) ;

    if (stkd->Dimensions != NULL)
    {
        fprintf (stream, "%s\n", prefix) ;
        print_detailed_dimensions (stream, new_prefix, stkd->Dimensions) ;
        fprintf (stream, "%s\n", prefix) ;
    }

    FREE_POINTER (free, new_prefix) ;
}

/******************************************************************************/

void print_all_floorplans
(
  FILE*             stream,
  String_t          prefix,
  StackDescription* stkd
)
{
  FOR_EVERY_ELEMENT_IN_LIST_FORWARD (StackElement, stk_el, stkd->BottomStackElement)
  {
    if (stk_el->Type == TDICE_STACK_ELEMENT_DIE)

      print_floorplan(stream, prefix, stk_el->Floorplan) ;
  }
}

/******************************************************************************/

int fill_floorplans (StackDescription* stkd)
{
  FOR_EVERY_ELEMENT_IN_LIST_FORWARD (StackElement, stk_el, stkd->BottomStackElement)
  {
    if (stk_el->Type == TDICE_STACK_ELEMENT_DIE)

      if (fill_floorplan (stk_el->Floorplan, stkd->Dimensions) == 1)

        return 1 ;
  }

  return 0 ;
}

/******************************************************************************/

void fill_thermal_cell_stack_description
(
  ThermalCell*      thermalcells,
  Time_t            delta_time,
  StackDescription* stkd
)
{
  FOR_EVERY_ELEMENT_IN_LIST_FORWARD (StackElement, stack_element,
                                     stkd->BottomStackElement)
  {
    fill_thermal_cell_stack_element
    (
      thermalcells,
      delta_time,
      stkd->Dimensions,
      stack_element
    ) ;
  }

  if (stkd->ConventionalHeatSink)

    fill_thermal_cell_conventional_heat_sink
    (
      thermalcells,
      stkd->Dimensions,
      stkd->ConventionalHeatSink
    ) ;
}

/******************************************************************************/

int fill_sources_stack_description
(
  Source_t*         sources,
  ThermalCell*      thermalcells,
  StackDescription* stkd
)
{
#ifdef PRINT_SOURCES
    fprintf (stderr,
        "fill_sources_stack_description ( l %d r %d c %d )\n",
        get_number_of_layers  (stkd->Dimensions),
        get_number_of_rows    (stkd->Dimensions),
        get_number_of_columns (stkd->Dimensions)) ;
#endif

    // reset all the source vector to 0

    Quantity_t ccounter ;
    Quantity_t ncells = get_number_of_cells (stkd->Dimensions) ;

    for (ccounter = 0 ; ccounter != ncells ; ccounter++)

        sources [ ccounter ] = 0.0 ;

    // set the sources due to the heatsink (overwrites all cells in the last layer)

    if (stkd->ConventionalHeatSink != NULL)

        fill_sources_conventional_heat_sink
        (
            sources,
            thermalcells,
            stkd->Dimensions,
            stkd->ConventionalHeatSink
        ) ;

    FOR_EVERY_ELEMENT_IN_LIST_FORWARD (StackElement, stack_element, stkd->BottomStackElement)

        if (fill_sources_stack_element (sources, stkd->Dimensions, stack_element) != 0)

            return 1 ;

    return 0 ;
}

/******************************************************************************/

void init_power_values
(
  StackDescription* stkd
)
{
  FOR_EVERY_ELEMENT_IN_LIST_FORWARD (StackElement, stack_element,
                                     stkd->BottomStackElement)

    init_power_values_stack_element (stack_element) ;
}

/******************************************************************************/

Bool_t insert_power_values_by_powers_queue
(
  StackDescription* stkd,
  PowersQueue*      pvalues
)
{
  if (pvalues->Length != get_total_number_of_floorplan_elements(stkd))
    return FALSE_V;

  FOR_EVERY_ELEMENT_IN_LIST_FORWARD (StackElement, stack_element,
                                     stkd->BottomStackElement)

    insert_power_values_stack_element (stack_element, pvalues) ;

  return TRUE_V;
}

/******************************************************************************/

Bool_t insert_power_values
(
  StackDescription* stkd,
  Power_t*          pvalues
)
{
  PowersQueue* pvalues_queue;
  pvalues_queue = alloc_and_init_powers_queue();
  if (pvalues == NULL)
    return FALSE_V;

  Quantity_t index;
  Quantity_t totalNFloorplanElements = get_total_number_of_floorplan_elements(stkd);

  for (index = 0; index < totalNFloorplanElements; index++)
    put_into_powers_queue(pvalues_queue, pvalues[index]);

  return insert_power_values_by_powers_queue(stkd, pvalues_queue);
}

/******************************************************************************/

void update_channel_inlet_stack_description
(
  Source_t*         sources,
  StackDescription* stkd
)
{
#ifdef PRINT_SOURCES
  fprintf (stderr,
    "update_channel_inlet_stack_description ( l %d r %d c %d )\n",
    get_number_of_layers  (stkd->Dimensions),
    get_number_of_rows    (stkd->Dimensions),
    get_number_of_columns (stkd->Dimensions)) ;
#endif

  FOR_EVERY_ELEMENT_IN_LIST_FORWARD (StackElement, stack_element,
                                     stkd->BottomStackElement)

    if (stack_element->Type == TDICE_STACK_ELEMENT_CHANNEL)

      fill_sources_channel
      (
        sources,
        stkd->Dimensions,
        stack_element->Offset,
        stack_element->Pointer.Channel
      ) ;
}

/******************************************************************************/

void fill_system_matrix_stack_description
(
  SystemMatrix          system_matrix,
  ThermalCell*          thermalcells,
  StackDescription*     stkd
)
{
#ifdef PRINT_SYSTEM_MATRIX
  fprintf
  (
    stderr,
    "fill_system_matrix_stack_description ( l %d r %d c %d )\n",
    get_number_of_layers  (stkd->Dimensions),
    get_number_of_rows    (stkd->Dimensions),
    get_number_of_columns (stkd->Dimensions)) ;
#endif

  SystemMatrix tmp_system_matrix = system_matrix ;

  *system_matrix.ColumnPointers++ = SYSTEMMATRIXCOLUMN_I ;

  FOR_EVERY_ELEMENT_IN_LIST_FORWARD (StackElement, stack_element,
                                     stkd->BottomStackElement)

    system_matrix = fill_system_matrix_stack_element
                    (
                      system_matrix, stkd->Dimensions,
                      thermalcells, stack_element
                    ) ;

    if (stkd->ConventionalHeatSink != NULL)

      fill_system_matrix_conventional_heat_sink
      (
        tmp_system_matrix, stkd->Dimensions, thermalcells
      ) ;
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
                           stkd->BottomStackElement,
                           floorplan_id
                         ) ;
  if (stk_el == NULL)

    return -1 ;

  if (stk_el->Type != TDICE_STACK_ELEMENT_DIE || stk_el->Floorplan == NULL)

    return -2 ;

  return stk_el->Floorplan->NElements ;
}

/******************************************************************************/

Quantity_t get_total_number_of_floorplan_elements
(
  StackDescription* stkd
)
{
  Quantity_t total_number_of_floorplan_elements = QUANTITY_I;

  FOR_EVERY_ELEMENT_IN_LIST_FORWARD (StackElement, stack_element,
                                     stkd->BottomStackElement)
    if (stack_element->Type == TDICE_STACK_ELEMENT_DIE)
      total_number_of_floorplan_elements += stack_element->Floorplan->NElements;

  return total_number_of_floorplan_elements;
}


/******************************************************************************/

Quantity_t get_number_of_channel_outlets (StackDescription* stkd)
{
  return (get_number_of_columns(stkd->Dimensions) - 2) / (Quantity_t) 2 ;
}

/******************************************************************************/
