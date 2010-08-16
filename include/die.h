/******************************************************************************
 *                                                                            *
 * Header file "Include/die.h"                                                *
 *                                                                            *
 * EPFL-STI-IEL-ESL                                                           *
 * Bâtiment ELG, ELG 130                                                      *
 * Station 11                                                                 *
 * 1015 Lausanne, Switzerland                    alessandro.vincenzi@epfl.ch  *
 ******************************************************************************/

#ifndef _TL_DIE_H_
#define _TL_DIE_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdio.h>

#include "layer.h"
#include "conductances.h"
#include "floorplan.h"
#include "environment_heat_sink.h"

/******************************************************************************
 *                                                                            *
 * "Die" : the representation of a single die.                                *
 *                                                                            *
 ******************************************************************************/

  struct Die
  {
    char* Id ;                  /* The id (string) of the die                 */

    Layer* LayersList ;         /* The list of layers composing the die       */

    int NLayers ;               /* The number of layer composing the die      */

    Layer* SourceLayer ;        /* Pointer to the source layer (in the list)  */

    struct Die* Next ;          /* To collect dies in a linked list           */

  } ;

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

  void
  init_die               (struct Die *die) ;

  struct Die *
  alloc_and_init_die     (void) ;

  void
  free_die               (struct Die *die) ;

  void
  free_dies_list         (struct Die *list) ;

  void
  print_die              (FILE *stream, char* prefix, struct Die *die) ;

  void
  print_dies_list        (FILE *stream, char* prefix, struct Die *list) ;

  struct Die *
  find_die_in_list       (struct Die *list, char *id) ;

  Conductances*   fill_conductances_die
  (
    struct Die*          die,
    Conductances*        conductances,
    Dimensions*          dimensions,
    EnvironmentHeatSink* environmentheatsink,
    LayerIndex_t         current_layer
  ) ;

  Capacity_t*     fill_capacities_die
  (
#   ifdef PRINT_CAPACITIES
    LayerIndex_t  current_layer,
#   endif
    struct Die*   die,
    Capacity_t*   capacities,
    Dimensions*   dimensions,
    Time_t        delta_time
  ) ;

  Source_t*           fill_sources_die
  (
#   ifdef PRINT_SOURCES
    LayerIndex_t      current_layer,
#   endif
    struct Die*       die,
    struct Floorplan* floorplan,
    Source_t*         sources,
    Dimensions*       dimensions
  ) ;

  int                    fill_ccs_system_matrix_die
  (
    struct Die*          die,
    Dimensions*          dimensions,
    Conductances* conductances,
    Capacity_t*          capacities,
    LayerIndex_t         current_layer,
    int*                 columns,
    int*                 rows,
    double*              values
  ) ;

  int                    fill_crs_system_matrix_die
  (
    struct Die*          die,
    Dimensions*          dimensions,
    Conductances* conductances,
    Capacity_t*          capacities,
    EnvironmentHeatSink* environmentheatsink,
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

#endif /* _TL_DIE_H_ */
