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

#include <stdio.h>

#include "material.h"
#include "dimensions.h"
#include "conductances.h"
#include "floorplan.h"

/******************************************************************************
 *                                                                            *
 * "Layer" : the representation of a single layer.                            *
 *                                                                            *
 ******************************************************************************/

  typedef enum
  {
    TL_LAYER_BOTTOM = 0,
    TL_LAYER_CENTER    ,
    TL_LAYER_TOP

  } LayerPosition_t ;

  struct layer
  {
    double Height ;                 /* The heigh of the layer in um. (1 cell) */

    Material* Material ;            /* The material composing the layer       */

    int LayersOffset ;              /* The offset (#of layers) counting from  */
                                    /* the first layer in the die             */

    struct layer *Next ;            /* To collect layers in a linked list     */
  } ;

  typedef struct layer Layer ;

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

  void
  init_layer                (Layer *layer) ;

  Layer *
  alloc_and_init_layer      (void) ;

  void
  free_layer                (Layer *layer) ;

  void
  free_layers_list          (Layer *list) ;

  void
  print_layer               (FILE *stream, char* prefix, Layer *layer) ;

  void
  print_layers_list         (FILE *stream, char* prefix, Layer *list) ;

  LayerPosition_t
  get_layer_position        (Dimensions *dimensions, int layer) ;

  Conductances *
  fill_conductances_layer    (
                             #ifdef DEBUG_FILL_CONDUCTANCES
                             FILE         *debug,
                             #endif
                             Layer        *layer,
                             Conductances *conductances,
                             Dimensions   *dimensions,
                             int          current_layer
                            ) ;

  double *
  fill_capacities_layer     (
                             #ifdef DEBUG_FILL_CAPACITIES
                             FILE       *debug,
                             int        current_layer,
                             #endif
                             Layer      *layer,
                             double     *capacities,
                             Dimensions *dimensions,
                             double     delta_time
                            ) ;

  double *
  fill_sources_active_layer (
                             #ifdef DEBUG_FILL_SOURCES
                             FILE       *debug,
                             int        current_layer,
                             Layer      *layer,
                             #endif
                             Floorplan  *floorplan,
                             double     *sources,
                             Dimensions *dimensions
                            ) ;

  double *
  fill_sources_empty_layer  (
                             #ifdef DEBUG_FILL_SOURCES
                             FILE       *debug,
                             int        current_layer,
                             Layer      *layer,
                             #endif
                             double     *sources,
                             Dimensions *dimensions
                            ) ;


  int
  fill_system_matrix_layer  (
                             #ifdef DEBUG_FILL_SYSTEM_MATRIX
                             FILE         *debug,
                             Layer        *layer,
                             #endif
                             Dimensions   *dimensions,
                             Conductances *conductances,
                             double       *capacities,
                             int          *columns,
                             int          *rows,
                             double       *values,
                             int          current_layer
                            ) ;

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

#ifdef __cplusplus
}
#endif
#endif /* _TL_LAYER_H_ */
