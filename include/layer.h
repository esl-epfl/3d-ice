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

#include "types.h"
#include "material.h"
#include "dimensions.h"
#include "conductances.h"
#include "floorplan.h"

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

  struct Conductances* fill_conductances_layer
  (
    Layer*        layer,
    struct Conductances* conductances,
    Dimensions*   dimensions,
    LayerIndex_t  current_layer
  ) ;

  Capacity_t*    fill_capacities_layer
  (
#   ifdef PRINT_CAPACITIES
    LayerIndex_t current_layer,
#   endif
    Layer*       layer,
    Capacity_t*  capacities,
    Dimensions   *dimensions,
    Time_t       delta_time
  ) ;

  Source_t*           fill_sources_active_layer
  (
#   ifdef PRINT_SOURCES
    LayerIndex_t      current_layer,
    Layer*            layer,
#   endif
    struct Floorplan* floorplan,
    Source_t*         sources,
    Dimensions*       dimensions
  ) ;

  Source_t*      fill_sources_empty_layer
  (
#   ifdef PRINT_SOURCES
    LayerIndex_t current_layer,
    Layer*       layer,
#   endif
    Source_t*    sources,
    Dimensions*  dimensions
  ) ;


  int                    fill_ccs_system_matrix_layer
  (
#   ifdef PRINT_SYSTEM_MATRIX
    Layer*               layer,
#   endif
    Dimensions*          dimensions,
    struct Conductances* conductances,
    Capacity_t*          capacities,
    LayerIndex_t         current_layer,
    int*                 columns,
    int*                 rows,
    double*              values
  ) ;

  int                    fill_crs_system_matrix_layer
  (
#   ifdef PRINT_SYSTEM_MATRIX
    Layer*               layer,
#   endif
    Dimensions*          dimensions,
    struct Conductances* conductances,
    Capacity_t*          capacities,
    LayerIndex_t         current_layer,
    int*                 rows,
    int*                 columns,
    double*              values
  ) ;

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* _TL_LAYER_H_ */
