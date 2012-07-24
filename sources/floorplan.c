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

#include "macros.h"
#include "floorplan.h"
#include "floorplan_file_parser.h"

/******************************************************************************/

void floorplan_init (Floorplan_t *floorplan)
{
    floorplan->FileName     = NULL ;
    floorplan->NElements    = (Quantity_t) 0u ;
    floorplan->Bpowers      = NULL ;

    floorplan_element_list_init (&floorplan->ElementsList) ;
    floorplan_matrix_init       (&floorplan->SurfaceCoefficients) ;
}

/******************************************************************************/

void floorplan_copy (Floorplan_t *dst, Floorplan_t *src)
{
    floorplan_destroy (dst) ;

    dst->FileName = (src->FileName == NULL) ? NULL : strdup (src->FileName) ;

    dst->NElements = src->NElements ;

    floorplan_element_list_copy (&dst->ElementsList, &src->ElementsList) ;

    floorplan_matrix_copy (&dst->SurfaceCoefficients, &src->SurfaceCoefficients) ;

    if (src->Bpowers == NULL)
    {
        dst->Bpowers = NULL ;

        return ;
    }

    dst->Bpowers = (Power_t *) malloc (sizeof (Power_t) * src->NElements) ;

    if (dst->Bpowers == NULL)
    {
        fprintf (stderr, "ERROR: malloc Bpowers in floorplan copy\n") ;

        return ;
    }

    memcpy (dst->Bpowers, src->Bpowers, sizeof (Power_t) * src->NElements) ;
}

/******************************************************************************/

void floorplan_destroy (Floorplan_t *floorplan)
{
    if (floorplan->FileName != NULL)

        free (floorplan->FileName) ;

    if (floorplan->Bpowers != NULL)

        free (floorplan->Bpowers) ;

    floorplan_element_list_destroy (&floorplan->ElementsList) ;
    floorplan_matrix_destroy       (&floorplan->SurfaceCoefficients) ;

    floorplan_init (floorplan) ;
}

/******************************************************************************/

Floorplan_t *floorplan_calloc (void)
{
    Floorplan_t *floorplan = (Floorplan_t *) malloc (sizeof(Floorplan_t));

    if (floorplan != NULL)

        floorplan_init (floorplan) ;

    return floorplan ;
}

/******************************************************************************/

Floorplan_t *floorplan_clone (Floorplan_t *floorplan)
{
    if (floorplan == NULL)

        return NULL ;

    Floorplan_t *newf = floorplan_calloc ( ) ;

    if (newf != NULL)

        floorplan_copy (newf, floorplan) ;

    return newf ;
}

/******************************************************************************/

void floorplan_free (Floorplan_t *floorplan)
{
    if (floorplan == NULL)

        return ;

    floorplan_destroy (floorplan) ;

    free (floorplan) ;
}

/******************************************************************************/

void floorplan_print (Floorplan_t *floorplan, FILE *stream, String_t prefix)
{
    FloorplanElementListNode_t *flpeln ;

    for (flpeln  = floorplan_element_list_begin (&floorplan->ElementsList) ;
         flpeln != NULL ;
         flpeln  = floorplan_element_list_next (flpeln))
    {
        FloorplanElement_t *flpel = floorplan_element_list_data (flpeln) ;

        floorplan_element_print (flpel, stream, prefix) ;

        fprintf (stream, "%s\n", prefix) ;
    }
}

/******************************************************************************/

Error_t fill_floorplan
(
    Floorplan_t  *floorplan,
    Dimensions_t *dimensions,
    String_t      filename
)
{
    Error_t result ;

    result = parse_floorplan_file (filename, floorplan, dimensions) ;

    if (result == TDICE_FAILURE)

        return TDICE_FAILURE ;

    floorplan->Bpowers =

        (Power_t *) malloc (sizeof (Power_t) * floorplan->NElements) ;

    if (floorplan->Bpowers == NULL)
    {
        fprintf (stderr, "Malloc Bpowers failed\n") ;

        return TDICE_FAILURE ;
    }

    CellIndex_t nnz = 0u ;

    FloorplanElementListNode_t *flpeln ;

    for (flpeln  = floorplan_element_list_begin (&floorplan->ElementsList) ;
         flpeln != NULL ;
         flpeln  = floorplan_element_list_next (flpeln))
    {
        FloorplanElement_t *flpel = floorplan_element_list_data (flpeln) ;

        ICElementListNode_t *iceln ;

        for (iceln  = ic_element_list_begin(&flpel->ICElements) ;
             iceln != NULL ;
             iceln  = ic_element_list_next (iceln))
        {
            ICElement_t *icel = ic_element_list_data(iceln) ;

            nnz +=    (icel->NE_Row    - icel->SW_Row    + 1)
                    * (icel->NE_Column - icel->SW_Column + 1) ;
        }
    }

    result = floorplan_matrix_build

        (&floorplan->SurfaceCoefficients, get_layer_area (dimensions),
         floorplan->NElements, nnz) ;

    if (result == TDICE_FAILURE)

        return TDICE_FAILURE ;

    floorplan_matrix_fill

        (&floorplan->SurfaceCoefficients, &floorplan->ElementsList, dimensions) ;

    return TDICE_SUCCESS ;
}

/******************************************************************************/

Quantity_t get_number_of_floorplan_elements_floorplan (Floorplan_t *floorplan)
{
    return floorplan->NElements ;
}

