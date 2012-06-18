/******************************************************************************
 * This file is part of 3D-ICE, version 2.1 .                                 *
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

void init_die (Die_t *this)
{
    this->Id                = NULL ;
    this->Used              = (Quantity_t) 0u ;
    this->NLayers           = (CellIndex_t) 0u ;
    this->SourceLayerOffset = (CellIndex_t) 0u ;
    this->TopLayer          = NULL ;
    this->SourceLayer       = NULL ;
    this->BottomLayer       = NULL ;
    this->Next              = NULL ;
}

/******************************************************************************/

void copy_die (Die_t *dst, Die_t *src)
{
    dst->Id = (src->Id == NULL) ? NULL : strdup (src->Id) ;

    dst->Used              = src->Used ;
    dst->NLayers           = src->NLayers ;
    dst->SourceLayerOffset = src->SourceLayerOffset ;
    dst->Next              = src->Next ;

    if (dst->BottomLayer != NULL)

        free_layers_list (dst->BottomLayer) ;

    dst->BottomLayer = clone_layers_list (src->BottomLayer) ;

    Layer_t *tmp_dst = dst->BottomLayer ;

    FOR_EVERY_ELEMENT_IN_LIST_NEXT (Layer_t, tmp_src, src->BottomLayer)
    {
        if (tmp_src == src->SourceLayer)

            dst->SourceLayer = tmp_dst ;

        dst->TopLayer = tmp_dst ;
        tmp_dst = tmp_dst->Next ;
    }
}

/******************************************************************************/

Die_t *calloc_die (void)
{
    Die_t *die = (Die_t *) malloc (sizeof(Die_t)) ;

    if (die != NULL)

        init_die (die) ;

    return die ;
}

/******************************************************************************/

Die_t *clone_die (Die_t *this)
{
    if (this == NULL)

        return NULL ;

    Die_t *die = calloc_die ( ) ;

    if (die != NULL)

        copy_die (die, this) ;

    return die ;
}

/******************************************************************************/

void free_die (Die_t *this)
{
    if (this == NULL)

        return ;

    if (this->Id != NULL)

        FREE_POINTER (free, this->Id) ;

    this->TopLayer = NULL ;
    FREE_POINTER (free_layers_list, this->BottomLayer) ;

    FREE_POINTER (free, this) ;
}

/******************************************************************************/

Die_t *clone_dies_list (Die_t *list)
{
    if (list == NULL)

        return NULL ;

    Die_t *new_list = NULL ;
    Die_t *prev     = NULL ;

    FOR_EVERY_ELEMENT_IN_LIST_NEXT (Die_t, die, list)
    {
        Die_t *tmp = clone_die (die) ;

        if (tmp == NULL)
        {
            free_dies_list (new_list) ;

            new_list = NULL ;

            break ;
        }

        if (new_list == NULL)

            new_list = tmp ;

        else

            prev->Next = tmp ;

        prev = tmp ;
    }

    return new_list ;
}

/******************************************************************************/

void free_dies_list (Die_t *list)
{
    FREE_LIST (Die_t, list, free_die) ;
}

/******************************************************************************/

Die_t *find_die_in_list (Die_t *list, String_t id)
{
    FOR_EVERY_ELEMENT_IN_LIST_NEXT (Die_t, die, list)
    {
        if (strcmp(die->Id, id) == 0) break ;
    }
    return die ;
}

/******************************************************************************/

void print_formatted_die (Die_t *this, FILE *stream, String_t prefix)
{
    fprintf (stream, "%sdie %s :\n", prefix, this->Id) ;

    FOR_EVERY_ELEMENT_IN_LIST_PREV (Layer_t, layer, this->TopLayer)
    {
        if (layer == this->SourceLayer)

            fprintf (stream,
                "%s   source  %4.1f %s ;\n",
                prefix, layer->Height, layer->Material->Id) ;

        else

            fprintf (stream,
                "%s    layer  %4.1f %s ;\n",
                prefix, layer->Height, layer->Material->Id) ;
    }
}

/******************************************************************************/

void print_formatted_dies_list (Die_t *list, FILE *stream, String_t prefix)
{
    FOR_EVERY_ELEMENT_IN_LIST_NEXT (Die_t, die, list)
    {
        if (die->Next == NULL)

            break ;

        print_formatted_die (die, stream, prefix) ;

        fprintf (stream, "%s\n", prefix) ;
    }

    print_formatted_die (die, stream, prefix) ;
}

/******************************************************************************/

void print_detailed_die (Die_t *this, FILE *stream, String_t prefix)
{
    String_t new_prefix = (String_t)

        malloc (sizeof (*new_prefix) * (5 + strlen(prefix))) ;

    if (new_prefix == NULL) return ;

    sprintf (new_prefix, "%s    ", prefix) ;

    fprintf (stream,
        "%sdie                         = %p\n",
        prefix, this) ;

    fprintf (stream,
        "%s  Id                        = %s\n",
        prefix, this->Id) ;

    fprintf (stream,
        "%s  Used                      = %d\n",
        prefix, this->Used) ;

    fprintf (stream,
        "%s  NLayers                   = %d\n",
        prefix, this->NLayers) ;

    fprintf (stream,
        "%s  SourceLayerOffset         = %d\n",
        prefix, this->SourceLayerOffset) ;

    fprintf (stream,
        "%s  TopLayer                  = %p\n",
        prefix, this->TopLayer) ;

    fprintf (stream,
        "%s  SourceLayer               = %p\n",
        prefix, this->SourceLayer) ;

    fprintf (stream,
        "%s  BottomLayer               = %p\n",
        prefix, this->BottomLayer) ;

    fprintf (stream, "%s\n", prefix) ;

    print_detailed_layers_list (this->BottomLayer, stream, new_prefix) ;

    fprintf (stream, "%s\n", prefix) ;

    fprintf (stream,
        "%s  Next                      = %p\n",
        prefix, this->Next) ;

    FREE_POINTER (free, new_prefix) ;
}

/******************************************************************************/

void print_detailed_dies_list (Die_t *list, FILE *stream, String_t prefix)
{
    FOR_EVERY_ELEMENT_IN_LIST_NEXT (Die_t, die, list)
    {
        if (die->Next == NULL)

            break ;

        print_detailed_die (die, stream, prefix) ;

        fprintf (stream, "%s\n", prefix) ;
    }

    print_detailed_die (die, stream, prefix) ;
}

/******************************************************************************/
