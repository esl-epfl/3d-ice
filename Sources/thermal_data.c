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

#include "thermal_data.h"

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
alloc_and_init_thermal_data (ThermalData *tdata, int size,
                             double temperature, double source, double capacity)
{
  if (tdata == NULL) return 0 ;

  tdata->Size = size ;

  tdata->Temperatures
    = (double *) malloc ( sizeof(double) * size ) ;

  if (tdata->Temperatures == NULL)
  {
    return 0 ;
  }

  tdata->Sources
    = (double *) malloc ( sizeof(double) * size ) ;

  if (tdata->Sources == NULL)
  {
    free (tdata->Temperatures) ;
    return 0 ;
  }

  tdata->Capacities
    = (double *) malloc ( sizeof(double) * size ) ;

  if (tdata->Capacities == NULL)
  {
    free (tdata->Temperatures) ;
    free (tdata->Sources) ;
    return 0 ;
  }

  tdata->Resistances
    = (Resistances *) malloc ( sizeof(Resistances) * size ) ;

  if (tdata->Resistances == NULL)
  {
    free (tdata->Temperatures) ;
    free (tdata->Sources) ;
    free (tdata->Capacities) ;
    return 0 ;
  }

  init_data (tdata->Temperatures, size, temperature) ;
  init_data (tdata->Sources,      size, source) ;
  init_data (tdata->Capacities,   size, capacity) ;

  return 1 ;
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

void
free_thermal_data (ThermalData *tdata)
{
  if (tdata == NULL) return ;

  free (tdata->Temperatures) ;
  free (tdata->Sources) ;
  free (tdata->Capacities) ;
  free (tdata->Resistances) ;
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

void
fill_resistances
(
  StackDescription *stkd,
  ThermalData *tdata
)
{
  fill_resistances_stack_description (stkd, tdata->Resistances) ;
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

void
fill_capacities
(
  StackDescription *stkd,
  ThermalData *tdata,
  double delta_time
)
{
  fill_capacities_stack_description (stkd, tdata->Capacities, delta_time) ;
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

void
fill_sources
(
  StackDescription *stkd,
  ThermalData *tdata
)
{
  fill_sources_stack_description (stkd, tdata->Sources) ;
}
