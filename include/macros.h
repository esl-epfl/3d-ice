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

#ifdef __cplusplus
extern "C"
{
#endif

/******************************************************************************/

# define MALLOC(pointer, size) \
                               \
    pointer = malloc ( sizeof(*pointer) * (size) )

/******************************************************************************/

  /* Max and min */

# define MAX(a,b)  (((a) > (b)) ? (a) : (b))
# define MIN(a,b)  (((a) < (b)) ? (a) : (b))

/******************************************************************************/

  /* Equivalent of two resistances in parallel */

# define PARALLEL(x,y) ( (x * y) / ( x + y) )

/******************************************************************************/

  /* Free a linked list of ... */

# define FREE_LIST(type, list, free_function) \
                                              \
    type* next = NULL ;                       \
    for ( ; list != NULL ; list = next)       \
    {                                         \
      next = list->Next ;                     \
      free_function (list) ;                  \
    }

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

# define FIRST_ROW_INDEX              GRIDDIMENSION_I
# define LAST_ROW_INDEX(dim)          (get_number_of_rows (dim) - 1)

# define IS_FIRST_ROW(row)            (row == FIRST_ROW_INDEX)
# define IS_LAST_ROW(row, dim)        (row == LAST_ROW_INDEX(dim))

# define FOR_EVERY_ROW(index, dim)            \
                                              \
    GridDimension_t index = GRIDDIMENSION_I ; \
    for (index = FIRST_ROW_INDEX ; index <= LAST_ROW_INDEX(dim) ; index++)

/******************************************************************************/

# define FIRST_COLUMN_INDEX           GRIDDIMENSION_I
# define LAST_COLUMN_INDEX(dim)       (get_number_of_columns (dim) - 1)

# define IS_FIRST_COLUMN(column)      (column == FIRST_COLUMN_INDEX)
# define IS_LAST_COLUMN(column, dim)  (column == LAST_COLUMN_INDEX(dim))
# define IS_CHANNEL_COLUMN(column)    (column & 1)
# define IS_WALL_COLUMN(column)       (! IS_CHANNEL_COLUMN(column) )

# define FOR_EVERY_COLUMN(index, dim)         \
                                              \
    GridDimension_t index = GRIDDIMENSION_I ; \
    for (index = FIRST_COLUMN_INDEX ; index <= LAST_COLUMN_INDEX(dim) ; index++)

/******************************************************************************/

# define FIRST_LAYER_INDEX            GRIDDIMENSION_I
# define LAST_LAYER_INDEX(dim)        (get_number_of_layers (dim) - 1)

# define IS_FIRST_LAYER(layer)        (layer == FIRST_LAYER_INDEX)
# define IS_LAST_LAYER(layer, dim)    (layer == LAST_LAYER_INDEX(dim))

# define FOR_EVERY_LAYER(index, dim)          \
                                              \
    GridDimension_t index = GRIDDIMENSION_I ; \
    for (index = FIRST_LAYER_INDEX ; index <= LAST_LAYER_INDEX(dim) ; index++)

/******************************************************************************/

# define FIRST_FLOORPLAN_ELEMENT_ROW_INDEX(flp_el) flp_el->SW_Row
# define LAST_FLOORPLAN_ELEMENT_ROW_INDEX(flp_el)  flp_el->NE_Row

# define FOR_EVERY_FLOORPLAN_ELEMENT_ROW(index, flp_el)   \
                                                          \
    GridDimension_t index = GRIDDIMENSION_I ;             \
    for                                                   \
    (                                                     \
      index = FIRST_FLOORPLAN_ELEMENT_ROW_INDEX(flp_el) ; \
      index <= LAST_FLOORPLAN_ELEMENT_ROW_INDEX(flp_el) ; \
      index++                                             \
    )

/******************************************************************************/

# define FIRST_FLOORPLAN_ELEMENT_COLUMN_INDEX(flp_el) flp_el->SW_Column
# define LAST_FLOORPLAN_ELEMENT_COLUMN_INDEX(flp_el)  flp_el->NE_Column

# define FOR_EVERY_FLOORPLAN_ELEMENT_COLUMN(index, flp_el)   \
                                                             \
    GridDimension_t index = GRIDDIMENSION_I ;                \
    for                                                      \
    (                                                        \
      index = FIRST_FLOORPLAN_ELEMENT_COLUMN_INDEX(flp_el) ; \
      index <= LAST_FLOORPLAN_ELEMENT_COLUMN_INDEX(flp_el) ; \
      index++                                                \
    )

/******************************************************************************/

/*
 * FlowRatePerChannel [ um3 / sec ] = FlowRate             [ um3 / sec ]
 *                                    / #ChannelColumns    [ ]
 *
 * CoolantVelocity      [ m / sec ] = FlowRatePerChannel   [ um3 / sec ]
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

# define CCONV(ncolumns, coolant_vhc, coolant_fr) \
                                                  \
   (Cconv_t) (                                    \
               (coolant_vhc * coolant_fr)         \
               /                                  \
               ((Cconv_t) (ncolumns - 1))         \
             )

/******************************************************************************/

 /*
  * "FlowRate[um3/sec]" = ( "FlowRate[ml/min]" * 1e+12 ) / 60.0
  */

# define CONVERT_COOLANT_FLOW_RATE(fr) ( fr * 1e+12 ) / 60.0

#ifdef __cplusplus
}
#endif

#endif /* _3DICE_MACROS_H_ */
