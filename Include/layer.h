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
extern "C" {
#endif

#include <stdio.h>

#include "types.h"
#include "material.h"
#include "dimensions.h"
#include "conductances.h"
#include "floorplan.h"

  enum LayerPosition_t
  {
    TL_LAYER_BOTTOM = 0,
    TL_LAYER_CENTER    ,
    TL_LAYER_TOP
  } ;

/******************************************************************************
 *                                "Layer"                                     *
 ******************************************************************************/

  struct Layer
  {
    CellDimension_t Height ;        /* The heigh of the layer in um. (1 cell) */

    Material* Material ;            /* The material composing the layer       */

    LayerIndex_t LayersOffset ;     /* The offset (#of layers) counting from  */
                                    /* the first layer in the die             */

    struct Layer* Next ;            /* To collect layers in a linked list     */
  } ;

  typedef struct Layer Layer;

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

  void init_layer (Layer* layer) ;

/******************************************************************************/

  Layer* alloc_and_init_layer (void) ;

/******************************************************************************/

  void free_layer (Layer* layer) ;

/******************************************************************************/

  void free_layers_list (Layer* list) ;

/******************************************************************************/

  void print_layer (FILE* stream, String_t prefix, Layer* layer) ;

/******************************************************************************/

  void print_layers_list (FILE* stream, String_t prefix, Layer* list) ;

/******************************************************************************/

  enum LayerPosition_t
  get_layer_position (Dimensions* dimensions, LayerIndex_t layer) ;

/******************************************************************************/

  struct Conductances *
  fill_conductances_layer    (
                             #ifdef DEBUG_FILL_CONDUCTANCES
                             FILE*                debug,
                             #endif
                             Layer*               layer,
                             struct Conductances* conductances,
                             Dimensions*   dimensions,
                             LayerIndex_t         current_layer
                            ) ;

  double *
  fill_capacities_layer     (
                             #ifdef DEBUG_FILL_CAPACITIES
                             FILE*        debug,
                             LayerIndex_t current_layer,
                             #endif
                             Layer*       layer,
                             double       *capacities,
                             Dimensions   *dimensions,
                             double       delta_time
                            ) ;

  double *
  fill_sources_active_layer (
                             #ifdef DEBUG_FILL_SOURCES
                             FILE         *debug,
                             LayerIndex_t current_layer,
                             Layer* layer,
                             #endif
                             struct Floorplan    *floorplan,
                             double       *sources,
                             Dimensions   *dimensions
                            ) ;

  double *
  fill_sources_empty_layer  (
                             #ifdef DEBUG_FILL_SOURCES
                             FILE         *debug,
                             LayerIndex_t current_layer,
                             Layer* layer,
                             #endif
                             double       *sources,
                             Dimensions   *dimensions
                            ) ;


  int
  fill_ccs_system_matrix_layer  (
                             #ifdef DEBUG_FILL_SYSTEM_MATRIX
                             FILE         *debug,
                             Layer* layer,
                             #endif
                             Dimensions   *dimensions,
                             struct Conductances *conductances,
                             double       *capacities,
                             int          *columns,
                             int          *rows,
                             double       *values,
                             LayerIndex_t current_layer
                            ) ;

  int
  fill_crs_system_matrix_layer  (
                             #ifdef DEBUG_FILL_SYSTEM_MATRIX
                             FILE         *debug,
                             Layer*       layer,
                             #endif
                             Dimensions   *dimensions,
                             struct Conductances *conductances,
                             double       *capacities,
                             int          *rows,
                             int          *columns,
                             double       *values,
                             LayerIndex_t current_layer
                            ) ;

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* _TL_LAYER_H_ */
