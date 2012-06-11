/******************************************************************************
 * This file is part of 3D-ICE, version 2.1 .                                 *
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
 *          Giseong Bak                                                       *
 *          Martino Ruggiero                                                  *
 *          Thomas Brunschwiler                                               *
 *          David Atienza                                                     *
 *                                                                            *
 * For any comment, suggestion or request  about 3D-ICE, please  register and *
 * write to the mailing list (see http://listes.epfl.ch/doc.cgi?liste=3d-ice) *
 * Any usage  of 3D-ICE  for research,  commercial or other  purposes must be *
 * properly acknowledged in the resulting products or publications.           *
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

    (StackDescription_t *this, Analysis_t *analysis, yyscan_t scanner) ;

/******************************************************************************/

void init_stack_description (StackDescription_t *this)
{
    this->FileName           = NULL ;
    this->MaterialsList      = NULL ;
    this->HeatSink           = NULL ;
    this->Channel            = NULL ;
    this->LayersList         = NULL ;
    this->DiesList           = NULL ;
    this->Dimensions         = NULL ;
    this->TopStackElement    = NULL ;
    this->BottomStackElement = NULL ;
}

/******************************************************************************/

Error_t fill_stack_description
(
    StackDescription_t *this,
    Analysis_t         *analysis,
    String_t            filename
)
{
    FILE*    input ;
    int      result ;
    yyscan_t scanner ;

    input = fopen (filename, "r") ;
    if (input == NULL)
    {
        fprintf (stderr, "Unable to open stack file %s\n", filename) ;

        return TDICE_FAILURE ;
    }

    this->FileName = strdup (filename) ;  // FIXME memory leak

    stack_description_lex_init (&scanner) ;
    stack_description_set_in (input, scanner) ;

    result = stack_description_parse (this, analysis, scanner) ;

    stack_description_lex_destroy (scanner) ;
    fclose (input) ;

    if (result == 0)

        return TDICE_SUCCESS ;

    else

        return TDICE_FAILURE ;
}

/******************************************************************************/

void free_stack_description (StackDescription_t *this)
{
    FREE_POINTER (free,                     this->FileName) ;
    FREE_POINTER (free_materials_list,      this->MaterialsList) ;
    FREE_POINTER (free_heat_sink,           this->HeatSink) ;
    FREE_POINTER (free_channel,             this->Channel) ;
    FREE_POINTER (free_layers_list,         this->LayersList) ;
    FREE_POINTER (free_dies_list,           this->DiesList) ;
    FREE_POINTER (free_dimensions,          this->Dimensions) ;
    FREE_POINTER (free_stack_elements_list, this->BottomStackElement) ;

    this->TopStackElement = NULL ;
}

/******************************************************************************/

void print_formatted_stack_description
(
  StackDescription_t *this,
  FILE               *stream,
  String_t            prefix
)
{
    if (this->MaterialsList != NULL)
    {
        print_formatted_materials_list (this->MaterialsList, stream, prefix) ;

        fprintf (stream, "%s\n", prefix) ;
    }

    if (this->HeatSink != NULL)
    {
        print_formatted_heat_sink (this->HeatSink, stream, prefix) ;

        fprintf (stream, "%s\n", prefix) ;
    }

    if (this->Channel != NULL)
    {
        print_formatted_channel

            (this->Channel, stream, prefix, this->Dimensions) ;

        fprintf (stream, "%s\n", prefix) ;
    }

    print_formatted_layers_list (this->LayersList, stream, prefix) ;

    fprintf (stream, "%s\n", prefix) ;

    print_formatted_dies_list (this->DiesList, stream, prefix) ;

    fprintf (stream, "%s\n", prefix) ;

    print_formatted_dimensions (this->Dimensions, stream, prefix) ;

    fprintf (stream, "%s\n", prefix) ;

    print_formatted_stack_elements_list (this->TopStackElement, stream, prefix) ;

    fprintf (stream, "%s\n", prefix) ;
}

/******************************************************************************/

