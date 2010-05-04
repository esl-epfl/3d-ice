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
