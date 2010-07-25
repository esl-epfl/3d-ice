/******************************************************************************
 *                                                                            *
 * Source file "Sources/heatsink.c"                                           *
 *                                                                            *
 * EPFL-STI-IEL-ESL                                                           *
 * Bâtiment ELG, ELG 130                                                      *
 * Station 11                                                                 *
 * 1015 Lausanne, Switzerland                    alessandro.vincenzi@epfl.ch  *
 ******************************************************************************/

#include <stdlib.h>

#include "heatsink.h"

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

void init_heatsink (HeatSink* heatsink)
{
  heatsink->HeatTransferC = 0.0 ;
  heatsink->EnvironmentT  = 0.0 ;
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

HeatSink* alloc_and_init_heatsink (void)
{
  HeatSink* heatsink = (HeatSink*) malloc ( sizeof(HeatSink) ) ;

  if (heatsink != NULL) init_heatsink (heatsink) ;

  return heatsink ;
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

void free_heatsink (HeatSink* heatsink)
{
  free (heatsink) ;
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

void print_heatsink (FILE* stream, String_t prefix, HeatSink* heatsink)
{
  fprintf(stream,
    "%sHeatsink\n",                prefix) ;
  fprintf(stream,
    "%s  Heat Transfert Coefficent %.5e\n",  prefix, heatsink->HeatTransferC) ;
  fprintf(stream,
    "%s  Environment temperature   %.4e\n",  prefix, heatsink->EnvironmentT) ;
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

// void fill_conductances_heatsink
// (
//   HeatSink* heatsink,
//   Layer* layer,
//   struct Conductances* conductances
// )
// {
//   CellDimension_t cell_length,
//                   cell_width = get_cell_width(s;
//
//
//       conductances->Top
//         = ((Conductance_t) 2 * thermal_conductivity * heatsink->HeatTransferC
//            * cell_length * cell_width)
//           /
//           (cell_height * heatsink->HeatTransferC
//            + (Conductance_t) 2 * thermal_conductivity) ;
//
//       conductances->Bottom
//         = (thermal_conductivity * cell_length * cell_width)
//           / (cell_height / (CellDimension_t) 2) ;
// }

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
