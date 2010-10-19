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
  Layer* layer = (Layer*) malloc (sizeof (Layer)) ;

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

void print_layer (FILE* stream, String_t prefix, Layer* layer)
{
  fprintf (stream,
    "%sLayer #%d height %5.2f um, material %s\n",
    prefix, layer->Offset, layer->Height, layer->Material->Id) ;
}

/******************************************************************************/

void print_layers_list (FILE* stream, String_t prefix, Layer* list)
{
  FOR_EVERY_ELEMENT_IN_LIST (Layer, layer, list)

    print_layer (stream, prefix, layer) ;
}

/******************************************************************************/

Conductances*   fill_conductances_layer
(
  Layer*                layer,
  Conductances*         conductances,
  Dimensions*           dimensions,
  ConventionalHeatSink* conventionalheatsink,
  GridDimension_t       layer_index
)
{
  void (*fill_conductances)
  (
#   ifdef PRINT_CONDUCTANCES
    Dimensions*            dimensions,
    GridDimension_t        layer_index,
    GridDimension_t        row_index,
    GridDimension_t        column_index,
#   endif
    Conductances*          conductances,
    CellDimension_t        cell_length,
    CellDimension_t        cell_width,
    CellDimension_t        cell_height,
    SolidTC_t              thermal_conductivity,
    AmbientHTC_t           ambient_htc
  ) ;

  if ( IS_FIRST_LAYER(layer_index) )

    fill_conductances = &fill_conductances_bottom_solid_cell ;

  else if ( IS_LAST_LAYER (layer_index, dimensions) )

    if (conventionalheatsink == NULL)

      fill_conductances = &fill_conductances_top_solid_cell ;

    else

      fill_conductances = &fill_conductances_top_solid_cell_ehtc ;

  else

    fill_conductances = &fill_conductances_central_solid_cell ;


#ifdef PRINT_CONDUCTANCES
  fprintf (stderr, "fill_conductances_layer %s\n", layer->Material->Id) ;
#endif

  FOR_EVERY_ROW (row_index, dimensions)
  {
    FOR_EVERY_COLUMN (column_index, dimensions)
    {
      fill_conductances
      (
#ifdef PRINT_CONDUCTANCES
        dimensions,
        layer_index, row_index, column_index,
#endif
        conductances,
        get_cell_length (dimensions, column_index),
        get_cell_width  (dimensions),
        layer->Height,
        layer->Material->ThermalConductivity,
        conventionalheatsink == NULL ? (AmbientHTC_t) 0
                                     : conventionalheatsink->AmbientHTC
      ) ;

      conductances++ ;

    } // FOR_EVERY_COLUMN
  } // FOR_EVERY_ROW

  return conductances ;
}

/******************************************************************************/

Capacity_t* fill_capacities_layer
(
#ifdef PRINT_CAPACITIES
  GridDimension_t layer_index,
#endif
  Layer*          layer,
  Capacity_t*     capacities,
  Dimensions*     dimensions,
  Time_t          delta_time
)
{
#ifdef PRINT_CAPACITIES
  fprintf (stderr,
    "layer_index = %d\tfill_capacities_layer %s\n",
    layer_index, layer->Material->Id) ;
#endif

  FOR_EVERY_ROW (row_index, dimensions)
  {
    FOR_EVERY_COLUMN (column_index, dimensions)
    {
      *capacities = CAPACITY
                    (
                      get_cell_length (dimensions, column_index),
                      get_cell_width (dimensions),
                      layer->Height,
                      layer->Material->VolumetricHeatCapacity,
                      delta_time
                    ) ;

#ifdef PRINT_CAPACITIES
      fprintf (stderr,
          "solid cell   |  l %2d r %4d c %4d [%6d] |  l %5.2f w %5.2f h %5.2f " \
                      " |  %.5e [capacity] = (l x w x h x %.5e [        vhc]) / %.5e [delta] |\n",
        layer_index, row_index, column_index,
        get_cell_offset_in_stack (dimensions, layer_index, row_index, column_index),
        get_cell_length(dimensions, column_index),
        get_cell_width (dimensions),
        layer->Height, *capacities, layer->Material->VolumetricHeatCapacity, delta_time
      ) ;
#endif

      capacities++ ;

    } // FOR_EVERY_COLUMN
  } // FOR_EVERY_ROW

  return capacities ;
}

/******************************************************************************/

Source_t* fill_sources_active_layer
(
# ifdef PRINT_SOURCES
  Layer*                layer,
# endif
  GridDimension_t       layer_index,
  ConventionalHeatSink* conventionalheatsink,
  Conductances*         conductances,
  Floorplan*            floorplan,
  Source_t*             sources,
  Dimensions*           dimensions
)
{
  CellDimension_t flp_el_surface = CELLDIMENSION_I ;
  Power_t         power          = POWER_I ;

#ifdef PRINT_SOURCES
  fprintf (stderr,
    "layer_index = %d\tfill_sources_source_layer   %s\n",
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
          "solid  cell  | l %2d r %4d c %4d [%6d] "
                       "| l %5.2f w %5.2f "  \
                       "| %.5e [source] = (%.5e [W] * l * w) / %.5e | %s\n",
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
      conductances,
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
  Conductances*         conductances,
  Source_t*             sources,
  Dimensions*           dimensions
)
{
#ifdef PRINT_SOURCES
  fprintf (stderr,
    "layer_index = %d\tfill_sources_empty_layer    %s\n",
    layer_index, layer->Material->Id) ;
#endif

  if ( layer_index == (get_number_of_layers(dimensions) - 1)
       && conventionalheatsink != NULL )

    fill_sources_conventional_heat_sink
    (
      conventionalheatsink,
      dimensions,
      sources,
      conductances,
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
  Conductances*         conductances,
  Capacity_t*           capacities,
  ConventionalHeatSink* conventionalheatsink,
  GridDimension_t       layer_index,
  SystemMatrix          system_matrix
)
{
#ifdef PRINT_SYSTEM_MATRIX
  fprintf (stderr,
    "(l %2d) fill_system_matrix_layer %s\n",
    layer_index, layer->Material->Id) ;
#endif

  FOR_EVERY_ROW (row_index, dimensions)
  {
    FOR_EVERY_COLUMN (column_index, dimensions)
    {

      system_matrix = add_solid_column
                      (
                        dimensions, conductances, capacities,
                        conventionalheatsink,
                        layer_index, row_index, column_index,
                        system_matrix
                      ) ;

      conductances    ++ ;
      capacities      ++ ;

    } // FOR_EVERY_COLUMN
  } // FOR_EVERY_ROW

  return system_matrix ;
}

/******************************************************************************/