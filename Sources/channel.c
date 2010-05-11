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

void
init_channel (Channel* channel)
{
  if (channel == NULL) return ;

  channel->Height       = 0.0 ;
  channel->LiquidHTC    = 0.0 ;
  channel->LiquidSH     = 0.0 ;
  channel->WallMaterial = NULL ;
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

Channel *
alloc_and_init_channel (void)
{
  Channel *channel = (Channel *) malloc ( sizeof(Channel) ) ;

  init_channel (channel) ;

  return channel ;
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

void
free_channel (Channel *channel)
{
  if (channel == NULL) return ;

  free (channel) ;
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

void print_channel (FILE *stream, char *prefix, Channel *channel)
{
  fprintf(stream, "%sChannel\n", prefix) ;
  fprintf(stream, "%s  Height                           %5.2f\n",
    prefix, channel->Height) ;
  fprintf(stream, "%s  Liquid heat transfert coefficent %.4e\n",
    prefix, channel->LiquidHTC) ;
  fprintf(stream, "%s  Liquid specific heat             %.4e\n",
    prefix, channel->LiquidSH) ;

  Material *wall = channel->WallMaterial ;

  if (wall != NULL)
  {
    fprintf(stream, "%s  Wall thermal conductivity        %.4e (%s)\n",
      prefix, wall->ThermalConductivity, wall->Id) ;
    fprintf(stream, "%s  Wall specific heat               %.4e (%s)\n",
      prefix, wall->SpecificHeat, wall->Id) ;
  }
  else
    fprintf(stream, "%s  Wall material not specified\n", prefix) ;
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

Resistances *
fill_resistances_channel
(
#ifdef DEBUG_FILL_RESISTANCES
  FILE *debug,
#endif
  Channel *channel,
  Resistances *resistances,
  Dimensions *dim,
  int current_layer
)
{
  int row, column ;

#ifdef DEBUG_FILL_RESISTANCES
  fprintf (debug,
    "%p current_layer = %d\tfill_resistances_channel %s\n",
    resistances, current_layer, channel->WallMaterial->Id) ;
#endif

  for (row = 0 ; row < dim->Grid.NRows ; row++)
  {
    for (column = 0 ; column < dim->Grid.NColumns ; column++, resistances++)
    {
      if (column % 2 == 0) // Even -> wall
      {
        fill_resistances_solid_cell
        (
#ifdef DEBUG_FILL_RESISTANCES
          debug,
          row,
          column,
#endif
          resistances,
          dim,
          get_cell_length (dim, column),
          dim->Cell.Width,
          channel->Height,
          channel->WallMaterial->ThermalConductivity,
          current_layer
        ) ;
      }
      else                 // Odd -> channel
      {
        fill_resistances_liquid_cell
        (
#ifdef DEBUG_FILL_RESISTANCES
          debug,
          row,
          column,
#endif
          resistances,
          dim,
          get_cell_length (dim, column),
          dim->Cell.Width,
          channel->Height,
          channel->LiquidHTC,
          channel->LiquidSH,
          current_layer
        ) ;
      }
    } /* column */
  } /* row */

  return resistances ;
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

static
double
capacity (double l, double w, double h, double s, double t)
{
  return (l * w * h * s) / t ;
}

double *
fill_capacities_channel
(
#ifdef DEBUG_FILL_CAPACITIES
  FILE *debug,
  int current_layer,
#endif
  Channel *channel,
  double *capacities,
  Dimensions *dim,
  double delta_time
)
{
  int row, column ;

#ifdef DEBUG_FILL_CAPACITIES
  fprintf (debug,
    "%p current_layer = %d\tfill_capacities_channel %s\n",
    capacities, current_layer, channel->WallMaterial->Id) ;
#endif

  for (row = 0 ; row < dim->Grid.NRows ; row++)
  {
    for (column = 0 ; column < dim->Grid.NColumns ; column++, capacities++)
    {
      if (column % 2 == 0 ) /* Even -> Wall */
      {
        *capacities = capacity
                      (
                        get_cell_length(dim, column),
                        dim->Cell.Width,
                        channel->Height,
                        channel->WallMaterial->SpecificHeat,
                        delta_time
                      ) ;
#ifdef DEBUG_FILL_CAPACITIES
      fprintf (debug,
        "%p wall   cell l %5d r %5d c %5d l %5.2f w %5.2f h %5.2f " \
        "sh %.5e %.5e --> %.5e\n",
        capacities, current_layer, row, column,
        get_cell_length(dim, column), dim->Cell.Width, channel->Height,
        channel->WallMaterial->SpecificHeat, delta_time, *capacities) ;
#endif
      }
      else                 /* Odd -> liquid */
      {
        *capacities  = capacity
                       (
                         get_cell_length(dim, column),
                         dim->Cell.Width,
                         channel->Height,
                         channel->LiquidSH,
                         delta_time
                       ) ;
#ifdef DEBUG_FILL_CAPACITIES
      fprintf (debug,
        "%p liquid cell l %5d r %5d c %5d l %5.2f w %5.2f h %5.2f " \
        "sh %.5e %.5e --> %.5e\n",
        capacities, current_layer, row, column,
        get_cell_length(dim, column), dim->Cell.Width, channel->Height,
        channel->LiquidSH, delta_time, *capacities) ;
#endif
      }
    } /* column */
  } /* row */

  return capacities ;
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

double *
fill_sources_channel
(
#ifdef DEBUG_FILL_SOURCES
  FILE *debug,
  int current_layer,
#endif
  Channel *channel,
  double *sources,
  Dimensions *dim
)
{
  int row, column ;

  double C
    = channel->LiquidSH * 1.62e6 * 0.5 * (dim->Cell.Length * channel->Height ) ;

#ifdef DEBUG_FILL_SOURCES
  fprintf (debug,
    "%p current_layer = %d\tfill_sources_channel %s %.5e\n",
    sources, current_layer, channel->WallMaterial->Id, C) ;
#endif

  for (row = 0 ; row < dim->Grid.NRows ; row++)
  {
    for (column = 0 ; column < dim->Grid.NColumns ; column++, sources++)
    {
      if (row == 0 && column % 2 != 0) /* Only first row and odd columns */
      {
        *sources = 2.0 * C * 300.0 ;
#ifdef DEBUG_FILL_SOURCES
      fprintf (debug,
        "liquid cell l %5d r %5d c %5d (%6d) -> %.5e\n",
        current_layer, row, column,
        current_layer * (dim->Grid.NColumns * dim->Grid.NRows)
          + row * dim->Grid.NColumns + column, *sources) ;
#endif
      }

    }
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
  FILE *debug,
  Channel *channel,
#endif
  Dimensions *dim,
  Resistances *resistances,
  double *capacities,
  int *columns,
  int *rows,
  double *values,
  int current_layer
)
{
  int c_row, c_column, added, tot_added ;

#ifdef DEBUG_FILL_SYSTEM_MATRIX
  fprintf (debug,
    "%p %p %p %p %p (l %2d) fill_system_matrix_channel %s \n",
    resistances, capacities, columns, rows, values,
    current_layer, channel->WallMaterial->Id) ;
#endif

  for (tot_added = 0 , c_row = 0 ; c_row < dim->Grid.NRows ; c_row++)
  {
    for
    (
      c_column = 0 ;

      c_column < dim->Grid.NColumns ;

      resistances ++ ,
      capacities  ++ ,
      columns     ++ ,
      rows        += added ,
      values      += added ,
      tot_added   += added ,
      c_column    ++
    )
    {
       if (c_column % 2 == 0 ) /* Even -> Wall */
       {
         added = add_solid_column (
#ifdef DEBUG_FILL_SYSTEM_MATRIX
                   debug,
#endif
                   dim, resistances, capacities,
                   current_layer, c_row, c_column,
                   columns, rows, values) ;
       }
       else                  /* Odd -> liquid */
       {
         added = add_liquid_column (
#ifdef DEBUG_FILL_SYSTEM_MATRIX
                   debug,
#endif
                   dim, resistances, capacities,
                   current_layer, c_row, c_column,
                   columns, rows, values) ;
       }
    } /* column */
  } /* row */

  return tot_added ;
}
