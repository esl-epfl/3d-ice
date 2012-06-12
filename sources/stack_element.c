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
init_stack_element (StackElement_t *this)
{
    this->Type            = TDICE_STACK_ELEMENT_NONE ;
    this->Pointer.Layer   = NULL ;
    this->Pointer.Die     = NULL ;
    this->Pointer.Channel = NULL ;
    this->Floorplan       = NULL ;
    this->Id              = NULL ;
    this->NLayers         = 0u ;
    this->Offset          = 0u ;
    this->Next            = NULL ;
    this->Prev            = NULL ;
}

/******************************************************************************/

StackElement_t *alloc_and_init_stack_element (void)
{
    StackElement_t *stack_element = (StackElement_t *) malloc (sizeof(StackElement_t)) ;

    if (stack_element != NULL)

        init_stack_element(stack_element) ;

    return stack_element ;
}

/******************************************************************************/

void free_stack_element (StackElement_t *this)
{
    if (   this->Type == TDICE_STACK_ELEMENT_DIE
        && this->Floorplan != NULL)

        FREE_POINTER (free_floorplan, this->Floorplan) ;

    FREE_POINTER (free, this->Id) ;
    FREE_POINTER (free, this) ;
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

void print_formatted_stack_elements_list
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

void print_detailed_stack_elements_list
(
    StackElement_t *list,
    FILE           *stream,
    String_t        prefix
)
{
    FOR_EVERY_ELEMENT_IN_LIST_PREV (StackElement_t, stk_el, list)
    {
        fprintf (stream,
            "%sstk_el                      = %p\n",
            prefix,   stk_el);

        fprintf (stream,
            "%s  Id                        = " "%s\n",
            prefix,   stk_el->Id);

        fprintf (stream,
            "%s  Type                      = %d\n",
            prefix,   stk_el->Type);

        if (stk_el->Type == TDICE_STACK_ELEMENT_DIE)
        {
            fprintf (stream,
                "%s  Pointer.Die               = %p\n",
                prefix,   stk_el->Pointer.Die);
        }
        else if (stk_el->Type == TDICE_STACK_ELEMENT_CHANNEL)
        {
            fprintf (stream,
                "%s  Pointer.Channel           = %p\n",
                prefix,   stk_el->Pointer.Channel);
        }
        else if (stk_el->Type == TDICE_STACK_ELEMENT_LAYER)
        {
            fprintf (stream,
                "%s  Pointer.Layer             = %p\n",
                prefix,   stk_el->Pointer.Layer);
        }
        else if (stk_el->Type == TDICE_STACK_ELEMENT_HEATSINK)
        {
            fprintf (stream,
                "%s  Pointer.HeatSink          = %p\n",
                prefix,   stk_el->Pointer.HeatSink);
        }

        fprintf (stream,
            "%s  NLayers                   = %d\n",
            prefix,   stk_el->NLayers);

        fprintf (stream,
            "%s  Floorplan                 = %p\n",
            prefix,   stk_el->Floorplan);

        fprintf (stream,
            "%s  Offset                    = %d\n",
            prefix,   stk_el->Offset);

        fprintf (stream,
            "%s  Next                      = %p\n",
            prefix,   stk_el->Next);

        fprintf (stream,
            "%s  Prev                      = %p\n",
            prefix,   stk_el->Prev);

        fprintf (stream, "%s\n", prefix) ;

    } // FOR_EVERY_ELEMENT_IN_LIST
}

/******************************************************************************/

CellIndex_t get_source_layer_offset (StackElement_t *this)
{
    CellIndex_t layer_offset = this->Offset ;

    if (this->Type == TDICE_STACK_ELEMENT_DIE)

        layer_offset += this->Pointer.Die->SourceLayerOffset ;

    else if (this->Type == TDICE_STACK_ELEMENT_CHANNEL)

        layer_offset += this->Pointer.Channel->SourceLayerOffset ;

    return layer_offset ;
}

/******************************************************************************/

void print_thermal_map_stack_element
(
    StackElement_t  *this,
    Dimensions_t    *dimensions,
    Temperature_t   *temperatures,
    FILE            *stream
)
{
    temperatures += get_cell_offset_in_stack

        (dimensions, get_source_layer_offset (this), 0, 0) ;

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
    StackElement_t  *this,
    Dimensions_t    *dimensions,
    Source_t        *sources,
    FILE            *stream
)
{
    sources += get_cell_offset_in_stack

        (dimensions, get_source_layer_offset (this), 0, 0) ;

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
    StackElement_t *this
)
{
    if (this->Type == TDICE_STACK_ELEMENT_DIE)

        return get_number_of_floorplan_elements_floorplan

            (this->Floorplan) ;

    else

        return 0u ;
}

/******************************************************************************/

FloorplanElement_t *get_floorplan_element_stack_element
(
    StackElement_t *this,
    String_t        floorplan_element_id
)
{
    if (this->Type != TDICE_STACK_ELEMENT_DIE)

        return NULL ;

    return get_floorplan_element_floorplan

        (this->Floorplan, floorplan_element_id) ;
}

/******************************************************************************/

Error_t insert_power_values_stack_element
(
    StackElement_t *this,
    PowersQueue_t  *pvalues
)
{
    if (this->Type == TDICE_STACK_ELEMENT_DIE)

        return insert_power_values_floorplan

            (this->Floorplan, pvalues) ;

    else

        return TDICE_SUCCESS ;
}

/******************************************************************************/
