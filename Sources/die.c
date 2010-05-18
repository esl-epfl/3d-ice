/******************************************************************************
 *                                                                            *
 * Source file "Sources/die.c"                                                *
 *                                                                            *
 * EPFL-STI-IEL-ESL                                                           *
 * Bâtiment ELG, ELG 130                                                      *
 * Station 11                                                                 *
 * 1015 Lausanne, Switzerland                    alessandro.vincenzi@epfl.ch  *
 ******************************************************************************/

#include <stdlib.h>
#include <string.h>

#include "die.h"

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

void
init_die
(
  Die *die
)
{
  die->Id          = NULL ;
  die->LayersList  = NULL ;
  die->NLayers     = 0    ;
  die->SourceLayer = NULL ;
  die->Next        = NULL ;
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

Die *
alloc_and_init_die
(
  void
)
{
  Die *die = (Die *) malloc ( sizeof(Die) ) ;

  if (die != NULL) init_die (die) ;

  return die ;
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

void
free_die
(
  Die *die
)
{
  free_layers_list (die->LayersList) ;
  free (die->Id) ;
  free (die) ;
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

void
free_dies_list
(
  Die *list
)
{
  Die *next ;

  for ( ; list != NULL ; list = next)
  {
    next = list->Next ;
    free_die(list) ;
  }
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

void
print_die
(
  FILE *stream,
  char *prefix,
  Die  *die
)
{
  fprintf (stream,
    "%sDie %s:\n",                prefix, die->Id) ;
  fprintf (stream,
    "%s  Number of layers  %d\n", prefix, die->NLayers);

  fprintf (stream,
    "%s  Source layer is layer #%d\n", prefix, die->SourceLayer->LayersOffset);

  char *new_prefix = (char *) malloc (sizeof(char)*(strlen(prefix) + 2));

  strcpy (new_prefix, prefix) ;
  strcat (new_prefix, "  ") ;

  print_layers_list (stream, new_prefix, die->LayersList) ;

  free (new_prefix) ;
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

void
print_dies_list
(
  FILE *stream,
  char* prefix,
  Die *list
)
{
  for ( ; list != NULL ; list = list->Next)

    print_die (stream, prefix, list) ;

}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

Die *
find_die_in_list
(
  Die* list,
  char *id
)
{
  for ( ; list != NULL ; list = list->Next)

    if (strcmp(list->Id, id) == 0) break ;

  return list ;
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

Conductances *
fill_conductances_die
(
#ifdef DEBUG_FILL_CONDUCTANCES
  FILE         *debug,
#endif
  Die          *die,
  Conductances *conductances,
  Dimensions   *dimensions,
  int          current_layer
)
{
  Layer *layer ;

#ifdef DEBUG_FILL_CONDUCTANCES
  fprintf (debug,
    "%p current_layer = %d\tfill_conductances_die     %s\n",
    conductances, current_layer, die->Id) ;
#endif

  for
  (
    layer =  die->LayersList;

    layer != NULL ;

    layer = layer->Next
  )

    conductances = fill_conductances_layer
                   (
#ifdef DEBUG_FILL_CONDUCTANCES
                     debug,
#endif
                     layer,
                     conductances,
                     dimensions,
                     current_layer + layer->LayersOffset
                   ) ;

  return conductances ;
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

double *
fill_capacities_die
(
#ifdef DEBUG_FILL_CAPACITIES
  FILE       *debug,
  int        current_layer,
#endif
  Die        *die,
  double     *capacities,
  Dimensions *dimensions,
  double     delta_time
)
{
  Layer *layer ;

#ifdef DEBUG_FILL_CAPACITIES
  fprintf (debug,
    "%p current_layer = %d\tfill_capacities_die     %s\n",
    capacities, current_layer, die->Id) ;
#endif

  for
  (
    layer = die->LayersList ;

    layer != NULL ;

    layer = layer->Next
  )

    capacities = fill_capacities_layer
                 (
#ifdef DEBUG_FILL_CAPACITIES
                   debug,
                   current_layer + layer->LayersOffset,
#endif
                   layer,
                   capacities,
                   dimensions,
                   delta_time
                 ) ;

  return capacities ;
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

double *
fill_sources_die
(
#ifdef DEBUG_FILL_SOURCES
  FILE       *debug,
  int        current_layer,
#endif
  Die        *die,
  Floorplan  *floorplan,
  double     *sources,
  Dimensions *dimensions
)
{
  Layer *layer ;

#ifdef DEBUG_FILL_SOURCES
  fprintf (debug,
    "%p current_layer = %d\tfill_sources_die %s floorplan %s\n",
    sources, current_layer, die->Id, floorplan->FileName) ;
#endif

  for
  (
    layer = die->LayersList ;

    layer != NULL ;

    layer = layer->Next
  )

    if ( die->SourceLayer == layer )

      sources = fill_sources_active_layer
                (
#ifdef DEBUG_FILL_SOURCES
                  debug,
                  current_layer + layer->LayersOffset,
                  layer,
#endif
                  floorplan,
                  sources,
                  dimensions
                ) ;

    else

      sources = fill_sources_empty_layer
                (
#ifdef DEBUG_FILL_SOURCES
                  debug,
                  current_layer + layer->LayersOffset,
                  layer,
#endif
                  sources,
                  dimensions
                ) ;

  return sources ;
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

int
fill_system_matrix_die
(
#ifdef DEBUG_FILL_SYSTEM_MATRIX
  FILE         *debug,
#endif
  Die          *die,
  Dimensions   *dimensions,
  Conductances *conductances,
  double       *capacities,
  int          *columns,
  int          *rows,
  double       *values,
  int          current_layer
)
{
  Layer *layer ;
  int tot_added, added ;

#ifdef DEBUG_FILL_SYSTEM_MATRIX
  fprintf (debug,
    "%p %p %p %p %p (l %2d) fill_system_matrix_die\n",
    conductances, capacities, columns, rows, values, current_layer) ;
#endif

  for
  (
    added     = 0 ,
    tot_added = 0 ,
    layer     = die->LayersList ;

    layer != NULL ;

    conductances  += get_layer_area (dimensions) ,
    capacities    += get_layer_area (dimensions) ,
    columns       += get_layer_area (dimensions) ,
    rows          += added ,
    values        += added ,
    tot_added     += added ,
    layer          = layer->Next
  )

    added = fill_system_matrix_layer
            (
#ifdef DEBUG_FILL_SYSTEM_MATRIX
              debug, layer,
#endif
              dimensions, conductances, capacities,
              columns, rows, values,
              current_layer + layer->LayersOffset
            ) ;

  return tot_added ;
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
