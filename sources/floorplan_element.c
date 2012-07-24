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

#include <string.h>
#include <stdlib.h>

#include "floorplan_element.h"
#include "macros.h"

/******************************************************************************/

void floorplan_element_init (FloorplanElement_t *flpel)
{
    flpel->Id             = NULL ;
    flpel->NICElements    = (Quantity_t) 0u ;

    ic_element_list_init (&flpel->ICElements) ;

    flpel->Area           = (ChipDimension_t) 0.0 ;
    flpel->PowerValues    = NULL ;
}

/******************************************************************************/

void floorplan_element_copy
(
    FloorplanElement_t *dst,
    FloorplanElement_t *src
)
{
    floorplan_element_destroy (dst) ;

    dst->Id = (src->Id == NULL) ? NULL : strdup (src->Id) ;

    dst->NICElements = src->NICElements ;
    dst->Area        = src->Area ;

    ic_element_list_copy (&dst->ICElements, &src->ICElements) ;

    dst->PowerValues = powers_queue_clone (src->PowerValues) ;
}

/******************************************************************************/

void floorplan_element_destroy (FloorplanElement_t *flpel)
{
    if (flpel->Id != NULL)

        free (flpel->Id) ;

    ic_element_list_destroy (&flpel->ICElements) ;

    powers_queue_free (flpel->PowerValues) ;

    floorplan_element_init (flpel) ;
}

/******************************************************************************/

FloorplanElement_t *floorplan_element_calloc (void)
{
    FloorplanElement_t *flpel =

        (FloorplanElement_t *) malloc (sizeof(FloorplanElement_t));

    if (flpel != NULL)

        floorplan_element_init (flpel) ;

    return flpel ;
}

/*****************************************************************************/

FloorplanElement_t *floorplan_element_clone (FloorplanElement_t *flpel)
{
    if (flpel == NULL)

        return NULL ;

    FloorplanElement_t *newf = floorplan_element_calloc ( ) ;

    if (newf != NULL)

        floorplan_element_copy (newf, flpel) ;

    return newf ;
}

/*****************************************************************************/

void floorplan_element_free (FloorplanElement_t *flpel)
{
    if (flpel == NULL)

        return ;

    floorplan_element_destroy (flpel) ;

    free (flpel) ;
}

/******************************************************************************/

bool floorplan_element_same_id
(
    FloorplanElement_t *flpel,
    FloorplanElement_t *other
)
{
    return strcmp (flpel->Id, other->Id) == 0 ? true : false ;
}

/******************************************************************************/

void floorplan_element_print
(
    FloorplanElement_t *flpel,
    FILE               *stream,
    String_t            prefix
)
{
    fprintf (stream,
        "%s%s:\n",
        prefix, flpel->Id) ;

    ic_element_list_print

        (&flpel->ICElements, stream, prefix) ;

    fprintf (stream,
        "%s   power values ",
        prefix) ;

    powers_queue_print (flpel->PowerValues, stream, (String_t)"") ;

    fprintf (stream, "\n") ;
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

Temperature_t get_max_temperature_floorplan_element
(
    FloorplanElement_t *flpel,
    Dimensions_t       *dimensions,
    Temperature_t      *temperatures
)
{
    Temperature_t tmp, max = 0.0 ;

    ICElement_t         *icel ;
    ICElementListNode_t *iceln = ic_element_list_begin (&flpel->ICElements) ;

    if (iceln != NULL)
    {
        icel = ic_element_list_data (iceln) ;

        max = get_max_temperature_ic_element (icel, dimensions, temperatures) ;
    }
    else

        return max ;

    for (iceln  = ic_element_list_next (iceln) ;
         iceln != NULL ;
         iceln = ic_element_list_next (iceln))
    {
        icel = ic_element_list_data (iceln) ;

        tmp = get_max_temperature_ic_element (icel, dimensions, temperatures) ;

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
    Temperature_t tmp, min = 0.0 ;

    ICElement_t         *icel ;
    ICElementListNode_t *iceln = ic_element_list_begin (&flpel->ICElements) ;

    if (iceln != NULL)
    {
        icel = ic_element_list_data (iceln) ;

        min = get_min_temperature_ic_element (icel, dimensions, temperatures) ;
    }
    else

        return min ;

    for (iceln  = ic_element_list_next (iceln) ;
         iceln != NULL ;
         iceln = ic_element_list_next (iceln))
    {
        icel = ic_element_list_data (iceln) ;

        tmp = get_min_temperature_ic_element (icel, dimensions, temperatures) ;

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

    ICElement_t         *icel ;
    ICElementListNode_t *iceln ;

    for (iceln  = ic_element_list_begin (&flpel->ICElements) ;
         iceln != NULL ;
         iceln  = ic_element_list_next (iceln))
    {
        icel = ic_element_list_data (iceln) ;

        avg += get_avg_temperature_ic_element (icel, dimensions, temperatures) ;
    }

    return avg / (Temperature_t) flpel->NICElements ;
}

/******************************************************************************/
