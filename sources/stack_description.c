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

#include "stack_description.h"
#include "macros.h"

/******************************************************************************/

void stack_description_init (StackDescription_t *stkd)
{
    stkd->FileName   = NULL ;
    stkd->HeatSink   = NULL ;
    stkd->Channel    = NULL ;
    stkd->Dimensions = NULL ;

         material_list_init (&stkd->Materials) ;
            layer_list_init (&stkd->Layers) ;
              die_list_init (&stkd->Dies) ;
    stack_element_list_init (&stkd->StackElements) ;
}

/******************************************************************************/

void stack_description_destroy (StackDescription_t *stkd)
{
    if (stkd->FileName != NULL)

        free (stkd->FileName) ;

    heat_sink_free  (stkd->HeatSink) ;
    channel_free    (stkd->Channel) ;
    dimensions_free (stkd->Dimensions) ;

         material_list_destroy (&stkd->Materials) ;
            layer_list_destroy (&stkd->Layers) ;
              die_list_destroy (&stkd->Dies) ;
    stack_element_list_destroy (&stkd->StackElements) ;

    stack_description_init (stkd) ;
}

/******************************************************************************/

void stack_description_print
(
  StackDescription_t *stkd,
  FILE               *stream,
  String_t            prefix
)
{
    material_list_print (&stkd->Materials, stream, prefix) ;

    if (stkd->HeatSink != NULL)
    {
        heat_sink_print (stkd->HeatSink, stream, prefix) ;

        fprintf (stream, "%s\n", prefix) ;
    }

    if (stkd->Channel != NULL)
    {
        channel_print (stkd->Channel, stream, prefix, stkd->Dimensions) ;

        fprintf (stream, "%s\n", prefix) ;
    }

    layer_list_print (&stkd->Layers, stream, prefix) ;

    fprintf (stream, "%s\n", prefix) ;

    die_list_print (&stkd->Dies, stream, prefix) ;

    fprintf (stream, "%s\n", prefix) ;

    dimensions_print (stkd->Dimensions, stream, prefix) ;

    fprintf (stream, "%s\n", prefix) ;

    fprintf (stream, "%sstack :\n", prefix) ;

    stack_element_list_print (&stkd->StackElements, stream, prefix) ;

    fprintf (stream, "%s\n", prefix) ;
}

/******************************************************************************/

Quantity_t get_number_of_floorplan_elements
(
  StackDescription_t *stkd,
  String_t            stack_element_id
)
{
    StackElement_t stkel ;

    stack_element_init (&stkel) ;

    stkel.Id = stack_element_id ;

    StackElement_t *tmp = stack_element_list_find (&stkd->StackElements, &stkel) ;

    if (tmp == NULL)

        return 0u ;

    return get_number_of_floorplan_elements_stack_element (tmp) ;
}

/******************************************************************************/

Quantity_t get_total_number_of_floorplan_elements (StackDescription_t *stkd)
{
    Quantity_t tmp = 0u ;

    StackElementListNode_t *stkeln ;

    for (stkeln  = stack_element_list_begin (&stkd->StackElements) ;
         stkeln != NULL ;
         stkeln  = stack_element_list_next (stkeln))
    {
        StackElement_t *stkel = stack_element_list_data (stkeln) ;

        tmp += get_number_of_floorplan_elements_stack_element (stkel) ;
    }

    return tmp ;
}

/******************************************************************************/
