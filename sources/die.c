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

void init_die (Die_t *die)
{
    die->Id                = NULL ;
    die->Used              = 0u ;
    die->NLayers           = 0u ;
    die->SourceLayerOffset = 0u ;
    die->TopLayer          = NULL ;
    die->SourceLayer       = NULL ;
    die->BottomLayer       = NULL ;
    die->Next              = NULL ;
}

/******************************************************************************/

Die_t *alloc_and_init_die (void)
{
    Die_t *die = (Die_t *) malloc (sizeof(Die_t)) ;

    if (die != NULL)

        init_die (die) ;

    return die ;
}

/******************************************************************************/

void free_die (Die_t *die)
{
    if (die->Id != NULL)

        FREE_POINTER (free, die->Id) ;

    die->TopLayer = NULL ;
    FREE_POINTER (free_layers_list, die->BottomLayer) ;

    FREE_POINTER (free, die) ;
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

void print_formatted_die (FILE  *stream, String_t prefix, Die_t *die)
{
    fprintf (stream, "%sdie %s :\n", prefix, die->Id) ;

    FOR_EVERY_ELEMENT_IN_LIST_PREV (Layer_t, layer, die->TopLayer)
    {
        if (layer == die->SourceLayer)

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

void print_formatted_dies_list (FILE  *stream, String_t prefix, Die_t *list)
{
    FOR_EVERY_ELEMENT_IN_LIST_NEXT (Die_t, die, list)
    {
        if (die->Next == NULL)

            break ;

        print_formatted_die (stream, prefix, die) ;

        fprintf (stream, "%s\n", prefix) ;
    }

    print_formatted_die (stream, prefix, die) ;
}

/******************************************************************************/

void print_detailed_die (FILE  *stream, String_t prefix, Die_t *die)
{
    String_t new_prefix = (String_t)

        malloc (sizeof (*new_prefix) * (5 + strlen(prefix))) ;

    if (new_prefix == NULL) return ;

    sprintf (new_prefix, "%s    ", prefix) ;

    fprintf (stream,
        "%sdie                         = %p\n",
        prefix, die) ;

    fprintf (stream,
        "%s  Id                        = %s\n",
        prefix, die->Id) ;

    fprintf (stream,
        "%s  Used                      = %d\n",
        prefix, die->Used) ;

    fprintf (stream,
        "%s  NLayers                   = %d\n",
        prefix, die->NLayers) ;

    fprintf (stream,
        "%s  SourceLayerOffset         = %d\n",
        prefix, die->SourceLayerOffset) ;

    fprintf (stream,
        "%s  TopLayer                  = %p\n",
        prefix, die->TopLayer) ;

    fprintf (stream,
        "%s  SourceLayer               = %p\n",
        prefix, die->SourceLayer) ;

    fprintf (stream,
        "%s  BottomLayer               = %p\n",
        prefix, die->BottomLayer) ;

    fprintf (stream, "%s\n", prefix) ;

    print_detailed_layers_list (stream, new_prefix, die->BottomLayer) ;

    fprintf (stream, "%s\n", prefix) ;

    fprintf (stream,
        "%s  Next                      = %p\n",
        prefix, die->Next) ;

    FREE_POINTER (free, new_prefix) ;
}

/******************************************************************************/

void print_detailed_dies_list (FILE  *stream, String_t prefix, Die_t *list)
{
    FOR_EVERY_ELEMENT_IN_LIST_NEXT (Die_t, die, list)
    {
        if (die->Next == NULL)

            break ;

        print_detailed_die (stream, prefix, die) ;

        fprintf (stream, "%s\n", prefix) ;
    }

    print_detailed_die (stream, prefix, die) ;
}

/******************************************************************************/
