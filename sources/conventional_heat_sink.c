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

#include "conventional_heat_sink.h"
#include "macros.h"

/******************************************************************************/

void init_conventional_heat_sink (ConventionalHeatSink *conventional_heat_sink)
{
    conventional_heat_sink->AmbientHTC         = 0.0 ;
    conventional_heat_sink->AmbientTemperature = 0.0 ;
    conventional_heat_sink->TopLayer           = NULL ;
}

/******************************************************************************/

ConventionalHeatSink *alloc_and_init_conventional_heat_sink (void)
{
    ConventionalHeatSink *conventional_heat_sink = (ConventionalHeatSink *)

        malloc (sizeof(ConventionalHeatSink)) ;

    if (conventional_heat_sink != NULL)

        init_conventional_heat_sink (conventional_heat_sink) ;

    return conventional_heat_sink ;
}

/******************************************************************************/

void free_conventional_heat_sink (ConventionalHeatSink *conventional_heat_sink)
{
    FREE_POINTER (free, conventional_heat_sink) ;
}

/******************************************************************************/

void print_formatted_conventional_heat_sink
(
  FILE                 *stream,
  char                 *prefix,
  ConventionalHeatSink *conventional_heat_sink
)
{
    fprintf (stream,
        "%sconventional heat sink :\n",
        prefix) ;

    fprintf (stream,
        "%s   heat transfer coefficient %.4e ;\n",
        prefix, conventional_heat_sink->AmbientHTC) ;

    fprintf (stream,
        "%s   ambient temperature       %.2f ;\n",
        prefix, conventional_heat_sink->AmbientTemperature) ;
}

/******************************************************************************/

void print_detailed_conventional_heat_sink
(
  FILE                 *stream,
  char                 *prefix,
  ConventionalHeatSink *conventional_heat_sink
)
{
    fprintf (stream,
        "%sconventional_heat_sink        = %p\n",
        prefix,   conventional_heat_sink) ;

    fprintf (stream,
        "%s  AmbientHTC                = %.4e\n",
        prefix,   conventional_heat_sink->AmbientHTC) ;

    fprintf (stream,
        "%s  AmbientTemperature        = %.2f\n",
        prefix,   conventional_heat_sink->AmbientTemperature) ;

    fprintf (stream,
        "%s  TopLayer                  = %p\n",
        prefix,   conventional_heat_sink->TopLayer) ;
}

/******************************************************************************/

void fill_thermal_cell_conventional_heat_sink
(
    ThermalCell          *thermal_cells,
    Dimensions           *dimensions,
    ConventionalHeatSink *conventional_heat_sink
)
{
    uint32_t layer_index = LAST_LAYER_INDEX (dimensions) ;

    uint32_t cell_index =

        get_cell_offset_in_stack (dimensions, layer_index, 0, 0) ;

    thermal_cells += cell_index ;

    FOR_EVERY_ROW (row_index, dimensions)
    {
        FOR_EVERY_COLUMN (column_index, dimensions)
        {

#ifdef PRINT_THERMAL_CELLS
            fprintf (stderr,
                "  l %2d r %4d c %4d [%7d] ",
                layer_index, row_index, column_index, cell_index++) ;
#endif

            fill_solid_cell_conventional_heat_sink
            (
                thermal_cells,

                get_cell_length (dimensions, column_index),
                get_cell_width  (dimensions, row_index),
                conventional_heat_sink->TopLayer->Height,

                conventional_heat_sink->TopLayer->Material->ThermalConductivity,
                conventional_heat_sink->AmbientHTC
            ) ;

            thermal_cells++ ;

        } // FOR_EVERY_COLUMN
    } // FOR_EVERY_ROW
}

/******************************************************************************/

void fill_sources_conventional_heat_sink
(
    double            *sources,
    ThermalCell          *thermal_cells,
    Dimensions           *dimensions,
    ConventionalHeatSink *conventional_heat_sink
)
{
    uint32_t layer_index = LAST_LAYER_INDEX (dimensions) ;

#ifdef PRINT_SOURCES
    fprintf (stderr,
        "layer_index = %d\tadd_sources_conventional_heat_sink\n",
        layer_index) ;
#endif

    uint32_t cell_index =

        get_cell_offset_in_stack (dimensions, layer_index, 0, 0) ;

    thermal_cells += cell_index ;
    sources       += cell_index ;

    FOR_EVERY_ROW (row_index, dimensions)
    {
        FOR_EVERY_COLUMN (column_index, dimensions)
        {
            *sources = conventional_heat_sink->AmbientTemperature
                       * thermal_cells->Top ;

#ifdef PRINT_SOURCES
            fprintf (stderr,
                "solid  cell  |  l %2d r %4d c %4d [%7d] | = %f * %.5e = %.5e\n",
                layer_index, row_index, column_index, cell_index++,
                conventional_heat_sink->AmbientTemperature,
                thermal_cells->Top, *sources) ;
#endif

            sources++ ;
            thermal_cells ++ ;

        } // FOR_EVERY_COLUMN
    } // FOR_EVERY_ROW
}

/******************************************************************************/

void fill_system_matrix_conventional_heat_sink
(
    SystemMatrix  system_matrix,
    Dimensions   *dimensions,
    ThermalCell  *thermal_cells
)
{
    uint32_t ncells = get_number_of_cells(dimensions) ;

    uint32_t cell_index = get_cell_offset_in_stack

        (dimensions, LAST_LAYER_INDEX(dimensions), 0 ,0) ;

    thermal_cells += cell_index ;

    while (cell_index < ncells)
    {
        uint32_t row_index ;

        for (row_index = system_matrix.ColumnPointers [ cell_index ] ;
             row_index < (uint32_t) system_matrix.ColumnPointers [ cell_index + 1 ] ;
             row_index ++)

            if ( (uint32_t) system_matrix.RowIndices [ row_index ] == cell_index )

                system_matrix.Values [ row_index ] += thermal_cells++->Top ;

        cell_index++ ;
    }
}

/******************************************************************************/
