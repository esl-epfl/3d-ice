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

void init_layer (Layer *layer)
{
  if (layer == NULL) return ;

  layer->Id       = 0 ;
  layer->Height   = 0.0 ;
  layer->Material = NULL ;
  layer->Next     = NULL ;
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

Layer *
alloc_and_init_layer (void)
{
  Layer *layer = (Layer *) malloc ( sizeof(Layer) ) ;

  init_layer (layer) ;

  return layer ;
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

void
free_layer (Layer *layer)
{
  if (layer == NULL) return ;

  free (layer) ;
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

void
free_layers_list (Layer *list)
{
  Layer *next_layer ;

  for (; list != NULL; list = next_layer)
  {
      next_layer = list->Next ;

      free_layer (list) ;
  }
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

void print_layer (FILE *stream, char* prefix, Layer *layer)
{
  fprintf (stream, "%sLayer %d\n",            prefix, layer->Id) ;
  fprintf (stream, "%s  Height   %5.2f um\n", prefix, layer->Height ) ;
  fprintf (stream, "%s  Material %s\n",       prefix, layer->Material->Id ) ;
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

void print_layers_list (FILE *stream, char* prefix, Layer *list)
{
  for ( ; list != NULL ; list = list->Next)
  {
    print_layer (stream, prefix, list) ;
  }
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

Layer *
find_layer_in_list (Layer *list, int id)
{
  for ( ; list != NULL ; list = list->Next)
  {
    if (list->Id == id)
    {
      break ;
    }
  }

 return list ;
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

LayerPosition_t
get_layer_position (GridDimensions *gd, int layer)
{
  if (layer == 0)
  {
    return TL_LAYER_BOTTOM ;
  }
  else if (layer == gd->NLayers - 1)
  {
    return TL_LAYER_TOP ;
  }
  else
  {
    return TL_LAYER_CENTER ;
  }
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

Resistances *
fill_resistances_layer
(
#ifdef DEBUG_FILL_RESISTANCES
  FILE *debug,
#endif
  Layer *layer,
  Resistances *resistances,
  Dimensions *dim,
  int current_layer
)
{
  int row, column ;

#ifdef DEBUG_FILL_RESISTANCES
  fprintf (debug,
    "%p current_layer = %d\tfill_resistances_layer   %s\n",
    resistances, current_layer, layer->Material->Id) ;
#endif

  for (row = 0 ; row < dim->Grid.NRows ; row++)
  {
    for (column = 0 ; column < dim->Grid.NColumns ; column++, resistances++)
    {
      fill_resistances_solid_cell
      (
#ifdef DEBUG_FILL_RESISTANCES
        debug,
        row,
        column,
#endif
        resistances,
        dim,
        get_cell_length (dim, column),
        dim->Cell.Width,
        layer->Height,
        layer->Material->ThermalConductivity,
        current_layer
      ) ;
    } /* column */
  } /* row */

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
  FILE *debug,
  int current_layer,
#endif
  Layer *layer,
  double *capacities,
  Dimensions *dim,
  double delta_time
)
{
  int row, column ;

#ifdef DEBUG_FILL_CAPACITIES
  fprintf (debug,
    "%p current_layer = %d\tfill_capacities_layer   %s\n",
    capacities, current_layer, layer->Material->Id) ;
#endif

  for (row = 0 ; row < dim->Grid.NRows ; row++)
  {
    for (column = 0 ; column < dim->Grid.NColumns ; column++, capacities++)
    {
#ifdef DEBUG_FILL_CAPACITIES
      fprintf (debug,
        "%p solid cell l %5d r %5d c %5d l %5.2f w %5.2f h %5.2f sh %.5e\n",
        capacities, current_layer, row, column,
        get_cell_length(dim, column), dim->Cell.Width, layer->Height,
        layer->Material->SpecificHeat) ;
#endif

      *capacities = capacity
                    (
                      get_cell_length(dim, column),
                      dim->Cell.Width,
                      layer->Height,
                      layer->Material->SpecificHeat,
                      delta_time
                    ) ;
    } /* column */
  } /* row */

  return capacities ;
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

double *
fill_sources_active_layer
(
#ifdef DEBUG_FILL_SOURCES
  FILE *debug,
  int current_layer,
  Layer *layer,
#endif
  Floorplan *floorplan,
  double *sources,
  Dimensions *dim
)
{
  int row, column ;
  double flp_el_surface, cell_surface ;
  FloorplanElement *flp_el ;

#ifdef DEBUG_FILL_SOURCES
  fprintf (debug,
    "%p current_layer = %d\tfill_sources_active_layer   %s\n",
    sources, current_layer, layer->Material->Id) ;
#endif

  for
  (
    flp_el = floorplan->ElementsList ;
    flp_el != NULL ;
    flp_el = flp_el->Next
  )
  {

    flp_el_surface = (double) (flp_el->Length * flp_el->Width) ;

    for (row = flp_el->SW_Row ; row < flp_el->NE_Row ; row++ )
    {
      for (column = flp_el->SW_Column ; column < flp_el->NE_Column ; column++ )
      {
#ifdef DEBUG_FILL_SOURCES
        fprintf (debug,
          "cell l %5d r %5d c %5d\t%s %.5e\n",
          current_layer, row, column, flp_el->Id, flp_el->PowerValue) ;
#endif

        cell_surface = dim->Cell.Width * get_cell_length(dim, column ) ;

        *(sources + (row * dim->Grid.NColumns) + column)

          = (flp_el->PowerValue * cell_surface) / flp_el_surface ;

      } /* column */

    } /* row */

  } /* flp_el */

  return sources + (dim->Grid.NRows * dim->Grid.NColumns) ;
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

double *
fill_sources_empty_layer
(
#ifdef DEBUG_FILL_SOURCES
  FILE *debug,
  int current_layer,
  Layer *layer,
#endif
  double *sources,
  Dimensions *dim
)
{
#ifdef DEBUG_FILL_SOURCES
  fprintf (debug,
    "%p current_layer = %d\tfill_sources_empty_layer   %s\n",
    sources, current_layer, layer->Material->Id) ;
#endif

  return sources + (dim->Grid.NRows * dim->Grid.NColumns) ;
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

int
fill_system_matrix_layer
(
#ifdef DEBUG_FILL_SYSTEM_MATRIX
  FILE *debug,
  Layer *layer,
#endif
  Dimensions *dim,
  Resistances *resistances,
  double *capacities,
  int *columns,
  int *rows,
  double *values,
  int current_layer
)
{
  int current_row, current_column, added, tot_added ;

#ifdef DEBUG_FILL_SYSTEM_MATRIX
  fprintf (debug,
    "%p %p %p %p %p (l %2d) fill_system_matrix_layer %s\n",
    resistances, capacities, columns, rows, values,
    current_layer, layer->Material->Id) ;
#endif

  for (current_row = 0 ; current_row < dim->Grid.NRows ; current_row++)
  {
    for
    (
      added          = 0 ,
      tot_added      = 0 ,
      current_column = 0 ;

      current_column < dim->Grid.NColumns ;

      resistances    ++ ,
      capacities     ++ ,
      columns        ++ ,
      rows           += added ,
      values         += added ,
      tot_added      += added ,
      current_column ++
    )
    {
      added = add_solid_column (
#ifdef DEBUG_FILL_SYSTEM_MATRIX
                debug,
#endif
                dim, resistances, capacities,
                current_layer, current_row, current_column,
                columns, rows, values) ;

    } /* column */

  } /* row */

  return tot_added ;
}
