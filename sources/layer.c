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

#include "layer.h"
#include "macros.h"

/******************************************************************************/

void init_layer (Layer_t *this)
{
    this->Height   = 0.0 ;
    this->Material = NULL ;
    this->Id       = NULL ;
    this->Used     = 0u ;
    this->Next     = NULL ;
    this->Prev     = NULL ;
}

/******************************************************************************/

Layer_t *alloc_and_init_layer (void)
{
    Layer_t *layer = (Layer_t *) malloc (sizeof(Layer_t));

    if (layer != NULL)

        init_layer (layer) ;

    return layer ;
}

/******************************************************************************/

void free_layer (Layer_t *this)
{
    if (this->Id != NULL)

        FREE_POINTER (free, this->Id) ;

    FREE_POINTER (free, this) ;
}

/******************************************************************************/

void free_layers_list (Layer_t *list)
{
    FREE_LIST (Layer_t, list, free_layer) ;
}

/******************************************************************************/

Layer_t *find_layer_in_list (Layer_t *list, String_t id)
{
    FOR_EVERY_ELEMENT_IN_LIST_NEXT (Layer_t, layer, list)
    {
        if (strcmp(layer->Id, id) == 0)
            break ;
    }
    return layer ;
}

/******************************************************************************/

void print_formatted_layer (Layer_t *this, FILE *stream, String_t prefix)
{
    fprintf (stream,
        "%slayer %s :\n",
        prefix, this->Id) ;

    fprintf (stream,
        "%s   height %.1f ;\n",
        prefix, this->Height) ;

    fprintf (stream,
        "%s   material %s ;\n",
        prefix, this->Material->Id) ;
}

/******************************************************************************/

void print_detailed_layer (Layer_t *this, FILE *stream, String_t prefix)
{
    fprintf (stream,
        "%slayer                   = %p\n",
        prefix, this) ;

    fprintf (stream,
        "%s  Height                = %.1f\n",
        prefix, this->Height) ;

    fprintf (stream,
        "%s  Material              = %p\n",
        prefix, this->Material) ;

    fprintf (stream,
        "%s  Id                    = %s\n",
        prefix, this->Id) ;

    fprintf (stream,
        "%s  Used                  = %d\n",
        prefix, this->Used) ;

    fprintf (stream,
        "%s  Next                  = %p\n",
        prefix, this->Next) ;

    fprintf (stream,
        "%s  Prev                  = %p\n",
        prefix, this->Prev) ;
}

/******************************************************************************/

void print_formatted_layers_list (Layer_t *list, FILE *stream, String_t prefix)
{
    FOR_EVERY_ELEMENT_IN_LIST_NEXT (Layer_t, layer, list)

        print_formatted_layer (layer, stream, prefix) ;
}

/******************************************************************************/

void print_detailed_layers_list (Layer_t *list, FILE *stream, String_t prefix)
{
    FOR_EVERY_ELEMENT_IN_LIST_NEXT (Layer_t, layer, list)
    {
        if (layer->Next == NULL)

            break ;

        print_detailed_layer (layer, stream, prefix) ;

        fprintf (stream, "%s\n", prefix) ;
    }

    print_detailed_layer (layer, stream, prefix) ;
}

/******************************************************************************/
