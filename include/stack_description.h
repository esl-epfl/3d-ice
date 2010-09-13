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

#ifndef _3DICE_STACK_DESCRIPTION_H_
#define _3DICE_STACK_DESCRIPTION_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdio.h>

#include "material.h"
#include "channel.h"
#include "die.h"
#include "stack_element.h"
#include "dimensions.h"
#include "conductances.h"
#include "environment_heat_sink.h"

/******************************************************************************/

  typedef struct
  {
    /* The name of the file used to fill the stack description */

    String_t FileName ;

    /* The list of materials componing the layers and channels */

    Material* MaterialsList ;

    /* The (if present) single type of channel used to compose the 3d stack */

    Channel* Channel ;

    /* The list of dies available to compose the 3d stack */

    Die* DiesList ;

    /* Information (if present) about the heat dissipation */
    /* throught the top surface                            */

    EnvironmentHeatSink* EnvironmentHeatSink;

    /* The list of stack elements componing the 3Dstack */

    StackElement* StackElementsList ;

    /* Collection of all the dimensions (chip, grid of cells, cell) */

    Dimensions* Dimensions ;

    /* Number of remaining time slots to consume */

    Quantity_t RemainingTimeSlots ;

  } StackDescription ;

/******************************************************************************/

  /* Given a valid address of a StackDescription structure, sets all its    */
  /* fields to a default value                                              */

  void init_stack_description (StackDescription* stkd) ;

/******************************************************************************/

  /* Given a valid address of a StackDescription structure and the path of  */
  /* a .stk file, parses the file and fills the stkd structure              */
  /*                                                                        */
  /* Returns:                                                               */
  /*  0 if the parsing succeeds                                             */
  /* -1 if the .stk file does not exist                                     */
  /*  1 if there is an error in the .stk file or in one of the .flp files   */
  /*      cited in it (message printed to stderr).                          */
  /*                                                                        */
  /* Note: The function parses the .stk file first and then all the .flp    */
  /* files related to the floorplans following the order used in the stack  */
  /* section (from bottom to top). It stops as soon as it finds an error    */
  /* and it returns 1, discarding the remaining files to parse.             */

  int fill_stack_description (StackDescription* stkd, String_t filename) ;

/******************************************************************************/

  /* Frees all the memory used by the given StackDescription structure      */

  void free_stack_description (StackDescription* stkd) ;

/******************************************************************************/

  /* Print on stream (stdout, stderr or a given opened output file) all the */
  /* data related to the stkd structure previously filled. Prefix is a      */
  /* string (it can be empty as "") prined as prefix at the beginning of    */
  /* every line.                                                            */
  /*                                                                        */
  /* To print the content of only one of the fields of stkd, use the        */
  /* printing function in the corresponding header file (see material.h)    */

  void print_stack_description
  (
    FILE*             stream,
    String_t          prefix,
    StackDescription* stkd
  ) ;

/******************************************************************************/

  /* Print on stream (stdout, stderr or a given opened output file) all the */
  /* data related to the floorplan structures contained in the dies.        */
  /* Prefix is a string (it can be empty as "") printed as prefix at the    */
  /* beginning of every line.                                               */

  void print_all_floorplans
  (
    FILE*             stream,
    String_t          prefix,
    StackDescription* stkd
  ) ;

/******************************************************************************/

  /* Given a StackDescription address and the id (string) of a floorplan,   */
  /* it returns the number of all the floorplan elements in that floorplan. */
  /* As floorplan id, we refer to the stack element id given to the die it  */
  /* belongs to.                                                            */
  /*                                                                        */
  /* Returns:                                                               */
  /*  -1 if the stack element id floorplan_id does not exist                */
  /*  -2 if floorplan_id exists but it is not the id of a die or it belongs */
  /*     to a die but the floorplan itself does not exist                   */
  /*  >= 0 the number of floorplan elements in floorplan_id                 */

  Quantity_t get_number_of_floorplan_elements_of_floorplan
  (
    StackDescription* stkd,
    String_t floorplan_id
  ) ;

/******************************************************************************/

  /* Given a StackDescription address, returns the number of channels       */
  /* (the number of channels is the same for every channel layer)           */

  Quantity_t get_number_of_channel_outlets (StackDescription* stkd) ;

