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

#include "floorplan.h"
#include "macros.h"

#include "../bison/floorplan_parser.h"
#include "../flex/floorplan_scanner.h"

// From Bison manual:
// The value returned by yyparse is 0 if parsing was successful (return is
// due to end-of-input). The value is 1 if parsing failed (return is due to
// a syntax error).

extern int floorplan_parse

    (Floorplan_t *this, Dimensions_t *dimensions, yyscan_t scanner) ;

/******************************************************************************/

void init_floorplan (Floorplan_t *this)
{
    this->FileName     = NULL ;
    this->NElements    = 0u ;
    this->ElementsList = NULL ;

    init_floorplan_matrix (&this->SurfaceCoefficients) ;
}

/******************************************************************************/

Floorplan_t *alloc_and_init_floorplan (void)
{
    Floorplan_t *floorplan = (Floorplan_t *) malloc (sizeof(Floorplan_t));

    if (floorplan != NULL)

        init_floorplan (floorplan) ;

    return floorplan ;
}

/******************************************************************************/

void free_floorplan (Floorplan_t *this)
{
    Destroy_SuperMatrix_Store (&this->SLUA) ;
    free_floorplan_matrix     (&this->SurfaceCoefficients) ;

    FREE_POINTER (free_floorplan_elements_list, this->ElementsList) ;
    FREE_POINTER (free,                         this->FileName) ;
    FREE_POINTER (free,                         this) ;
}

/******************************************************************************/

void print_detailed_floorplan
(
    FILE        *stream,
    String_t     prefix,
    Floorplan_t *this
)
{
    fprintf(stream,
        "%sFloorplan read from file %s\n", prefix, this->FileName) ;

    print_detailed_floorplan_elements_list

        (stream, prefix, this->ElementsList) ;
}

/******************************************************************************/

Error_t fill_floorplan
(
    Floorplan_t  *this,
    Dimensions_t *dimensions,
    String_t      file_name
)
{
    FILE *input ;
    int result ;
    yyscan_t scanner ;

    input = fopen (file_name, "r") ;

    if (input == NULL)
    {
        fprintf (stderr, "Unable to open floorplan file %s\n", file_name) ;

        return TDICE_FAILURE ;
    }

    this->FileName = strdup (file_name) ;  // FIXME memory leak

    floorplan_lex_init  (&scanner) ;
    floorplan_set_in    (input, scanner) ;
    //floorplan_set_debug (1, scanner) ;

    result = floorplan_parse (this, dimensions, scanner) ;

    floorplan_lex_destroy (scanner) ;
    fclose (input) ;

    if (result != 0)

        return TDICE_FAILURE ;

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

    Error_t error = alloc_floorplan_matrix

        (&this->SurfaceCoefficients, get_layer_area (dimensions),
         this->NElements, nnz) ;

    if (error == TDICE_FAILURE)

        return TDICE_FAILURE ;

    fill_floorplan_matrix

        (&this->SurfaceCoefficients, this->ElementsList, dimensions) ;

    dCreate_CompCol_Matrix
    (
        &this->SLUA,
        this->SurfaceCoefficients.NRows,
        this->SurfaceCoefficients.NColumns,
        this->SurfaceCoefficients.NNz,
        this->SurfaceCoefficients.Values,
        (int*) this->SurfaceCoefficients.RowIndices,
        (int*) this->SurfaceCoefficients.ColumnPointers,
        SLU_NC, SLU_D, SLU_GE
    ) ;

    return TDICE_SUCCESS ;
}

/******************************************************************************/

Quantity_t get_number_of_floorplan_elements_floorplan (Floorplan_t *this)
{
    return this->NElements ;
}

/******************************************************************************/

FloorplanElement_t *get_floorplan_element_floorplan
(
    Floorplan_t *this,
    String_t     floorplan_element_id
)
{
    return find_floorplan_element_in_list

        (this->ElementsList, floorplan_element_id) ;
}

/******************************************************************************/

Error_t fill_sources_floorplan
(
    Source_t     *sources,
    Floorplan_t  *this
)
{
    Quantity_t  index = 0u ;

    Power_t *powers = malloc (sizeof (Power_t) * this->NElements) ;

    FOR_EVERY_ELEMENT_IN_LIST_NEXT (FloorplanElement_t, flp_el, this->ElementsList)
    {
        if (is_empty_powers_queue (flp_el->PowerValues) == true)
        {
            FREE_POINTER (free, powers) ;

            return TDICE_FAILURE ;
        }

        powers [ index++ ] = get_from_powers_queue (flp_el->PowerValues) ;

        pop_from_powers_queue (flp_el->PowerValues) ;
    }

    // Does the mv multiplication to compute the source vector

    sp_dgemv("N", 1.0, &this->SLUA, powers, 1, 1.0, sources, 1) ;

    FREE_POINTER (free, powers) ;

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
