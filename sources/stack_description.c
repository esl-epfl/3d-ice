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

/******************************************************************************/

void init_stack_description (StackDescription_t *stkd)
{
    stkd->FileName           = NULL ;
    stkd->MaterialsList      = NULL ;
    stkd->HeatSink           = NULL ;
    stkd->Channel            = NULL ;
    stkd->LayersList         = NULL ;
    stkd->DiesList           = NULL ;
    stkd->Dimensions         = NULL ;
    stkd->TopStackElement    = NULL ;
    stkd->BottomStackElement = NULL ;
}

/******************************************************************************/

void destroy_stack_description (StackDescription_t *stkd)
{
    FREE_POINTER (free,                     stkd->FileName) ;
    FREE_POINTER (free_materials_list,      stkd->MaterialsList) ;
    FREE_POINTER (free_heat_sink,           stkd->HeatSink) ;
    FREE_POINTER (free_channel,             stkd->Channel) ;
    FREE_POINTER (free_layers_list,         stkd->LayersList) ;
    FREE_POINTER (free_dies_list,           stkd->DiesList) ;
    FREE_POINTER (free_dimensions,          stkd->Dimensions) ;
    FREE_POINTER (free_stack_elements_list, stkd->BottomStackElement) ;

    stkd->TopStackElement = NULL ;
}

/******************************************************************************/

void print_stack_description
(
  StackDescription_t *stkd,
  FILE               *stream,
  String_t            prefix
)
{
    if (stkd->MaterialsList != NULL)
    {
        print_materials_list (stkd->MaterialsList, stream, prefix) ;

        fprintf (stream, "%s\n", prefix) ;
    }

    if (stkd->HeatSink != NULL)
    {
        print_heat_sink (stkd->HeatSink, stream, prefix) ;

        fprintf (stream, "%s\n", prefix) ;
    }

    if (stkd->Channel != NULL)
    {
        print_channel

            (stkd->Channel, stream, prefix, stkd->Dimensions) ;

        fprintf (stream, "%s\n", prefix) ;
    }

    print_layers_list (stkd->LayersList, stream, prefix) ;

    fprintf (stream, "%s\n", prefix) ;

    print_dies_list (stkd->DiesList, stream, prefix) ;

    fprintf (stream, "%s\n", prefix) ;

    print_dimensions (stkd->Dimensions, stream, prefix) ;

    fprintf (stream, "%s\n", prefix) ;

    print_stack_elements_list (stkd->TopStackElement, stream, prefix) ;

    fprintf (stream, "%s\n", prefix) ;
}

/******************************************************************************/

Quantity_t get_number_of_floorplan_elements
(
  StackDescription_t *stkd,
  String_t            stack_element_id
)
{
    StackElement_t *stack_element = find_stack_element_in_list

        (stkd->BottomStackElement, stack_element_id) ;

    if (stack_element == NULL)

        return 0 ;

    return get_number_of_floorplan_elements_stack_element (stack_element) ;
}

/******************************************************************************/

Quantity_t get_total_number_of_floorplan_elements (StackDescription_t *stkd)
{
    Quantity_t tmp = 0u ;

    FOR_EVERY_ELEMENT_IN_LIST_NEXT

        (StackElement_t, stack_element, stkd->BottomStackElement)

        tmp += get_number_of_floorplan_elements_stack_element (stack_element) ;

    return tmp ;
}

/******************************************************************************/
