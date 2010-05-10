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
extern "C"
{
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

  typedef struct
  {
    char *FileName ; /* The name of the file scanned to fill the floorplan */

    int NElements ;   /* The number of floorplan elements in the floorplan */

    FloorplanElement *ElementsList ;     /* The list of floorplan elements */

  } Floorplan ;

/******************************************************************************/

  void init_floorplan (Floorplan *floorplan) ;

  Floorplan *alloc_and_init_floorplan () ;

  void free_floorplan (Floorplan *floorplan) ;

  int fill_floorplan (Floorplan *floorplan, Dimensions *dim) ;

  void print_floorplan (FILE *stream, char* prefix, Floorplan *floorplan) ;

  int check_intersections (Floorplan *floorplan,
                           FloorplanElement *floorplan_element) ;

  int check_location (Floorplan *floorplan,
                      FloorplanElement *floorplan_element,
                      ChipDimensions *cd) ;

  int align_to_grid (Floorplan *floorplan,
                     FloorplanElement *floorplan_element,
                     Dimensions *dim) ;

  void update_floorplan_power_values (Floorplan *floorplan, double *values) ;

/******************************************************************************/

#ifdef __cplusplus
}
#endif
#endif /* _TL_FLOORPLAN_H_ */