/******************************************************************************/

  /* Get the maximum temperature of a given floorplan element                */
  /*                                                                         */
  /* stkd                  the StackDescription structure to query           */
  /* floorplan_id          the id of the floorplan (the stack element id)    */
  /* floorplan_element_id  the id of the floorplan element                   */
  /* temperatures          the address of the array temperature to access    */
  /* max_temperature       the address where the max temperature will be     */
  /*                       written                                           */
  /*                                                                         */
  /* Returns:                                                                */
  /*   0 if both the ids are correct                                         */
  /*  -1 if the stack element id floorplan_id does not exist                 */
  /*  -2 if floorplan_id exists but it is not the id of a die or it belongs  */
  /*     to a die but the floorplan itself does not exist                    */
  /*  -3 if floorplan_id exists but floorplan_element_id do not              */

  int get_max_temperature_of_floorplan_element
  (
    StackDescription* stkd,
    String_t          floorplan_id,
    String_t          floorplan_element_id,
    Temperature_t*    temperatures,
    Temperature_t*    max_temperature
  ) ;

/******************************************************************************/

  /* Get the minimum temperature of a given floorplan element                */
  /*                                                                         */
  /* stkd                  the StackDescription structure to query           */
  /* floorplan_id          the id of the floorplan (the stack element id)    */
  /* floorplan_element_id  the id of the floorplan element                   */
  /* temperatures          the address of the array temperature to access    */
  /* min_temperature       the address where the min temperature will be     */
  /*                       written                                           */
  /*                                                                         */
  /* Returns:                                                                */
  /*   0 if both the ids are correct                                         */
  /*  -1 if the stack element id floorplan_id does not exist                 */
  /*  -2 if floorplan_id exists but it is not the id of a die or it belongs  */
  /*     to a die but the floorplan itself does not exist                    */
  /*  -3 if floorplan_id exists but floorplan_element_id do not              */

  int get_min_temperature_of_floorplan_element
  (
    StackDescription* stkd,
    String_t          floorplan_id,
    String_t          floorplan_element_id,
    Temperature_t*    temperatures,
    Temperature_t*    min_temperature
  ) ;

/******************************************************************************/

  /* Get the average temperature of a given floorplan element                */
  /*                                                                         */
  /* stkd                  the StackDescription structure to query           */
  /* floorplan_id          the id of the floorplan (the stack element id)    */
  /* floorplan_element_id  the id of the floorplan element                   */
  /* temperatures          the address of the array temperature to access    */
  /* avg_temperature       the address where the avg temperature will be     */
  /*                       written                                           */
  /*                                                                         */
  /* Returns:                                                                */
  /*   0 if both the ids are correct                                         */
  /*  -1 if the stack element id floorplan_id does not exist                 */
  /*  -2 if floorplan_id exists but it is not the id of a die or it belongs  */
  /*     to a die but the floorplan itself does not exist                    */
  /*  -3 if floorplan_id exists but floorplan_element_id do not              */

  int get_avg_temperature_of_floorplan_element
  (
    StackDescription* stkd,
    String_t          floorplan_id,
    String_t          floorplan_element_id,
    Temperature_t*    temperatures,
    Temperature_t*    avg_temperature
  ) ;

/******************************************************************************/

  /* Get the minimum, average and maximum temperature of a given floorplan   */
  /* element                                                                 */
  /*                                                                         */
  /* stkd                  the StackDescription structure to query           */
  /* floorplan_id          the id of the floorplan (the stack element id)    */
  /* floorplan_element_id  the id of the floorplan element                   */
  /* temperatures          the address of the array temperature to access    */
  /* min_temperature       the address where the min temperature will be     */
  /*                       written                                           */
  /* avg_temperature       the address where the avg temperature will be     */
  /*                       written                                           */
  /* max_temperature       the address where the max temperature will be     */
  /*                       written                                           */
  /*                                                                         */
  /* Returns:                                                                */
  /*   0 if both the ids are correct                                         */
  /*  -1 if the stack element id floorplan_id does not exist                 */
  /*  -2 if floorplan_id exists but it is not the id of a die or it belongs  */
  /*     to a die but the floorplan itself does not exist                    */
  /*  -3 if floorplan_id exists but floorplan_element_id do not              */

  int get_min_avg_max_temperatures_of_floorplan_element
  (
    StackDescription* stkd,
    String_t          floorplan_id,
    String_t          floorplan_element_id,
    Temperature_t*    temperatures,
    Temperature_t*    min_temperature,
    Temperature_t*    avg_temperature,
    Temperature_t*    max_temperature
  ) ;

