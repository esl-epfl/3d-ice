/******************************************************************************
 * This file is part of 3D-ICE, version 1.0.2 .                               *
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

#include <stdlib.h>
#include <string.h>

#include "die.h"
#include "macros.h"

/******************************************************************************/

void init_die (Die* die)
{
  die->Id          = STRING_I ;
  die->Used        = QUANTITY_I ;
  die->NLayers     = GRIDDIMENSION_I ;
  die->TopLayer    = NULL ;
  die->SourceLayer = NULL ;
  die->BottomLayer = NULL ;
  die->Next        = NULL ;
}

/******************************************************************************/

Die* alloc_and_init_die (void)
{
  Die* die = malloc (sizeof(*die)) ;

  if (die != NULL) init_die (die) ;

  return die ;
}

/******************************************************************************/

void free_die (Die* die)
{
  die->TopLayer = NULL ;
  FREE_POINTER (free_layers_list, die->BottomLayer) ;
  FREE_POINTER (free,             die->Id) ;
  FREE_POINTER (free,             die) ;
}

/******************************************************************************/

void free_dies_list (Die* list)
{
  FREE_LIST (Die, list, free_die) ;
}

/******************************************************************************/

void print_formatted_die
(
  FILE*    stream,
  String_t prefix,
  Die*     die
)
{
  String_t new_prefix_layer
    = malloc (sizeof(*new_prefix_layer) * (10 + strlen(prefix))) ;

  if (new_prefix_layer == NULL) return ;

  sprintf (new_prefix_layer, "%s    layer", prefix) ;

  String_t new_prefix_source
    = malloc (sizeof(*new_prefix_source) * (10 + strlen(prefix))) ;

  sprintf (new_prefix_source, "%s   source", prefix) ;

  if (new_prefix_source == NULL) return ;

  fprintf (stream, "%sdie %s :\n", prefix, die->Id) ;

  FOR_EVERY_ELEMENT_IN_LIST_BACKWARD (Layer, layer, die->TopLayer)
  {
    if (layer == die->SourceLayer)

      print_formatted_layer (stream, new_prefix_source, layer) ;

    else

      print_formatted_layer (stream, new_prefix_layer, layer) ;
  }

  FREE_POINTER (free, new_prefix_layer) ;
  FREE_POINTER (free, new_prefix_source) ;
}

/******************************************************************************/

void print_detailed_die
(
  FILE*    stream,
  String_t prefix,
  Die*     die
)
{
  String_t new_prefix = malloc (sizeof(*new_prefix) * (5 + strlen(prefix))) ;

  if (new_prefix == NULL) return ;

  sprintf (new_prefix, "%s    ", prefix) ;

  fprintf (stream,
           "%sdie                         = %p\n",
           prefix, die) ;

  fprintf (stream,
           "%s  Id                        = %s\n",
           prefix, die->Id) ;

  fprintf (stream,
           "%s  Used                      = %d\n",
           prefix, die->Used) ;

  fprintf (stream,
           "%s  NLayers                   = %d\n",
           prefix, die->NLayers) ;

  fprintf (stream,
           "%s  TopLayer                  = %p\n",
           prefix, die->TopLayer) ;

  fprintf (stream,
           "%s  SourceLayer               = %p\n",
           prefix, die->SourceLayer) ;

  fprintf (stream,
           "%s  BottomLayer               = %p\n",
           prefix, die->BottomLayer) ;

  fprintf (stream,
           "%s  Next                      = %p\n",
           prefix, die->Next) ;

  fprintf (stream, "%s\n", prefix) ;
  print_detailed_layers_list (stream, new_prefix, die->BottomLayer) ;
  fprintf (stream, "%s\n", prefix) ;

  FREE_POINTER (free, new_prefix) ;
}

/******************************************************************************/

void print_formatted_dies_list
(
  FILE*    stream,
  String_t prefix,
  Die*     list
)
{
  FOR_EVERY_ELEMENT_IN_LIST_EXCEPT_LAST (Die, die, list)
  {
    print_formatted_die (stream, prefix, die) ;
    fprintf (stream, "%s\n", prefix) ;
  }
  print_formatted_die (stream, prefix, die) ;
}

/******************************************************************************/

void print_detailed_dies_list
(
  FILE*    stream,
  String_t prefix,
  Die*     list
)
{
  FOR_EVERY_ELEMENT_IN_LIST_EXCEPT_LAST (Die, die, list)
  {
    print_detailed_die (stream, prefix, die) ;
    fprintf (stream, "%s\n", prefix) ;
  }
  print_detailed_die (stream, prefix, die) ;
}

/******************************************************************************/

Die* find_die_in_list (Die* list, String_t id)
{
  FOR_EVERY_ELEMENT_IN_LIST_FORWARD (Die, die, list)

    if (strcmp(die->Id, id) == 0) break ;

  return die ;
}

/******************************************************************************/

void fill_thermal_cell_die
(
  ThermalCell*     thermalcells,
  Time_t           delta_time,
  Dimensions*      dimensions,
  GridDimension_t  layer_index,
  Die*             die
)
{
  FOR_EVERY_ELEMENT_IN_LIST_FORWARD (Layer, layer, die->BottomLayer)

    fill_thermal_cell_layer
    (
      thermalcells,
      delta_time,
      dimensions,
      layer_index + layer->Offset,
      layer
    ) ;
}

/******************************************************************************/

int fill_sources_die
(
  Source_t*             sources,
  Dimensions*           dimensions,
  GridDimension_t       layer_index,
  Floorplan*            floorplan,
  Die*                  die
)
{
#ifdef PRINT_SOURCES
  fprintf (stderr,
    "layer_index = %d\tfill_sources_die %s floorplan %s\n",
    layer_index, die->Id, floorplan->FileName) ;
#endif

  return fill_sources_layer (sources, dimensions,
                             layer_index + die->SourceLayer->Offset,
                             floorplan
#   ifdef PRINT_SOURCES
                            ,die->SourceLayer
#   endif
                            ) ;
}

/******************************************************************************/

void init_power_values_die
(
  Floorplan*            floorplan
)
{
  init_power_values_floorplan
  (
    floorplan
  ) ;
}

/******************************************************************************/

void insert_power_values_die
(
  Floorplan*            floorplan,
  PowersQueue*          pvalues
)
{
  insert_power_values_floorplan
  (
    floorplan,
    pvalues
  ) ;
}

/******************************************************************************/

SystemMatrix fill_system_matrix_die
(
  Die*                  die,
  Dimensions*           dimensions,
  ThermalCell*          thermalcells,
  GridDimension_t       layer_index,
  SystemMatrix          system_matrix
)
{
# ifdef PRINT_SYSTEM_MATRIX
  fprintf (stderr, "(l %2d) fill_system_matrix_die\n", layer_index) ;
# endif

  FOR_EVERY_ELEMENT_IN_LIST_FORWARD (Layer, layer, die->BottomLayer)
  {
    system_matrix = fill_system_matrix_layer
                    (
#                     ifdef PRINT_SYSTEM_MATRIX
                      layer,
#                     endif
                      dimensions, thermalcells,
                      layer_index + layer->Offset,
                      system_matrix
                     ) ;

  }  // FOR_EVERY_ELEMENT_IN_LIST

  return system_matrix ;
}

/******************************************************************************/
