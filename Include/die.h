/******************************************************************************
 *                                                                            *
 * Header file "Include/die.h"                                                *
 *                                                                            *
 * EPFL-STI-IEL-ESL                                                           *
 * Bâtiment ELG, ELG 130                                                      *
 * Station 11                                                                 *
 * 1015 Lausanne, Switzerland                    alessandro.vincenzi@epfl.ch  *
 ******************************************************************************/

#ifndef _TL_DIE_H_
#define _TL_DIE_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdio.h>

#include "layer.h"

/******************************************************************************
 *                                                                            *
 * "Die" : the representation of a single die.                                *
 *                                                                            *
 ******************************************************************************/

  struct die
  {
    char* Id ;               /* The id (string) of the die */

    Layer *LayersList ;      /* The list of layers composing the die */

    int NLayers ;            /* The number of layer composing the die */

    int SourcesId ;          /* The id of the layer with the sources on it */

    struct die* Next ;       /* To collect dies in a linked list */

  } ;

  typedef struct die Die ;

/******************************************************************************/

  void init_die (Die *die) ;

  Die *alloc_and_init_die (void) ;

  void free_die (Die *die) ;

  void free_dies_list (Die *list) ;

  void print_die (FILE *stream, char* prefix, Die *die) ;

  void print_dies_list (FILE *stream, char* prefix, Die *list) ;

  Die *find_die_in_list (Die *list, char *id) ;

/******************************************************************************/

#ifdef __cplusplus
}
#endif
#endif /* _TL_DIE_H_ */
