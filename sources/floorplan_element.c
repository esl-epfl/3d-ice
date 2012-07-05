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

void init_floorplan_element (FloorplanElement_t *flpel)
{
    flpel->Id             = NULL ;
    flpel->NICElements    = 0u ;
    flpel->ICElementsList = NULL ;
    flpel->Area           = 0u ;
    flpel->PowerValues    = NULL ;
    flpel->Next           = NULL ;
}

/******************************************************************************/

FloorplanElement_t *calloc_floorplan_element (void)
{
    FloorplanElement_t *floorplan_element = (FloorplanElement_t *)

        malloc (sizeof(FloorplanElement_t));

    if (floorplan_element != NULL)

        init_floorplan_element(floorplan_element) ;

    return floorplan_element ;
}

/*****************************************************************************/

void free_floorplan_element (FloorplanElement_t *flpel)
{
    FREE_POINTER (free,                  flpel->Id) ;
    FREE_POINTER (free_ic_elements_list, flpel->ICElementsList) ;
    FREE_POINTER (free_powers_queue,     flpel->PowerValues) ;
    FREE_POINTER (free,                  flpel) ;
}

/******************************************************************************/

void free_floorplan_elements_list (FloorplanElement_t *list)
{
    FREE_LIST (FloorplanElement_t, list, free_floorplan_element) ;
}

/******************************************************************************/

void print_floorplan_element
(
    FloorplanElement_t *flpel,
    FILE               *stream,
    String_t            prefix
)
{
    fprintf (stream,
        "%s%s:\n",
        prefix, flpel->Id) ;

    print_ic_elements_list (flpel->ICElementsList, stream, prefix) ;

    fprintf (stream,
        "%s   power values ",
        prefix) ;

    print_powers_queue (flpel->PowerValues, stream, (String_t)"") ;

    fprintf (stream, "\n") ;
}

/******************************************************************************/

void print_floorplan_elements_list
(
    FloorplanElement_t *list,
    FILE               *stream,
    String_t            prefix
)
{
    FOR_EVERY_ELEMENT_IN_LIST_NEXT (FloorplanElement_t, flp_el, list)
    {
        if (flp_el->Next == NULL)

            break ;

        print_floorplan_element (flp_el, stream, prefix) ;

        fprintf (stream, "%s\n", prefix) ;
    }

    print_floorplan_element (flp_el, stream, prefix) ;
}

/******************************************************************************/

Error_t insert_power_values_floorplan_element
(
    FloorplanElement_t *flpel,
    PowersQueue_t      *pvalues
)
{
    if (is_empty_powers_queue(pvalues) == true)

        return TDICE_FAILURE ;

    put_into_powers_queue (flpel->PowerValues, get_from_powers_queue (pvalues));

    return TDICE_SUCCESS ;
}

/******************************************************************************/

FloorplanElement_t *find_floorplan_element_in_list
(
    FloorplanElement_t *list,
    String_t            id
)
{
    FOR_EVERY_ELEMENT_IN_LIST_NEXT (FloorplanElement_t, flp_el, list)
    {
        if (strcmp (flp_el->Id, id) == 0) break ;
    }
    return flp_el ;
}

/******************************************************************************/

Temperature_t get_max_temperature_floorplan_element
(
    FloorplanElement_t *flpel,
    Dimensions_t       *dimensions,
    Temperature_t      *temperatures
)
{
    Temperature_t tmp, max = 0.0 ;

    FOR_EVERY_ELEMENT_IN_LIST_NEXT (ICElement_t, icelement, flpel->ICElementsList)
    {
        tmp = get_max_temperature_ic_element (icelement, dimensions, temperatures) ;

        if (tmp > max) max = tmp ;
    }

    return max ;
}

/******************************************************************************/

Temperature_t get_min_temperature_floorplan_element
(
    FloorplanElement_t *flpel,
    Dimensions_t       *dimensions,
    Temperature_t      *temperatures
)
{
    Temperature_t tmp, min ;

    min = get_min_temperature_ic_element

        (flpel->ICElementsList, dimensions, temperatures) ;

    // FIXME : skip the first here ...

    FOR_EVERY_ELEMENT_IN_LIST_NEXT (ICElement_t, icelement, flpel->ICElementsList)
    {
        tmp = get_max_temperature_ic_element (icelement, dimensions, temperatures) ;

        if (tmp < min) min = tmp ;
    }

    return min ;
}

/******************************************************************************/

Temperature_t get_avg_temperature_floorplan_element
(
    FloorplanElement_t *flpel,
    Dimensions_t       *dimensions,
    Temperature_t      *temperatures
)
{
    Temperature_t avg = 0.0 ;

    FOR_EVERY_ELEMENT_IN_LIST_NEXT (ICElement_t, icelement, flpel->ICElementsList)

        avg += get_avg_temperature_ic_element (icelement, dimensions, temperatures) ;

    return avg / (Temperature_t) flpel->NICElements ;
}

/******************************************************************************/
