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

void
init_die (Die *die)
{
  if (die == NULL) return ;

  die->Id         = NULL ;
  die->LayersList = NULL ;
  die->NLayers    = 0 ;
  die->SourcesId  = 0 ;
  die->Next       = NULL ;
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

Die *
alloc_and_init_die (void)
{
  Die *die = (Die *) malloc ( sizeof(Die) ) ;

  init_die (die) ;

  return die ;
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

void
free_die (Die *die)
{
  if (die == NULL) return ;

  free (die->Id) ;
  free_layers_list (die->LayersList) ;
  free (die) ;
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

void
free_dies_list (Die *list)
{
  Die *next_die ;

  for ( ; list != NULL ; list = next_die)
  {
    next_die = list->Next ;

    free_die(list) ;
  }
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

void
print_die (FILE *stream, char* prefix, Die *die)
{
  fprintf (stream, "%sDie %s:\n", prefix, die->Id) ;
  fprintf (stream, "%s  Number of layers  %d\n", prefix, die->NLayers);
  fprintf (stream, "%s  Sources on layer  %d\n", prefix,die->SourcesId ) ;

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
print_dies_list (FILE *stream, char* prefix, Die *list)
{
  for ( ; list != NULL ; list = list->Next)
  {
    print_die (stream, prefix, list) ;
  }
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

Die *
find_die_in_list (Die* list, char *id)
{
  for ( ; list != NULL ; list = list->Next)
  {
    if (strcmp(list->Id, id) == 0)
    {
      break ;
    }
  }

  return list ;
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

Resistances *
fill_resistances_die
(
#ifdef DEBUG_FILL_RESISTANCES
  FILE *debug,
#endif
  Die *die,
  Resistances *resistances,
  Dimensions *dim,
  int current_layer
)
{
  Layer *layer ;

#ifdef DEBUG_FILL_RESISTANCES
  fprintf (debug,
    "%p current_layer = %d\tfill_resistances_die     %s\n",
    resistances, current_layer, die->Id) ;
#endif

  for
  (
    layer =  die->LayersList;

    layer != NULL ;

    current_layer++,
    layer = layer->Next
  )
  {
    resistances = fill_resistances_layer
                  (
#ifdef DEBUG_FILL_RESISTANCES
                    debug,
#endif
                    layer,
                    resistances,
                    dim,
                    current_layer
                  ) ;
  }

  return resistances ;
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

double *
fill_capacities_die
(
#ifdef DEBUG_FILL_CAPACITIES
  FILE *debug,
  int current_layer,
#endif
  Die *die,
  double *capacities,
  Dimensions *dim,
  double delta_time
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

#ifdef DEBUG_FILL_CAPACITIES
    current_layer++,
#endif
    layer = layer->Next
  )
  {
    capacities = fill_capacities_layer
                 (
#ifdef DEBUG_FILL_CAPACITIES
                   debug,
                   current_layer,
#endif
                   layer,
                   capacities,
                   dim,
                   delta_time
                 ) ;
  }

  return capacities ;
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

double *
fill_sources_die
(
#ifdef DEBUG_FILL_SOURCES
  FILE *debug,
  int current_layer,
#endif
  Die *die,
  Floorplan *floorplan,
  double *sources,
  Dimensions *dim
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

#ifdef DEBUG_FILL_SOURCES
    current_layer++,
#endif
    layer = layer->Next
  )
  {
    if ( die->SourcesId == layer->Id )
    {
      sources = fill_sources_active_layer
                (
#ifdef DEBUG_FILL_SOURCES
                  debug,
                  current_layer,
                  layer,
#endif
                  floorplan,
                  sources,
                  dim
                ) ;
    }
    else
    {
      sources = fill_sources_empty_layer
                (
#ifdef DEBUG_FILL_SOURCES
                  debug,
                  current_layer,
                  layer,
#endif
                  sources,
                  dim
                ) ;
    }
  }

  return sources ;
}
