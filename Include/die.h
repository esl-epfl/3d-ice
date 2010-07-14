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
extern "C" {
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

  struct Conductances *
  fill_conductances_die  (
                          #ifdef DEBUG_FILL_CONDUCTANCES
                          FILE         *debug,
                          #endif
                          struct Die   *die,
                          struct Conductances *conductances,
                          Dimensions* dimensions,
                          int          current_layer
                         ) ;

  double *
  fill_capacities_die    (
                          #ifdef DEBUG_FILL_CAPACITIES
                          FILE       *debug,
                          int        current_layer,
                          #endif
                          struct Die *die,
                          double     *capacities,
                          Dimensions* dimensions,
                          double     delta_time
                         ) ;

  double *
  fill_sources_die       (
                          #ifdef DEBUG_FILL_SOURCES
                          FILE              *debug,
                          int               current_layer,
                          #endif
                          struct Die        *die,
                          struct Floorplan  *floorplan,
                          double            *sources,
                          Dimensions*       dimensions
                         ) ;

  int
  fill_ccs_system_matrix_die (
                          #ifdef DEBUG_FILL_SYSTEM_MATRIX
                          FILE         *debug,
                          #endif
                          struct Die   *die,
                          Dimensions*  dimensions,
                          struct Conductances *conductances,
                          double       *capacities,
                          int          *columns,
                          int          *rows,
                          double       *values,
                          int          current_layer
                         ) ;

  int
  fill_crs_system_matrix_die (
                          #ifdef DEBUG_FILL_SYSTEM_MATRIX
                          FILE         *debug,
                          #endif
                          struct Die   *die,
                          Dimensions*  dimensions,
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

#ifdef __cplusplus
}
#endif

#endif /* _TL_DIE_H_ */
