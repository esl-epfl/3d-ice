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
#include "conventional_heat_sink.h"

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

    ConventionalHeatSink* ConventionalHeatSink;

    /* The list of stack elements componing the 3Dstack */

    StackElement* StackElementsList ;

    /* Collection of all the dimensions (chip, grid of cells, cell) */

    Dimensions* Dimensions ;

  } StackDescription ;

/******************************************************************************/

  /* Sets all the fields of stkd to a default value                         */

  void init_stack_description (StackDescription* stkd) ;

/******************************************************************************/

  /* Fills the stkd structure parsing the *.stk file                        */
  /*                                                                        */
  /* Returns:                                                               */
  /*                                                                        */
  /* -1 if the .stk file does not exist                                     */
  /*  0 if the parsing succeeds                                             */
  /*  1 if there is an error in the .stk file or in one of the .flp files   */
  /*       used in it (message printed to stderr).                          */
  /*                                                                        */
  /* Note: The function parses the .stk file first and then all the .flp    */
  /* files related to the floorplans following the order used in the stack  */
  /* section (from bottom to top). It stops as soon as it finds an error    */
  /* and it returns 1, discarding the remaining files to parse.             */

  int fill_stack_description (StackDescription* stkd, String_t filename) ;

/******************************************************************************/

  /* Frees all the memory                                                   */

  void free_stack_description (StackDescription* stkd) ;

/******************************************************************************/

  /* Print on stream (stdout, stderr or a given opened output file) all the */
  /* data related to the stkd structure previously filled. Prefix is a      */
  /* string (it can be empty as "") printed as prefix at the beginning of   */
  /* every line.                                                            */
  /*                                                                        */
  /* To print the content of only one of the fields of stkd, use the        */
  /* printing function in the corresponding header file                     */

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

  /* Returns the number of remaining power values (time slots) to emulate   */

  Quantity_t get_number_of_remaining_power_values (StackDescription* stkd) ;

/******************************************************************************/

  /* Given a StackDescription and the id (string) of a floorplan, returns   */
  /* the number of all the floorplan elements in that floorplan.            */
  /* The floorplan_id, is the string used to name the die stack element it  */
  /* belongs to (refers to the the .stk file used to fill stkd)             */
  /*                                                                        */
  /* Returns:                                                               */
  /*                                                                        */
  /*   -1 if the stack element id floorplan_id does not exist               */
  /*   -2 if floorplan_id exists but it is not the id of a die or it        */
  /*      belongs to a die but the floorplan itself does not exist          */
  /* >= 0 the number of floorplan elements in floorplan_id                  */

  Quantity_t get_number_of_floorplan_elements
  (
    StackDescription* stkd,
    String_t floorplan_id
  ) ;

/******************************************************************************/

  /* Given a StackDescription, returns the number of channels outlets in a  */
  /* channel layer (the number of channels is the number of liquid columns  */
  /* and it is the same for every channel layer)                            */

  Quantity_t get_number_of_channel_outlets (StackDescription* stkd) ;

/******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* _3DICE_STACK_DESCRIPTION_H_ */
