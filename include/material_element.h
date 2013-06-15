/******************************************************************************
 * This file is part of 3D-ICE, version 2.2.4 .                               *
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

#ifndef _3DICE_MATERIAL_ELEMENT_H_
#define _3DICE_MATERIAL_ELEMENT_H_

/*! \file material_element.h */

#ifdef __cplusplus
extern "C"
{
#endif

/******************************************************************************/

#include <stdio.h> // For the file type FILE

#include "types.h"
#include "string_t.h"

#include "dimensions.h"
#include "material.h"
#include "ic_element_list.h"

/******************************************************************************/

    /*! \struct MaterialElement_t
     *
     *
     * TODO
     */

    struct MaterialElement_t
    {
        /*! The material used as layout */

        Material_t Material ;

        /*! The number of layout elements */

        Quantity_t NMElements ;

        /*! The list of IC elements that ... */

        ICElementList_t MElements ;

    } ;

    /*! Definition of the type MaterialElement_t */

    typedef struct MaterialElement_t MaterialElement_t ;


/******************************************************************************/



    /*! Inits the fields of the \a melement structure with default values
     *
     * \param melement the address of the structure to initalize
     */

    void material_element_init (MaterialElement_t *melement) ;



    /*! Copies the structure \a src into \a dst , as an assignement
     *
     * The function destroys the content of \a dst and then makes the copy
     *
     * \param dst the address of the left term sructure (destination)
     * \param src the address of the right term structure (source)
     */

    void material_element_copy

        (MaterialElement_t *dst, MaterialElement_t *src) ;



    /*! Destroys the content of the fields of the structure \a mpel
     *
     * The function releases any dynamic memory used by the structure and
     * resets its state calling \a material_element_init .
     *
     * \param melement the address of the structure to destroy
     */

    void material_element_destroy (MaterialElement_t *melement) ;



    /*! Allocates memory for a structure of type MatrerialElement_t
     *
     * The content of the new structure is set to default values
     * calling \a material_element_init
     *
     * \return the pointer to the new structure
     * \return \c NULL if the memory allocation fails
     */

    MaterialElement_t *material_element_calloc (void) ;



    /*! Allocates memory for a new copy of the structure \a mpel
     *
     * \param melement the address of the structure to clone
     *
     * \return a pointer to a new structure
     * \return \c NULL if the memory allocation fails
     * \return \c NULL if the parameter \a mpel is \c NULL
     */

    MaterialElement_t *material_element_clone (MaterialElement_t *melement) ;



    /*! Frees the memory space pointed by \a melement
     *
     * The function destroys the structure \a melement and then frees
     * its memory. The pointer \a melement must have been returned by
     * a previous call to \a material_element_calloc or
     * \a material_element_clone .
     *
     * If \a melement is \c NULL, no operation is performed.
     *
     * \param melement the pointer to free
     */

    void material_element_free (MaterialElement_t *melement) ;



    /*! Tests if two material elements have the same Material
     *
     * \param melement the first material element
     * \param other the second material element
     *
     * \return \c TRUE if \a melement and \a other have the same Material
     * \return \c FALSE otherwise
     */

    bool material_element_same_material

        (MaterialElement_t *melement, MaterialElement_t *other) ;



    /*! Prints the material element structure as it looks in the
     *  material layout file
     *
     * \param melement    the address of the structure to print
     * \param stream the output stream (must be already open)
     * \param prefix a string to be printed as prefix at the
     *               beginning of each line
     */

    void material_element_print

        (MaterialElement_t *melement, FILE *stream, String_t prefix) ;



    /*! Returns the material of a thermal cell in a given location
     *
     * \param melement          the address of the material element
     * \param row_index    the index of the row of the thermal cell
     * \param column_index the index of the column of the thermal cell
     * \param dimensions   pointer to the structure storing the dimensions
     *                     of the stack
     *
     * \return \c NULL if the material of the thermal cell in position
     *         (\a row_index, \a column_index ) is not found
     *
     * \return The address of the material of the thermal cell in position
     *         (\a row_index, \a column_index )
     */

    Material_t *get_material_at_location (MaterialElement_t *melement,
                                          CellIndex_t        row_index,
                                          CellIndex_t        column_index,
                                          Dimensions_t      *dimensions) ;

/******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* _3DICE_MATERIAL_ELEMENT_H_ */
