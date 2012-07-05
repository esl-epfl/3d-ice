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
#include <string.h>

#include "stack_element.h"
#include "macros.h"

/******************************************************************************/

void
init_stack_element (StackElement_t *stkel)
{
    stkel->Type            = TDICE_STACK_ELEMENT_NONE ;
    stkel->Pointer.Layer   = NULL ;
    stkel->Pointer.Die     = NULL ;
    stkel->Pointer.Channel = NULL ;
    stkel->Floorplan       = NULL ;
    stkel->Id              = NULL ;
    stkel->NLayers         = 0u ;
    stkel->Offset          = 0u ;
    stkel->Next            = NULL ;
    stkel->Prev            = NULL ;
}

/******************************************************************************/

StackElement_t *calloc_stack_element (void)
{
    StackElement_t *stack_element = (StackElement_t *) malloc (sizeof(StackElement_t)) ;

    if (stack_element != NULL)

        init_stack_element(stack_element) ;

    return stack_element ;
}

/******************************************************************************/

void free_stack_element (StackElement_t *stkel)
{
    if (   stkel->Type == TDICE_STACK_ELEMENT_DIE
        && stkel->Floorplan != NULL)

        FREE_POINTER (free_floorplan, stkel->Floorplan) ;

    FREE_POINTER (free, stkel->Id) ;
    FREE_POINTER (free, stkel) ;
}

/******************************************************************************/

void free_stack_elements_list (StackElement_t *list)
{
    FREE_LIST (StackElement_t, list, free_stack_element) ;
}

/******************************************************************************/

StackElement_t *find_stack_element_in_list (StackElement_t *list, String_t id)
{
    FOR_EVERY_ELEMENT_IN_LIST_NEXT (StackElement_t, stk_el, list)
    {
        if (strcmp(stk_el->Id, id) == 0)  break ;
    }
    return stk_el ;
}

/******************************************************************************/

void print_stack_elements_list
(
    StackElement_t *list,
    FILE           *stream,
    String_t        prefix
)
{
    Quantity_t max_stk_el_id_length = 0 ;
    Quantity_t max_die_id_length = 0 ;

    FOR_EVERY_ELEMENT_IN_LIST_PREV (StackElement_t, stk_el, list)
    {
        max_stk_el_id_length =

            MAX (max_stk_el_id_length, strlen (stk_el->Id)) ;

        if (stk_el->Type == TDICE_STACK_ELEMENT_DIE)

            max_die_id_length =

                MAX (max_die_id_length, strlen (stk_el->Pointer.Die->Id)) ;
    }

    fprintf (stream, "%sstack : \n", prefix) ;

    FOR_EVERY_ELEMENT_IN_LIST_PREV (StackElement_t, stack_element, list)
    {
        switch (stack_element->Type)
        {
            case TDICE_STACK_ELEMENT_CHANNEL :

                fprintf (stream,
                    "%s   channel  %-*s ;\n",
                    prefix, max_stk_el_id_length, stack_element->Id) ;
                break ;

            case TDICE_STACK_ELEMENT_DIE :

                fprintf (stream,
                    "%s   die      %-*s %-*s floorplan \"%s\" ;\n",
                    prefix,
                    max_stk_el_id_length, stack_element->Id,
                    max_die_id_length, stack_element->Pointer.Die->Id,
                    stack_element->Floorplan->FileName) ;
                break ;

            case TDICE_STACK_ELEMENT_LAYER :

                fprintf (stream,
                    "%s   layer    %-*s %s",
                    prefix,
                    max_stk_el_id_length, stack_element->Id,
                    stack_element->Pointer.Layer->Id) ;

                break ;

            case TDICE_STACK_ELEMENT_HEATSINK :

                break ;

            case TDICE_STACK_ELEMENT_NONE :

                fprintf (stderr, "Warning: found stack element type none\n") ;
                break ;

            default :

                fprintf (stderr, "Undefined stack element type %d\n", stack_element->Type) ;
        }
    }
}

/******************************************************************************/

CellIndex_t get_source_layer_offset (StackElement_t *stkel)
{
    CellIndex_t layer_offset = stkel->Offset ;

    if (stkel->Type == TDICE_STACK_ELEMENT_DIE)

        layer_offset += stkel->Pointer.Die->SourceLayerOffset ;

    else if (stkel->Type == TDICE_STACK_ELEMENT_CHANNEL)

        layer_offset += stkel->Pointer.Channel->SourceLayerOffset ;

    return layer_offset ;
}

/******************************************************************************/

void print_thermal_map_stack_element
(
    StackElement_t  *stkel,
    Dimensions_t    *dimensions,
    Temperature_t   *temperatures,
    FILE            *stream
)
{
    temperatures += get_cell_offset_in_stack

        (dimensions, get_source_layer_offset (stkel), 0, 0) ;

    FOR_EVERY_ROW (row_index, dimensions)
    {
        FOR_EVERY_COLUMN (column_index, dimensions)
        {
            fprintf (stream, "%7.3f  ", *temperatures++) ;
        }

        fprintf (stream, "\n") ;
    }
}

/******************************************************************************/

void print_power_map_stack_element
(
    StackElement_t  *stkel,
    Dimensions_t    *dimensions,
    Source_t        *sources,
    FILE            *stream
)
{
    sources += get_cell_offset_in_stack

        (dimensions, get_source_layer_offset (stkel), 0, 0) ;

    FOR_EVERY_ROW (row_index, dimensions)
    {
        FOR_EVERY_COLUMN (column_index, dimensions)
        {
            fprintf (stream, "%7.3f  ", *sources++) ;
        }

        fprintf (stream, "\n") ;
    }
}

/******************************************************************************/

Quantity_t get_number_of_floorplan_elements_stack_element
(
    StackElement_t *stkel
)
{
    if (stkel->Type == TDICE_STACK_ELEMENT_DIE)

        return get_number_of_floorplan_elements_floorplan

            (stkel->Floorplan) ;

    else

        return 0u ;
}

/******************************************************************************/
