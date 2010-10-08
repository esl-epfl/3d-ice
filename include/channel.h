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

#ifndef _3DICE_CHANNEL_H_
#define _3DICE_CHANNEL_H_

#ifdef __cplusplus
extern "C"
{
#endif

/******************************************************************************/

#include <stdio.h>

#include "types.h"
#include "material.h"
#include "conductances.h"

/******************************************************************************/

  typedef struct
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

    Material* Wall ;

  } Channel ;

/******************************************************************************/

  void init_channel (Channel* channel) ;

/******************************************************************************/

  Channel* alloc_and_init_channel (void) ;

/******************************************************************************/

  void free_channel (Channel* channel) ;

/******************************************************************************/

  void print_channel (FILE* stream, String_t prefix, Channel* channel) ;

/******************************************************************************/

  Bool_t is_channel_column (ColumnIndex_t column) ;

/******************************************************************************/

  Conductances* fill_conductances_channel
  (
#   ifdef PRINT_CONDUCTANCES
    LayerIndex_t  current_layer,
#   endif
    Channel*      channel,
    Conductances* conductances,
    Dimensions*   dimensions
  ) ;

/******************************************************************************/

  Capacity_t* fill_capacities_channel
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

  Source_t* fill_sources_channel
  (
#   ifdef PRINT_SOURCES
    LayerIndex_t current_layer,
#   endif
    Channel*     channel,
    Source_t*    sources,
    Dimensions*  dimensions
  ) ;

/******************************************************************************/

  Quantity_t fill_system_matrix_channel
  (
#   ifdef PRINT_SYSTEM_MATRIX
    Channel*             channel,
#   endif
    Dimensions*          dimensions,
    Conductances*        conductances,
    Capacity_t*          capacities,
    LayerIndex_t         current_layer,
    ColumnIndex_t*       column_pointers,
    RowIndex_t*          row_indices,
    SystemMatrixValue_t* values
  ) ;

/******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* _3DICE_CHANNEL_H_ */
