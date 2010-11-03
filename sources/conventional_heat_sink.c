/******************************************************************************
 * This file is part of 3D-ICE, version 1.0.1 .                               *
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
  free (conventionalheatsink) ;
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
           STRING_F "conventional heat sink :\n",
           prefix) ;

  fprintf (stream,
           STRING_F "   heat transfer coefficient " AMBIENTHTC_F " ;\n",
           prefix, conventionalheatsink->AmbientHTC) ;

  fprintf (stream,
           STRING_F "   ambient temperature       " TEMPERATURE_F " ;\n",
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
           STRING_F "conventionalheatsink        = %p\n",
           prefix,   conventionalheatsink) ;

  fprintf (stream,
           STRING_F "  AmbientHTC                = " AMBIENTHTC_F "\n",
           prefix,   conventionalheatsink->AmbientHTC) ;

  fprintf (stream,
           STRING_F "  AmbientTemperature        = " TEMPERATURE_F "\n",
           prefix,   conventionalheatsink->AmbientTemperature) ;
}

/******************************************************************************/

void fill_sources_conventional_heat_sink
(
  Source_t*             sources,
  Dimensions*           dimensions,
  ThermalGridData*      thermalgriddata,
  ConventionalHeatSink* conventionalheatsink
)
{
#ifdef PRINT_SOURCES
  fprintf (stderr,
    "layer_index = %d\tadd_sources_conventional_heat_sink\n",
    LAST_LAYER_INDEX(dimensions)) ;
#endif

  sources += get_cell_offset_in_stack (dimensions,
                                       LAST_LAYER_INDEX(dimensions), 0, 0) ;

  FOR_EVERY_ROW (row_index, dimensions)
  {
    FOR_EVERY_COLUMN (column_index, dimensions)
    {
      *sources++ = (conventionalheatsink->AmbientTemperature
                    * get_conductance (thermalgriddata, dimensions,
                                       LAST_LAYER_INDEX(dimensions), row_index, column_index,
                                       TDICE_CONDUCTANCE_TOP)) ;
#ifdef PRINT_SOURCES
        fprintf (stderr,
          "solid  cell  |  l %2d r %4d c %4d [%6d] | = %f * %.5e = %.5e\n",
          LAST_LAYER_INDEX(dimensions), row_index, column_index,
          get_cell_offset_in_stack (dimensions,
                                    LAST_LAYER_INDEX(dimensions), row_index, column_index),
          conventionalheatsink->AmbientTemperature,
          get_conductance (thermalgriddata, dimensions,
                           LAST_LAYER_INDEX(dimensions), row_index, column_index,
                           TDICE_CONDUCTANCE_TOP),
          *(sources-1)) ;
#endif
    } // FOR_EVERY_COLUMN
  } // FOR_EVERY_ROW

}

/******************************************************************************/

void add_sources_conventional_heat_sink
(
  Source_t*             sources,
  Dimensions*           dimensions,
  ThermalGridData*      thermalgriddata,
  ConventionalHeatSink* conventionalheatsink
)
{
#ifdef PRINT_SOURCES
  fprintf (stderr,
    "layer_index = %d\tadd_sources_conventional_heat_sink\n", LAST_LAYER_INDEX(dimensions)) ;
#endif

  sources += get_cell_offset_in_stack (dimensions,
                                       LAST_LAYER_INDEX(dimensions), 0, 0) ;

  FOR_EVERY_ROW (row_index, dimensions)
  {
    FOR_EVERY_COLUMN (column_index, dimensions)
    {
      *sources++ += (conventionalheatsink->AmbientTemperature
                     * get_conductance (thermalgriddata, dimensions,
                                        LAST_LAYER_INDEX(dimensions), row_index, column_index,
                                        TDICE_CONDUCTANCE_TOP)) ;
#ifdef PRINT_SOURCES
        fprintf (stderr,
          "solid  cell  |  l %2d r %4d c %4d [%6d] | += %f * %.5e = %.5e\n",
          LAST_LAYER_INDEX(dimensions), row_index, column_index,
          get_cell_offset_in_stack (dimensions,
                                    LAST_LAYER_INDEX(dimensions), row_index, column_index),
          conventionalheatsink->AmbientTemperature,
          get_conductance (thermalgriddata, dimensions,
                           LAST_LAYER_INDEX(dimensions), row_index, column_index,
                           TDICE_CONDUCTANCE_TOP),
          *(sources-1)) ;
#endif
    } // FOR_EVERY_COLUMN
  } // FOR_EVERY_ROW

}

/******************************************************************************/
