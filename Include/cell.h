/******************************************************************************
 *                                                                            *
 * Header file "Include/cell.h"                                               *
 *                                                                            *
 * EPFL-STI-IEL-ESL                                                           *
 * Bâtiment ELG, ELG 130                                                      *
 * Station 11                                                                 *
 * 1015 Lausanne, Switzerland                    alessandro.vincenzi@epfl.ch  *
 ******************************************************************************/

#ifndef _TL_CELL_H_
#define _TL_CELL_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include "stack_description.h"

/******************************************************************************
 *                                                                            *
 * "Cell" : the representation of a single cell.                              *
 *                                                                            *
 ******************************************************************************/

  typedef struct
  {
    double ResistanceNorth ;
    double ResistanceSouth ;

    double ResistanceEast ;
    double ResistanceWest ;

    double ResistanceTop ;
    double ResistanceBottom ;

    double Capacity ;

  } Cell ;

/******************************************************************************/

  void build_cell_grid (StackDescription *stkd,
                        Cell *cell_grid,
                        double delta_time) ;

/******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* _TL_CELL_H_ */
