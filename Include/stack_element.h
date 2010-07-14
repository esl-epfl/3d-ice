/******************************************************************************
 *                                                                            *
 * Header file "Include/stack_element.h"                                      *
 *                                                                            *
 * EPFL-STI-IEL-ESL                                                           *
 * Bâtiment ELG, ELG 130                                                      *
 * Station 11                                                                 *
 * 1015 Lausanne, Switzerland                    alessandro.vincenzi@epfl.ch  *
 ******************************************************************************/

#ifndef _TL_STACK_ELEMENT_H_
#define _TL_STACK_ELEMENT_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>

#include "layer.h"
#include "die.h"
#include "floorplan.h"

/******************************************************************************
 *                                                                            *
 * "StackElement" : the representation of a single element composing 3DStack. *
 *                                                                            *
 ******************************************************************************/

  enum StackElement_t
  {
    TL_STACK_ELEMENT_NONE = 0,
    TL_STACK_ELEMENT_LAYER   ,
    TL_STACK_ELEMENT_CHANNEL ,
    TL_STACK_ELEMENT_DIE
  } ;

  union StackElement_p
  {
    Layer*       Layer ;
    struct Die   *Die ;
  } ;

  struct StackElement
  {
    char *Id ;                /* The id (string) of the stack element         */

    enum StackElement_t Type ;
                              /* The type of the stack element                */

    union StackElement_p Pointer ;
                              /* The pointer to the effective stack element   */
                              /* corresponding to the value in Type           */

    int NLayers ;             /* The number of layer composing the stack      */
                              /* element                                      */

    struct Floorplan *Floorplan ;
                              /* A pointer to a Floorplan. This field is      */
                              /* used only if Type == TL_STACK_ELEMENT_DIE    */

    int LayersOffset ;        /* The offset (#of layers) counting from the    */
                              /* first layer in the stack                     */

    struct StackElement *Next ;             /* To collect stack elements in a */
                                            /* linked list                    */

  } ;

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

  void
  init_stack_element         (struct StackElement *stack_element) ;

  struct StackElement *
  alloc_and_init_stack_element (void) ;

  void
  free_stack_element         (struct StackElement *stack_element) ;

  void
  free_stack_elements_list   (struct StackElement *list) ;

  void
  print_stack_elements_list  (FILE *stream, char *prefix, struct StackElement *list) ;

  struct StackElement *
  find_stack_element_in_list (struct StackElement *list, char *id) ;

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* _TL_STACK_ELEMENT_H_ */
