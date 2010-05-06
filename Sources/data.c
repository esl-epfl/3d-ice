/******************************************************************************
 *                                                                            *
 * Source file "Sources/data.c"                                               *
 *                                                                            *
 * EPFL-STI-IEL-ESL                                                           *
 * Bâtiment ELG, ELG 130                                                      *
 * Station 11                                                                 *
 * 1015 Lausanne, Switzerland                    alessandro.vincenzi@epfl.ch  *
 ******************************************************************************/

#include <stdlib.h>

#include "data.h"

static
void
init_data (double *data, int size, double init_value)
{
  while (size--) *data++ = init_value ;
}


/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

int
alloc_and_init_data (Data *data, int size,
                     double temperature, double source, double capacity)
{
  if (data == NULL) return 0 ;

  data->Size = size ;

  data->Temperatures
    = (double *) malloc ( sizeof(double) * size ) ;

  if (data->Temperatures == NULL)
  {
    return 0 ;
  }

  data->Sources
    = (double *) malloc ( sizeof(double) * size ) ;

  if (data->Sources == NULL)
  {
    free (data->Temperatures) ;
    return 0 ;
  }

  data->Capacities
    = (double *) malloc ( sizeof(double) * size ) ;

  if (data->Capacities == NULL)
  {
    free (data->Temperatures) ;
    free (data->Sources) ;
    return 0 ;
  }

  data->Resistances
    = (Resistances *) malloc ( sizeof(Resistances) * size ) ;

  if (data->Resistances == NULL)
  {
    free (data->Temperatures) ;
    free (data->Sources) ;
    free (data->Capacities) ;
    return 0 ;
  }

  init_data (data->Temperatures, data->Size, temperature) ;
  init_data (data->Sources,      data->Size, source) ;
  init_data (data->Capacities,   data->Size, capacity) ;

  return 1 ;
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

void
free_data (Data *data)
{
  if (data == NULL) return ;

  free (data->Temperatures) ;
  free (data->Sources) ;
  free (data->Capacities) ;
  free (data->Resistances) ;
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

void
fill_resistances
(
  StackDescription *stkd,
  Data *data
)
{
  fill_resistances_stack_description(stkd, data->Resistances) ;
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
