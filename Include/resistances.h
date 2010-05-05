/******************************************************************************
 *                                                                            *
 * Header file "Include/resistances.h"                                        *
 *                                                                            *
 * EPFL-STI-IEL-ESL                                                           *
 * Bâtiment ELG, ELG 130                                                      *
 * Station 11                                                                 *
 * 1015 Lausanne, Switzerland                    alessandro.vincenzi@epfl.ch  *
 ******************************************************************************/

#ifndef _TL_RESISTANCES_H_
#define _TL_RESISTANCES_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include "stack_description.h"

/******************************************************************************
 *                                                                            *
 * "Resistances" : the six resistances inside a cell.                         *
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

  } Resistances ;

/******************************************************************************/

  void build_resistances_grid (Resistances *grid, StackDescription *stkd) ;

/******************************************************************************/

#ifdef __cplusplus
}
#endif
#endif /* _TL_RESISTANCES_H_ */
