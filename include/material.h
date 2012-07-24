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
     *  \brief Structure used to store data about the materials that compose
     *         the 2D/3D stack.
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



    /*! Inits the fields of the \a material structure with default values
     *
     * \param material the address of the structure to initalize
     */

    void material_init (Material_t *material) ;



    /*! Copies the structure \a src into \a dst , as an assignement
     *
     * The function destroys the content of \a dst and then makes the copy
     *
     * \param dst the address of the left term sructure (destination)
     * \param src the address of the right term structure (source)
     */

    void material_copy (Material_t *dst, Material_t *src) ;



    /*! Destroys the content of the fields of the structure \a material
     *
     * The function releases any dynamic memory used by the structure and
     * resets its state calling \a material_init .
     *
     * \param material the address of the structure to destroy
     */

    void material_destroy (Material_t *material) ;



    /*! Allocates memory for a structure of type Material_t
     *
     * The content of the new structure is set to default values
     * calling \a material_init
     *
     * \return the pointer to the new structure
     * \return \c NULL if the memory allocation fails
     */

    Material_t *material_calloc ( void ) ;



    /*! Allocates memory for a new copy of the structure \a material
     *
     * \param material the address of the structure to clone
     *
     * \return a pointer to a new structure
     * \return \c NULL if the memory allocation fails
     * \return \c NULL if the parameter \a material is \c NULL
     */

    Material_t *material_clone (Material_t *material) ;



    /*! Frees the memory space pointed by \a material
     *
     * The function destroys the structure \a material and then frees
     * its memory. The pointer \a material must have been returned by
     * a previous call to \a material_calloc or \a material_clone .
     *
     * If \a material is \c NULL, no operation is performed.
     *
     * \param material the pointer to free
     */

    void material_free (Material_t *material) ;



    /*! Tests if two materials have the same Id
     *
     * \param material the first material
     * \param other the second material
     *
     * \return \c TRUE if \a material and \a other have the same Id
     * \return \c FALSE otherwise
     */

    bool material_same_id (Material_t *material, Material_t *other) ;



    /*! Prints the material declaration as it looks in the stack file
     *
     * \param material the address of the structure to print
     * \param stream the output stream (must be already open)
     * \param prefix a string to be printed as prefix at the
     *               beginning of each line
     */

    void material_print (Material_t *material, FILE *stream, String_t prefix) ;

/******************************************************************************/

#ifdef __cplusplus
}
#endif
#endif    /* _3DICE_MATERIAL_H_ */
