/******************************************************************************
 * This file is part of 3D-ICE, version 3.1.0 .                               *
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
 *                             Copyright (C) 2021                             *
 *   Embedded Systems Laboratory - Ecole Polytechnique Federale de Lausanne   *
 *                            All Rights Reserved.                            *
 *                                                                            *
 * Authors: Arvind Sridhar              Alessandro Vincenzi                   *
 *          Giseong Bak                 Martino Ruggiero                      *
 *          Thomas Brunschwiler         Eder Zulian                           *
 *          Federico Terraneo           Darong Huang                          *
 *          Luis Costero                Marina Zapater                        *
 *          David Atienza                                                     *
 *                                                                            *
 * For any comment, suggestion or request  about 3D-ICE, please  register and *
 * write to the mailing list (see http://listes.epfl.ch/doc.cgi?liste=3d-ice) *
 * Any usage  of 3D-ICE  for research,  commercial or other  purposes must be *
 * properly acknowledged in the resulting products or publications.           *
 *                                                                            *
 * EPFL-STI-IEL-ESL                     Mail : 3d-ice@listes.epfl.ch          *
 * Batiment ELG, ELG 130                       (SUBSCRIPTION IS NECESSARY)    *
 * Station 11                                                                 *
 * 1015 Lausanne, Switzerland           Url  : http://esl.epfl.ch/3d-ice      *
 ******************************************************************************/
#ifndef _CONNECTION_H_
#define _CONNECTION_H_

/*! \file connection.h */

#ifdef __cplusplus
extern "C"
{
#endif

/******************************************************************************/
#include <stdio.h> // For the file type FILE
#include "types.h"
#include "string_t.h"

/******************************************************************************/
  
struct Connection_t
{
    CellIndex_t node1;
    CellIndex_t node1_layer;
    CellIndex_t node2;
    CellIndex_t node2_layer;
    CellIndex_t direction; // connection direction in z (top bottom =0), x (west, east =1), or y (north south=2);
    ChipDimension_t  value; // connection value, it is intercoonect length if two nodes in the same layer, otherwise, it is the area
};

typedef struct Connection_t Connection_t;

/******************************************************************************/
  /** WARNING: We need to define these functions. If we want to change the name,
      we can do it in person_list.c
      The mandatory functions are
      TTT_data_init
      TTT_data_destroy
      TTT_data_copy
      TTT_data_equal
      TTT_data_print
  **/
      
  void connection_init (Connection_t *connection) ;
  void connection_destroy (Connection_t *connection) ;
  void connection_free (Connection_t *connection);
  void connection_copy (Connection_t *dst, Connection_t *src) ;  
  bool connection_equal (Connection_t *connection, Connection_t *other) ;
  void connection_print (Connection_t *connection, FILE *stream, String_t prefix) ;

/******************************************************************************/

#ifdef __cplusplus
}
#endif
#endif    /* _CONNECTION_H_ */
