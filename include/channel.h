/******************************************************************************
 * Header file "3D-ICe/include/channel.h"                                     *
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

#ifndef _3DICE_CHANNEL_H_
#define _3DICE_CHANNEL_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdio.h>

#include "types.h"
#include "material.h"
#include "conductances.h"

/******************************************************************************
 *                                 "Channel"                                  *
 ******************************************************************************/

  struct Channel
  {

    /* The heigh of the channel in [um] (1 cell)    */

    CellDimension_t Height ;

    /* The heat transfert coefficents of the cooling liquid */
    /* in [ (W / ( um2 * K ) ]                              */

    CoolantHTCs_t CoolantHTCs ;

    /* The volumetric heat capacity of the cooling liquid  */
    /* in [ J / ( um3 * K ) ]                              */

    CoolantVHC_t CoolantVHC ;

    /* The temperarute of the incoming liquid in [K]  */

    Temperature_t CoolantTIn ;

    /* The flow rate per channel layer of the incolimg liquid   */
    /* Specified in [ ml / min ] but stored in [ um3 / sec ]    */
    /* Shared by all the channels for each layer in the 3DStack */

    CoolantFR_t CoolantFR ;

    /* The material composing the wall */

    Material *Wall ;

    /* TRUE if the flow has changed since the last simulation */
    /* FALSE otherwise                                        */

    Bool_t FlowRateChanged ;

  } ;

  typedef struct Channel Channel ;

/******************************************************************************/

  void init_channel (Channel* channel) ;

/******************************************************************************/

  Channel* alloc_and_init_channel (void) ;

/******************************************************************************/

  void free_channel (Channel* channel) ;

/******************************************************************************/

  void print_channel (FILE* stream, String_t prefix, Channel* channel) ;

/******************************************************************************/

  Conductances*   fill_conductances_channel
  (
#   ifdef PRINT_CAPACITIES
    LayerIndex_t  current_layer
#   endif
    Channel*      channel,
    Conductances* conductances,
    Dimensions*   dimensions
  ) ;

/******************************************************************************/

  Capacity_t*    fill_capacities_channel
  (
#   ifdef PRINT_CAPACITIES
    LayerIndex_t current_layer,
#   endif
    Channel*     channel,
    Capacity_t*  capacities,
    Dimensions*  dimensions,
    Time_t       delta_time
  ) ;

/******************************************************************************/

  Source_t*      fill_sources_channel
  (
#   ifdef PRINT_SOURCES
    LayerIndex_t current_layer,
#   endif
    Channel*     channel,
    Source_t*    sources,
    Dimensions*  dimensions
  ) ;

/******************************************************************************/

  Quantity_t      fill_ccs_system_matrix_channel
  (
#   ifdef PRINT_SYSTEM_MATRIX
    Channel*      channel,
#   endif
    Dimensions*   dimensions,
    Conductances* conductances,
    Capacity_t*   capacities,
    LayerIndex_t  current_layer,
    int*          columns,
    int*          rows,
    double*       values
  ) ;

/******************************************************************************/

  Quantity_t      fill_crs_system_matrix_channel
  (
#   ifdef PRINT_SYSTEM_MATRIX
    Channel*      channel,
#   endif
    Dimensions*   dimensions,
    Conductances* conductances,
    Capacity_t*   capacities,
    LayerIndex_t  current_layer,
    int*          rows,
    int*          columns,
    double*       values
  ) ;

/******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* _3DICE_CHANNEL_H_ */
