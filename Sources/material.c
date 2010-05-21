/******************************************************************************
 *                                                                            *
 * Source file "Sources/material.c"                                           *
 *                                                                            *
 * EPFL-STI-IEL-ESL                                                           *
 * Bâtiment ELG, ELG 130                                                      *
 * Station 11                                                                 *
 * 1015 Lausanne, Switzerland                    alessandro.vincenzi@epfl.ch  *
 ******************************************************************************/

#include <stdlib.h>
#include <string.h>

#include "material.h"

/******************************************************************************/

void init_material (struct Material *material)
{
  material->Id                  = NULL ;
  material->VolHeatCapacity     = 0.0  ;
  material->ThermalConductivity = 0.0  ;
  material->Next                = NULL ;
}

/******************************************************************************/

struct Material *alloc_and_init_material (void)
{
  struct Material *material
    = (struct Material *) malloc (sizeof (struct Material)) ;

  if (material != NULL) init_material (material) ;

  return material ;
}

/******************************************************************************/

void free_material (struct Material *material)
{
  free (material->Id) ;
  free (material) ;
}

/******************************************************************************/

void free_materials_list (struct Material *list)
{
  struct Material *next ;

  for ( ; list != NULL ; list = next)
  {
      next = list->Next ;
      free_material (list) ;
  }
}

/******************************************************************************/

void print_material (FILE *stream, char *prefix, struct Material *material)
{
  fprintf (stream,
    "%sMaterial %s:\n",                    prefix,
                                           material->Id) ;
  fprintf (stream,
    "%s  Volumetric Heat Capacity %.3e\n", prefix,
                                           material->VolHeatCapacity) ;
  fprintf (stream,
    "%s  Thermal Conductivity     %.3e\n", prefix,
                                           material->ThermalConductivity) ;
}

/******************************************************************************/

void print_materials_list (FILE *stream, char *prefix, struct Material *list)
{
  for ( ; list != NULL ; list = list->Next)

    print_material (stream, prefix, list) ;
}

/******************************************************************************/

struct Material *find_material_in_list (struct Material *list, char *id)
{
  for ( ; list != NULL ; list = list->Next)

    if (strcmp (list->Id, id) == 0) break ;

  return list ;
}

/******************************************************************************/
