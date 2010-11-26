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

#include <stdlib.h>

#include "layer.h"
#include "macros.h"

/******************************************************************************/

void init_layer (Layer* layer)
{
  layer->Height   = CELLDIMENSION_I ;
  layer->Offset   = GRIDDIMENSION_I ;
  layer->Material = NULL ;
  layer->Next     = NULL ;
  layer->Prev     = NULL ;
}

/******************************************************************************/

Layer* alloc_and_init_layer (void)
{
  Layer* layer = malloc (sizeof(*layer));

  if (layer != NULL) init_layer (layer) ;

  return layer ;
}

/******************************************************************************/

void free_layer (Layer* layer)
{
  free (layer) ;
}

/******************************************************************************/

void free_layers_list (Layer* list)
{
  FREE_LIST (Layer, list, free_layer) ;
}

/******************************************************************************/

void print_formatted_layer
(
  FILE*    stream,
  String_t prefix,
  Layer*   layer
)
{
  fprintf (stream,
    "%s %7.1f  %s ;\n",
    prefix, layer->Height, layer->Material->Id) ;
}

/******************************************************************************/

void print_detailed_layer
(
  FILE*    stream,
  String_t prefix,
  Layer*   layer
)
{
  fprintf (stream,
           "%slayer                   = %p\n",
           prefix, layer) ;

  fprintf (stream,
           "%s  Height                = %.1f\n",
           prefix, layer->Height) ;

  fprintf (stream,
           "%s  Offset                = %d\n",
           prefix, layer->Offset) ;

  fprintf (stream,
           "%s  Material              = %p\n",
           prefix, layer->Material) ;

  fprintf (stream,
           "%s  Next                  = %p\n",
           prefix, layer->Next) ;

  fprintf (stream,
           "%s  Prev                  = %p\n",
           prefix, layer->Prev) ;
}

/******************************************************************************/

void print_formatted_layers_list (FILE* stream, String_t prefix, Layer* list)
{
  FOR_EVERY_ELEMENT_IN_LIST_FORWARD (Layer, layer, list)

    print_formatted_layer (stream, prefix, layer) ;
}

/******************************************************************************/

void print_detailed_layers_list (FILE* stream, String_t prefix, Layer* list)
{
  FOR_EVERY_ELEMENT_IN_LIST_EXCEPT_LAST (Layer, layer, list)
  {
    print_detailed_layer (stream, prefix, layer) ;
    fprintf (stream, "%s\n", prefix) ;
  }
  print_detailed_layer (stream, prefix, layer) ;
}

/******************************************************************************/

void fill_thermal_cell_layer
(
  ThermalCell*     thermalcells,
  Time_t           delta_time,
  Dimensions*      dimensions,
  GridDimension_t  layer_index,
  Layer*           layer
)
{
  void (*fill_cell)
  (
#   ifdef PRINT_THERMAL_CELLS
    Dimensions*           dimensions,
    GridDimension_t       layer_index,
    GridDimension_t       row_index,
    GridDimension_t       column_index,
#   endif
    ThermalCell*          thermalcell,
    CellDimension_t       cell_length,
    CellDimension_t       cell_width,
    CellDimension_t       cell_height,
    SolidTC_t             thermal_conductivity,
    SolidVHC_t            volumetric_heat_capacity,
    Time_t                delta_time
  ) ;

  if (IS_FIRST_LAYER(layer_index))

    fill_cell = fill_solid_cell_bottom ;

  else if (IS_LAST_LAYER(layer_index, dimensions))

    fill_cell = fill_solid_cell_top ;

  else

    fill_cell = fill_solid_cell_central ;

  thermalcells += get_cell_offset_in_stack (dimensions, layer_index, 0, 0) ;

  FOR_EVERY_ROW (row_index, dimensions)
  {
    FOR_EVERY_COLUMN (column_index, dimensions)
    {
      fill_cell
      (
#   ifdef PRINT_THERMAL_CELLS
        dimensions,
        layer_index, row_index, column_index,
#   endif
        thermalcells,
        get_cell_length(dimensions, column_index),
        get_cell_width(dimensions),
        layer->Height,
        layer->Material->ThermalConductivity,
        layer->Material->VolumetricHeatCapacity,
        delta_time
      ) ;

      thermalcells ++ ;
    }
  }
}

/******************************************************************************/

void fill_sources_layer
(
  Source_t*             sources,
  Dimensions*           dimensions,
  GridDimension_t       layer_index,
  Floorplan*            floorplan
# ifdef PRINT_SOURCES
  ,Layer*               layer
# endif
)
{
#ifdef PRINT_SOURCES
  fprintf (stderr,
           "layer_index = %d\tfill_sources_source_layer   %s\n",
           layer_index, layer->Material->Id) ;
#endif

  sources += get_cell_offset_in_stack (dimensions, layer_index, 0, 0) ;

  fill_sources_floorplan
  (
#ifdef PRINT_SOURCES
    layer_index,
#endif
    sources, dimensions, floorplan
  ) ;
}

/******************************************************************************/

SystemMatrix fill_system_matrix_layer
(
# ifdef PRINT_SYSTEM_MATRIX
  Layer*                layer,
# endif
  Dimensions*           dimensions,
  ThermalCell*          thermalcells,
  GridDimension_t       layer_index,
  SystemMatrix          system_matrix
)
{
#ifdef PRINT_SYSTEM_MATRIX
  fprintf (stderr,
    "(l %d) fill_system_matrix_layer %s\n",
    layer_index, layer->Material->Id) ;
#endif

  FOR_EVERY_ROW (row_index, dimensions)
  {
    FOR_EVERY_COLUMN (column_index, dimensions)
    {

      system_matrix = add_solid_column
                      (
                        dimensions, thermalcells,
                        layer_index, row_index, column_index,
                        system_matrix
                      ) ;

    } // FOR_EVERY_COLUMN
  } // FOR_EVERY_ROW

  return system_matrix ;
}

/******************************************************************************/
