/******************************************************************************
 *                                                                            *
 * Header file "Include/floorplan_element.h"                                  *
 *                                                                            *
 * EPFL-STI-IEL-ESL                                                           *
 * Bâtiment ELG, ELG 130                                                      *
 * Station 11                                                                 *
 * 1015 Lausanne, Switzerland                    alessandro.vincenzi@epfl.ch  *
 ******************************************************************************/

#ifndef _TL_FLOORPLAN_ELEMENT_H_
#define _TL_FLOORPLAN_ELEMENT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdio.h>

#include "dimensions.h"

/******************************************************************************
 *                                                                            *
 * "FloorplanElement" : the single element composing a floorplan              *
 *                                                                            *
 ******************************************************************************/

  struct FloorplanElement
  {
    char *Id ;                    /* The id (string) of the floorplan element */

    int SW_X ;                    /* The south-west X position (um)           */
    int SW_Y ;                    /* The south-west Y position (um)           */

    int Length ;                  /* The length of the floorplan element (um) */
    int Width ;                   /* The width of the floorplan element (um)  */

    int SW_Row ;                  /* The south-west row index                 */

    int SW_Column ;               /* The south-west column index              */

    int NE_Row ;                  /* The north-east row index                 */

    int NE_Column ;               /* The north-east column index              */

    double PowerValue ;           /* The current power value                  */

    struct FloorplanElement *Next ;  /* To collect floorplan elements         */
                                     /* in a linked list                      */

  } ;

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

  void
  init_floorplan_element           (struct FloorplanElement *floorplan_element) ;

  struct FloorplanElement *
  alloc_and_init_floorplan_element (void) ;

  void
  free_floorplan_element           (struct FloorplanElement *floorplan_element) ;

  void
  free_floorplan_elements_list     (struct FloorplanElement *list) ;

  struct FloorplanElement *
  find_floorplan_element_in_list   (struct FloorplanElement *list, char *id) ;

  void
  print_floorplan_element          (
                                    FILE *stream,
                                    char *prefix,
                                    struct FloorplanElement *floorplan_element
                                   ) ;

  void
  print_floorplan_elements_list    (
                                    FILE *stream,
                                    char *prefix,
                                    struct FloorplanElement *list
                                   ) ;

  int
  check_intersection               (
                                    struct FloorplanElement *floorplan_element_a,
                                    struct FloorplanElement *floorplan_element_b
                                   ) ;

  void
  get_max_temperature_floorplan_element
                                   (
                                    struct FloorplanElement *floorplan_element,
                                    Dimensions *dimensions,
                                    double *temperatures,
                                    double *max_temperature
                                   );

  void
  get_min_temperature_floorplan_element
                                   (
                                    struct FloorplanElement *floorplan_element,
                                    Dimensions *dimensions,
                                    double *temperatures,
                                    double *min_temperature
                                   );

  void
  get_avg_temperature_floorplan_element
                                   (
                                    struct FloorplanElement *floorplan_element,
                                    Dimensions *dimensions,
                                    double *temperatures,
                                    double *avg_temperature
                                   );

  void
  get_min_avg_max_temperatures_floorplan_element
                                   (
                                    struct FloorplanElement *floorplan_element,
                                    Dimensions       *dimensions,
                                    double           *temperatures,
                                    double           *min_temperature,
                                    double           *avg_temperature,
                                    double           *max_temperature
                                   );

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* _TL_FLOORPLAN_ELEMENT_H_ */
