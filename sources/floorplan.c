/******************************************************************************
 * This file is part of 3D-ICE, version 1.0.2 .                               *
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
 *          Martino Ruggiero                                                  *
 *          Thomas Brunschwiler                                               *
 *          David Atienza                                                     *
 *                                                                            *
 * For any comment, suggestion or request  about 3D-ICE, please  register and *
 * write to the mailing list (see http://listes.epfl.ch/doc.cgi?liste=3d-ice) *
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

    (Floorplan  *floorplan, Dimensions *dimensions, yyscan_t scanner) ;

/******************************************************************************/

void init_floorplan (Floorplan *floorplan)
{
    floorplan->FileName     = NULL ;
    floorplan->NElements    = 0u ;
    floorplan->ElementsList = NULL ;
}

/******************************************************************************/

Floorplan *alloc_and_init_floorplan (void)
{
    Floorplan *floorplan = (Floorplan *) malloc (sizeof(Floorplan));

    if (floorplan != NULL)

        init_floorplan (floorplan) ;

    return floorplan ;
}

/******************************************************************************/

void free_floorplan (Floorplan *floorplan)
{
    FREE_POINTER (free_floorplan_elements_list, floorplan->ElementsList) ;
    FREE_POINTER (free,                         floorplan->FileName) ;
    FREE_POINTER (free,                         floorplan) ;
}

/******************************************************************************/

