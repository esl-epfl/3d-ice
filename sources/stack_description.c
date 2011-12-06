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

#include "stack_description.h"
#include "macros.h"

#include "../bison/stack_description_parser.h"
#include "../flex/stack_description_scanner.h"

// From Bison manual:
// The value returned by yyparse is 0 if parsing was successful (return is
// due to end-of-input). The value is 1 if parsing failed (return is due to
// a syntax error).

extern int stack_description_parse

    (StackDescription *stkd, Analysis *analysis, yyscan_t scanner) ;

/******************************************************************************/

void init_stack_description (StackDescription* stkd)
{
  stkd->FileName             = NULL ;
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
  StackDescription *stkd,
  Analysis         *analysis,
  char             *filename
)
{
    FILE*    input ;
    int      result ;
    yyscan_t scanner ;

    input = fopen (filename, "r") ;
    if (input == NULL)
    {
        fprintf (stderr, "Unable to open stack file %s\n", filename) ;

        return EXIT_FAILURE ;
    }

    stkd->FileName = strdup (filename) ;  // FIXME memory leak

    stack_description_lex_init (&scanner) ;
    stack_description_set_in (input, scanner) ;

    result = stack_description_parse (stkd, analysis, scanner) ;

    stack_description_lex_destroy (scanner) ;
    fclose (input) ;

    if (result == 0)

        return TDICE_SUCCESS ;

    else

        return TDICE_FAILURE ;
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
  FILE             *stream,
  char             *prefix,
  StackDescription *stkd
)
{
    print_formatted_materials_list (stream, prefix, stkd->MaterialsList) ;

    fprintf (stream, "%s\n", prefix) ;

    if (stkd->ConventionalHeatSink != NULL)
    {
        print_formatted_conventional_heat_sink

            (stream, prefix, stkd->ConventionalHeatSink) ;

        fprintf (stream, "%s\n", prefix) ;
    }

    if (stkd->Channel != NULL)
    {
        print_formatted_channel

            (stream, prefix, stkd->Channel, stkd->Dimensions) ;

        fprintf (stream, "%s\n", prefix) ;
    }

    print_formatted_dies_list (stream, prefix, stkd->DiesList) ;

    fprintf (stream, "%s\n", prefix) ;

    print_formatted_dimensions (stream, prefix, stkd->Dimensions) ;

    fprintf (stream, "%s\n", prefix) ;

    print_formatted_stack_elements_list (stream, prefix, stkd->TopStackElement) ;

    fprintf (stream, "%s\n", prefix) ;
}

/******************************************************************************/

void print_detailed_stack_description
(
  FILE             *stream,
  char             *prefix,
  StackDescription *stkd
)
{
    char * new_prefix = malloc (sizeof(*new_prefix) * (5 + strlen(prefix))) ;

    if (new_prefix == NULL) return ;

    sprintf (new_prefix, "%s    ", prefix) ;

    fprintf (stream,
             "%sStackDescription                = %p\n",
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

        print_detailed_materials_list

            (stream, new_prefix, stkd->MaterialsList) ;

        fprintf (stream, "%s\n", prefix) ;
    }

    fprintf (stream,
             "%s  ConventionalHeatSink          = %p\n",
             prefix, stkd->ConventionalHeatSink) ;

    if (stkd->ConventionalHeatSink != NULL)
    {
        fprintf (stream, "%s\n", prefix) ;

        print_detailed_conventional_heat_sink

            (stream, new_prefix, stkd->ConventionalHeatSink) ;

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

        print_detailed_dies_list

            (stream, new_prefix, stkd->DiesList) ;

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

        print_detailed_stack_elements_list

            (stream, new_prefix, stkd->TopStackElement) ;

        fprintf (stream, "%s\n", prefix) ;
    }

    fprintf (stream,
             "%s  Dimensions                    = %p\n",
             prefix, stkd->Dimensions) ;

    if (stkd->Dimensions != NULL)
    {
        fprintf (stream, "%s\n", prefix) ;

        print_detailed_dimensions

            (stream, new_prefix, stkd->Dimensions) ;

        fprintf (stream, "%s\n", prefix) ;
    }

    FREE_POINTER (free, new_prefix) ;
}

/******************************************************************************/

void print_floorplans
(
    FILE             *stream,
    char             *prefix,
    StackDescription *stkd
)
{
    FOR_EVERY_ELEMENT_IN_LIST_NEXT

    (StackElement, stk_el, stkd->BottomStackElement)

        if (stk_el->Type == TDICE_STACK_ELEMENT_DIE)

            print_detailed_floorplan (stream, prefix, stk_el->Floorplan) ;
}

/******************************************************************************/

