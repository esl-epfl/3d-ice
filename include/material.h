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
         * To know, after the parsing of a stack file, if a
         * material has been declared but never used
         */

        Quantity_t Used ;

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

        /*!
         * Pointer to collect materials in a linked list
         */

        struct Material_t *Next ;
    } ;

    /*! Definition of the type Material_t
     */

    typedef struct Material_t Material_t ;

/******************************************************************************/



    /*! Sets all the fields of \a material to a default value (zero or \c NULL ).
     *
     * \param material the address of the material to initialize
     */

    void init_material (Material_t *material) ;



    /*! Allocates a Material_t in memory and sets its fields to their default
     *  value with \c init_material
     *
     * \return the pointer to a new Material_t
     * \return \c NULL if the memory allocation fails
     */

    Material_t *alloc_and_init_material (void) ;



    /*! Frees the memory related to \a material
     *
     * The parametrer \a material must be a pointer previously obtained with
     * \c alloc_and_init_material
     *
     * \param material the address of the material structure to free
     */

    void free_material (Material_t *material) ;



    /*! Frees a list of materials
     *
     * If frees, calling \c free_material, the material pointed by the
     * parameter \a list and all the materials it finds following the
     * linked list throught the field Material_t::Next.
     *
     * \param list the pointer to the first elment in the list to be freed
     */

    void free_materials_list (Material_t *list) ;



    /*! Searches for a Material_t in a linked list of materials.
     *
     * Id based search of a Material_t structure in a list.
     *
     * \param list the pointer to the list
     * \param id   the identifier of the material to be found
     *
     * \return the address of a Material_t, if founded
     * \return \c NULL if the search fails
     */

    Material_t *find_material_in_list (Material_t *list, String_t id) ;



    /*! Prints the material as it looks in the stack file
     *
     * \param stream   the output stream (must be already open)
     * \param prefix   a string to be printed as prefix at the beginning of each line
     * \param material the material to print
     */

    void print_formatted_material

         (FILE *stream, String_t prefix, Material_t *material) ;



    /*! Prints a list of materials as they look in the stack file
     *
     * \param stream the output stream (must be already open)
     * \param prefix a string to be printed as prefix at the beginning of each line
     * \param list   the pointer to the first material in the list
     */

    void print_formatted_materials_list

         (FILE *stream, String_t prefix, Material_t *list) ;



    /*! Prints detailed information about all the fields of a material
     *
     * \param stream   the output stream (must be already open)
     * \param prefix   a string to be printed as prefix at the beginning of each line
     * \param material the material to print
     */

    void print_detailed_material

         (FILE *stream, String_t prefix, Material_t *material) ;



    /*! Prints a list of detailed information about all the fields of the materials
     *
     * \param stream the output stream (must be already open)
     * \param prefix a string to be printed as prefix at the beginning of each line
     * \param list the pointer to the first material in the list
     */

    void print_detailed_materials_list

         (FILE *stream, String_t prefix, Material_t *list) ;

/******************************************************************************/

#ifdef __cplusplus
}
#endif
#endif    /* _3DICE_MATERIAL_H_ */
