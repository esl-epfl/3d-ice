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
#include "thermal_cell.h"
#include "system_matrix.h"

/******************************************************************************/

  typedef struct
  {
    /* The channel model */

    ChannelModel_t ChannelModel ;

    /* The height of the channel in [um] (1 cell)    */

    CellDimension_t Height ;

    /* The length of the channel in [um]             */

    CellDimension_t Length ;

    /* The pitch of the channel in [um]              */

    CellDimension_t Pitch ;

    /* Porosity */

    Porosity_t Porosity ;

    /* The heat transfert coefficients of the cooling liquid */
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

    /* Darcy Velocity
     * [ um / sec ]
     */

    DarcyVelocity_t DarcyVelocity ;

    /* The material composing the wall */

    Material* WallMaterial ;

  } Channel ;

/******************************************************************************/

  void init_channel (Channel* channel) ;

/******************************************************************************/

  Channel* alloc_and_init_channel (void) ;

/******************************************************************************/

  void free_channel (Channel* channel) ;

/******************************************************************************/

  void print_formatted_channel
  (
    FILE*       stream,
    String_t    prefix,
    Channel*    channel,
    Dimensions* dimensions
  ) ;

/******************************************************************************/

  void print_detailed_channel
  (
    FILE*    stream,
    String_t prefix,
    Channel* channel
  ) ;

/******************************************************************************/


  void fill_thermal_cell_channel
  (
    ThermalCell*     thermalcells,
    Time_t           delta_time,
    Dimensions*      dimensions,
    GridDimension_t  layer_index,
    Channel*         channel
  ) ;

/******************************************************************************/

  void fill_sources_channel
  (
    Source_t*        sources,
    Dimensions*      dimensions,
    GridDimension_t  layer_index,
    Channel*         channel
  ) ;

/******************************************************************************/

  SystemMatrix fill_system_matrix_channel
  (
    Channel*         channel,
    Dimensions*      dimensions,
    ThermalCell*     thermalcells,
    GridDimension_t  layer_index,
    SystemMatrix     system_matrix
  ) ;

/******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* _3DICE_CHANNEL_H_ */
