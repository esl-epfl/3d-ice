/******************************************************************************
 *                                                                            *
 * Header file "Include/floorplan.h"                                          *
 *                                                                            *
 * EPFL-STI-IEL-ESL                                                           *
 * Bâtiment ELG, ELG 130                                                      *
 * Station 11                                                                 *
 * 1015 Lausanne, Switzerland                    alessandro.vincenzi@epfl.ch  *
 ******************************************************************************/

#ifndef _TL_FLOORPLAN_H_
#define _TL_FLOORPLAN_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dimensions.h"
#include "floorplan_element.h"

/******************************************************************************
 *                                                                            *
 * "Floorplan" : the representation of a single floorplan                     *
 *                                                                            *
 ******************************************************************************/

  struct Floorplan
  {
    char *FileName ;    /* The name of the file scanned to fill the floorplan */

    int NElements ;     /* The number of floorplan elements in the floorplan  */

    struct FloorplanElement *ElementsList ; /* The list of floorplan elements */

  } ;


/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

  void
  init_floorplan           (struct Floorplan *floorplan) ;

  struct Floorplan *
  alloc_and_init_floorplan (void) ;

  void
  free_floorplan           (struct Floorplan *floorplan) ;

  int
  fill_floorplan           (struct Floorplan *floorplan, Dimensions *dim) ;

  void
  print_floorplan          (FILE *stream, char* prefix, struct Floorplan *floorplan) ;

  int
  check_intersections      (
                            struct Floorplan *floorplan,
                            struct FloorplanElement *floorplan_element
                           ) ;

  int
  check_location           (
                            struct Floorplan *floorplan,
                            struct FloorplanElement *floorplan_element,
                            Dimensions *dimensions
                           ) ;

  int
  align_to_grid            (
                            struct Floorplan *floorplan,
                            struct FloorplanElement *floorplan_element,
                            Dimensions *dimensions
                           ) ;

  void
  insert_power_values_floorplan              (
                                              struct Floorplan *floorplan,
                                              double    *power_values
                                             ) ;

  int
  insert_power_value_floorplan_element       (
                                               struct Floorplan *floorplan,
                                               char      *floorplan_element_id,
                                               double    power_value
                                             ) ;

  int
  get_max_temperature_floorplan              (
                                              struct Floorplan  *floorplan,
                                              char       *floorplan_element_id,
                                              Dimensions *dimensions,
                                              double     *temperatures,
                                              double     *max_temperature
                                             );

  int
  get_min_temperature_floorplan              (
                                              struct Floorplan *floorplan,
                                              char       *floorplan_element_id,
                                              Dimensions *dimensions,
                                              double     *temperatures,
                                              double     *min_temperature
                                             );

  int
  get_avg_temperature_floorplan              (
                                              struct Floorplan  *floorplan,
                                              char       *floorplan_element_id,
                                              Dimensions *dimensions,
                                              double     *temperatures,
                                              double     *avg_temperature
                                             );

  int
  get_min_avg_max_temperatures_floorplan     (
                                              struct Floorplan  *floorplan,
                                              char       *floorplan_element_id,
                                              Dimensions *dimensions,
                                              double     *temperatures,
                                              double     *min_temperature,
                                              double     *avg_temperature,
                                              double     *max_temperature
                                             );

  int
  get_all_max_temperatures_floorplan         (
                                              struct Floorplan  *floorplan,
                                              Dimensions *dimensions,
                                              double     *temperatures,
                                              double     *max_temperature
                                             );

  int
  get_all_min_temperatures_floorplan         (
                                              struct Floorplan  *floorplan,
                                              Dimensions *dimensions,
                                              double     *temperatures,
                                              double     *min_temperature
                                             );

  int
  get_all_avg_temperatures_floorplan         (
                                              struct Floorplan  *floorplan,
                                              Dimensions *dimensions,
                                              double     *temperatures,
                                              double     *avg_temperature
                                             );

  int
  get_all_min_avg_max_temperatures_floorplan (
                                              struct Floorplan  *floorplan,
                                              Dimensions *dimensions,
                                              double     *temperatures,
                                              double     *min_temperature,
                                              double     *avg_temperature,
                                              double     *max_temperature
                                             );

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* _TL_FLOORPLAN_H_ */
