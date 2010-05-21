/******************************************************************************
 *                                                                            *
 * Header file "Include/material.h"                                           *
 *                                                                            *
 * EPFL-STI-IEL-ESL                                                           *
 * Bâtiment ELG, ELG 130                                                      *
 * Station 11                                                                 *
 * 1015 Lausanne, Switzerland                    alessandro.vincenzi@epfl.ch  *
 ******************************************************************************/

#ifndef _TL_MATERIAL_H_
#define _TL_MATERIAL_H_

#include <stdio.h>

/******************************************************************************
 *                                "Material"                                  *
 ******************************************************************************/

  struct Material
  {
                                     /*                                       */
    char *Id ;                       /* The Id (string) of the material       */
                                     /*                                       */
    double VolHeatCapacity ;         /* The volume-specific heat capacity     */
                                     /*   [ J / ( um3 * K ) ]                 */
                                     /*                                       */
    double ThermalConductivity ;     /* The thermal conductivity              */
                                     /*   [ W / ( um * K ) ]                  */
                                     /*                                       */
    struct Material *Next ;          /* To collect materials in a linked list */
                                     /*                                       */
  };

/******************************************************************************/

  /* Given a valid address of a Material structure, "init_material" sets all  */
  /* its fields to a default value. If the address is not valid, the call to  */
  /* this function will cause a segmentation fault signal.                    */

  void init_material (struct Material *material) ;

/******************************************************************************/

  /* Allocates in dynamic memory a Material structure and sets its fields to  */
  /* their default values. Returns a valid Material address if the allocation */
  /* succeed or NULL if it fails.                                             */

  struct Material *alloc_and_init_material (void) ;

/******************************************************************************/

  /* Given the address of a Material structure, "free_material" frees both    */
  /* the dynamic memory used by it fields and the memory pointed by the       */
  /* address received. If the address belongs to an automatic variable then   */
  /* using this function will cause a segmentation fault signal.              */

  void free_material (struct Material *material) ;

/******************************************************************************/

  /* Given the address of a Material structure, "free_material_list" frees    */
  /* the material pointed by this address and all the materials it finds      */
  /* following the linked list (see the Next field inside struct material).   */
  /* This function uses the previous "free_material" function so every        */
  /* Material in the linked list must be a dynamically allocated structure.   */

  void free_materials_list (struct Material *list) ;

/******************************************************************************/

  /* Prints on the stream the content of the fields of the material. Since    */
  /* the Id is a string, the behavior of this functions depends on the        */
  /* actual values it contains. Prefix is a string (at least empty and null   */
  /* terminated) that is printed in every line at the before the description. */

  void print_material (FILE *stream, char *prefix, struct Material *material) ;

/******************************************************************************/

  /* Prints on the stream the content of the fields of all the materials it   */
  /* finds following the linked list created with the Next field. It uses     */
  /* the previous "print_material" function. The string "prefix" is used and  */
  /* printed for every material in the list                                   */

  void
  print_materials_list (FILE *stream, char *prefix, struct Material *list) ;

/******************************************************************************/

  /* Given the address of a Material structure and following the linked list, */
  /* this function search a material with id "id" and returns its address.    */
  /* It returns NULL if it does not find a material with such Id.             */

  struct Material *find_material_in_list (struct Material *list, char *id) ;

/******************************************************************************/

#endif /* _TL_MATERIAL_H_ */
