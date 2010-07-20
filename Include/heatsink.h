/******************************************************************************
 *                                                                            *
 * Header file "Include/heatsink.h"                                           *
 *                                                                            *
 * EPFL-STI-IEL-ESL                                                           *
 * Bâtiment ELG, ELG 130                                                      *
 * Station 11                                                                 *
 * 1015 Lausanne, Switzerland                    alessandro.vincenzi@epfl.ch  *
 ******************************************************************************/

#ifndef _TL_HEATSINK_H_
#define _TL_HEATSINK_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdio.h>

#include "types.h"

/******************************************************************************
 *                                 "Heatsink"                                 *
 ******************************************************************************/

  struct Heatsink
  {
    HeatsinkHTC_t HeatTransferC ; /* The heat transfert coefficent */

    Temperature_t EnvironmentT ; /* The temperarute of the environment [K] */
  } ;

  typedef struct Heatsink Heatsink ;

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

  void init_heatsink (Heatsink* heatsink) ;

  Heatsink * alloc_and_init_heatsink (void) ;

  void free_heatsink (Heatsink* heatsink) ;

  void print_heatsink (FILE* stream, String_t prefix, Heatsink* heatsink) ;

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* _TL_HEATSINK_H_ */
