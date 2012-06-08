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

#include "slu_ddefs.h"

// From Bison manual:
// The value returned by yyparse is 0 if parsing was successful (return is
// due to end-of-input). The value is 1 if parsing failed (return is due to
// a syntax error).

extern int floorplan_parse

    (Floorplan_t *floorplan, Dimensions_t *dimensions, yyscan_t scanner) ;

/******************************************************************************/

void init_floorplan (Floorplan_t *floorplan)
{
    floorplan->FileName     = NULL ;
    floorplan->NElements    = 0u ;
    floorplan->ElementsList = NULL ;

    init_floorplan_matrix (&floorplan->SurfaceCoefficients) ;
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

void free_floorplan (Floorplan_t *floorplan)
{
    free_floorplan_matrix (&floorplan->SurfaceCoefficients) ;

    FREE_POINTER (free_floorplan_elements_list, floorplan->ElementsList) ;
    FREE_POINTER (free,                         floorplan->FileName) ;
    FREE_POINTER (free,                         floorplan) ;
}

/******************************************************************************/

void print_detailed_floorplan
(
    FILE        *stream,
    String_t     prefix,
    Floorplan_t *floorplan
)
{
    fprintf(stream,
        "%sFloorplan read from file %s\n", prefix, floorplan->FileName) ;

    print_detailed_floorplan_elements_list

        (stream, prefix, floorplan->ElementsList) ;
}

/******************************************************************************/

Error_t fill_floorplan
(
    Floorplan_t  *floorplan,
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

    floorplan->FileName = strdup (file_name) ;  // FIXME memory leak

    floorplan_lex_init  (&scanner) ;
    floorplan_set_in    (input, scanner) ;
    //floorplan_set_debug (1, scanner) ;

    result = floorplan_parse (floorplan, dimensions, scanner) ;

    floorplan_lex_destroy (scanner) ;
    fclose (input) ;

    if (result == 0)

        return TDICE_SUCCESS ;

    else

        return TDICE_FAILURE ;
}

/******************************************************************************/

Quantity_t get_number_of_floorplan_elements_floorplan
(
    Floorplan_t *floorplan
)
{
    return floorplan->NElements ;
}

/******************************************************************************/

FloorplanElement_t *get_floorplan_element_floorplan
(
    Floorplan_t *floorplan,
    String_t     floorplan_element_id
)
{
    return find_floorplan_element_in_list

        (floorplan->ElementsList, floorplan_element_id) ;
}

/******************************************************************************/

Error_t fill_sources_floorplan
(
    Source_t     *sources,
    Floorplan_t  *floorplan
)
{
    Quantity_t  index = 0u ;

    Power_t *powers = malloc (sizeof (Power_t) * floorplan->NElements) ;

    FOR_EVERY_ELEMENT_IN_LIST_NEXT (FloorplanElement_t, flp_el, floorplan->ElementsList)
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

    SuperMatrix SLUA ;

    dCreate_CompCol_Matrix
    (
        &SLUA,
        floorplan->SurfaceCoefficients.NRows,
        floorplan->SurfaceCoefficients.NColumns,
        floorplan->SurfaceCoefficients.NNz,
        floorplan->SurfaceCoefficients.Values,
        (int*) floorplan->SurfaceCoefficients.RowIndices,
        (int*) floorplan->SurfaceCoefficients.ColumnPointers,
        SLU_NC, SLU_D, SLU_GE
    ) ;

    sp_dgemv("N", 1.0, &SLUA, powers, 1, 1.0, sources, 1) ;

    Destroy_SuperMatrix_Store (&SLUA) ;

    FREE_POINTER (free, powers) ;

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

    FOR_EVERY_ELEMENT_IN_LIST_NEXT

    (FloorplanElement_t, floorplan_element, floorplan->ElementsList)
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

    FOR_EVERY_ELEMENT_IN_LIST_NEXT

    (FloorplanElement_t, flp_el, floorplan->ElementsList)

        *tmp++ = get_max_temperature_floorplan_element

                     (flp_el, dimensions, temperatures) ;

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

    FOR_EVERY_ELEMENT_IN_LIST_NEXT

    (FloorplanElement_t, flp_el, floorplan->ElementsList)

        *tmp++ = get_min_temperature_floorplan_element

                     (flp_el, dimensions, temperatures) ;

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

    FOR_EVERY_ELEMENT_IN_LIST_NEXT

    (FloorplanElement_t, flp_el, floorplan->ElementsList)

        *tmp++ = get_avg_temperature_floorplan_element

                     (flp_el, dimensions, temperatures) ;

    return avg_temperatures ;
}

/******************************************************************************/
