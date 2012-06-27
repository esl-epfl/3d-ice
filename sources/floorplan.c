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

#include "macros.h"
#include "floorplan.h"
#include "floorplan_file_parser.h"

/******************************************************************************/

void init_floorplan (Floorplan_t *this)
{
    this->FileName     = NULL ;
    this->NElements    = 0u ;
    this->ElementsList = NULL ;
    this->Bpowers      = NULL ;

    init_floorplan_matrix (&this->SurfaceCoefficients) ;
}

/******************************************************************************/

Floorplan_t *calloc_floorplan (void)
{
    Floorplan_t *floorplan = (Floorplan_t *) malloc (sizeof(Floorplan_t));

    if (floorplan != NULL)

        init_floorplan (floorplan) ;

    return floorplan ;
}

/******************************************************************************/

void free_floorplan (Floorplan_t *this)
{
    destroy_floorplan_matrix (&this->SurfaceCoefficients) ;

    FREE_POINTER (free                        , this->Bpowers) ;
    FREE_POINTER (free_floorplan_elements_list, this->ElementsList) ;
    FREE_POINTER (free,                         this->FileName) ;
    FREE_POINTER (free,                         this) ;
}

/******************************************************************************/

Error_t fill_floorplan
(
    Floorplan_t  *this,
    Dimensions_t *dimensions,
    String_t      filename
)
{
    Error_t result ;

    result = parse_floorplan_file (filename, this, dimensions) ;

    if (result == TDICE_FAILURE)

        return TDICE_FAILURE ;

    this->Bpowers = malloc (sizeof (Power_t) * this->NElements) ;

    if (this->Bpowers == NULL)
    {
        fprintf (stderr, "Malloc Bpowers failed\n") ;

        return TDICE_FAILURE ;
    }

    CellIndex_t nnz = 0u ;

    FOR_EVERY_ELEMENT_IN_LIST_NEXT

    (FloorplanElement_t, flp_el, this->ElementsList)
    {
        FOR_EVERY_ELEMENT_IN_LIST_NEXT (ICElement_t, icel, flp_el->ICElementsList)
        {
            nnz +=    (icel->NE_Row    - icel->SW_Row    + 1)
                    * (icel->NE_Column - icel->SW_Column + 1) ;
        }
    }

    result = build_floorplan_matrix

        (&this->SurfaceCoefficients, get_layer_area (dimensions),
         this->NElements, nnz) ;

    if (result == TDICE_FAILURE)

        return TDICE_FAILURE ;

    fill_floorplan_matrix

        (&this->SurfaceCoefficients, this->ElementsList, dimensions) ;

    return TDICE_SUCCESS ;
}

/******************************************************************************/

Quantity_t get_number_of_floorplan_elements_floorplan (Floorplan_t *this)
{
    return this->NElements ;
}

/******************************************************************************/

FloorplanElement_t *get_floorplan_element
(
    Floorplan_t *this,
    String_t     floorplan_element_id
)
{
    return find_floorplan_element_in_list

        (this->ElementsList, floorplan_element_id) ;
}

/******************************************************************************/

Error_t fill_sources_floorplan (Floorplan_t *this, Source_t *sources)
{
    Quantity_t index = 0u ;

    FOR_EVERY_ELEMENT_IN_LIST_NEXT (FloorplanElement_t, flp_el, this->ElementsList)
    {
        if (is_empty_powers_queue (flp_el->PowerValues) == true)

            return TDICE_FAILURE ;

        this->Bpowers [ index++ ] = get_from_powers_queue (flp_el->PowerValues) ;
    }

    // Does the mv multiplication to compute the source vector

    multiply_floorplan_matrix

        (&this->SurfaceCoefficients, sources, this->Bpowers) ;

    return TDICE_SUCCESS ;
}

/******************************************************************************/

Error_t insert_power_values_floorplan
(
    Floorplan_t   *this,
    PowersQueue_t *pvalues
)
{
    Error_t result ;

    FOR_EVERY_ELEMENT_IN_LIST_NEXT

    (FloorplanElement_t, floorplan_element, this->ElementsList)
    {
        result = insert_power_values_floorplan_element

                    (floorplan_element, pvalues) ;

        if (result == TDICE_FAILURE)

            return TDICE_FAILURE ;
    }

    return TDICE_SUCCESS ;
}

/******************************************************************************/

Temperature_t *get_all_max_temperatures_floorplan
(
    Floorplan_t   *this,
    Dimensions_t  *dimensions,
    Temperature_t *temperatures,
    Quantity_t    *n_floorplan_elements,
    Temperature_t *max_temperatures
)
{
    if (max_temperatures == NULL)
    {
        *n_floorplan_elements =

            get_number_of_floorplan_elements_floorplan (this) ;

        max_temperatures =

            (Temperature_t *) malloc (sizeof (Temperature_t) * *n_floorplan_elements) ;
    }

    Temperature_t *tmp = max_temperatures ;

    FOR_EVERY_ELEMENT_IN_LIST_NEXT

    (FloorplanElement_t, flp_el, this->ElementsList)

        *tmp++ = get_max_temperature_floorplan_element

                     (flp_el, dimensions, temperatures) ;

    return max_temperatures ;
}

/******************************************************************************/

Temperature_t *get_all_min_temperatures_floorplan
(
    Floorplan_t   *this,
    Dimensions_t  *dimensions,
    Temperature_t *temperatures,
    Quantity_t    *n_floorplan_elements,
    Temperature_t *min_temperatures
)
{
    if (min_temperatures == NULL)
    {
        *n_floorplan_elements =

            get_number_of_floorplan_elements_floorplan (this) ;

        min_temperatures =

            (Temperature_t *) malloc (sizeof (Temperature_t) * *n_floorplan_elements) ;
    }

    Temperature_t *tmp = min_temperatures ;

    FOR_EVERY_ELEMENT_IN_LIST_NEXT

    (FloorplanElement_t, flp_el, this->ElementsList)

        *tmp++ = get_min_temperature_floorplan_element

                     (flp_el, dimensions, temperatures) ;

    return min_temperatures ;
}

/******************************************************************************/

Temperature_t *get_all_avg_temperatures_floorplan
(
    Floorplan_t   *this,
    Dimensions_t  *dimensions,
    Temperature_t *temperatures,
    Quantity_t    *n_floorplan_elements,
    Temperature_t *avg_temperatures
)
{
    if (avg_temperatures == NULL)
    {
        *n_floorplan_elements =

            get_number_of_floorplan_elements_floorplan (this) ;

        avg_temperatures =

            (Temperature_t *) malloc (sizeof (Temperature_t) * *n_floorplan_elements) ;
    }

    Temperature_t *tmp = avg_temperatures ;

    FOR_EVERY_ELEMENT_IN_LIST_NEXT

    (FloorplanElement_t, flp_el, this->ElementsList)

        *tmp++ = get_avg_temperature_floorplan_element

                     (flp_el, dimensions, temperatures) ;

    return avg_temperatures ;
}

/******************************************************************************/
