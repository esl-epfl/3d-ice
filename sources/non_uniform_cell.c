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
#include <stdlib.h> // For the memory functions malloc/free

#include "non_uniform_cell.h"
#include "macros.h"

void non_uniform_cell_init (Non_uniform_cell_t *non_uniform_cell)
{
    non_uniform_cell->layer_info=0;
    non_uniform_cell->left_x=0;
    non_uniform_cell->left_y=0;
    non_uniform_cell->length=0;
    non_uniform_cell->width=0;
    non_uniform_cell->isChannel=0;
}

void non_uniform_cell_destroy (Non_uniform_cell_t *non_uniform_cell)
{
  non_uniform_cell_init (non_uniform_cell) ;
}

void non_uniform_cell_free (Non_uniform_cell_t *non_uniform_cell)
{
    if (non_uniform_cell == NULL)

        return ;

    non_uniform_cell_destroy (non_uniform_cell) ;

    free (non_uniform_cell) ;
}

void non_uniform_cell_copy (Non_uniform_cell_t *dst, Non_uniform_cell_t *src)
{
  non_uniform_cell_init (dst) ;

  dst->layer_info = src->layer_info;
  dst->left_x = src->left_x;
  dst->left_y = src->left_y;
  dst->length = src->length;
  dst->width = src->width;
  dst->isChannel = src->isChannel;

}

bool non_uniform_cell_equal (Non_uniform_cell_t *non_uniform_cell, Non_uniform_cell_t *other)
{
  return  non_uniform_cell->layer_info == other->layer_info &&
         non_uniform_cell->left_x == other->left_x &&
         non_uniform_cell->left_y == other->left_y &&
         non_uniform_cell->length == other->length &&
         non_uniform_cell->width == other->width &&
         non_uniform_cell->isChannel == other->isChannel;
}

void non_uniform_cell_print (Non_uniform_cell_t *non_uniform_cell, FILE *stream, String_t prefix)
{
  fprintf(stream, "%sLayer info: %d\n", prefix, non_uniform_cell->layer_info) ;
}
