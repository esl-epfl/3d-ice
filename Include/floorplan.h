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

  void insert_power_values_floorplan (Floorplan *floorplan,
                                      double *power_values) ;

  void insert_power_value_floorplan (Floorplan *floorplan,
                                      double *power_values) ;

  int insert_power_value_floorplan_element (Floorplan *floorplan,
                                            char *floorplan_element_id,
                                            double power_value) ;

  int get_max_temperature_floorplan
      ( Floorplan *floorplan,
        char *floorplan_element_id,
        Dimensions *dim,
        double *temperatures,
        double *max_temperature);

  int get_min_temperature_floorplan
      ( Floorplan *floorplan,
        char *floorplan_element_id,
        Dimensions *dim,
        double *temperatures,
        double *min_temperature);

  int get_avg_temperature_floorplan
      ( Floorplan *floorplan,
        char *floorplan_element_id,
        Dimensions *dim,
        double *temperatures,
        double *avg_temperature);

  int get_min_avg_max_temperatures_floorplan
      ( Floorplan *floorplan,
        char *floorplan_element_id,
        Dimensions *dim,
        double *temperatures,
        double *min_temperature,
        double *avg_temperature,
        double *max_temperature);


  int get_all_max_temperatures_floorplan
      ( Floorplan *floorplan,
        Dimensions *dim,
        double *temperatures,
        double *max_temperature);

  int get_all_min_temperatures_floorplan
      ( Floorplan *floorplan,
        Dimensions *dim,
        double *temperatures,
        double *min_temperature);

  int get_all_avg_temperatures_floorplan
      ( Floorplan *floorplan,
        Dimensions *dim,
        double *temperatures,
        double *avg_temperature);

  int get_all_min_avg_max_temperatures_floorplan
      ( Floorplan *floorplan,
        Dimensions *dim,
        double *temperatures,
        double *min_temperature,
        double *avg_temperature,
        double *max_temperature);

/******************************************************************************/

#ifdef __cplusplus
}
#endif
#endif /* _TL_FLOORPLAN_H_ */
