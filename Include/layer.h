/******************************************************************************
 *                                                                            *
 * Header file "Include/layer.h"                                              *
 *                                                                            *
 * EPFL-STI-IEL-ESL                                                           *
 * Bâtiment ELG, ELG 130                                                      *
 * Station 11                                                                 *
 * 1015 Lausanne, Switzerland                    alessandro.vincenzi@epfl.ch  *
 ******************************************************************************/

#ifndef _TL_LAYER_H_
#define _TL_LAYER_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include "material.h"
#include "dimensions.h"
#include "resistances.h"

#include <stdio.h>

/******************************************************************************
 *                                                                            *
 * "Layer" : the representation of a single layer.                            *
 *                                                                            *
 ******************************************************************************/

  typedef enum
  {
    TL_LAYER_BOTTOM = 0,
    TL_LAYER_CENTER,
    TL_LAYER_TOP

  } LayerPosition_t ;

  struct layer
  {
    int Id ;                /* The identifier of the layer */

    double Height ;         /* The heigh of the layer in um. (1 cell) */

    Material* Material ;    /* The material composing the layer */

    struct layer *Next ;    /* To collect layers in a linked list */

  } ;

  typedef struct layer Layer ;

/******************************************************************************/

  void init_layer(Layer *layer) ;

  Layer *alloc_and_init_layer (void) ;

  void free_layer (Layer *layer) ;

  void free_layers_list (Layer *list) ;

  void print_layer (FILE *stream, char* prefix, Layer *layer) ;

  void print_layers_list (FILE *stream, char* prefix, Layer *list) ;

  Layer *find_layer_in_list (Layer *list, int id) ;

  LayerPosition_t get_layer_position (GridDimensions *gd, int layer) ;

  Resistances *fill_resistances_layer (
#ifdef DEBUG_FILL_RESISTANCES
                                       FILE *debug,
#endif
                                       Layer *layer,
                                       Resistances *resistances,
                                       Dimensions *dim,
                                       int current_layer) ;

#ifdef __cplusplus
}
#endif
#endif /* _TL_LAYER_H_ */
