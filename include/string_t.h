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

#ifndef _3DICE_STRING_T_H_
#define _3DICE_STRING_T_H_

/*! \file string_t.h */

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>

/******************************************************************************/

    /*! Definition of the primitive type String_t */

    typedef char* String_t ;



/******************************************************************************/



    /*! Inits the \a string structure with a default value
     *
     * \param string the address of the string to initalize
     */

    void string_init (String_t *string) ;



    /*! Tests if two strings are equal
     *
     * \param string the first string
     * \param other the second string
     *
     * \return \c TRUE if \a string and \a other are the same string
     * \return \c FALSE otherwise
     */

    bool string_equal (String_t *string, String_t *other) ;



    /*! Copies the string \a src into \a dst , as an assignement
     *
     * The function destroys the content of \a dst and then makes the copy
     *
     * \param dst the address of the left term string (destination)
     * \param src the address of the right term string (source)
     */

    void string_copy (String_t *dst, String_t *src) ;



    /*! Copies the C string \a src into \a dst , as an assignement
     *
     * The function destroys the content of \a dst and then makes the copy
     *
     * \param dst the address of the left term string (destination)
     * \param src the address of the right term C string (source)
     */

    void string_copy_cstr (String_t *dst, char *src) ;



    /*! Destroys the content of the \a string
     *
     * The function releases the memory used by the string and
     * resets its state calling \a string_init .
     *
     * \param string the address of the string to destroy
     */

    void string_destroy (String_t *string) ;

/******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* _3DICE_STRING_T_H_ */
