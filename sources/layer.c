/******************************************************************************
 *                                                                            *
 * Source file "Sources/layer.c"                                              *
 *                                                                            *
 * EPFL-STI-IEL-ESL                                                           *
 * Bâtiment ELG, ELG 130                                                      *
 * Station 11                                                                 *
 * 1015 Lausanne, Switzerland                    alessandro.vincenzi@epfl.ch  *
 ******************************************************************************/

#include <stdlib.h>

#include "layer.h"
#include "system_matrix.h"

/******************************************************************************/

void init_layer (Layer *layer)
{
  layer->Height       = 0.0  ;
  layer->LayersOffset = 0    ;
  layer->Material     = NULL ;
  layer->Next         = NULL ;
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

void free_layers_list (Layer *list)
{
  Layer* next ;

  for ( ; list != NULL ; list = next)
  {
      next = list->Next ;
      free_layer (list) ;
  }
}

/******************************************************************************/

void print_layer (FILE* stream, String_t prefix, Layer* layer)
{
  fprintf (stream,
    "%sLayer #%d height %5.2f um, material %s\n",
    prefix, layer->LayersOffset, layer->Height, layer->Material->Id) ;
}

/******************************************************************************/

void print_layers_list (FILE* stream, String_t prefix, Layer* list)
{
  for ( ; list != NULL ; list = list->Next)

    print_layer (stream, prefix, list) ;
}

/******************************************************************************/

Conductances*   fill_conductances_layer
(
  Layer*               layer,
  Conductances*        conductances,
  Dimensions*          dimensions,
  EnvironmentHeatSink* environmentheatsink,
  LayerIndex_t         current_layer
)
{
  RowIndex_t    row ;
  ColumnIndex_t column ;

  void (*fill_conductances)
  (
#   ifdef PRINT_CONDUCTANCES
    Dimensions*            dimensions,
    LayerIndex_t           current_layer,
    RowIndex_t             current_row,
    ColumnIndex_t          current_column,
#   endif
    Conductances*          conductances,
    CellDimension_t        cell_length,
    CellDimension_t        cell_width,
    CellDimension_t        cell_height,
    ThermalConductivity_t  thermal_conductivity,
    EnvironmentHTC_t       environment_heat_transfer_coefficient
  );

  if (current_layer == 0)
  {
    fill_conductances = &fill_conductances_bottom_solid_cell;
  }
  else if (current_layer == get_number_of_layers(dimensions) - 1)
  {
    if (environmentheatsink == NULL)

      fill_conductances = &fill_conductances_top_solid_cell;

    else

      fill_conductances = &fill_conductances_top_solid_cell_ehtc;

  }
  else
  {
    fill_conductances = &fill_conductances_central_solid_cell;
  }

#ifdef PRINT_CONDUCTANCES
  fprintf (stderr, "fill_conductances_layer %s\n", layer->Material->Id) ;
#endif

  for
  (
    row = 0 ;
    row < get_number_of_rows (dimensions) ;
    row++
  )

    for
    (
      column = 0 ;
      column < get_number_of_columns (dimensions) ;
      column++, conductances++
    )

      fill_conductances
      (
#ifdef PRINT_CONDUCTANCES
        dimensions,
        current_layer, row, column,
#endif
        conductances,
        get_cell_length (dimensions, column),
        get_cell_width  (dimensions),
        layer->Height,
        layer->Material->ThermalConductivity,
        environmentheatsink->EnvironmentHTC
      ) ;

  return conductances ;
}

/******************************************************************************/

static
double
capacity (double l, double w, double h, double s, double t)
{
  return (l * w * h * s) / t ;
}

Capacity_t*    fill_capacities_layer
(
#ifdef PRINT_CAPACITIES
  LayerIndex_t current_layer,
#endif
  Layer*       layer,
  Capacity_t*  capacities,
  Dimensions*  dimensions,
  Time_t       delta_time
)
{
  RowIndex_t    row ;
  ColumnIndex_t column ;

#ifdef PRINT_CAPACITIES
  fprintf
  (
    stderr,
    "current_layer = %d\tfill_capacities_layer %s\n",
    current_layer,
    layer->Material->Id
  ) ;
#endif

  for
  (
    row = 0 ;
    row < get_number_of_rows (dimensions) ;
    row++
  )

    for
    (
      column = 0 ;
      column < get_number_of_columns (dimensions) ;
      column++,
      capacities++
    )
    {
      *capacities = capacity
                    (
                      get_cell_length (dimensions, column),
                      get_cell_width (dimensions),
                      layer->Height,
                      layer->Material->VolHeatCapacity,
                      delta_time
                    ) ;

#ifdef PRINT_CAPACITIES
      fprintf
      (
        stderr,
        "solid cell   |  l %2d r %4d c %4d [%6d] |  l %5.2f w %5.2f h %5.2f "   \
                    " |  vhc %.5e --> %.5e\n",
        current_layer, row, column,
        get_cell_offset_in_stack (dimensions, current_layer, row, column),
        get_cell_length(dimensions, column),
        get_cell_width (dimensions),
        layer->Height, layer->Material->VolHeatCapacity,
        *capacities
      ) ;
#endif
    }

  return capacities ;
}

/******************************************************************************/

Source_t*              fill_sources_active_layer
(
# ifdef PRINT_SOURCES
  LayerIndex_t         current_layer,
  Layer*               layer,
# endif
  struct Floorplan*    floorplan,
  Source_t*            sources,
  Dimensions*          dimensions
)
{
  int              row, column ;
  double           flp_el_surface ;
  struct FloorplanElement *flp_el ;

#ifdef PRINT_SOURCES
  fprintf
  (
    stderr,
    "current_layer = %d\tfill_sources_source_layer   %s\n",
    current_layer, layer->Material->Id
  ) ;
#endif

  for
  (
    flp_el  = floorplan->ElementsList ;
    flp_el != NULL ;
    flp_el  = flp_el->Next
  )
  {
    flp_el_surface = (double) (flp_el->Length * flp_el->Width) ;

    for
    (
      row = flp_el->SW_Row ;
      row <= flp_el->NE_Row ;
      row++
    )

      for
      (
        column = flp_el->SW_Column ;
        column < flp_el->NE_Column ;
        column++
      )
      {
        sources [get_cell_offset_in_layer (dimensions, row, column)]

          = (
               get_from_powers_queue(flp_el->PowerValues)
               * get_cell_top_surface (dimensions, column)
            )
            /  flp_el_surface ;

#ifdef PRINT_SOURCES
        fprintf
        (
          stderr,
          "solid  cell  |  l %2d r %4d c %4d [%6d] | %s %.5e -> %.5e\n",
          current_layer, row, column,
          get_cell_offset_in_stack (dimensions, current_layer, row, column),
          flp_el->Id,
          get_from_powers_queue(flp_el->PowerValues),
          sources [get_cell_offset_in_layer (dimensions, row, column)]
        ) ;
#endif

      }

  }

  return sources + get_layer_area (dimensions) ;
}

/******************************************************************************/

Source_t*              fill_sources_empty_layer
(
# ifdef PRINT_SOURCES
  LayerIndex_t         current_layer,
  Layer*               layer,
# endif
  Source_t*            sources,
  Dimensions*          dimensions
)
{
#ifdef PRINT_SOURCES
  fprintf (stderr,
    "current_layer = %d\tfill_sources_empty_layer    %s\n",
    current_layer, layer->Material->Id) ;
#endif

  return sources + get_layer_area (dimensions) ;
}

/******************************************************************************/

int                    fill_ccs_system_matrix_layer
(
# ifdef PRINT_SYSTEM_MATRIX
  Layer*               layer,
# endif
  Dimensions*          dimensions,
  struct Conductances* conductances,
  Capacity_t*          capacities,
  LayerIndex_t         current_layer,
  int*                 columns,
  int*                 rows,
  double*              values
)
{
  int row, column, added, tot_added ;

#ifdef PRINT_SYSTEM_MATRIX
  fprintf
  (
    stderr,
    "(l %2d) fill_ccs_system_matrix_layer %s\n",
    current_layer,
    layer->Material->Id
  ) ;
#endif

  for
  (
    tot_added = 0,
    row = 0 ;
    row < get_number_of_rows (dimensions) ;
    row++
  )

    for
    (
      column = 0 ;
      column < get_number_of_columns (dimensions) ;
      conductances ++ ,
      capacities   ++ ,
      columns      ++ ,
      rows         += added ,
      values       += added ,
      tot_added    += added ,
      column       ++
    )

      added = add_ccs_solid_column
              (
                dimensions, conductances, capacities,
                current_layer, row, column,
                columns, rows, values
              ) ;

  return tot_added ;
}

/******************************************************************************/

int                    fill_crs_system_matrix_layer
(
# ifdef PRINT_SYSTEM_MATRIX
  Layer*               layer,
# endif
  Dimensions*          dimensions,
  struct Conductances* conductances,
  Capacity_t*          capacities,
  EnvironmentHeatSink* environmentheatsink,
  LayerIndex_t         current_layer,
  int*                 rows,
  int*                 columns,
  double*              values
)
{
  int row, column, added, tot_added ;

#ifdef PRINT_SYSTEM_MATRIX
  fprintf
  (
    stderr,
    "(l %2d) fill_crs_system_matrix_layer %s\n",
    current_layer,
    layer->Material->Id
  ) ;
#endif

  for
  (
    tot_added = 0,
    row = 0 ;
    row < get_number_of_rows (dimensions) ;
    row++
  )

    for
    (
      column = 0 ;
      column < get_number_of_columns (dimensions) ;
      conductances ++ ,
      capacities   ++ ,
      rows         ++ ,
      columns      += added ,
      values       += added ,
      tot_added    += added ,
      column       ++
    )

      added = add_crs_solid_column
              (
                dimensions, conductances, capacities,
                environmentheatsink,
                current_layer, row, column,
                rows, columns, values
              ) ;

  return tot_added ;
}

/******************************************************************************/