void print_detailed_floorplan
(
    FILE      *stream,
    char      *prefix,
    Floorplan *floorplan
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
    Floorplan  *floorplan,
    Dimensions *dimensions,
    char       *file_name
)
{
    FILE *input ;
    int result ;
    yyscan_t scanner ;

    input = fopen (file_name, "r") ;

    if (input == NULL)
    {
        fprintf (stderr, "Unable to open floorplan file %s\n", file_name) ;

        return EXIT_FAILURE ;
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

Error_t fill_sources_floorplan
(
    double     *sources,
    Dimensions *dimensions,
    Floorplan  *floorplan
)
{
    FOR_EVERY_ELEMENT_IN_LIST_NEXT (FloorplanElement, floorplan_element, floorplan->ElementsList)

        if (fill_sources_floorplan_element (sources, dimensions, floorplan_element) == TDICE_FAILURE)

            return TDICE_FAILURE ;

    return TDICE_SUCCESS ;
}

/******************************************************************************/

void init_power_values_floorplan
(
    Floorplan *floorplan
)
{
    FOR_EVERY_ELEMENT_IN_LIST_NEXT

    (FloorplanElement, floorplan_element, floorplan->ElementsList)

        init_power_values_floorplan_element (floorplan_element) ;
}

/******************************************************************************/

void insert_power_values_floorplan
(
    Floorplan   *floorplan,
    PowersQueue *pvalues
)
{
    FOR_EVERY_ELEMENT_IN_LIST_NEXT

    (FloorplanElement, floorplan_element, floorplan->ElementsList)

        insert_power_values_floorplan_element (floorplan_element, pvalues) ;
}

/******************************************************************************/

int get_max_temperature_floorplan
(
    Floorplan  *floorplan,
    char       *floorplan_element_id,
    Dimensions *dimensions,
    double     *temperatures,
    double     *max_temperature
)
{
    FloorplanElement *flp_el = find_floorplan_element_in_list

        (floorplan->ElementsList, floorplan_element_id) ;

    if (flp_el == NULL)

        return -3 ;

    *max_temperature = get_max_temperature_floorplan_element

                           (flp_el, dimensions, temperatures) ;

    return 0 ;
}

/******************************************************************************/

int get_min_temperature_floorplan
(
    Floorplan  *floorplan,
    char       *floorplan_element_id,
    Dimensions *dimensions,
    double     *temperatures,
    double     *min_temperature
)
{
    FloorplanElement *flp_el = find_floorplan_element_in_list

        (floorplan->ElementsList, floorplan_element_id) ;

    if (flp_el == NULL)

        return -3 ;

    *min_temperature = get_min_temperature_floorplan_element

                           (flp_el, dimensions, temperatures) ;

    return 0 ;
}

/******************************************************************************/

int get_avg_temperature_floorplan
(
    Floorplan  *floorplan,
    char       *floorplan_element_id,
    Dimensions *dimensions,
    double     *temperatures,
    double     *avg_temperature
)
{
    FloorplanElement *flp_el = find_floorplan_element_in_list

        (floorplan->ElementsList, floorplan_element_id) ;

    if (flp_el == NULL)

        return -3 ;

    *avg_temperature = get_avg_temperature_floorplan_element

                           (flp_el, dimensions, temperatures) ;

    return 0 ;
}

/******************************************************************************/

int get_all_max_temperatures_floorplan
(
  Floorplan  *floorplan,
  Dimensions *dimensions,
  double     *temperatures,
  double     *max_temperature
)
{
    FOR_EVERY_ELEMENT_IN_LIST_NEXT

    (FloorplanElement, flp_el, floorplan->ElementsList)

        *max_temperature++ = get_max_temperature_floorplan_element

                                 (flp_el, dimensions, temperatures) ;

    return 0 ;
}

/******************************************************************************/

void print_all_max_temperatures_floorplan
(
    Floorplan  *floorplan,
    Dimensions *dimensions,
    double     *temperatures,
    FILE       *stream
)
{
    double temperature ;

    FOR_EVERY_ELEMENT_IN_LIST_NEXT

    (FloorplanElement, flp_el, floorplan->ElementsList)
    {
        temperature = get_max_temperature_floorplan_element

                          (flp_el, dimensions, temperatures) ;

        fprintf (stream, "%7.3f ", temperature) ;
    }
}

/******************************************************************************/

int get_all_min_temperatures_floorplan
(
    Floorplan  *floorplan,
    Dimensions *dimensions,
    double     *temperatures,
    double     *min_temperature
)
{
    FOR_EVERY_ELEMENT_IN_LIST_NEXT

    (FloorplanElement, flp_el, floorplan->ElementsList)

        *min_temperature++ = get_min_temperature_floorplan_element

                                 (flp_el, dimensions, temperatures) ;

    return 0 ;
}

/******************************************************************************/

void print_all_min_temperatures_floorplan
(
    Floorplan  *floorplan,
    Dimensions *dimensions,
    double     *temperatures,
    FILE       *stream
)
{
    double temperature ;

    FOR_EVERY_ELEMENT_IN_LIST_NEXT

    (FloorplanElement, flp_el, floorplan->ElementsList)
    {
        temperature = get_min_temperature_floorplan_element

                          (flp_el, dimensions, temperatures) ;

        fprintf (stream, "%7.3f ", temperature) ;
    }
}

/******************************************************************************/

int get_all_avg_temperatures_floorplan
(
    Floorplan  *floorplan,
    Dimensions *dimensions,
    double     *temperatures,
    double     *avg_temperature
)
{
    FOR_EVERY_ELEMENT_IN_LIST_NEXT

    (FloorplanElement, flp_el, floorplan->ElementsList)

        *avg_temperature++ = get_avg_temperature_floorplan_element

                                 (flp_el, dimensions, temperatures) ;

    return 0 ;
}

/******************************************************************************/

void print_all_avg_temperatures_floorplan
(
    Floorplan  *floorplan,
    Dimensions *dimensions,
    double     *temperatures,
    FILE       *stream
)
{
    double temperature ;

    FOR_EVERY_ELEMENT_IN_LIST_NEXT

    (FloorplanElement, flp_el, floorplan->ElementsList)
    {
        temperature = get_avg_temperature_floorplan_element

                          (flp_el, dimensions, temperatures) ;

        fprintf (stream, "%7.3f ", temperature) ;
    }
}

/******************************************************************************/
