/******************************************************************************
 *                                                                            *
 * Source file "Sources/stack_element.c"                                      *
 *                                                                            *
 * EPFL-STI-IEL-ESL                                                           *
 * Bâtiment ELG, ELG 130                                                      *
 * Station 11                                                                 *
 * 1015 Lausanne, Switzerland                    alessandro.vincenzi@epfl.ch  *
 ******************************************************************************/

#include <stdlib.h>

#include "stack_element.h"

void
init_stack_element (StackElement *stack_element)
{
  stack_element->Type            = TL_STACK_ELEMENT_NONE ;
  stack_element->Pointer.Layer   = NULL ;
  stack_element->Pointer.Die     = NULL ;
  stack_element->Floorplan       = NULL ;
  stack_element->Id              = NULL ;
  stack_element->NLayers         = 0 ;
  stack_element->Next            = NULL ;
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

StackElement *
alloc_and_init_stack_element
(
  void
)
{
  StackElement *stack_element
    = (StackElement *) malloc( sizeof(StackElement) ) ;

  if (stack_element != NULL) init_stack_element(stack_element) ;

  return stack_element ;
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

void
free_stack_element
(
  StackElement *stack_element
)
{
  if (stack_element->Type == TL_STACK_ELEMENT_DIE
      && stack_element->Floorplan != NULL)

    free_floorplan (stack_element->Floorplan) ;

  else if (stack_element->Type == TL_STACK_ELEMENT_LAYER
           && stack_element->Pointer.Layer != NULL)

    free_layer (stack_element->Pointer.Layer) ;

  free (stack_element->Id) ;
  free (stack_element) ;
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

void
free_stack_elements_list
(
  StackElement *list
)
{
  StackElement *next ;

  for ( ; list != NULL; list = next)
    {
      next = list->Next ;
      free_stack_element (list) ;
    }
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

void
print_stack_elements_list
(
  FILE         *stream,
  char         *prefix,
  StackElement *list
)
{
  for ( ; list != NULL ; list = list->Next)
  {
    fprintf (stream, "%s%s  ", prefix, list->Id);

    switch (list->Type)
    {
      case TL_STACK_ELEMENT_NONE :

        fprintf (stream, "NO TYPE\n") ;
        break ;

      case TL_STACK_ELEMENT_DIE :

        fprintf (stream,
          "die     (%s) %s \n",
          list->Pointer.Die->Id, list->Floorplan->FileName) ;
        break ;

      case TL_STACK_ELEMENT_LAYER :

        fprintf (stream,
          "layer   (%s) %5.2f um\n",
          list->Pointer.Layer->Material->Id, list->Pointer.Layer->Height) ;
        break ;

      case TL_STACK_ELEMENT_CHANNEL :

        fprintf (stream, "channel\n") ;
        break ;

      default :

        fprintf (stream, "Error! Unknown type %d\n", list->Type) ;
        break ;
    }
  }
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

StackElement *
find_stack_element_in_list
(
  StackElement *list,
  char         *id
)
{
  for ( ; list != NULL ; list = list->Next)

    if (strcmp(list->Id, id) == 0)  break ;

 return list ;
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