/******************************************************************************/

  /* Get the temperature of the liquid when leaving the specific outlet of   */
  /* a specific channel layer                                                */
  /*                                                                         */
  /* stkd                  the StackDescription structure to query           */
  /* channel_id            the id of the channel (the stack element id)      */
  /* outlet_number         the id (number) of the outlet. It must be an      */
  /*                       integer between 0 (the west most channel) and the */
  /*                       number of channels.                               */
  /* temperatures          the address of the array temperature to access    */
  /* outlet_temperature    the address where the temperature of the liquid   */
  /*                       will be written                                   */
  /*                                                                         */
  /* Returns:                                                                */
  /*   0 if both the id of the channel and the outlet are correct            */
  /*  -1 if the stack element id channel_id does not exist                   */
  /*  -2 if channel_id exists but it does not refer to a channel layer       */
  /*  -3 if channel_id exists but outlet_number does not refer to a channel  */

  int get_temperature_of_channel_outlet
  (
    StackDescription* stkd,
    String_t          channel_id,
    ColumnIndex_t     outlet_number,
    Temperature_t*    temperatures,
    Temperature_t*    outlet_temperature
  ) ;

/******************************************************************************/

  /* Get the maximum temperature of all the floorplan element in a floorplan */
  /*                                                                         */
  /* stkd                  the StackDescription structure to query           */
  /* floorplan_id          the id of the floorplan (the stack element id)    */
  /* temperatures          the address of the array temperature to access    */
  /* max_temperature       the address where the max temperatures will be    */
  /*                       written                                           */
  /*                                                                         */
  /* Returns:                                                                */
  /*   0 if the ids is correct                                               */
  /*  -1 if the stack element id floorplan_id does not exist                 */
  /*  -2 if floorplan_id exists but it is not the id of a die or it belongs  */
  /*     to a die but the floorplan itself does not exist                    */
  /*                                                                         */
  /* If the floorplan named floorplan_id has n floorplan element, then the   */
  /* function will access n elements of type Temperature_t starting from     */
  /* max_temperatures (i.e. the memory must be allocated before calling this */
  /* function). Temperatures will be written following the same order of     */
  /* declaration found in the corresponding .flp file (max_temperature[0] is */
  /* the maximum temperature of the first floorplan element found in the     */
  /* file).                                                                  */

  int get_all_max_temperatures_of_floorplan
  (
    StackDescription* stkd,
    String_t          floorplan_id,
    Temperature_t*    temperatures,
    Temperature_t*    max_temperature
  ) ;

/******************************************************************************/

  /* Get the minimum temperature of all the floorplan element in a floorplan */
  /*                                                                         */
  /* stkd                  the StackDescription structure to query           */
  /* floorplan_id          the id of the floorplan (the stack element id)    */
  /* temperatures          the address of the array temperature to access    */
  /* min_temperature       the address where the min temperatures will be    */
  /*                       written                                           */
  /*                                                                         */
  /* Returns:                                                                */
  /*   0 if the ids is correct                                               */
  /*  -1 if the stack element id floorplan_id does not exist                 */
  /*  -2 if floorplan_id exists but it is not the id of a die or it belongs  */
  /*     to a die but the Floorplan itself does not exist                    */
  /*                                                                         */
  /* If the floorplan named floorplan_id has n floorplan element, then the   */
  /* function will access n elements of type Temperature_t starting from     */
  /* min_temperatures (i.e. the memory must be allocated before calling this */
  /* function). Temperatures will be written following the same order of     */
  /* declaration found in the corresponding .flp file (max_temperature[0] is */
  /* the minimum temperature of the first floorplan element found in the     */
  /* file).                                                                  */

  int get_all_min_temperatures_of_floorplan
  (
    StackDescription* stkd,
    String_t          floorplan_id,
    Temperature_t*    temperatures,
    Temperature_t*    min_temperature
  ) ;

/******************************************************************************/

  /* Get the average temperature of all the floorplan element in a floorplan */
  /*                                                                         */
  /* stkd                  the StackDescription structure to query           */
  /* floorplan_id          the id of the floorplan (the stack element id)    */
  /* temperatures          the address of the array temperature to access    */
  /* avg_temperature       the address where the avg temperatures will be    */
  /*                       written                                           */
  /*                                                                         */
  /* Returns:                                                                */
  /*   0 if the ids is correct                                               */
  /*  -1 if the stack element id floorplan_id does not exist                 */
  /*  -2 if floorplan_id exists but it is not the id of a die or it belongs  */
  /*     to a die but the Floorplan itself does not exist                    */
  /*                                                                         */
  /* If the floorplan named floorplan_id has n floorplan element, then the   */
  /* function will access n elements of type Temperature_t starting from     */
  /* avg_temperatures (i.e. the memory must be allocated before calling this */
  /* function). Temperatures will be written following the same order of     */
  /* declaration found in the corresponding .flp file (max_temperature[0] is */
  /* the average temperature of the first floorplan element found in the     */
  /* file).                                                                  */

  int get_all_avg_temperatures_of_floorplan
  (
    StackDescription* stkd,
    String_t          floorplan_id,
    Temperature_t*    temperatures,
    Temperature_t*    avg_temperature
  ) ;

