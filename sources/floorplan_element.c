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

#include <string.h>
#include <stdlib.h>

#include "floorplan_element.h"
#include "macros.h"

/******************************************************************************/

void init_floorplan_element (FloorplanElement *floorplan_element)
{
    floorplan_element->Id               = NULL ;
    floorplan_element->NICElements      = 0u ;
    floorplan_element->ICElementsList   = NULL ;
    floorplan_element->EffectiveSurface = 0u ;
    floorplan_element->PowerValues      = NULL ;
    floorplan_element->Next             = NULL ;
}

/******************************************************************************/

FloorplanElement *alloc_and_init_floorplan_element (void)
{
    FloorplanElement *floorplan_element = (FloorplanElement *)

        malloc (sizeof(FloorplanElement));

    if (floorplan_element != NULL)

        init_floorplan_element(floorplan_element) ;

    return floorplan_element ;
}

/*****************************************************************************/

void free_floorplan_element (FloorplanElement *floorplan_element)
{
    FREE_POINTER (free,                  floorplan_element->Id) ;
    FREE_POINTER (free_ic_elements_list, floorplan_element->ICElementsList) ;
    FREE_POINTER (free_powers_queue,     floorplan_element->PowerValues) ;
    FREE_POINTER (free,                  floorplan_element) ;
}

/******************************************************************************/

void free_floorplan_elements_list (FloorplanElement *list)
{
    FREE_LIST (FloorplanElement, list, free_floorplan_element) ;
}

/******************************************************************************/

void print_detailed_floorplan_element
(
    FILE             *stream,
    String_t          prefix,
    FloorplanElement *floorplan_element
)
{
    String_t new_prefix = (String_t)

        malloc (sizeof (*new_prefix) * (5 + strlen(prefix))) ;

    if (new_prefix == NULL) return ;

    sprintf (new_prefix, "%s    ", prefix) ;

    fprintf (stream,
        "%sFloorplan Element           = %p\n",
        prefix, floorplan_element) ;

    fprintf (stream,
        "%s    Id                      = %s\n",
        prefix, floorplan_element->Id) ;

    fprintf (stream,
        "%s    NICElements             = %d\n",
        prefix, floorplan_element->NICElements) ;

    fprintf (stream,
        "%s    ICElementsList          = %p\n",
        prefix, floorplan_element->ICElementsList) ;

    fprintf (stream, "%s\n", prefix) ;

    print_detailed_ic_elements_list

        (stream, new_prefix, floorplan_element->ICElementsList) ;

    fprintf (stream, "%s\n", prefix) ;

    fprintf (stream,
        "%s    Id                      = %.3f\n",
        prefix, floorplan_element->EffectiveSurface) ;

    fprintf (stream,
        "%s    PowerValues             = %p\n",
        prefix, floorplan_element->PowerValues) ;

    fprintf (stream, "%s\n", prefix) ;

    print_detailed_powers_queue

        (stream, new_prefix, floorplan_element->PowerValues) ;

    fprintf (stream, "%s\n", prefix) ;

    fprintf (stream,
        "%s    Next                    = %p\n",
        prefix, floorplan_element->Next) ;

    FREE_POINTER (free, new_prefix) ;
}

/******************************************************************************/

void print_detailed_floorplan_elements_list
(
    FILE             *stream,
    String_t          prefix,
    FloorplanElement *list
)
{
    FOR_EVERY_ELEMENT_IN_LIST_NEXT (FloorplanElement, flp_el, list)
    {
        if (flp_el->Next == NULL)

            break ;

        print_detailed_floorplan_element (stream, prefix, flp_el) ;

        fprintf (stream, "%s\n", prefix) ;
    }

    print_detailed_floorplan_element (stream, prefix, flp_el) ;
}

/******************************************************************************/

void print_formatted_floorplan_element
(
    FILE             *stream,
    String_t          prefix,
    FloorplanElement *floorplan_element
)
{
    fprintf (stream,
        "%s%s:\n",
        prefix, floorplan_element->Id) ;

    print_formatted_ic_elements_list

        (stream, prefix, floorplan_element->ICElementsList) ;

    fprintf (stream,
        "%s   power values ",
        prefix) ;

    print_formatted_powers_queue (stream, floorplan_element->PowerValues) ;

    fprintf (stream, "\n") ;
}

