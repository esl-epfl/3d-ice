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

/******************************************************************************
 *                                                                            *
 * "Die" : the representation of a single die.                                *
 *                                                                            *
 ******************************************************************************/

  struct die
  {
    char* Id ;                  /* The id (string) of the die                 */

    Layer *LayersList ;         /* The list of layers composing the die       */

    int NLayers ;               /* The number of layer composing the die      */

    int SourcesId ;             /* The id of the layer with the sources on it */

    struct die* Next ;          /* To collect dies in a linked list           */

  } ;

  typedef struct die Die ;

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

  void
  init_die               (Die *die) ;

  Die *
  alloc_and_init_die     (void) ;

  void
  free_die               (Die *die) ;

  void
  free_dies_list         (Die *list) ;

  void
  print_die              (FILE *stream, char* prefix, Die *die) ;

  void
  print_dies_list        (FILE *stream, char* prefix, Die *list) ;

  Die *
  find_die_in_list       (Die *list, char *id) ;

  Conductances *
  fill_conductances_die  (
                          #ifdef DEBUG_FILL_CONDUCTANCES
                          FILE         *debug,
                          #endif
                          Die          *die,
                          Conductances *conductances,
                          Dimensions   *dimensions,
                          int          current_layer
                         ) ;

  double *
  fill_capacities_die    (
                          #ifdef DEBUG_FILL_CAPACITIES
                          FILE       *debug,
                          int        current_layer,
                          #endif
                          Die        *die,
                          double     *capacities,
                          Dimensions *dimensions,
                          double     delta_time
                         ) ;

  double *
  fill_sources_die       (
                          #ifdef DEBUG_FILL_SOURCES
                          FILE       *debug,
                          int        current_layer,
                          #endif
                          Die        *die,
                          Floorplan  *floorplan,
                          double     *sources,
                          Dimensions *dimensions
                         ) ;

  int
  fill_system_matrix_die (
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
                         ) ;

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

#ifdef __cplusplus
}
#endif
#endif /* _TL_DIE_H_ */
