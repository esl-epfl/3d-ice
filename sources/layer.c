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
    STRING_F " " CELLDIMENSION_F "  " STRING_F "\n",
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
           STRING_F "layer           = %p\n",
           prefix, layer) ;

  fprintf (stream,
           STRING_F "layer->Height   = " CELLDIMENSION_F "\n",
           prefix, layer->Height) ;

  fprintf (stream,
           STRING_F "layer->Offset   = " GRIDDIMENSION_F "\n",
           prefix, layer->Offset) ;

  fprintf (stream,
           STRING_F "layer->Material = %p\n",
           prefix, layer->Material) ;

  fprintf (stream,
           STRING_F "layer->Next     = %p\n",
           prefix, layer->Next) ;
}

/******************************************************************************/

void print_formatted_layers_list (FILE* stream, String_t prefix, Layer* list)
{
  FOR_EVERY_ELEMENT_IN_LIST (Layer, layer, list)

    print_formatted_layer (stream, prefix, layer) ;
}

/******************************************************************************/

void print_detailed_layers_list (FILE* stream, String_t prefix, Layer* list)
{
  FOR_EVERY_ELEMENT_IN_LIST_EXCEPT_LAST (Layer, layer, list)
  {
    print_detailed_layer (stream, prefix, layer) ;
    fprintf (stream, STRING_F "\n", prefix) ;
  }
  print_detailed_layer (stream, prefix, layer) ;
}

/******************************************************************************/

void fill_thermal_grid_data_layer
(
  ThermalGridData* thermalgriddata,
  GridDimension_t  layer_index,
  Layer*           layer
)
{
# ifdef PRINT_THERMAL_GRID_DATA
  fprintf (stderr,
           "\n#" GRIDDIMENSION_F "\tLayer   [" STRING_F "]\n\n",
           layer_index, layer->Material->Id) ;
# endif

  fill_thermal_grid_data
  (
    thermalgriddata,
    layer_index,
    TDICE_LAYER_SOLID,
    layer->Material->ThermalConductivity,
    layer->Material->VolumetricHeatCapacity,
    layer->Height
  ) ;
}

/******************************************************************************/

Source_t* fill_sources_active_layer
(
# ifdef PRINT_SOURCES
  Layer*                layer,
# endif
  GridDimension_t       layer_index,
  ConventionalHeatSink* conventionalheatsink,
  ThermalGridData*      thermalgriddata,
  Floorplan*            floorplan,
  Source_t*             sources,
  Dimensions*           dimensions
)
{
  CellDimension_t flp_el_surface = CELLDIMENSION_I ;
  Power_t         power          = POWER_I ;

#ifdef PRINT_SOURCES
  fprintf (stderr,
           "layer_index = " GRIDDIMENSION_F \
           "\tfill_sources_source_layer   " STRING_F "\n",
           layer_index, layer->Material->Id) ;
#endif

  FOR_EVERY_ELEMENT_IN_LIST (FloorplanElement, flp_el, floorplan->ElementsList)
  {
    flp_el_surface
      = (CellDimension_t) (flp_el->EffectiveLength * flp_el->EffectiveWidth) ;

    power = get_from_powers_queue(flp_el->PowerValues) ;

    FOR_EVERY_FLOORPLAN_ELEMENT_ROW (row_index, flp_el)
    {
      FOR_EVERY_FLOORPLAN_ELEMENT_COLUMN (column_index, flp_el)
      {
        sources [get_cell_offset_in_layer (dimensions, row_index, column_index)]

          = (
               power * get_cell_length (dimensions, column_index)
                     * get_cell_width (dimensions)
            )
            /  flp_el_surface ;

#ifdef PRINT_SOURCES
        fprintf (stderr,
          "solid  cell  | l " GRIDDIMENSION_F     \
                        " r " GRIDDIMENSION_F     \
                        " c " GRIDDIMENSION_F     \
                        " ["  GRIDDIMENSION_F "]" \
                      " | l " CELLDIMENSION_F     \
                        " w " CELLDIMENSION_F     \
                      " | " SOURCE_F " [source] = ("
                            POWER_F " [W] * l * w) / "
                            CELLDIMENSION_F " | "
                            STRING_F "\n",
          layer_index, row_index, column_index,
          get_cell_offset_in_stack (dimensions, layer_index, row_index, column_index),
          get_cell_length (dimensions, column_index), get_cell_width (dimensions),
          sources [get_cell_offset_in_layer (dimensions, row_index, column_index)],
          get_from_powers_queue(flp_el->PowerValues), flp_el_surface,
          flp_el->Id) ;
#endif

      } // FOR_EVERY_FLOORPLAN_ELEMENT_COLUMN
    } // FOR_EVERY_FLOORPLAN_ELEMENT_ROW

    pop_from_powers_queue (flp_el->PowerValues) ;

  } // FOR_EVERY_ELEMENT_IN_LIST

  if ( IS_LAST_LAYER (layer_index, dimensions)
       && conventionalheatsink != NULL )

    add_sources_conventional_heat_sink
    (
      conventionalheatsink,
      dimensions,
      sources,
      thermalgriddata,
      layer_index
    ) ;

  return sources + get_layer_area (dimensions) ;
}

/******************************************************************************/

Source_t* fill_sources_empty_layer
(
# ifdef PRINT_SOURCES
  Layer*                layer,
# endif
  GridDimension_t       layer_index,
  ConventionalHeatSink* conventionalheatsink,
  ThermalGridData*      thermalgriddata,
  Source_t*             sources,
  Dimensions*           dimensions
)
{
#ifdef PRINT_SOURCES
  fprintf (stderr,
    "layer_index = " GRIDDIMENSION_F \
    "\tfill_sources_empty_layer    " STRING_F "\n",
    layer_index, layer->Material->Id) ;
#endif

  if ( layer_index == (get_number_of_layers(dimensions) - 1)
       && conventionalheatsink != NULL )

    fill_sources_conventional_heat_sink
    (
      conventionalheatsink,
      dimensions,
      sources,
      thermalgriddata,
      layer_index
    ) ;

  return sources + get_layer_area (dimensions) ;
}

/******************************************************************************/

SystemMatrix fill_system_matrix_layer
(
# ifdef PRINT_SYSTEM_MATRIX
  Layer*                layer,
# endif
  Dimensions*           dimensions,
  ThermalGridData*      thermalgriddata,
  GridDimension_t       layer_index,
  SystemMatrix          system_matrix
)
{
#ifdef PRINT_SYSTEM_MATRIX
  fprintf (stderr,
    "(l " GRIDDIMENSION_F ") fill_system_matrix_layer " STRING_F "\n",
    layer_index, layer->Material->Id) ;
#endif

  FOR_EVERY_ROW (row_index, dimensions)
  {
    FOR_EVERY_COLUMN (column_index, dimensions)
    {

      system_matrix = add_solid_column
                      (
                        dimensions, thermalgriddata,
                        layer_index, row_index, column_index,
                        system_matrix
                      ) ;

    } // FOR_EVERY_COLUMN
  } // FOR_EVERY_ROW

  return system_matrix ;
}

/******************************************************************************/
