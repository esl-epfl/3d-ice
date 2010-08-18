/******************************************************************************
 * Source file "3D-ICe/sources/environment_heat_sink.c"                       *
 *                                                                            *
 * This file is part of 3D-ICe (http://esl.epfl.ch/3D-ICe), revision 0.1      *
 *                                                                            *
 * 3D-ICe is free software: you can redistribute it and/or modify it under    *
 * the terms of the GNU General Public License as published by the Free       *
 * Software Foundation, either version 3 of the License, or any later         *
 * version.                                                                   *
 *                                                                            *
 * 3D-ICe is distributed in the hope that it will be useful, but WITHOUT      *
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or      *
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for   *
 * more details.                                                              *
 *                                                                            *
 * You should have received a copy of the GNU General Public License along    *
 * with 3D-ICe.  If not, see <http://www.gnu.org/licenses/>.                  *
 *                                                                            *
 * Copyright (C) 2010,                                                        *
 * Embedded Systems Laboratory - Ecole Polytechnique Federale de Lausanne.    *
 * All Rights Reserved.                                                       *
 *                                                                            *
 * Authors: Alessandro Vincenzi, Arvind Sridhar.                              *
 *                                                                            *
 * EPFL-STI-IEL-ESL                                                           *
 * Bâtiment ELG, ELG 130                                                      *
 * Station 11                                                                 *
 * 1015 Lausanne, Switzerland                          threed-ice@esl.epfl.ch *
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
  Quantity_t counter = get_layer_area (dimensions) ;
  sources += get_cell_offset_in_stack (dimensions, layer, 0, 0) ;
  conductances += get_cell_offset_in_stack (dimensions, layer, 0, 0) ;

  while (counter-- > 0)
    *sources++ += environmentheatsink->EnvironmentT
                  * (conductances++)->Top ;
}

/******************************************************************************/
