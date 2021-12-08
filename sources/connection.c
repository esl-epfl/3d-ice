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

#include "connection.h"
#include "macros.h"

void connection_init (Connection_t *connection)
{
    connection->node1=0;
    connection->node2=0;
    connection->node1_layer=0;
    connection->node2_layer=0;
    connection->direction = 0;
    connection->value=0;
}

void connection_destroy (Connection_t *connection)
{
    connection_init(connection) ;
}

void connection_free (Connection_t *connection)
{
    if (connection == NULL)

        return ;

    connection_destroy (connection) ;

    free (connection) ;
}

void connection_copy (Connection_t *dst, Connection_t *src)
{
  connection_init (dst) ;

  dst->node1 = src->node1;
  dst->node2 = src->node2;
  dst->node1_layer = src->node1_layer;
  dst->node2_layer = src->node2_layer;
  dst->direction = src->direction;
  dst->value = src->value;
}

bool connection_equal (Connection_t *connection, Connection_t *other)
{
  return  connection->node1 == other->node1 &&
         connection->node2 == other->node2 ;
}

void connection_print (Connection_t *connection, FILE *stream, String_t prefix)
{
  fprintf(stream, "%sNode%d in layer %d <-> Node%d in layer %d\n", prefix, connection->node1, connection->node1_layer, connection->node2, connection->node2_layer) ;
  fprintf(stream, "%sDirection %d, Value %f\n", prefix, connection->direction, connection->value) ;
}
