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

#ifndef _3DICE_DIE_H_
#define _3DICE_DIE_H_

/*! \file die.h */

#ifdef __cplusplus
extern "C"
{
#endif

/******************************************************************************/

#include <stdio.h>

#include "types.h"

#include "floorplan.h"
#include "layer_list.h"

/******************************************************************************/

    /*! \struct Die_t
     *
     *  \brief Structure used to store data about the dies that compose the 2D/3D stack.
     *
     *  Dies are one of the elements that can be used to build a 3d stack
     */

    struct Die_t
    {
        /*! The id (string) of the die */

        String_t Id ;

        /*! The number of layer composing the die */

        CellIndex_t NLayers ;

        /*! The offset (\# layers) of the source layer within the die */

        CellIndex_t SourceLayerOffset ;

        /*! The list of layers makeing the die. The first layer in the list
         *  will be the bottom-most layer, the last layer in the list the
         *  top most one.
         */

        LayerList_t Layers ;

        /*! The floorplan used on the source layer */

        Floorplan_t Floorplan ;
    } ;

    /*! Definition of the type Die_t */

    typedef struct Die_t Die_t ;

/******************************************************************************/

    void die_init (Die_t *die) ;

    void die_copy (Die_t *dst, Die_t *src) ;

    void die_destroy (Die_t *die) ;


    Die_t *die_calloc ( void ) ;

    Die_t *die_clone (Die_t *die) ;

    void die_free (Die_t *die) ;


    bool die_same_id (Die_t *die, Die_t *other) ;

    void die_print (Die_t *die, FILE *stream, String_t prefix) ;

/******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* _3DICE_DIE_H_ */