/******************************************************************************/

  /* Get the minimum, average and maximum temperature of all the floorplan   */
  /* element in a floorplan                                                  */
  /*                                                                         */
  /* stkd                  the StackDescription structure to query           */
  /* floorplan_id          the id of the floorplan (the stack element id)    */
  /* temperatures          the address of the array temperature to access    */
  /* min_temperature       the address where the min temperatures will be    */
  /*                       written                                           */
  /* avg_temperature       the address where the avg temperatures will be    */
  /*                       written                                           */
  /* max_temperature       the address where the max temperatures will be    */
  /*                       written                                           */
  /*                                                                         */
  /* Returns:                                                                */
  /*   0 if the ids is correct                                               */
  /*  -1 if the stack element id floorplan_id does not exist                 */
  /*  -2 if floorplan_id exists but it is not the id of a die or it belongs  */
  /*     to a die but the Floorplan itself does not exist                    */
  /*                                                                         */
  /* If the floorplan named floorplan_id has n floorplan element, then the   */
  /* function will access n elements of type Temperature_t starting from     */
  /* min_temperatures, avg_temperatures, etc. (i.e. the memory must be       */
  /* allocated before calling this function). Temperatures will be written   */
  /* following the same order of declaration found in the corresponding .flp */
  /* file (min_temperature[0] is the minimum temperature of the first        */
  /* floorplan element found in the file, etc...).                           */

  int get_all_min_avg_max_temperatures_of_floorplan
  (
    StackDescription* stkd,
    String_t          floorplan_id,
    Temperature_t*    temperatures,
    Temperature_t*    min_temperature,
    Temperature_t*    avg_temperature,
    Temperature_t*    max_temperature
  ) ;

/******************************************************************************/

  /* Get the temperature of the liquid when leaving all the outlets of a     */
  /* specific channel layer                                                  */
  /*                                                                         */
  /* stkd                  the StackDescription structure to query           */
  /* channel_id            the id of the channel (the stack element id)      */
  /* temperatures          the address of the array temperature to access    */
  /* outlet_temperatures   the address where the temperatures of the liquid  */
  /*                       will be written                                   */
  /*                                                                         */
  /* Returns:                                                                */
  /*   0 if both the id of the channel and the outlet are correct            */
  /*  -1 if the stack element id channel_id does not exist                   */
  /*  -2 if channel_id exists but it does not refer to a channel layer       */
  /*                                                                         */
  /* If a channel layer has n channels, then the function will access n      */
  /* elements of type Temperature_t starting from outlet_temperature (i.e.   */
  /* the memory must be allocated before calling). The temperatures are      */
  /* written following the west -> east order.                               */

  int get_all_temperatures_of_channel_outlets
  (
    StackDescription* stkd,
    String_t          channel_id,
    Temperature_t*    temperatures,
    Temperature_t*    outlet_temperature
  ) ;

/******************************************************************************/

  /* Given the address of a StackDescrption structure and the array of       */
  /* temperatures, print to file_name the thermal map of stack_element_id.   */
  /* stack_element_id is the id given to one of the stack element composing  */
  /* the 3D stack. If it refers to a die, the active source layer will be    */
  /* printed.                                                                */
  /*                                                                         */
  /* Returns:                                                                */
  /*   0 if the ids is correct                                               */
  /*  -1 if the stack element id floorplan_id does not exist                 */
  /*  -2 if there has been an error when opening the file file_name          */
  /*                                                                         */
  /* The thermal map is printex as a matrix with get_number_of_columns ()    */
  /* columns and get_number_of_rows () rows.                                 */

  int print_thermal_map
  (
    StackDescription* stkd,
    Temperature_t*    temperatures,
    String_t          stack_element_id,
    String_t          file_name
  ) ;

/******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* _3DICE_STACK_DESCRIPTION_H_ */