void print_detailed_stack_description
(
  StackDescription_t *this,
  FILE               *stream,
  String_t            prefix
)
{
    String_t new_prefix =

        (String_t) malloc (sizeof(*new_prefix) * (5 + strlen(prefix))) ;

    if (new_prefix == NULL) return ;

    sprintf (new_prefix, "%s    ", prefix) ;

    fprintf (stream,
             "%sStackDescription                = %p\n",
             prefix, this) ;

    fprintf (stream,
             "%s  FileName                      = " "%s\n",
             prefix, this->FileName) ;

    fprintf (stream,
             "%s  MaterialsList                 = %p\n",
             prefix, this->MaterialsList) ;

    if (this->MaterialsList != NULL)
    {
        fprintf (stream, "%s\n", prefix) ;

        print_detailed_materials_list

            (this->MaterialsList, stream, new_prefix) ;

        fprintf (stream, "%s\n", prefix) ;
    }

    fprintf (stream,
             "%s  HeatSink                      = %p\n",
             prefix, this->HeatSink) ;

    if (this->HeatSink != NULL)
    {
        fprintf (stream, "%s\n", prefix) ;

        print_detailed_heat_sink (this->HeatSink, stream, new_prefix) ;

        fprintf (stream, "%s\n", prefix) ;
    }

    fprintf (stream,
             "%s  Channel                       = %p\n",
             prefix, this->Channel) ;

    if (this->Channel != NULL)
    {
        fprintf (stream, "%s\n", prefix) ;

        print_detailed_channel (this->Channel, stream, new_prefix) ;

        fprintf (stream, "%s\n", prefix) ;
    }

    fprintf (stream,
             "%s  LayersList                    = %p\n",
             prefix, this->LayersList) ;

    if (this->LayersList != NULL)
    {
        fprintf (stream, "%s\n", prefix) ;

        sprintf (new_prefix, "%s        ", prefix) ;

        print_detailed_layers_list

            (this->LayersList, stream, new_prefix) ;

        sprintf (new_prefix, "%s    ", prefix) ;

        fprintf (stream, "%s\n", prefix) ;
    }

    fprintf (stream,
             "%s  DiesList                      = %p\n",
             prefix, this->DiesList) ;

    if (this->DiesList != NULL)
    {
        fprintf (stream, "%s\n", prefix) ;

        print_detailed_dies_list

            (this->DiesList, stream, new_prefix) ;

        fprintf (stream, "%s\n", prefix) ;
    }

    fprintf (stream,
             "%s  Dimensions                    = %p\n",
             prefix, this->Dimensions) ;

    if (this->Dimensions != NULL)
    {
        fprintf (stream, "%s\n", prefix) ;

        print_detailed_dimensions

            (this->Dimensions, stream, new_prefix) ;

        fprintf (stream, "%s\n", prefix) ;
    }

    fprintf (stream,
             "%s  TopStackElement               = %p\n",
             prefix, this->TopStackElement) ;

    fprintf (stream,
             "%s  BottomStackElement            = %p\n",
             prefix, this->BottomStackElement) ;

    if (this->TopStackElement != NULL)
    {
        fprintf (stream, "%s\n", prefix) ;

        print_detailed_stack_elements_list

            (this->TopStackElement, stream, new_prefix) ;

        fprintf (stream, "%s\n", prefix) ;
    }

    FREE_POINTER (free, new_prefix) ;
}

/******************************************************************************/

void print_floorplans
(
    StackDescription_t *this,
    FILE               *stream,
    String_t            prefix
)
{
    FOR_EVERY_ELEMENT_IN_LIST_NEXT

    (StackElement_t, stk_el, this->BottomStackElement)

        if (stk_el->Type == TDICE_STACK_ELEMENT_DIE)

            print_detailed_floorplan (stk_el->Floorplan, stream, prefix) ;
}

/******************************************************************************/

Quantity_t get_number_of_floorplan_elements
(
  StackDescription_t *this,
  String_t            stack_element_id
)
{
    StackElement_t *stack_element = find_stack_element_in_list

        (this->BottomStackElement, stack_element_id) ;

    if (stack_element == NULL)

        return 0 ;

    return get_number_of_floorplan_elements_stack_element (stack_element) ;
}

/******************************************************************************/

Quantity_t get_total_number_of_floorplan_elements (StackDescription_t *this)
{
    Quantity_t tmp = 0u ;

    FOR_EVERY_ELEMENT_IN_LIST_NEXT

        (StackElement_t, stack_element, this->BottomStackElement)

        tmp += get_number_of_floorplan_elements_stack_element (stack_element) ;

    return tmp ;
}

/******************************************************************************/

FloorplanElement_t *get_floorplan_element
(
    StackDescription_t *this,
    String_t            stack_element_id,
    String_t            floorplan_element_id
)
{
    StackElement_t *stack_element = find_stack_element_in_list

        (this->BottomStackElement, stack_element_id) ;

    if (stack_element == NULL)

        return NULL ;

    return get_floorplan_element_stack_element

        (stack_element, floorplan_element_id) ;
}

/******************************************************************************/

Error_t insert_power_values
(
    StackDescription_t *this,
    PowersQueue_t      *pvalues
)
{
    Error_t result ;

    FOR_EVERY_ELEMENT_IN_LIST_NEXT

    (StackElement_t, stack_element, this->BottomStackElement)
    {
        result = insert_power_values_stack_element (stack_element, pvalues) ;

        if (result == TDICE_FAILURE)

            return TDICE_FAILURE ;
    }

    return TDICE_SUCCESS ;
}

/******************************************************************************/
