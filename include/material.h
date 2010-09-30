/******************************************************************************
 * This file is part of 3D-ICE, version 1.0 .                                 *
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
 * For any comment, suggestion or request  about 3D-ICE, please  register and *
 * write to the mailing list (see http://listes.epfl.ch/doc.cgi?liste=3d-ice) *                                                                            *
 *                                                                            *
 * EPFL-STI-IEL-ESL                                                           *
 * Batiment ELG, ELG 130                Mail : 3d-ice@listes.epfl.ch          *
 * Station 11                                  (SUBSCRIPTION IS NECESSARY)    *
 * 1015 Lausanne, Switzerland           Url  : http://esl.epfl.ch/3d-ice.html *
 ******************************************************************************/

#ifndef _3DICE_MATERIAL_H_
#define _3DICE_MATERIAL_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdio.h>

#include "types.h"

/******************************************************************************/

  struct Material
  {
    /* The Id of the material */

    String_t Id ;

    /* For parsing purpose */

    Quantity_t Used ;

    /* The volume-specific heat capacity [ J / ( um3 * K ) ] */

    VolHeatCapacity_t VolHeatCapacity ;

    /* The thermal conductivity [ W / ( um * K ) ] */

    ThermalConductivity_t ThermalConductivity ;

    /* To collect materials in a linked list */

    struct Material* Next ;

  };

  typedef struct Material Material;

/******************************************************************************/

  /* Given a valid address of a Material structure,  */
  /* sets all its fields to a default value.         */

  void init_material (Material* material) ;

/******************************************************************************/

  /* Allocates a Material structure and sets its     */
  /* fields to their default values. Returns a valid */
  /* Material address if the allocation succeed or   */
  /* NULL if it fails.                               */

  Material* alloc_and_init_material (void) ;

/******************************************************************************/

  /* Given the address of a Material structure,      */
  /* frees both the Id string and the memory pointed */
  /* by the address received.                        */

  void free_material (Material* material) ;

/******************************************************************************/

  /* Given the address of a Material structure,      */
  /* frees the material pointed by this address and  */
  /* all the materials it finds following the linked */
  /* list (see Next field).                          */

  void free_materials_list (Material* list) ;

/******************************************************************************/

  /* Prints on the stream the content of the fields  */
  /* of the material. "prefix" is a string (at least */
  /* empty and null terminated) that is printed in   */
  /* every line before the description.              */

  void print_material (FILE* stream, String_t prefix, Material* material) ;

/******************************************************************************/

  /* Prints on the stream the content of the fields  */
  /* of all the materials it finds following the     */
  /* linked list (see Next field). "prefix" is       */
  /* printed for every material in the list.         */

  void print_materials_list (FILE* stream, String_t prefix, Material* list) ;

/******************************************************************************/

  /* Id based search of a Material structure in a    */
  /* material list. Returns the address of a         */
  /* founded structure or NULL if it does not find   */
  /* it.                                             */

  Material* find_material_in_list (Material* list, String_t id) ;

/******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* _3DICE_MATERIAL_H_ */
