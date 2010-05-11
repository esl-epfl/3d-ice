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

/******************************************************************************
 *                                                                            *
 * "FloorplanElement" : the representation of a single element composing a    *
 *                      floorplan.                                            *
 *                                                                            *
 ******************************************************************************/

  struct floorplan_element
  {
    char *Id ; /* The id (string) of the floorplan element */

    int SW_X ;           /* The south-west corner X position (um) */
    int SW_Y ;           /* The south-west corner Y position (um) */

    int Length ;         /* The length of the floorplan element (um) */
    int Width ;          /* The width of the floorplan element (um) */

    int SW_Row ;         /* The row index corresponding to the south-west */
                         /* corner                                        */

    int SW_Column ;      /* The column index corresponding to the south-west */
                         /* corner                                           */

    int NE_Row ;         /* The row index corresponding to the north-east */
                         /* corner                                        */

    int NE_Column ;      /* The column index corresponding to the north-east */
                         /* corner                                           */

    double PowerValue ;  /* The current power value */

    struct floorplan_element *Next ; /* To collect floorplan elements */
                                     /* in a linked list              */

  } ;

  typedef struct floorplan_element FloorplanElement ;

/******************************************************************************/

  void init_floorplan_element(FloorplanElement *floorplan_element) ;

  FloorplanElement *alloc_and_init_floorplan_element (void) ;

  void free_floorplan_element  (FloorplanElement *floorplan_element) ;

  void free_floorplan_elements_list (FloorplanElement *list) ;

  FloorplanElement *find_floorplan_element_in_list (FloorplanElement *list,
                                                    char *id) ;

  void print_floorplan_element (FILE *stream,
                                char *prefix,
                                FloorplanElement *floorplan_element) ;

  void print_floorplan_elements_list (FILE *stream,
                                      char *prefix,
                                      FloorplanElement *list) ;

  int check_intersection (FloorplanElement *floorplan_element_a,
                          FloorplanElement *floorplan_element_b) ;

/******************************************************************************/

#ifdef __cplusplus
}
#endif
#endif /* _TL_FLOORPLAN_ELEMENT_H_ */
