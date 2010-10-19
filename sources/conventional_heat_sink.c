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

    = (ConventionalHeatSink*) malloc ( sizeof(ConventionalHeatSink) ) ;

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

void print_conventional_heat_sink
(
  FILE*                 stream,
  String_t              prefix,
  ConventionalHeatSink* conventionalheatsink
)
{
  fprintf(stream, "%sConventional Heat Sink\n", prefix) ;
  fprintf(stream, "%s  Heat Transfer coefficent %.5e\n",
                  prefix, conventionalheatsink->AmbientHTC) ;
  fprintf(stream, "%s  Ambient temperature      %.4e\n",
                  prefix, conventionalheatsink->AmbientTemperature) ;
}

/******************************************************************************/

void fill_sources_conventional_heat_sink
(
  ConventionalHeatSink* conventionalheatsink,
  Dimensions*           dimensions,
  Source_t*             sources,
  Conductances*         conductances,
  GridDimension_t       layer_index
)
{
#ifdef PRINT_SOURCES
  fprintf (stderr,
    "layer_index = %d\tadd_sources_conventional_heat_sink\n", layer_index) ;
#endif

  conductances += get_cell_offset_in_stack (dimensions, layer_index, 0, 0) ;

  FOR_EVERY_ROW (row_index, dimensions)
  {
    FOR_EVERY_COLUMN (column_index, dimensions)
    {
      *sources = (conventionalheatsink->AmbientTemperature * conductances->Top) ;
#ifdef PRINT_SOURCES
        fprintf (stderr,
          "solid  cell  |  l %2d r %4d c %4d [%6d] | = %f * %.5e = %.5e\n",
          layer_index, row_index, column_index,
          get_cell_offset_in_stack (dimensions, layer_index, row_index, column_index),
          conventionalheatsink->AmbientTemperature, conductances->Top, *sources) ;
#endif
      sources++ ;
      conductances++ ;

    } // FOR_EVERY_COLUMN
  } // FOR_EVERY_ROW

}

/******************************************************************************/

void add_sources_conventional_heat_sink
(
  ConventionalHeatSink* conventionalheatsink,
  Dimensions*           dimensions,
  Source_t*             sources,
  Conductances*         conductances,
  GridDimension_t       layer_index
)
{
#ifdef PRINT_SOURCES
  fprintf (stderr,
    "layer_index = %d\tadd_sources_conventional_heat_sink\n", layer_index) ;
#endif

  conductances += get_cell_offset_in_stack (dimensions, layer_index, 0, 0) ;

  FOR_EVERY_ROW (row_index, dimensions)
  {
    FOR_EVERY_COLUMN (column_index, dimensions)
    {
      *sources += (conventionalheatsink->AmbientTemperature * conductances->Top) ;
#ifdef PRINT_SOURCES
        fprintf (stderr,
          "solid  cell  |  l %2d r %4d c %4d [%6d] | += %f * %.5e = %.5e\n",
          layer_index, row_index, column_index,
          get_cell_offset_in_stack (dimensions, layer_index, row_index, column_index),
          conventionalheatsink->AmbientTemperature, conductances->Top, *sources) ;
#endif
      sources++ ;
      conductances++ ;

    } // FOR_EVERY_COLUMN
  } // FOR_EVERY_ROW

}

/******************************************************************************/