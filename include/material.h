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

#ifndef _3DICE_MATERIAL_H_
#define _3DICE_MATERIAL_H_

/*! \file material.h */

#ifdef __cplusplus
extern "C"
{
#endif

/******************************************************************************/

#include <stdio.h>

#include "types.h"

/******************************************************************************/

    /*! \struct Material_t
     *
     *  \brief Structure used to store data about the materials that compose the 2D/3D stack.
     *
     *  Material_ts are used when declaring layers composing a die or the
     *  stack or when declaring the properties of the walls in a channel.
     */

    struct Material_t
    {
        /*!
         * The identifier used to refer to the material in the stack file
         */

        String_t Id ;

        /*!
         * The volume-specific heat capacity of the material,
         * expressed as \f$ \frac{J}{\mu m^3 \cdot K } \f$
         */

        SolidVHC_t VolumetricHeatCapacity ;

        /*!
         * The thermal conductivity of the material,
         * expressed as \f$ \frac{W}{\mu m \cdot K } \f$
         */

        SolidTC_t ThermalConductivity ;
    } ;

    /*! Definition of the type Material_t
     */

    typedef struct Material_t Material_t ;

/******************************************************************************/


    void material_init (Material_t *material) ;

    void material_copy (Material_t *dst, Material_t *src) ;

    void material_destroy (Material_t *material) ;

    Material_t *material_calloc ( void ) ;

    Material_t *material_clone (Material_t *material) ;

    void material_free (Material_t *material) ;

    bool material_same_id (Material_t *material, Material_t *other) ;

    void material_print (Material_t *material, FILE *stream, String_t prefix) ;

/******************************************************************************/

#ifdef __cplusplus
}
#endif
#endif    /* _3DICE_MATERIAL_H_ */
