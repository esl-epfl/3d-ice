/******************************************************************************
 * Header file "3D-ICe/include/environment_heat_sink.h"                       *
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

#ifndef _3DICE_ENVIRONMENT_HEAT_SINK_H_
#define _3DICE_ENVIRONMENT_HEAT_SINK_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdio.h>

#include "types.h"
#include "conductances.h"

/******************************************************************************/

  typedef struct
  {
    /* The heat transfert coefficent */

    EnvironmentHTC_t EnvironmentHTC ;

    /* The temperarute of the environment in [K] */

    Temperature_t EnvironmentT ;

  } EnvironmentHeatSink ;

/******************************************************************************/

  void init_environment_heat_sink (EnvironmentHeatSink* environmentheatsink) ;

/******************************************************************************/

  EnvironmentHeatSink* alloc_and_init_environment_heat_sink (void) ;

/******************************************************************************/

  void free_environment_heat_sink (EnvironmentHeatSink* environmentheatsink) ;

/******************************************************************************/

  void print_environment_heat_sink
  (
    FILE* stream,
    String_t prefix,
    EnvironmentHeatSink* environmentheatsink
  ) ;

/******************************************************************************/

  void add_sources_enviroment_heat_sink
  (
    EnvironmentHeatSink* environmentheatsink,
    Dimensions*          dimensions,
    Source_t*            sources,
    Conductances*        conductances,
    LayerIndex_t         layer
  ) ;

/******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* _3DICE_ENVIRONMENT_HEAT_SINK_H_ */