/******************************************************************************/

FloorplanElement_t *get_floorplan_element
(
    Floorplan_t *floorplan,
    String_t     floorplan_element_id
)
{
    FloorplanElement_t flpel ;

    floorplan_element_init (&flpel) ;

    flpel.Id = floorplan_element_id ;

    return floorplan_element_list_find (&floorplan->ElementsList, &flpel) ;
}

/******************************************************************************/

Error_t fill_sources_floorplan (Floorplan_t *floorplan, Source_t *sources)
{
    Quantity_t index = 0u ;

    FloorplanElementListNode_t *flpeln ;

    for (flpeln  = floorplan_element_list_begin (&floorplan->ElementsList) ;
            flpeln != NULL ;
            flpeln  = floorplan_element_list_next (flpeln))
    {
        FloorplanElement_t *flpel = floorplan_element_list_data (flpeln) ;

        if (is_empty_powers_queue (flpel->PowerValues) == true)

            return TDICE_FAILURE ;

        floorplan->Bpowers [ index++ ] = get_from_powers_queue (flpel->PowerValues) ;
    }

    // Does the mv multiplication to compute the source vector

    floorplan_matrix_multiply

        (&floorplan->SurfaceCoefficients, sources, floorplan->Bpowers) ;

    return TDICE_SUCCESS ;
}

/******************************************************************************/

Error_t insert_power_values_floorplan
(
    Floorplan_t   *floorplan,
    PowersQueue_t *pvalues
)
{
    Error_t result ;

    FloorplanElementListNode_t *flpeln ;

    for (flpeln  = floorplan_element_list_begin (&floorplan->ElementsList) ;
         flpeln != NULL ;
         flpeln  = floorplan_element_list_next (flpeln))
    {
        FloorplanElement_t *flpel = floorplan_element_list_data (flpeln) ;

        result = insert_power_values_floorplan_element (flpel, pvalues) ;

        if (result == TDICE_FAILURE)

            return TDICE_FAILURE ;
    }

    return TDICE_SUCCESS ;
}

/******************************************************************************/

Temperature_t *get_all_max_temperatures_floorplan
(
    Floorplan_t   *floorplan,
    Dimensions_t  *dimensions,
    Temperature_t *temperatures,
    Quantity_t    *n_floorplan_elements,
    Temperature_t *max_temperatures
)
{
    if (max_temperatures == NULL)
    {
        *n_floorplan_elements =

            get_number_of_floorplan_elements_floorplan (floorplan) ;

        max_temperatures =

            (Temperature_t *) malloc (sizeof (Temperature_t) * *n_floorplan_elements) ;
    }

    Temperature_t *tmp = max_temperatures ;

    FloorplanElementListNode_t *flpeln ;

    for (flpeln  = floorplan_element_list_begin (&floorplan->ElementsList) ;
         flpeln != NULL ;
         flpeln  = floorplan_element_list_next (flpeln))
    {
        FloorplanElement_t *flpel = floorplan_element_list_data (flpeln) ;

        *tmp++ = get_max_temperature_floorplan_element

                     (flpel, dimensions, temperatures) ;
    }

    return max_temperatures ;
}

/******************************************************************************/

Temperature_t *get_all_min_temperatures_floorplan
(
    Floorplan_t   *floorplan,
    Dimensions_t  *dimensions,
    Temperature_t *temperatures,
    Quantity_t    *n_floorplan_elements,
    Temperature_t *min_temperatures
)
{
    if (min_temperatures == NULL)
    {
        *n_floorplan_elements =

            get_number_of_floorplan_elements_floorplan (floorplan) ;

        min_temperatures =

            (Temperature_t *) malloc (sizeof (Temperature_t) * *n_floorplan_elements) ;
    }

    Temperature_t *tmp = min_temperatures ;

    FloorplanElementListNode_t *flpeln ;

    for (flpeln  = floorplan_element_list_begin (&floorplan->ElementsList) ;
         flpeln != NULL ;
         flpeln  = floorplan_element_list_next (flpeln))
    {
        FloorplanElement_t *flpel = floorplan_element_list_data (flpeln) ;

        *tmp++ = get_min_temperature_floorplan_element

                     (flpel, dimensions, temperatures) ;
    }

    return min_temperatures ;
}

/******************************************************************************/

Temperature_t *get_all_avg_temperatures_floorplan
(
    Floorplan_t   *floorplan,
    Dimensions_t  *dimensions,
    Temperature_t *temperatures,
    Quantity_t    *n_floorplan_elements,
    Temperature_t *avg_temperatures
)
{
    if (avg_temperatures == NULL)
    {
        *n_floorplan_elements =

            get_number_of_floorplan_elements_floorplan (floorplan) ;

        avg_temperatures =

            (Temperature_t *) malloc (sizeof (Temperature_t) * *n_floorplan_elements) ;
    }

    Temperature_t *tmp = avg_temperatures ;


    FloorplanElementListNode_t *flpeln ;

    for (flpeln  = floorplan_element_list_begin (&floorplan->ElementsList) ;
         flpeln != NULL ;
         flpeln  = floorplan_element_list_next (flpeln))
    {
        FloorplanElement_t *flpel = floorplan_element_list_data (flpeln) ;

        *tmp++ = get_avg_temperature_floorplan_element

                     (flpel, dimensions, temperatures) ;
    }

    return avg_temperatures ;
}

/******************************************************************************/
