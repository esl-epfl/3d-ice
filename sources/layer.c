/******************************************************************************
 * This file is part of 3D-ICE, version 2.2 .                                 *
 *                                                                            *
 * 3D-ICE is free software: you can  redistribute it and/or  modify it  under *
 * the terms of the  GNU General  Public  License as  published by  the  Free *
 * Software  Foundation, either  version  3  of  the License,  or  any  later *
 * version.                                                                   *
 *                                                                            *
 * 3D-ICE is  distributed  in the hope  that it will  be useful, but  WITHOUT *
 * ANY  WARRANTY; without  even the  implied warranty  of MERCHANTABILITY  or *
 * FITNESS  FOR A PARTICULAR  PURPOSE. See the GNU General Public License for *
 * more details.                                                              *
 *                                                                            *
 * You should have  received a copy of  the GNU General  Public License along *
 * with 3D-ICE. If not, see <http://www.gnu.org/licenses/>.                   *
 *                                                                            *
 *                             Copyright (C) 2010                             *
 *   Embedded Systems Laboratory - Ecole Polytechnique Federale de Lausanne   *
 *                            All Rights Reserved.                            *
 *                                                                            *
 * Authors: Arvind Sridhar                                                    *
 *          Alessandro Vincenzi                                               *
 *          Giseong Bak                                                       *
 *          Martino Ruggiero                                                  *
 *          Thomas Brunschwiler                                               *
 *          David Atienza                                                     *
 *                                                                            *
 * For any comment, suggestion or request  about 3D-ICE, please  register and *
 * write to the mailing list (see http://listes.epfl.ch/doc.cgi?liste=3d-ice) *
 * Any usage  of 3D-ICE  for research,  commercial or other  purposes must be *
 * properly acknowledged in the resulting products or publications.           *
 *                                                                            *
 * EPFL-STI-IEL-ESL                                                           *
 * Batiment ELG, ELG 130                Mail : 3d-ice@listes.epfl.ch          *
 * Station 11                                  (SUBSCRIPTION IS NECESSARY)    *
 * 1015 Lausanne, Switzerland           Url  : http://esl.epfl.ch/3d-ice.html *
 ******************************************************************************/

#include <stdlib.h>
#include <string.h>

#include "layer.h"
#include "macros.h"

/******************************************************************************/

void layer_init (Layer_t *layer)
{
    layer->Id     = NULL ;
    layer->Height = (CellDimension_t) 0.0 ;

    material_init (&layer->Material) ;
}

/******************************************************************************/

void layer_copy (Layer_t *dst, Layer_t *src)
{
    layer_destroy (dst) ;

    dst->Id = (src->Id == NULL) ? NULL : strdup (src->Id) ;

    dst->Height   = src->Height ;

    material_copy (&dst->Material, &src->Material) ;
}

/******************************************************************************/

void layer_destroy (Layer_t *layer)
{
    if (layer->Id != NULL)

        free (layer->Id) ;

    material_destroy (&layer->Material) ;

    layer_init (layer) ;
}

/******************************************************************************/

Layer_t *layer_calloc (void)
{
    Layer_t *layer = (Layer_t *) malloc (sizeof(Layer_t));

    if (layer != NULL)

        layer_init (layer) ;

    return layer ;
}

/******************************************************************************/

Layer_t *layer_clone (Layer_t *layer)
{
    if (layer == NULL)

        return NULL ;

    Layer_t *newl = layer_calloc ( ) ;

    if (newl != NULL)

        layer_copy (newl, layer) ;

    return newl ;
}

/******************************************************************************/

void layer_free (Layer_t *layer)
{
    if (layer == NULL)

        return ;

    layer_destroy (layer) ;

    free (layer) ;
}

/******************************************************************************/

bool layer_same_id (Layer_t *layer, Layer_t *other)
{
    return strcmp (layer->Id, other->Id) == 0 ? true : false ;
}

/******************************************************************************/

void layer_print (Layer_t *layer, FILE *stream, String_t prefix)
{
    fprintf (stream,
        "%slayer %s :\n",
        prefix, layer->Id) ;

    fprintf (stream,
        "%s   height %.1f ;\n",
        prefix, layer->Height) ;

    fprintf (stream,
        "%s   material %s ;\n",
        prefix, layer->Material.Id) ;
}

/******************************************************************************/