/******************************************************************************/

void print_formatted_floorplan_elements_list
(
    FILE             *stream,
    String_t          prefix,
    FloorplanElement *list
)
{
    FOR_EVERY_ELEMENT_IN_LIST_NEXT (FloorplanElement, flp_el, list)
    {
        if (flp_el->Next == NULL)

            break ;

        print_formatted_floorplan_element (stream, prefix, flp_el) ;

        fprintf (stream, "%s\n", prefix) ;
    }

    print_formatted_floorplan_element (stream, prefix, flp_el) ;
}

/******************************************************************************/

Error_t fill_sources_floorplan_element
(
    Source_t         *sources,
    Dimensions       *dimensions,
    FloorplanElement *floorplan_element
)
{
    if (is_empty_powers_queue (floorplan_element->PowerValues) == true)

        return TDICE_FAILURE ;

    FOR_EVERY_ELEMENT_IN_LIST_NEXT (ICElement, icelement, floorplan_element->ICElementsList)

        fill_sources_ic_element

            (sources, dimensions,
             get_from_powers_queue (floorplan_element->PowerValues),
             floorplan_element->EffectiveSurface,
             icelement) ;

    pop_from_powers_queue (floorplan_element->PowerValues) ;

    return TDICE_SUCCESS ;
}

/******************************************************************************/

Error_t insert_power_values_floorplan_element
(
    FloorplanElement *floorplan_element,
    PowersQueue      *pvalues
)
{
    if (is_empty_powers_queue(pvalues) == true)

        return TDICE_FAILURE ;

    Power_t power = get_from_powers_queue (pvalues) ;

    put_into_powers_queue (floorplan_element->PowerValues, power);

    pop_from_powers_queue (pvalues) ;

    return TDICE_SUCCESS ;
}

/******************************************************************************/

FloorplanElement *find_floorplan_element_in_list
(
    FloorplanElement *list,
    String_t          id
)
{
    FOR_EVERY_ELEMENT_IN_LIST_NEXT (FloorplanElement, flp_el, list)
    {
        if (strcmp (flp_el->Id, id) == 0) break ;
    }
    return flp_el ;
}

/******************************************************************************/

Temperature_t get_max_temperature_floorplan_element
(
    FloorplanElement *floorplan_element,
    Dimensions       *dimensions,
    Temperature_t    *temperatures
)
{
    Temperature_t tmp, max = 0.0 ;

    FOR_EVERY_ELEMENT_IN_LIST_NEXT (ICElement, icelement, floorplan_element->ICElementsList)
    {
        tmp = get_max_temperature_ic_element (icelement, dimensions, temperatures) ;

        if (tmp > max) max = tmp ;
    }

    return max ;
}

/******************************************************************************/

Temperature_t get_min_temperature_floorplan_element
(
    FloorplanElement *floorplan_element,
    Dimensions       *dimensions,
    Temperature_t    *temperatures
)
{
    Temperature_t tmp, min ;

    min = get_min_temperature_ic_element

        (floorplan_element->ICElementsList, dimensions, temperatures) ;

    // FIXME : skip the first here ...

    FOR_EVERY_ELEMENT_IN_LIST_NEXT (ICElement, icelement, floorplan_element->ICElementsList)
    {
        tmp = get_max_temperature_ic_element (icelement, dimensions, temperatures) ;

        if (tmp < min) min = tmp ;
    }

    return min ;
}

/******************************************************************************/

Temperature_t get_avg_temperature_floorplan_element
(
    FloorplanElement *floorplan_element,
    Dimensions       *dimensions,
    Temperature_t    *temperatures
)
{
    Temperature_t avg = 0.0 ;

    FOR_EVERY_ELEMENT_IN_LIST_NEXT (ICElement, icelement, floorplan_element->ICElementsList)

        avg += get_avg_temperature_ic_element (icelement, dimensions, temperatures) ;

    return avg / (Temperature_t) floorplan_element->NICElements ;
}

/******************************************************************************/
