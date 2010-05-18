/******************************************************************************
 *                                                                            *
 * Source file "Sources/channel.c"                                            *
 *                                                                            *
 * EPFL-STI-IEL-ESL                                                           *
 * Bâtiment ELG, ELG 130                                                      *
 * Station 11                                                                 *
 * 1015 Lausanne, Switzerland                    alessandro.vincenzi@epfl.ch  *
 ******************************************************************************/

#include <stdlib.h>

#include "channel.h"
#include "system_matrix.h"

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

void
init_channel
(
  Channel* channel
)
{
  channel->Height           = 0.0  ;
  channel->CoolantHTC       = 0.0  ;
  channel->CoolantVHC       = 0.0  ;
  channel->CoolantTIn       = 0.0  ;
  channel->FlowRate         = 0.0  ;
  channel->FlowRateChanged  = 0    ;
  channel->WallMaterial     = NULL ;
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

Channel *
alloc_and_init_channel
(
  void
)
{
  Channel *channel = (Channel *) malloc ( sizeof(Channel) ) ;

  if (channel != NULL) init_channel (channel) ;

  return channel ;
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

void
free_channel
(
  Channel *channel
)
{
  free (channel) ;
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

void
print_channel
(
  FILE    *stream,
  char    *prefix,
  Channel *channel
)
{
  fprintf(stream,
    "%sChannel\n",                                   prefix) ;
  fprintf(stream,
    "%s  Height                            %5.2f\n", prefix,
                                                     channel->Height) ;
  fprintf(stream,
    "%s  Coolant Heat Transfert Coefficent %.5e\n",  prefix,
                                                     channel->CoolantHTC) ;
  fprintf(stream,
    "%s  Coolant Volumetric Heat Capacity  %.4e\n",  prefix,
                                                     channel->CoolantVHC) ;
  fprintf(stream,
    "%s  Coolant incoming temperature      %.4e\n",  prefix,
                                                     channel->CoolantTIn) ;
  fprintf(stream,
    "%s  Flow Rate                         %.4e\n",  prefix,
                                                     channel->FlowRate) ;

  Material *wall = channel->WallMaterial ;

  if (wall != NULL)
  {
    fprintf(stream,
    "%s  Wall thermal conductivity         %.4e (%s)\n",
    prefix, wall->ThermalConductivity, wall->Id) ;
    fprintf(stream,
    "%s  Wall Volum. Heat Capacity         %.4e (%s)\n",
    prefix, wall->VolHeatCapacity, wall->Id) ;
  }
  else
    fprintf(stream,
    "%s  Wall material not specified\n", prefix) ;
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

Conductances *
fill_conductances_channel
(
#ifdef DEBUG_FILL_CONDUCTANCES
  FILE         *debug,
#endif
  Channel      *channel,
  Conductances *conductances,
  Dimensions   *dimensions,
  int          current_layer
)
{
  int row, column ;

#ifdef DEBUG_FILL_CONDUCTANCES
  fprintf (debug,
    "%p current_layer = %d\tfill_conductances_channel %s\n",
    conductances, current_layer, channel->WallMaterial->Id) ;
#endif

  for
  (
    row = 0 ;
    row < get_number_of_rows (dimensions) ;
    row++
  )

    for
    (
      column = 0 ;
      column < get_number_of_columns (dimensions) ;
      column++,
      conductances++
    )

      if (column % 2 == 0) // Even -> wall

        fill_conductances_solid_cell
        (
#ifdef DEBUG_FILL_CONDUCTANCES
          debug,
          row,
          column,
#endif
          conductances,
          dimensions,
          get_cell_length (dimensions, column),
          get_cell_width (dimensions),
          channel->Height,
          channel->WallMaterial->ThermalConductivity,
          current_layer
        ) ;

      else                 // Odd -> channel

        fill_conductances_liquid_cell
        (
#ifdef DEBUG_FILL_CONDUCTANCES
          debug,
          row,
          column,
#endif
          conductances,
          dimensions,
          get_cell_length (dimensions, column),
          get_cell_width (dimensions),
          channel->Height,
          channel->CoolantHTC,
          channel->CoolantVHC,
          channel->FlowRate,
          current_layer
        ) ;

  return conductances ;
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

static
double
capacity             /*   ( [um3] . [ J / ( um3 . K ) ]) / [sec]  */
(                    /* = [ J / (K . sec) ]                       */
  double length,     /* = [ W / K ]                               */
  double width,
  double height,
  double vhc,
  double time)
{
  return ((length * width * height) * vhc) / time ;
}



double *
fill_capacities_channel
(
#ifdef DEBUG_FILL_CAPACITIES
  FILE       *debug,
  int        current_layer,
#endif
  Channel    *channel,
  double     *capacities,
  Dimensions *dimensions,
  double     delta_time
)
{
  int row, column ;

#ifdef DEBUG_FILL_CAPACITIES
  fprintf (debug,
    "%p current_layer = %d\tfill_capacities_channel %s\n",
    capacities, current_layer, channel->WallMaterial->Id) ;
#endif

  for
  (
    row = 0 ;
    row < get_number_of_rows (dimensions) ;
    row++
  )

    for
    (
      column = 0 ;
      column < get_number_of_columns (dimensions) ;
      column++,
      capacities++
    )

      if (column % 2 == 0 ) /* Even -> Wall */
      {
        *capacities = capacity
                      (
                        get_cell_length (dimensions, column),
                        get_cell_width (dimensions),
                        channel->Height,
                        channel->WallMaterial->VolHeatCapacity,
                        delta_time
                      ) ;
#ifdef DEBUG_FILL_CAPACITIES
      fprintf (debug,
        "%p wall   cell l %5d r %5d c %5d l %5.2f w %5.2f h %5.2f " \
        "vhc %.5e %.5e --> %.5e\n",
        capacities, current_layer, row, column,
        get_cell_length(dimensions, column), get_cell_width (dimensions),
        channel->Height,
        channel->WallMaterial->VolHeatCapacity, delta_time, *capacities) ;
#endif
      }
      else                 /* Odd -> liquid */
      {
        *capacities  = capacity
                       (
                         get_cell_length(dimensions, column),
                         get_cell_width (dimensions),
                         channel->Height,
                         channel->CoolantVHC,
                         delta_time
                       ) ;
#ifdef DEBUG_FILL_CAPACITIES
      fprintf (debug,
        "%p liquid cell l %5d r %5d c %5d l %5.2f w %5.2f h %5.2f " \
        "vhc %.5e %.5e --> %.5e\n",
        capacities, current_layer, row, column,
        get_cell_length(dimensions, column), get_cell_width (dimensions),
        channel->Height,
        channel->CoolantVHC, delta_time, *capacities) ;
#endif
      }

  return capacities ;
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

double *
fill_sources_channel
(
#ifdef DEBUG_FILL_SOURCES
  FILE       *debug,
  int        current_layer,
#endif
  Channel    *channel,
  double     *sources,
  Dimensions *dimensions
)
{
  int row, column ;

  double C = (channel->CoolantVHC * channel->FlowRate)
             / (double) ( get_number_of_columns (dimensions) - 1 );

#ifdef DEBUG_FILL_SOURCES
  fprintf (debug,
    "%p current_layer = %d\tfill_sources_channel %s " \
    " %.5e = (%.5e * %.5e) / %d \n",
    sources, current_layer, channel->WallMaterial->Id,
    C, channel->CoolantVHC, channel->FlowRate,
    get_number_of_columns (dimensions) - 1) ;
#endif

  for
  (
    row = 0 ;
    row < get_number_of_rows (dimensions) ;
    row++
  )

    for
    (
      column = 0 ;
      column < get_number_of_columns (dimensions) ;
      column++,
      sources++
    )

      if (row == 0 && column % 2 != 0) /* Only first row and odd columns */
      {
        *sources = 2.0 * C * channel->CoolantTIn ;

#ifdef DEBUG_FILL_SOURCES
      fprintf (debug,
        "liquid cell l %5d r %5d c %5d (%6d) -> %.5e  (Tin = %.2f)\n",
        current_layer, row, column,
        get_cell_offset_in_stack (dimensions, current_layer, row, column),
        *sources, channel->CoolantTIn) ;
#endif
      }

  return sources ;
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

int
fill_system_matrix_channel
(
#ifdef DEBUG_FILL_SYSTEM_MATRIX
  FILE         *debug,
  Channel      *channel,
#endif
  Dimensions   *dimensions,
  Conductances *conductances,
  double       *capacities,
  int          *columns,
  int          *rows,
  double       *values,
  int          current_layer
)
{
  int row, column, added, tot_added ;

#ifdef DEBUG_FILL_SYSTEM_MATRIX
  fprintf (debug,
    "%p %p %p %p %p (l %2d) fill_system_matrix_channel %s \n",
    conductances, capacities, columns, rows, values,
    current_layer, channel->WallMaterial->Id) ;
#endif

  for
  (
    tot_added = 0 ,
    row       = 0 ;

    row < get_number_of_rows (dimensions) ;

    row++
  )

    for
    (
      column = 0 ;

      column < get_number_of_columns (dimensions) ;

      conductances ++ ,
      capacities   ++ ,
      columns      ++ ,
      rows         += added ,
      values       += added ,
      tot_added    += added ,
      column       ++
    )

       if (column % 2 == 0 ) /* Even -> Wall */

         added = add_solid_column
                 (
#ifdef DEBUG_FILL_SYSTEM_MATRIX
                   debug,
#endif
                   dimensions, conductances, capacities,
                   current_layer, row, column,
                   columns, rows, values
                 ) ;

       else                  /* Odd -> liquid */

         added = add_liquid_column
                 (
#ifdef DEBUG_FILL_SYSTEM_MATRIX
                   debug,
#endif
                   dimensions, conductances, capacities,
                   current_layer, row, column,
                   columns, rows, values
                 ) ;

  return tot_added ;
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
