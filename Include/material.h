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

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdio.h>

/******************************************************************************
 *                                                                            *
 * "Material" : the representation of a single material                       *
 *                                                                            *
 ******************************************************************************/

  struct material
  {
    char *Id ;                       /* The Id (string) of the material       */

    double SpecificHeat ;            /* The specific heat (capacity)          */

    double ThermalConductivity ;     /* The thermal conductivity (resistance) */

    struct material *Next ;          /* To collect materials in a linked list */

  };

  typedef struct material Material ;

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

  void
  init_material            (Material *material) ;

  Material *
  alloc_and_init_material  (void) ;

  void
  free_material            (Material *material) ;

  void
  free_materials_list      (Material *list) ;

  void
  print_material           (FILE *stream, char *prefix, Material *material) ;

  void
  print_materials_list     (FILE *stream, char *prefix, Material *list) ;

  Material *
  find_material_in_list    (Material *list, char *id) ;

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

#ifdef __cplusplus
}
#endif
#endif /* _TL_MATERIAL_H_ */
