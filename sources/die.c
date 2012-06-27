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

Die_t *calloc_die (void)
{
    Die_t *die = (Die_t *) malloc (sizeof(Die_t)) ;

    if (die != NULL)

        init_die (die) ;

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

void print_die (Die_t *this, FILE *stream, String_t prefix)
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

void print_dies_list (Die_t *list, FILE *stream, String_t prefix)
{
    FOR_EVERY_ELEMENT_IN_LIST_NEXT (Die_t, die, list)
    {
        if (die->Next == NULL)

            break ;

        print_die (die, stream, prefix) ;

        fprintf (stream, "%s\n", prefix) ;
    }

    print_die (die, stream, prefix) ;
}

/******************************************************************************/
