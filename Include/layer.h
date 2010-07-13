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

  enum LayerPosition_t
  {
    TL_LAYER_BOTTOM = 0,
    TL_LAYER_CENTER    ,
    TL_LAYER_TOP
  } ;

  struct Layer
  {
    double Height ;                 /* The heigh of the layer in um. (1 cell) */

    Material* Material ;            /* The material composing the layer       */

    int LayersOffset ;              /* The offset (#of layers) counting from  */
                                    /* the first layer in the die             */

    struct Layer *Next ;            /* To collect layers in a linked list     */
  } ;

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

  void
  init_layer                (struct Layer *layer) ;

  struct Layer *
  alloc_and_init_layer      (void) ;

  void
  free_layer                (struct Layer *layer) ;

  void
  free_layers_list          (struct Layer *list) ;

  void
  print_layer               (FILE *stream, char* prefix, struct Layer *layer) ;

  void
  print_layers_list         (FILE *stream, char* prefix, struct Layer *list) ;

  enum LayerPosition_t
  get_layer_position        (struct Dimensions *dimensions, int layer) ;

  struct Conductances *
  fill_conductances_layer    (
                             #ifdef DEBUG_FILL_CONDUCTANCES
                             FILE         *debug,
                             #endif
                             struct Layer *layer,
                             struct Conductances *conductances,
                             struct Dimensions   *dimensions,
                             int          current_layer
                            ) ;

  double *
  fill_capacities_layer     (
                             #ifdef DEBUG_FILL_CAPACITIES
                             FILE         *debug,
                             int          current_layer,
                             #endif
                             struct Layer *layer,
                             double       *capacities,
                             struct Dimensions   *dimensions,
                             double       delta_time
                            ) ;

  double *
  fill_sources_active_layer (
                             #ifdef DEBUG_FILL_SOURCES
                             FILE         *debug,
                             int          current_layer,
                             struct Layer *layer,
                             #endif
                             struct Floorplan    *floorplan,
                             double       *sources,
                             struct Dimensions   *dimensions
                            ) ;

  double *
  fill_sources_empty_layer  (
                             #ifdef DEBUG_FILL_SOURCES
                             FILE         *debug,
                             int          current_layer,
                             struct Layer *layer,
                             #endif
                             double       *sources,
                             struct Dimensions   *dimensions
                            ) ;


  int
  fill_ccs_system_matrix_layer  (
                             #ifdef DEBUG_FILL_SYSTEM_MATRIX
                             FILE         *debug,
                             struct Layer *layer,
                             #endif
                             struct Dimensions   *dimensions,
                             struct Conductances *conductances,
                             double       *capacities,
                             int          *columns,
                             int          *rows,
                             double       *values,
                             int          current_layer
                            ) ;

  int
  fill_crs_system_matrix_layer  (
                             #ifdef DEBUG_FILL_SYSTEM_MATRIX
                             FILE         *debug,
                             struct Layer *layer,
                             #endif
                             struct Dimensions   *dimensions,
                             struct Conductances *conductances,
                             double       *capacities,
                             int          *rows,
                             int          *columns,
                             double       *values,
                             int          current_layer
                            ) ;

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

#endif /* _TL_LAYER_H_ */