void fill_thermal_cell_stack_description
(
    ThermalCell      *thermal_cells,
    Analysis         *analysis,
    StackDescription *stkd
)
{
    FOR_EVERY_ELEMENT_IN_LIST_NEXT

    (StackElement, stack_element, stkd->BottomStackElement)

        fill_thermal_cell_stack_element

            (thermal_cells, analysis->StepTime, stkd->Dimensions, stack_element) ;

    if (stkd->ConventionalHeatSink)

        fill_thermal_cell_conventional_heat_sink

            (thermal_cells, stkd->Dimensions, stkd->ConventionalHeatSink) ;

    if (analysis->AnalysisType == TDICE_ANALYSIS_TYPE_STEADY)

        reset_capacities (thermal_cells, get_number_of_cells(stkd->Dimensions)) ;
}

/******************************************************************************/

Error_t fill_sources_stack_description
(
    double           *sources,
    ThermalCell      *thermal_cells,
    StackDescription *stkd
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

    uint32_t ccounter ;
    uint32_t ncells = get_number_of_cells (stkd->Dimensions) ;

    for (ccounter = 0 ; ccounter != ncells ; ccounter++)

        sources [ ccounter ] = 0.0 ;

    // set the sources due to the heatsink (overwrites all cells in the last layer)

    if (stkd->ConventionalHeatSink != NULL)

        fill_sources_conventional_heat_sink

            (sources, thermal_cells, stkd->Dimensions, stkd->ConventionalHeatSink) ;

    FOR_EVERY_ELEMENT_IN_LIST_NEXT

    (StackElement, stack_element, stkd->BottomStackElement)

        if (fill_sources_stack_element (sources, stkd->Dimensions, stack_element) == TDICE_FAILURE)

            return TDICE_FAILURE ;

    return TDICE_SUCCESS ;
}

/******************************************************************************/

void fill_system_matrix_stack_description
(
    SystemMatrix      system_matrix,
    ThermalCell*      thermal_cells,
    StackDescription *stkd
)
{
#ifdef PRINT_SYSTEM_MATRIX
    fprintf (stderr,
        "fill_system_matrix_stack_description ( l %d r %d c %d )\n",
        get_number_of_layers  (stkd->Dimensions),
        get_number_of_rows    (stkd->Dimensions),
        get_number_of_columns (stkd->Dimensions)) ;
#endif

    SystemMatrix tmp_system_matrix = system_matrix ;

    *system_matrix.ColumnPointers++ = 0u ;

    FOR_EVERY_ELEMENT_IN_LIST_NEXT

    (StackElement, stack_element, stkd->BottomStackElement)

        system_matrix = fill_system_matrix_stack_element

            (system_matrix, stkd->Dimensions, thermal_cells, stack_element) ;

    if (stkd->ConventionalHeatSink != NULL)

        fill_system_matrix_conventional_heat_sink

            (tmp_system_matrix, stkd->Dimensions, thermal_cells) ;
}

/******************************************************************************/

uint32_t get_number_of_floorplan_elements
(
  StackDescription *stkd,
  char             *stack_element_id
)
{
    StackElement *stack_element = find_stack_element_in_list

        (stkd->BottomStackElement, stack_element_id) ;

    if (stack_element == NULL)

        return 0 ;

    return get_number_of_floorplan_elements_stack_element (stack_element) ;
}

/******************************************************************************/

uint32_t get_total_number_of_floorplan_elements (StackDescription *stkd)
{
    uint32_t tmp = 0u ;

    FOR_EVERY_ELEMENT_IN_LIST_NEXT

        (StackElement, stack_element, stkd->BottomStackElement)

        tmp += get_number_of_floorplan_elements_stack_element (stack_element) ;

    return tmp ;
}

/******************************************************************************/

bool insert_power_values_by_powers_queue
(
  StackDescription* stkd,
  PowersQueue*      pvalues
)
{
    if (pvalues->Length != (unsigned int) get_total_number_of_floorplan_elements(stkd))

        return false ;

    FOR_EVERY_ELEMENT_IN_LIST_NEXT

    (StackElement, stack_element, stkd->BottomStackElement)

        insert_power_values_stack_element (stack_element, pvalues) ;

  return true ;
}

/******************************************************************************/

bool insert_power_values
(
  StackDescription* stkd,
  double*          pvalues
)
{
  PowersQueue* pvalues_queue;
  pvalues_queue = alloc_and_init_powers_queue();
  if (pvalues == NULL)
    return false;

  uint32_t index;
  uint32_t totalNFloorplanElements = get_total_number_of_floorplan_elements(stkd);

  for (index = 0; index < totalNFloorplanElements; index++)
    put_into_powers_queue(pvalues_queue, pvalues[index]);

  return insert_power_values_by_powers_queue(stkd, pvalues_queue);
}

/******************************************************************************/
