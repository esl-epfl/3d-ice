/******************************************************************************
 *                                                                            *
 * Header file "Include/stack_descritption.h"                                 *
 *                                                                            *
 * EPFL-STI-IEL-ESL                                                           *
 * Bâtiment ELG, ELG 130                                                      *
 * Station 11                                                                 *
 * 1015 Lausanne, Switzerland                    alessandro.vincenzi@epfl.ch  *
 ******************************************************************************/

#ifndef _TL_STACK_DESCRIPTION_H_
#define _TL_STACK_DESCRIPTION_H_

#ifdef __cplusplus
extern "C"
{
#endif


#include <stdio.h>

#include "material.h"
#include "channel.h"
#include "die.h"
#include "stack_element.h"
#include "dimensions.h"
#include "conductances.h"
#include "environment_heat_sink.h"

/******************************************************************************
 *                                                                            *
 * "StackDescription" : the collection of data concerning a 3Dstack           *
 *                                                                            *
 ******************************************************************************/

  struct StackDescription
  {
    char         *FileName ;         /* The name of the file used to fill the */
                                     /* stack description                     */

    Material *MaterialsList ;              /* The list of materials componing */
                                           /* the layers and channels         */

    Channel *Channel ;      /* The "single" instance of a channel used */
                                   /* to compose the 3d stack                 */

    struct Die   *DiesList ;         /* The list of dies available to compose */
                                     /* the 3Dstack                           */

    EnvironmentHeatSink* EnvironmentHeatSink;
                                          /* Informations about the heat      */
                                          /* dissipation throught the top     */
                                          /* surface                          */

    struct StackElement *StackElementsList ;
                                          /* The list of stack elements       */
                                          /* componing the 3Dstack            */

    Dimensions* Dimensions ;              /* Collection of all the dimensions */
                                          /* (chip, grid of cells, cell)      */

    int PowerValuesChanged ;

  } ;

  typedef struct StackDescription StackDescription ;

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

  void
  init_stack_description               (struct StackDescription *stkd) ;

  int
  fill_stack_description               (
                                        struct StackDescription *stkd,
                                        char *filename
                                       ) ;

  void
  free_stack_description               (struct StackDescription *stkd) ;

  void
  print_stack_description              (
                                        FILE             *stream,
                                        char             *prefix,
                                        struct StackDescription *stkd
                                       ) ;

  void
  print_all_floorplans                 (
                                        FILE             *stream,
                                        char             *prefix,
                                        struct StackDescription *stkd
                                       ) ;

  void
  fill_conductances_stack_description  (
                                        struct StackDescription *stkd,
                                        struct Conductances     *conductances
                                       ) ;

  void
  fill_capacities_stack_description    (
                                        struct StackDescription *stkd,
                                        double           *capacities,
                                        double           delta_time
                                       ) ;

  void
  fill_sources_stack_description       (
                                        struct StackDescription* stkd,
                                        Source_t*                sources,
                                        Conductances*            conductances
                                       ) ;

  void
  fill_ccs_system_matrix_stack_description (
                                            struct StackDescription *stkd,
                                            struct Conductances     *conductances,
                                            double           *capacities,
                                            int              *columns,
                                            int              *rows,
                                            double           *values
                                           ) ;

  void
  fill_crs_system_matrix_stack_description (
                                            struct StackDescription *stkd,
                                            struct Conductances     *conductances,
                                            double           *capacities,
                                            int              *rows,
                                            int              *columns,
                                            double           *values
                                           ) ;

  int
  get_total_number_of_floorplan_elements
                              (
                               struct StackDescription *stkd
                              ) ;

  int
  get_number_of_floorplan_elements_in_floorplan
                              (
                               struct StackDescription *stkd,
                               char             *stack_element_id
                              ) ;

  void
  change_coolant_flow_rate (struct StackDescription *stkd, double flow_rate) ;

//  void
//  insert_all_power_values     (struct StackDescription *stkd, double *power_values) ;

//  int
//  insert_power_values_in_floorplan
//                              (
//                               struct StackDescription *stkd,
//                               char             *stack_element_id,
//                               double           *power_values
//                              ) ;

//  int
//  insert_power_value_in_floorplan_element
//                              (
//                               struct StackDescription *stkd,
//                               char             *stack_element_id,
//                               char             *floorplan_element_id,
//                               double           power_value
//                              ) ;

  int
  get_max_temperature_in_floorplan_element
                              (
                               struct StackDescription *stkd,
                               char             *stack_element_id,
                               char             *floorplan_element_id,
                               double           *temperatures,
                               double           *max_temperature
                              ) ;

  int
  get_min_temperature_in_floorplan_element
                              (
                               struct StackDescription *stkd,
                               char             *stack_element_id,
                               char             *floorplan_element_id,
                               double           *temperatures,
                               double           *min_temperature
                              ) ;

  int
  get_avg_temperature_in_floorplan_element
                              (
                               struct StackDescription *stkd,
                               char             *stack_element_id,
                               char             *floorplan_element_id,
                               double           *temperatures,
                               double           *avg_temperature
                              ) ;

  int
  get_min_avg_max_temperatures_in_floorplan_element
                              (
                               struct StackDescription *stkd,
                               char             *stack_element_id,
                               char             *floorplan_element_id,
                               double           *temperatures,
                               double           *min_temperature,
                               double           *avg_temperature,
                               double           *max_temperature
                              ) ;

  int
  get_all_max_temperatures_in_floorplan
                              (
                               struct StackDescription *stkd,
                               char   *stack_element_id,
                               double *temperatures,
                               double *max_temperature
                              ) ;

  int
  get_all_min_temperatures_in_floorplan
                              (
                               struct StackDescription *stkd,
                               char             *stack_element_id,
                               double           *temperatures,
                               double           *min_temperature
                              ) ;

  int
  get_all_avg_temperatures_in_floorplan
                              (
                               struct StackDescription *stkd,
                               char             *stack_element_id,
                               double           *temperatures,
                               double           *avg_temperature
                              ) ;

  int
  get_all_min_avg_max_temperatures_in_floorplan
                              (
                               struct StackDescription *stkd,
                               char             *stack_element_id,
                               double           *temperatures,
                               double           *min_temperature,
                               double           *avg_temperature,
                               double           *max_temperature
                              ) ;

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* _TL_STACK_DESCRIPTION_H_ */
