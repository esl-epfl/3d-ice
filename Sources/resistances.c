/******************************************************************************
 *                                                                            *
 * Source file "Sources/resistances.c"                                        *
 *                                                                            *
 * EPFL-STI-IEL-ESL                                                           *
 * Bâtiment ELG, ELG 130                                                      *
 * Station 11                                                                 *
 * 1015 Lausanne, Switzerland                    alessandro.vincenzi@epfl.ch  *
 ******************************************************************************/

#include "resistances.h"

static Resistances *__grid ;
static int         __layer ;

#ifdef DEBUG_BUILD_CELL_GRID
static FILE   *debug ;
static int    __row ;
static int    __column ;
#endif

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

static
void
build_solid_cell (LayerPosition_t position,
                  double length, double width, double height,
                  double thermalconductivity)
{
#ifdef DEBUG_BUILD_CELL_GRID
  fprintf
  (
    debug,
    "build_solid_cell\tl %2d (%d)  r %4d  c %4d" \
                    "\tl %5.2f  w %5.2f h %5.2f" \
                    "\ttc   %.4e sh  %.4e\n",
    __layer, position, __row, __column,
    length, width, height,
    thermalconductivity, specificheat
  ) ;
#endif

  __grid->ResistanceNorth = __grid->ResistanceSouth

    = (thermalconductivity * length * height ) / (width / 2.0) ;

  __grid->ResistanceEast = __grid->ResistanceWest

    = (thermalconductivity * width * height ) / (length / 2.0) ;

  if (position == TL_LAYER_BOTTOM)
  {
    __grid->ResistanceBottom = 0.0 ;

    __grid->ResistanceTop

      = (thermalconductivity * length * width) / height ;

  }
  else if (position == TL_LAYER_TOP)
  {
    __grid->ResistanceTop = 0.0 ;

    __grid->ResistanceBottom

      = (thermalconductivity * length * width) / height ;
  }
  else
  {
    __grid->ResistanceTop = __grid->ResistanceBottom

     = (thermalconductivity * length * width) / (height / 2.0) ;
  }
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

// FlowRate = 1.667e+12;
// FlowArea = #Channels * Ay = 11 * (50 * layer->height);
//
// Velocity = FlowRate / FlowArea
//          = 1.667e+12 / (11 * (50 * layer->height));
//
// C = SpecificHeatH2O * Velocity * Ay * 0.5;
//   = SpecificHeatH2O * FlowRate * ( 1.0 / (  FlowArea    ) ) * Ay * 0.5;
//   = SpecificHeatH2O * FlowRate * ( 1.0 / (#Channels * Ay) ) * Ay * 0.5;
//   = SpecificHeatH2O * FlowRate * ( 1.0 / (#Channels) ) * 0.5;
//   =   4.1692e-12    * 1.667e12 * ( 1.0 /    11.0     ) * 0.5;

static
void
build_liquid_cell (LayerPosition_t position,
                   double length, double width, double height,
                   double liquidhtc, double liquidsh)
{
  double C = liquidsh * 1.62e6 * (length * height ) * 0.5;

#ifdef DEBUG_BUILD_CELL_GRID
  fprintf (debug,
    "build_liquid_cell\tl %2d (%d)  r %4d  c %4d"  \
                     "\tl %5.2f  w %5.2f  h %5.2f" \
                     "\tlhtc %.4e lsh %.4e\n",
    __layer, position, __row, __column,
    length, width, height, liquidhtc, liquidsh) ;
#endif

  if (position == TL_LAYER_BOTTOM)
  {
    fprintf (stderr, "Warning: channel on bottom layer not supported\n") ;
  }
  else if (position == TL_LAYER_TOP)
  {
    fprintf (stderr, "Warning: channel on top layer not supported\n") ;
  }

  __grid->ResistanceNorth =  C ;
  __grid->ResistanceSouth = -C ;

  __grid->ResistanceEast = __grid->ResistanceWest

    = liquidhtc * width * height ;

  __grid->ResistanceTop = __grid->ResistanceBottom

    = liquidhtc * width * height ;
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

static
void
build_cell_grid_layer (Dimensions *dim, Layer *layer)
{
  int row    = 0 ;
  int column = 0 ;

#ifdef DEBUG_BUILD_CELL_GRID
  fprintf (debug,
    "__layer = %d\tbuild_cell_grid_layer   %s\n",
    __layer, layer->Material->Id, layer->Position) ;
#endif

  for (row = 0 ; row < dim->Grid.NRows ; row++)
  {
    for (column = 0 ; column < dim->Grid.NColumns ; column++, __grid++)
    {

#ifdef DEBUG_BUILD_CELL_GRID
      __row    = row ;
      __column = column ;
#endif

      build_solid_cell (get_layer_position (&dim->Grid, __layer),
                        get_cell_length (dim, column),
                        dim->Cell.Width, layer->Height,
                        layer->Material->ThermalConductivity) ;
    } /* column */
  } /* row */
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

static
void
build_cell_grid_channel (Dimensions *dim, Channel *channel)
{
  int row    = 0 ;
  int column = 0 ;

#ifdef DEBUG_BUILD_CELL_GRID
  fprintf (debug, "__layer = %d\tbuild_cell_grid_channel %s\n",
    __layer, channel->WallMaterial->Id) ;
#endif

  for (row = 0 ; row < dim->Grid.NRows ; row++)
  {
    for (column = 0 ; column < dim->Grid.NColumns ; column++, __grid++)
    {

#ifdef DEBUG_BUILD_CELL_GRID
      __row    = row ;
      __column = column ;
#endif

      if (column % 2 == 0) // Even -> wall
      {
        build_solid_cell (get_layer_position (&dim->Grid, __layer),
                          get_cell_length (dim, column),
                          dim->Cell.Width, channel->Height,
                          channel->WallMaterial->ThermalConductivity) ;
      }
      else                 // Odd -> channel
      {
        build_liquid_cell (get_layer_position (&dim->Grid, __layer),
                           get_cell_length (dim, column),
                           dim->Cell.Width, channel->Height,
                           channel->LiquidHTC, channel->LiquidSH) ;
      }
    } /* column */
  } /* row */
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

static
void
build_cell_grid_die (Dimensions *dim, Die *die)
{
  Layer *layer = die->LayersList ;

#ifdef DEBUG_BUILD_CELL_GRID
  fprintf (debug, "__layer = %d\tbuild_cell_grid_die     %s\n",
    __layer, die->Id) ;
#endif

  for ( ; layer != NULL ; layer = layer->Next, __layer++)
  {
     build_cell_grid_layer (dim, layer) ;
  }
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

void
build_resistances_grid (Resistances *grid, StackDescription *stkd)
{
  StackElement *stack_element = stkd->StackElementsList ;

  __grid        = grid ;
  __layer       = 0 ;

#ifdef DEBUG_BUILD_CELL_GRID
  debug = fopen("build_cell_grid.txt", "w") ;
  if (debug == NULL)
  {
    perror("build_cell_grid.txt") ;
    return ;
  }
  fprintf (debug, "build_cell_grid ( l %d r %d c %d )\n",
    stkd->Dimensions.Grid.NLayers,
    stkd->Dimensions.Grid.NRows,
    stkd->Dimensions.Grid.NColumns) ;
#endif

  for ( ; stack_element != NULL ; stack_element = stack_element->Next)
  {
    switch (stack_element->Type)
    {
      case TL_STACK_ELEMENT_DIE :

        build_cell_grid_die (stkd->Dimensions, stack_element->Pointer.Die) ;
        break ;

      case TL_STACK_ELEMENT_LAYER :

        build_cell_grid_layer (stkd->Dimensions, stack_element->Pointer.Layer) ;
        __layer++ ;
        break ;

      case TL_STACK_ELEMENT_CHANNEL :

        build_cell_grid_channel (stkd->Dimensions, stkd->Channel) ;
        __layer++ ;
        break ;

      case TL_STACK_ELEMENT_NONE :

        fprintf (stderr,  "Error! Found stack element with no type\n") ;
        break ;

      default :

        fprintf (stderr, "Error! Unknown staxck element type %d\n",
          stack_element->Type) ;

    } /* stack_elementy->Type */

  } /* stack_element */

#ifdef DEBUG_BUILD_CELL_GRID
  fclose(debug) ;
#endif
}
