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

#include "die.h"
#include "macros.h"

/******************************************************************************/

void die_init (Die_t *die)
{
    die->Id                = NULL ;
    die->NLayers           = (CellIndex_t) 0u ;
    die->SourceLayerOffset = (CellIndex_t) 0u ;

    layer_list_init (&die->Layers) ;

    floorplan_init (&die->Floorplan) ;
}

/******************************************************************************/

void die_copy (Die_t *dst, Die_t *src)
{
    die_destroy (dst) ;

    dst->Id = (src->Id == NULL) ? NULL : strdup (src->Id) ;

    dst->NLayers           = src->NLayers ;
    dst->SourceLayerOffset = src->SourceLayerOffset ;

    layer_list_copy (&dst->Layers, &src->Layers) ;

    floorplan_copy (&dst->Floorplan, &src->Floorplan) ;
}

/******************************************************************************/

void die_destroy (Die_t *die)
{
    if (die->Id != NULL)

        free (die->Id) ;

    layer_list_destroy (&die->Layers) ;
    floorplan_destroy  (&die->Floorplan) ;

    die_init (die) ;
}

/******************************************************************************/

Die_t *die_calloc (void)
{
    Die_t *die = (Die_t *) malloc (sizeof(Die_t)) ;

    if (die != NULL)

        die_init (die) ;

    return die ;
}

/******************************************************************************/

Die_t *die_clone (Die_t *die)
{
    if (die == NULL)

        return NULL ;

    Die_t *newd = die_calloc ( ) ;

    if (newd != NULL)

        die_copy (newd, die) ;

    return newd ;
}

/******************************************************************************/

void die_free (Die_t *die)
{
    if (die == NULL)

        return ;

    die_destroy (die) ;

    free (die) ;
}

/******************************************************************************/

bool die_same_id (Die_t *die, Die_t *other)
{
    return strcmp (die->Id, other->Id) == 0 ? true : false ;
}

/******************************************************************************/

void die_print (Die_t *die, FILE *stream, String_t prefix)
{
    fprintf (stream, "%sdie %s :\n", prefix, die->Id) ;

    Quantity_t counter = die->NLayers ;

    LayerListNode_t *lnd ;

    for (lnd = layer_list_begin (&die->Layers) ;
         lnd != NULL ;
         lnd = layer_list_next (lnd))
    {
        if (--counter == die->SourceLayerOffset)

            fprintf (stream,
                "%s   source  %4.1f %s ;\n",
                prefix,
                layer_list_data(lnd)->Height,
                layer_list_data(lnd)->Material.Id) ;

        else

            fprintf (stream,
                "%s    layer  %4.1f %s ;\n",
                prefix,
                layer_list_data(lnd)->Height,
                layer_list_data(lnd)->Material.Id) ;
    }
}

/******************************************************************************/
