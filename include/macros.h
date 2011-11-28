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

#ifndef _3DICE_MACROS_H_
#define _3DICE_MACROS_H_

/*! \file macros.h */

#ifdef __cplusplus
extern "C"
{
#endif

#include "math.h"

/******************************************************************************/

# define PI (3.14159265358979323846)

/******************************************************************************/

  /* Max and min */

# define MAX(a,b)  (((a) > (b)) ? (a) : (b))
# define MIN(a,b)  (((a) < (b)) ? (a) : (b))

/******************************************************************************/

  /* Equivalent of two resistances in parallel */

# define PARALLEL(x,y) ( (x * y) / ( x + y) )

/******************************************************************************/

# define FREE_POINTER(function, pointer) \
                                         \
    do                                   \
    {                                    \
      function (pointer) ;               \
      pointer = NULL ;                   \
    } while (0)

/******************************************************************************/

  /* Free a linked list of ... */

# define FREE_LIST(type, list, free_function)    \
                                                 \
    do                                           \
    {                                            \
        type* next = NULL ;                      \
        for ( ; list != NULL ; list = next)      \
        {                                        \
            next = list->Next ;                  \
            FREE_POINTER (free_function, list) ; \
        }                                        \
    } while (0)                                  \

/******************************************************************************/

# define JOIN_ELEMENTS(first, second) \
                                      \
    do                                \
    {                                 \
      first->Next  = second ;         \
      second->Prev = first  ;         \
    } while (0)

/******************************************************************************/

  /* Cross a linked list of ... from left to right */

# define FOR_EVERY_ELEMENT_IN_LIST_FORWARD(type, index, begin) \
                                                       \
   type* index = NULL ;                                \
   for (index = begin ; index != NULL ; index = index->Next)

/******************************************************************************/

  /* Cross a linked list of ... from right to left */

# define FOR_EVERY_ELEMENT_IN_LIST_BACKWARD(type, index, begin) \
                                                                \
   type* index = NULL ;                                         \
   for (index = begin ; index != NULL ; index = index->Prev)

/******************************************************************************/

  /* Cross a linked list of ... except the last */

# define FOR_EVERY_ELEMENT_IN_LIST_EXCEPT_LAST(type, index, begin) \
                                                                   \
   type* index = NULL ;                                            \
   for (index = begin ; index->Next != NULL ; index = index->Next)

/******************************************************************************/

# define FIRST_ROW_INDEX              0u
# define LAST_ROW_INDEX(dim)          (get_number_of_rows (dim) - 1)

# define IS_FIRST_ROW(row)            (row == FIRST_ROW_INDEX)
# define IS_LAST_ROW(row, dim)        (row == LAST_ROW_INDEX(dim))

# define FOR_EVERY_ROW(index, dim)                   \
                                                     \
    uint32_t index = 0u ;                            \
    for (index = FIRST_ROW_INDEX ; index <= LAST_ROW_INDEX(dim) ; index++)

/******************************************************************************/

# define FIRST_COLUMN_INDEX           0u
# define LAST_COLUMN_INDEX(dim)       (get_number_of_columns (dim) - 1)

# define IS_FIRST_COLUMN(column)      (column == FIRST_COLUMN_INDEX)
# define IS_LAST_COLUMN(column, dim)  (column == LAST_COLUMN_INDEX(dim))

# define IS_CHANNEL_COLUMN(channel_model, column) \
                                                  \
        (channel_model == TDICE_CHANNEL_MODEL_MC_RM4 ? column & 1 : true)

# define IS_WALL_COLUMN(column)       (! IS_CHANNEL_COLUMN(column))

# define FOR_EVERY_COLUMN(index, dim)                      \
                                                           \
    uint32_t index = 0u ;                                  \
    for (index = FIRST_COLUMN_INDEX ; index <= LAST_COLUMN_INDEX(dim) ; index++)

/******************************************************************************/

# define FIRST_LAYER_INDEX            0u
# define LAST_LAYER_INDEX(dim)        (get_number_of_layers (dim) - 1)

# define IS_FIRST_LAYER(layer)        (layer == FIRST_LAYER_INDEX)
# define IS_LAST_LAYER(layer, dim)    (layer == LAST_LAYER_INDEX(dim))

# define FOR_EVERY_LAYER(index, dim)                     \
                                                         \
    uint32_t index = 0u ;                         \
    for (index = FIRST_LAYER_INDEX ; index <= LAST_LAYER_INDEX(dim) ; index++)

/******************************************************************************/

# define FIRST_FLOORPLAN_ELEMENT_ROW_INDEX(flp_el) (uint32_t)flp_el->SW_Row
# define LAST_FLOORPLAN_ELEMENT_ROW_INDEX(flp_el)  (uint32_t)flp_el->NE_Row

# define FOR_EVERY_FLOORPLAN_ELEMENT_ROW(index, flp_el)   \
                                                          \
    uint32_t index = 0u ;                                 \
    for                                                   \
    (                                                     \
      index = FIRST_FLOORPLAN_ELEMENT_ROW_INDEX(flp_el) ; \
      index <= LAST_FLOORPLAN_ELEMENT_ROW_INDEX(flp_el) ; \
      index++                                             \
    )

/******************************************************************************/

# define FIRST_FLOORPLAN_ELEMENT_COLUMN_INDEX(flp_el) (uint32_t)flp_el->SW_Column
# define LAST_FLOORPLAN_ELEMENT_COLUMN_INDEX(flp_el)  (uint32_t)flp_el->NE_Column

# define FOR_EVERY_FLOORPLAN_ELEMENT_COLUMN(index, flp_el)   \
                                                             \
    uint32_t index = 0u ;                                    \
    for                                                      \
    (                                                        \
      index = FIRST_FLOORPLAN_ELEMENT_COLUMN_INDEX(flp_el) ; \
      index <= LAST_FLOORPLAN_ELEMENT_COLUMN_INDEX(flp_el) ; \
      index++                                                \
    )

/******************************************************************************/

/* 4RM for Microchannels
 *
 * FlowRatePerChannel [ um3 / sec ] = FlowRate             [ um3 / sec ]
 *                                    / #ChannelColumns    [ ]
 *
 * CoolantVelocity     [ m / sec ]  = FlowRatePerChannel   [ um3 / sec ]
 *                                    * ( 1 / Ay )         [   1 / um2 ]
 *
 * Cconv         [ J / ( K . sec) ]  = CoolantVHC          [ J / ( um3 . K ) ]
 *                                     * CoolantVelocity   [ m / sec ]
 *                                     * ( Ay / 2 )        [ um2 ]
 * [ J / ( K . sec) ] = [ W / K ]
 *
 * CoolantVelocity = FlowRate / (#ChannelColumns * Ay )
 *
 * Cconv           = (CoolantVHC * FlowRate) / (#ChannelColumns * 2)
 */

# define CCONV_MC_4RM(nchannels, coolant_vhc, coolant_fr) \
                                                          \
  (double)  (                                             \
               (coolant_vhc * coolant_fr)                 \
               /                                          \
               ((double) (nchannels * 2))                 \
            )

/* 2RM for Microchannels
 *
 * FlowRatePerChannel [ um3 / sec ] = FlowRate                            [ um3 / sec ]
 *                                    / #Channels                         [ ]
 *
 * CoolantVelocity      [ m / sec ] = FlowRatePerChannel                  [ um3 / sec ]
 *                                    / (CavityHeight * ChannelWidth )    [ um2 ]
 *
 * Cconv         [ J / ( K . sec) ] = CoolantVHC                          [ J / ( um3 . K ) ]
 *                                    * CoolantVelocity                   [ m / sec ]
 *                                    * ( Cavityheight * CellLength / 2 ) [ um2 ]
 *                                    * Porosity                          [ ]
 * [ J / ( K . sec) ] = [ W / K ]
 *
 * CoolantVelocity = FlowRate / (#Channels * CavityHeight * ChannelWidth )
 *
 * Cconv           = (CoolantVHC * FlowRate * Porosity) / (#Channels * 2) * (CellLength / ChannelWidth)
 */

# define CCONV_MC_2RM(nchannels, coolant_vhc, coolant_fr, porosity, cell_length, channel_length) \
                                                                                                 \
  (double) (                                                                                     \
               (coolant_vhc * coolant_fr * porosity)                                             \
               /                                                                                 \
               ((double) (nchannels * 2))                                                        \
               *                                                                                 \
               (cell_length / channel_length)                                                    \
            )

/* Pin-fins
 *
 * Cconv     [ J / ( K . sec) ] = CoolantVHC                             [ J / ( um3 . K ) ]
 *                                * DarcyVelosity                        [ m / sec ]
 *                                * ( CavityHeight * CellLength / 2 )    [ um2 ]
 *                                * Porosity                             [ ]
 */

# define CCONV_PF(coolant_vhc, darcy_velocity, cell_length, cavity_height)    \
                                                                              \
  (double) (                                                                  \
                 (coolant_vhc * darcy_velocity * cell_length * cavity_height) \
                 /                                                            \
                 ((double) 2)                                                 \
            )

/******************************************************************************/

/* Effective HTC for Microchannel 2RM
 *
 * HTC_eff         = HTC * (ChannelWidth + CavityHeight) / ChannelPitch
 */

# define EFFECTIVE_HTC_MC_2RM(htc, channel_width, cavity_height, channel_pitch)  \
                                                                                 \
  (double) (                                                                     \
                   htc * (channel_width + cavity_height) / channel_pitch         \
                 )

/* Effective HTC for Pinfin
 *
 * H_eff_inline    = ((2.527E-5 / (DarcyVelosity + 1.35)^0.64 )+1.533E-6)^(-1) * 1E-12
 */

# define EFFECTIVE_HTC_PF_INLINE(darcy_velocity)                                      \
                                                                                      \
  (double) (                                                                          \
                   1e-12                                                              \
                   /                                                                  \
                   (2.527e-05 / pow((darcy_velocity/1e+06 + 1.35), 0.64) + 1.533e-06) \
                 )
/*
 * H_eff_stag    = ((2.527E-5 / (DarcyVelosity + 1.35)^1.52 )+1.533E-6)^(-1) * 1E-12
 */

# define EFFECTIVE_HTC_PF_STAGGERED(darcy_velocity)                                   \
                                                                                      \
  (double) (                                                                          \
                   1e-12                                                              \
                   /                                                                  \
                   (2.527e-05 / pow((darcy_velocity/1e+06 + 1.35), 1.52) + 1.533e-06) \
                 )

/******************************************************************************/

 /*
  * "FlowRate[um3/sec]" = ( "FlowRate[ml/min]" * 1e+12 ) / 60.0
  */

# define FLOW_RATE_FROM_MLMIN_TO_UM3SEC(fr) (( fr * 1e+12 ) / 60.0)

# define FLOW_RATE_FROM_UM3SEC_TO_MLMIN(fr) (( fr * 60.0 ) / 1e+12)


# define POROSITY(diameter, pitch) (1.0 - (PI * diameter * diameter / 4.0) / (pitch * pitch))

#define DIAMETER(porosity, pitch) (sqrt (( (1.0 - porosity) * pitch*pitch * 4.0 ) / PI ))

/******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* _3DICE_MACROS_H_ */
