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

void init_conventional_heat_sink (ConventionalHeatSink* conventionalheatsink)
{
  conventionalheatsink->AmbientHTC         = AMBIENTHTC_I ;
  conventionalheatsink->AmbientTemperature = TEMPERATURE_I ;
  conventionalheatsink->TopLayer           = NULL ;
  conventionalheatsink->IsSourceLayer      = FALSE_V ;
}

/******************************************************************************/

ConventionalHeatSink* alloc_and_init_conventional_heat_sink (void)
{
  ConventionalHeatSink* conventionalheatsink
    = malloc (sizeof(ConventionalHeatSink)) ;

  if (conventionalheatsink != NULL)

    init_conventional_heat_sink (conventionalheatsink) ;

  return conventionalheatsink ;
}

/******************************************************************************/

void free_conventional_heat_sink (ConventionalHeatSink* conventionalheatsink)
{
  FREE_POINTER (free, conventionalheatsink) ;
}

/******************************************************************************/

void print_formatted_conventional_heat_sink
(
  FILE*                 stream,
  String_t              prefix,
  ConventionalHeatSink* conventionalheatsink
)
{
  fprintf (stream,
           "%sconventional heat sink :\n",
           prefix) ;

  fprintf (stream,
           "%s   heat transfer coefficient %.4e ;\n",
           prefix, conventionalheatsink->AmbientHTC) ;

  fprintf (stream,
           "%s   ambient temperature       %.2f ;\n",
           prefix, conventionalheatsink->AmbientTemperature) ;
}

/******************************************************************************/

void print_detailed_conventional_heat_sink
(
  FILE*                 stream,
  String_t              prefix,
  ConventionalHeatSink* conventionalheatsink
)
{
  fprintf (stream,
           "%sconventionalheatsink        = %p\n",
           prefix,   conventionalheatsink) ;

  fprintf (stream,
           "%s  AmbientHTC                = %.4e\n",
           prefix,   conventionalheatsink->AmbientHTC) ;

  fprintf (stream,
           "%s  AmbientTemperature        = %.2f\n",
           prefix,   conventionalheatsink->AmbientTemperature) ;

  fprintf (stream,
           "%s  TopLayer                  = %p\n",
           prefix,   conventionalheatsink->TopLayer) ;

  fprintf (stream,
           "%s  IsSourceLayer             = %d\n",
           prefix,   conventionalheatsink->IsSourceLayer) ;
}

/******************************************************************************/

void fill_thermal_cell_conventional_heat_sink
(
    ThermalCell          *thermal_cells,
    Dimensions           *dimensions,
    ConventionalHeatSink *conventional_heat_sink
)
{
    GridDimension_t layer_index = LAST_LAYER_INDEX (dimensions) ;

    GridDimension_t cell_index =

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
    Source_t             *sources,
    ThermalCell          *thermal_cells,
    Dimensions           *dimensions,
    ConventionalHeatSink *conventional_heat_sink
)
{
    GridDimension_t layer_index = LAST_LAYER_INDEX (dimensions) ;

#ifdef PRINT_SOURCES
    fprintf (stderr,
        "layer_index = %d\tadd_sources_conventional_heat_sink\n",
        layer_index) ;
#endif

    GridDimension_t cell_index =

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
  SystemMatrix          system_matrix,
  Dimensions*           dimensions,
  ThermalCell*          thermalcells
)
{
  GridDimension_t cell_index = get_cell_offset_in_stack
                               (
                                 dimensions,
                                 LAST_LAYER_INDEX(dimensions), 0 ,0
                               ) ;

  thermalcells += cell_index ;

  while (cell_index < get_number_of_cells(dimensions))
  {
    GridDimension_t row_index ;

    for (row_index = system_matrix.ColumnPointers [ cell_index ] ;
         row_index < system_matrix.ColumnPointers [ cell_index + 1 ] ;
         row_index ++)

      if ( system_matrix.RowIndices [ row_index ] == cell_index )

        system_matrix.Values [ row_index ] += thermalcells++->Top ;

    cell_index++ ;
  }
}

/******************************************************************************/
