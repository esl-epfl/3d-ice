/******************************************************************************
 * This file is part of 3D-ICE, version 2.2 .                                 *
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

void stack_element_init (StackElement_t *stkel)
{
    stkel->Type             = (StackElementType_t) TDICE_STACK_ELEMENT_NONE ;
    stkel->Pointer.Layer    = NULL ;
    stkel->Pointer.Die      = NULL ;
    stkel->Pointer.Channel  = NULL ;
    stkel->Pointer.HeatSink = NULL ;
    stkel->Id               = NULL ;
    stkel->NLayers          = (CellIndex_t) 0u ;
    stkel->Offset           = (CellIndex_t) 0u ;
}

/******************************************************************************/

void stack_element_copy (StackElement_t *dst, StackElement_t *src)
{
    stack_element_destroy (dst) ;

    dst->Type     = src->Type ;
    dst->NLayers  = src->NLayers ;
    dst->Offset   = src->Offset ;

    dst->Id = src->Id == NULL ? NULL : strdup (src->Id) ;

    if (src->Type == TDICE_STACK_ELEMENT_LAYER)

        dst->Pointer.Layer = layer_clone (src->Pointer.Layer) ;

    else if (src->Type == TDICE_STACK_ELEMENT_DIE)

        dst->Pointer.Die = die_clone (src->Pointer.Die) ;

    else

        // Channel or HeatSink

        dst->Pointer = src->Pointer ;
}

/******************************************************************************/

void stack_element_destroy (StackElement_t *stkel)
{
    if (stkel->Id != NULL)

        free (stkel->Id) ;

    if (stkel->Type == TDICE_STACK_ELEMENT_DIE)

        die_free (stkel->Pointer.Die) ;

    else if (stkel->Type == TDICE_STACK_ELEMENT_LAYER)

        layer_free (stkel->Pointer.Layer) ;

    stack_element_init (stkel) ;
}

/******************************************************************************/

StackElement_t *stack_element_calloc (void)
{
    StackElement_t *stkel = (StackElement_t *) malloc (sizeof(StackElement_t)) ;

    if (stkel != NULL)

        stack_element_init (stkel) ;

    return stkel ;
}

/******************************************************************************/

StackElement_t *stack_element_clone (StackElement_t *stkel)
{
    if (stkel == NULL)

        return NULL ;

    StackElement_t *news = stack_element_calloc ( ) ;

    if (news != NULL)

        stack_element_copy (news, stkel) ;

    return news ;
}

/******************************************************************************/

void stack_element_free (StackElement_t *stkel)
{
    if (stkel == NULL)

        return ;

    stack_element_destroy (stkel) ;

    free (stkel) ;
}

/******************************************************************************/

bool stack_element_same_id (StackElement_t *stkel, StackElement_t *other)
{
    return strcmp (stkel->Id, other->Id) == 0 ? true : false ;
}

/******************************************************************************/

void stack_element_print
(
    StackElement_t *stkel,
    FILE           *stream,
    String_t        prefix
)
{
    switch (stkel->Type)
    {
        case TDICE_STACK_ELEMENT_CHANNEL :

            fprintf (stream,
                "%s   channel  %s ;\n",
                prefix, stkel->Id) ;
            break ;

        case TDICE_STACK_ELEMENT_DIE :

            fprintf (stream,
                "%s   die      %s %s floorplan \"%s\" ;\n",
                prefix,
                stkel->Id,
                stkel->Pointer.Die->Id,
                stkel->Pointer.Die->Floorplan.FileName) ;

            break ;

        case TDICE_STACK_ELEMENT_LAYER :

            fprintf (stream,
                "%s   layer    %s %s ;\n",
                prefix,
                stkel->Id, stkel->Pointer.Layer->Id) ;

            break ;

        case TDICE_STACK_ELEMENT_HEATSINK :

            break ;

        case TDICE_STACK_ELEMENT_NONE :

            fprintf (stderr, "Warning: found stack element type none\n") ;
            break ;

        default :

            fprintf (stderr, "Undefined stack element type %d\n", stkel->Type) ;
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

void stack_element_print_thermal_map
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

void stack_element_print_power_map
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

            (&stkel->Pointer.Die->Floorplan) ;

    else

        return 0u ;
}

/******************************************************************************/
