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
/******************************************************************************/
/******************************************************************************/

void
init_layer
(
  Layer *layer
)
{
  layer->Id       = 0    ;
  layer->Height   = 0.0  ;
  layer->Material = NULL ;
  layer->Next     = NULL ;
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

Layer *
alloc_and_init_layer
(
  void
)
{
  Layer *layer = (Layer *) malloc ( sizeof(Layer) ) ;

  if (layer != NULL) init_layer (layer) ;

  return layer ;
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

void
free_layer
(
  Layer *layer
)
{
  free (layer) ;
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

void
free_layers_list
(
  Layer *list
)
{
  Layer *next ;

  for ( ; list != NULL ; list = next)
  {
      next = list->Next ;
      free_layer (list) ;
  }
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

void
print_layer
(
  FILE  *stream,
  char  *prefix,
  Layer *layer
)
{
  fprintf (stream,
    "%sLayer %d\n",            prefix, layer->Id) ;
  fprintf (stream,
    "%s  Height   %5.2f um\n", prefix, layer->Height ) ;
  fprintf (stream,
    "%s  Material %s\n",       prefix, layer->Material->Id ) ;
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

void
print_layers_list
(
  FILE  *stream,
  char  *prefix,
  Layer *list
)
{
  for ( ; list != NULL ; list = list->Next)

    print_layer (stream, prefix, list) ;

}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

Layer *
find_layer_in_list
(
  Layer *list,
  int id
)
{
  for ( ; list != NULL ; list = list->Next)

    if (list->Id == id) break ;

  return list ;
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

LayerPosition_t
get_layer_position
(
  Dimensions *dimensions,
  int        layer
)
{
  if (layer == 0)

    return TL_LAYER_BOTTOM ;

  else if (layer == get_number_of_layers(dimensions) - 1)

    return TL_LAYER_TOP ;

  return TL_LAYER_CENTER ;
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

Resistances *
fill_resistances_layer
(
#ifdef DEBUG_FILL_RESISTANCES
  FILE        *debug,
#endif
  Layer       *layer,
  Resistances *resistances,
  Dimensions  *dimensions,
  int         current_layer
)
{
  int row, column ;

#ifdef DEBUG_FILL_RESISTANCES
  fprintf (debug,
    "%p current_layer = %d\tfill_resistances_layer   %s\n",
    resistances, current_layer, layer->Material->Id) ;
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
      resistances++
    )

      fill_resistances_solid_cell
      (
#ifdef DEBUG_FILL_RESISTANCES
        debug, row, column,
#endif
        resistances,
        dimensions,
        get_cell_length (dimensions, column),
        get_cell_width (dimensions),
        layer->Height,
        layer->Material->ThermalConductivity,
        current_layer
      ) ;

  return resistances ;
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

static
double
capacity (double l, double w, double h, double s, double t)
{
  return (l * w * h * s) / t ;
}

double *
fill_capacities_layer
(
#ifdef DEBUG_FILL_CAPACITIES
  FILE       *debug,
  int        current_layer,
#endif
  Layer      *layer,
  double     *capacities,
  Dimensions *dimensions,
  double     delta_time
)
{
  int row, column ;

#ifdef DEBUG_FILL_CAPACITIES
  fprintf (debug,
    "%p current_layer = %d\tfill_capacities_layer   %s\n",
    capacities, current_layer, layer->Material->Id) ;
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
                      layer->Material->SpecificHeat,
                      delta_time
                    ) ;

#ifdef DEBUG_FILL_CAPACITIES
      fprintf (debug,
        "%p solid cell l %5d r %5d c %5d l %5.2f w %5.2f h %5.2f " \
        "sh %.5e %.5e --> %.5e\n",
        capacities, current_layer, row, column,
        get_cell_length(dimensions, column),
        get_cell_width (dimensions, column),
        layer->Height, layer->Material->SpecificHeat, delta_time, *capacities) ;
#endif
    }

  return capacities ;
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

double *
fill_sources_active_layer
(
#ifdef DEBUG_FILL_SOURCES
  FILE       *debug,
  int        current_layer,
  Layer      *layer,
#endif
  Floorplan  *floorplan,
  double     *sources,
  Dimensions *dimensions
)
{
  int              row, column ;
  double           flp_el_surface ;
  FloorplanElement *flp_el ;

#ifdef DEBUG_FILL_SOURCES
  fprintf (debug,
    "%p current_layer = %d\tfill_sources_active_layer   %s\n",
    sources, current_layer, layer->Material->Id) ;
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

          = (flp_el->PowerValue * get_cell_top_surface (dimensions, column))
            / flp_el_surface ;

#ifdef DEBUG_FILL_SOURCES
        fprintf (debug,
          "cell l %5d r %5d c %5d (%6d)\t%s %.5e -> %.5e\n",
          current_layer, row, column,
          get_cell_offset_in_stack (dimensions, current_layer, row, column),
          flp_el->Id, flp_el->PowerValue,
          sources [get_cell_offset_in_layer (dimensions, row, column)]) ;
#endif

      }

  }

  return sources + get_layer_area (dimensions) ;
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

double *
fill_sources_empty_layer
(
#ifdef DEBUG_FILL_SOURCES
  FILE       *debug,
  int        current_layer,
  Layer      *layer,
#endif
  double     *sources,
  Dimensions *dimensions
)
{
#ifdef DEBUG_FILL_SOURCES
  fprintf (debug,
    "%p current_layer = %d\tfill_sources_empty_layer   %s\n",
    sources, current_layer, layer->Material->Id) ;
#endif

  return sources + get_layer_area (dimensions) ;
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

int
fill_system_matrix_layer
(
#ifdef DEBUG_FILL_SYSTEM_MATRIX
  FILE        *debug,
  Layer       *layer,
#endif
  Dimensions  *dimensions,
  Resistances *resistances,
  double      *capacities,
  int         *columns,
  int         *rows,
  double      *values,
  int         current_layer
)
{
  int row, column, added, tot_added ;

#ifdef DEBUG_FILL_SYSTEM_MATRIX
  fprintf (debug,
    "%p %p %p %p %p (l %2d) fill_system_matrix_layer %s\n",
    resistances, capacities, columns, rows, values,
    current_layer, layer->Material->Id) ;
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
      resistances ++ ,
      capacities  ++ ,
      columns     ++ ,
      rows        += added ,
      values      += added ,
      tot_added   += added ,
      column      ++
    )

      added = add_solid_column
              (
#ifdef DEBUG_FILL_SYSTEM_MATRIX
                debug,
#endif
                dimensions, resistances, capacities,
                current_layer, row, column,
                columns, rows, values
              ) ;

  return tot_added ;
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
