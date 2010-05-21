/******************************************************************************
 *                                                                            *
 * Header file "Include/system_vector.h"                                      *
 *                                                                            *
 * EPFL-STI-IEL-ESL                                                           *
 * Bâtiment ELG, ELG 130                                                      *
 * Station 11                                                                 *
 * 1015 Lausanne, Switzerland                    alessandro.vincenzi@epfl.ch  *
 ******************************************************************************/

#ifndef _TL_SYSTEM_VECTOR_
#define _TL_SYSTEM_VECTOR_

/******************************************************************************
 *                                                                            *
 * "SystemVector" : the representation of the B vector.                       *
 *                                                                            *
 ******************************************************************************/

  struct SystemVector
  {
    double *Values ;
    int    Size ;

  } ;

/******************************************************************************/

  int
  alloc_system_vector  (struct SystemVector *vector, int size) ;

  void
  fill_system_vector   (
                        struct SystemVector *vector,
                        double       *source,
                        double       *capacity,
                        double       *temperature
                       ) ;

  void
  free_system_vector   (struct SystemVector *vector) ;

/******************************************************************************/

#endif /* _TL_SYSTEM_MATRIX_ */
