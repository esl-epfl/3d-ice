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
#include "resistances.h"

/******************************************************************************
 *                                                                            *
 * "StackDescription" : the representation of the 3DStack.                    *
 *                                                                            *
 ******************************************************************************/

  typedef struct
  {
    char         *FileName ; /* The name of the file used to fill the */
                             /* stack description                     */

    Material     *MaterialsList ; /* The list of materials componing */
                                  /* the layers and channels         */

    Channel      *Channel ;  /* The "single" instance of a channel used */
                             /* to compose the 3d stack                 */

    Die          *DiesList ; /* The list of dies available to compose */
                             /* the 3Dstack                           */

    StackElement *StackElementsList ; /* The list of stack elements */
                                      /* componing the 3Dstack      */

    Dimensions   *Dimensions ;  /* Collection of all the dimensions */
                                /* (chip, grid of cells, cell)      */

  } StackDescription ;

/******************************************************************************/

  void init_stack_description (StackDescription *stkd) ;

  int fill_stack_description (StackDescription *stkd, char *filename) ;

  void free_stack_description (StackDescription *stkd) ;

  void print_stack_description (FILE *stream,
                                char *prefix,
                                StackDescription *stkd) ;

  void fill_resistances_stack_description (StackDescription *stkd,
                                           Resistances *resistances) ;

  void fill_capacities_stack_description  (StackDescription *stkd,
                                           double *capacities,
                                           double delta_time) ;

  void fill_sources_stack_description (StackDescription *stkd,
                                       double *sources) ;

  void fill_system_matrix_stack_description (
  StackDescription *stkd,
  Resistances *resistances,
  double *capacities,
  int *columns,
  int *rows,
  double *values) ;

  void update_stack_description_power_values (StackDescription *stkd,
                                              double *power_values );

/******************************************************************************/

#ifdef __cplusplus
}
#endif
#endif /* _TL_STACK_DESCRIPTION_H_ */
