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
#ifndef _NON_UNIFORM_CELL_H_
#define _NON_UNIFORM_CELL_H_

/*! \file non_uniform_cell.h */

#ifdef __cplusplus
extern "C"
{
#endif

/******************************************************************************/
#include <stdio.h> // For the file type FILE
#include "types.h"
#include "string_t.h"

/******************************************************************************/
  
struct Non_uniform_cell_t
{
    CellIndex_t layer_info;
    ChipDimension_t left_x;
    ChipDimension_t left_y;
    ChipDimension_t length;
    ChipDimension_t width;
    CellIndex_t isChannel;
};

typedef struct Non_uniform_cell_t Non_uniform_cell_t;

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
      
  void non_uniform_cell_init (Non_uniform_cell_t *non_uniform_cell) ;
  void non_uniform_cell_destroy (Non_uniform_cell_t *non_uniform_cell) ;
  void non_uniform_cell_free (Non_uniform_cell_t *non_uniform_cell);
  void non_uniform_cell_copy (Non_uniform_cell_t *dst, Non_uniform_cell_t *src) ;  
  bool non_uniform_cell_equal (Non_uniform_cell_t *non_uniform_cell, Non_uniform_cell_t *other) ;
  void non_uniform_cell_print (Non_uniform_cell_t *non_uniform_cell, FILE *stream, String_t prefix) ;

/******************************************************************************/

#ifdef __cplusplus
}
#endif
#endif    /* _NON_UNIFORM_CELL_H_ */
