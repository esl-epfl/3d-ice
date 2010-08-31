/******************************************************************************
 * Bison source file "3D-ICE/bison/floorplan_parser.y"                        *
 *                                                                            *
 * This file is part of 3D-ICE 1.0 (beta).                                    *
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
 * with 3D-ICe. If not, see <http://www.gnu.org/licenses/>.                   *
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
 * EPFL-STI-IEL-ESL                                                           *
 * Batiment ELG, ELG 130                                                      *
 * Station 11                                          threed-ice@esl.epfl.ch *
 * 1015 Lausanne, Switzerland                       http://esl.epfl.ch/3D-ICE *
 ******************************************************************************/

#include <stdlib.h>

#include "environment_heat_sink.h"

/******************************************************************************/

void init_environment_heat_sink (EnvironmentHeatSink* environmentheatsink)
{
  environmentheatsink->EnvironmentHTC = 0.0 ;
  environmentheatsink->EnvironmentT   = 0.0 ;
}

/******************************************************************************/

EnvironmentHeatSink* alloc_and_init_environment_heat_sink (void)
{
  EnvironmentHeatSink* environmentheatsink

    = (EnvironmentHeatSink*) malloc ( sizeof(EnvironmentHeatSink) ) ;

  if (environmentheatsink != NULL)

    init_environment_heat_sink (environmentheatsink) ;

  return environmentheatsink ;
}

/******************************************************************************/

void free_environment_heat_sink (EnvironmentHeatSink* environmentheatsink)
{
  free (environmentheatsink) ;
}

/******************************************************************************/

void print_environment_heat_sink
(
  FILE*                stream,
  String_t             prefix,
  EnvironmentHeatSink* environmentheatsink
)
{
  fprintf(stream,
    "%sEnvironment Heat Sink\n",            prefix) ;
  fprintf(stream,
    "%s  Environement HTC %.5e\n",          prefix,
                                            environmentheatsink->EnvironmentHTC) ;
  fprintf(stream,
    "%s  Environment temperature   %.4e\n", prefix,
                                            environmentheatsink->EnvironmentT) ;
}

/******************************************************************************/

void add_sources_enviroment_heat_sink
(
  EnvironmentHeatSink* environmentheatsink,
  Dimensions*          dimensions,
  Source_t*            sources,
  Conductances*        conductances,
  LayerIndex_t         layer
)
{
  RowIndex_t    row ;
  ColumnIndex_t column ;
  Quantity_t offset = get_cell_offset_in_stack (dimensions, layer, 0, 0) ;

#ifdef PRINT_SOURCES
  fprintf (stderr,
    "current_layer = %d\tadd_sources_enviroment_heat_sink\n", layer) ;
#endif

  sources      += offset;
  conductances += offset ;

  for
  (
    row = 0 ;
    row < get_number_of_rows (dimensions) ;
    row++
  )

    for
    (
      column = 0 ;
      column < get_number_of_columns (dimensions) ;
      column++ ,
      sources++ ,
      conductances++
    )
    {

      *sources += environmentheatsink->EnvironmentT * conductances->Top ;

#ifdef PRINT_SOURCES
        fprintf (stderr,
          "solid  cell  |  l %2d r %4d c %4d [%6d] | += %f * %.5e -> %.5e\n",
          layer, row, column,
          get_cell_offset_in_stack (dimensions, layer, row, column),
          environmentheatsink->EnvironmentT, conductances->Top, *sources);
#endif
    }
}

/******************************************************************************/
