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

#ifndef _3DICE_INSPECTION_POINT_H_
#define _3DICE_INSPECTION_POINT_H_

/*! \file inspection_point.h */

#ifdef __cplusplus
extern "C"
{
#endif

/******************************************************************************/

#include <stdio.h>

#include "types.h"

#include "dimensions.h"
#include "floorplan_element.h"
#include "stack_element.h"
#include "network_message.h"

/******************************************************************************/

    /*! \struct InspectionPoint_t
     *
     *  \brief Structure containing info about the output to be printed
     *         while simulating
     */

    struct InspectionPoint_t
    {
        /*! The path of the file used to store the output */

        String_t FileName ;

        /*! The type of output instance (final, slot, step) */

        OutputInstant_t Instant ;

        /*! Type of Output requested (cell, flp, flpel, tmap, pmap) */

        OutputType_t Type ;

        /*! The kind of quantity to be measured */

        OutputQuantity_t Quantity ;

        /*! X coordinate of the thermal cell as specified in the stack file */

        ChipDimension_t Xval ;

        /*! Actual nearest X coordinate of the thermal cell */

        ChipDimension_t ActualXval ;

        /*! Y coordnate of the thermal cell as specified in the stack file */

        ChipDimension_t Yval ;

        /*! Actual nearest Y coordnate of the thermal cell */

        ChipDimension_t ActualYval ;

        /*! Row Index of the thermal cell */

        CellIndex_t RowIndex ;

        /*! Column Index of the thermal cell */

        CellIndex_t ColumnIndex ;

        /*! Pointer to the StackElement that will be used to print the outputs.
         *  The inspection point will be applied to ipoint stack element */

        StackElement_t *StackElement ;

        /*! Pointer to the Floorplan Element */

        FloorplanElement_t *FloorplanElement ;
    } ;

    /*! definition of the type InspectionPoint_t */

    typedef struct InspectionPoint_t InspectionPoint_t ;



/******************************************************************************/



    /*! Inits the fields of the \a ipoint structure with default values
     *
     * \param ipoint the address of the structure to initalize
     */

    void inspection_point_init (InspectionPoint_t *ipoint) ;



    /*! Copies the structure \a src into \a dst , as an assignement
     *
     * The function destroys the content of \a dst and then makes the copy
     *
     * \param dst the address of the left term sructure (destination)
     * \param src the address of the right term structure (source)
     */

    void inspection_point_copy

        (InspectionPoint_t *dst, InspectionPoint_t *src) ;



    /*! Destroys the content of the fields of the structure \a ipoint
     *
     * The function releases any dynamic memory used by the structure and
     * resets its state calling \a inspection_point_init .
     *
     * \param ipoint the address of the structure to destroy
     */

    void inspection_point_destroy (InspectionPoint_t *ipoint) ;



    /*! Allocates memory for a structure of type InspectionPoint_t
     *
     * The content of the new structure is set to default values
     * calling \a inspection_point_init
     *
     * \return the pointer to the new structure
     * \return \c NULL if the memory allocation fails
     */

    InspectionPoint_t *inspection_point_calloc (void) ;



    /*! Allocates memory for a new copy of the structure \a ipoint
     *
     * \param ipoint the address of the structure to clone
     *
     * \return a pointer to a new structure
     * \return \c NULL if the memory allocation fails
     * \return \c NULL if the parameter \a ipoint is \c NULL
     */

    InspectionPoint_t *inspection_point_clone (InspectionPoint_t *ipoint) ;



    /*! Frees the memory space pointed by \a ipoint
     *
     * The function destroys the structure \a ipoint and then frees
     * its memory. The pointer \a ipoint must have been returned by
     * a previous call to \a inspection_point_calloc or
     * \a inspection_point_clone .
     *
     * If \a ipoint is \c NULL, no operation is performed.
     *
     * \param ipoint the pointer to free
     */

    void inspection_point_free (InspectionPoint_t *ipoint) ;



    /*! Tests if two insection points generates he output into the same file
     *
     * \param ipoint the first inspection point
     * \param other the second inspection point
     *
     * \return \c TRUE if \a ipoint and \a other have the same FileName
     * \return \c FALSE otherwise
     */

    bool inspection_point_same_filename

        (InspectionPoint_t *ipoint, InspectionPoint_t *other) ;



    /*! Prints the insection point declaration as it looks in the stack file
     *
     * \param ipoint the address of the structure to print
     * \param stream the output stream (must be already open)
     * \param prefix a string to be printed as prefix at the
     *               beginning of each line
     */

    void inspection_point_print

        (InspectionPoint_t *ipoint, FILE *stream, String_t prefix) ;



    /*! Aligns the Tcell inspection point to the grid of thermal cells
     *
     *  The function computes the values ActualXval, ActualYval,
     *  RowIndex and ColumnIndex while XVal and YVal are set to
     *  \a xval and \a yval
     *
     *  \param ipoint  the pointer to the Tcell inspection point to align
     *  \param xval  the requested X coordinate of the cell
     *  \param yval  the requested Y coordinate of the cell
     * \param dimensions the address of the dimension structure
     */

    void align_tcell
    (
        InspectionPoint_t  *ipoint,
        ChipDimension_t     xval,
        ChipDimension_t     yval,
        Dimensions_t       *dimensions
    ) ;



    /*! Checks if the inspection point has a specific set up
     *
     * \param ipoint     the address of the InspectionPoint structure
     * \param type     the type of the inspection point (tcell, tmap, ...)
     * \param quantity the quantity to be measured (max, min, avg)
     *
     * \return \c true if \a ipoint has type \a type and quantity
     *         \a quantity, \c false otherwise
     */

    bool is_inspection_point
    (
        InspectionPoint_t *ipoint,
        OutputType_t       type,
        OutputQuantity_t   quantity
    ) ;



    /*! Generates the file in which a particular inspection point
     *  will be printed
     *
     * \param ipoint     the address of the InspectionPoint structure
     * \param dimensions the address of the dimension structure
     * \param prefix string to be printed as suffix for every line in the header
     *
     *  \return FIXME
     */

    Error_t generate_inspection_point_header

        (InspectionPoint_t *ipoint, Dimensions_t *dimensions, String_t prefix) ;



    /*! Generates the output implemented by the inspection point
     *
     * \param ipoint the address of the InspectionPoint structure
     * \param dimensions the address of the dimension structure
     * \param temperatures pointer to the first element of the temparature array
     * \param sources      pointer to the first element of the source array
     * \param current_time time instant of the measurement
     *
     * \return FIXME
     */

    Error_t generate_inspection_point_output
    (
        InspectionPoint_t *ipoint,
        Dimensions_t      *dimensions,
        Temperature_t     *temperatures,
        Source_t          *sources,
        Time_t             current_time
    ) ;



    /*! Fills a message with the output implemented by the inspection point
     *
     * \param ipoint the address of the InspectionPoint structure
     * \param output_quantity the quantity to report (max, min, avg)
     * \param dimensions the address of the dimension structure
     * \param temperatures pointer to the first element of the temparature array
     * \param sources      pointer to the first element of the source array
     * \param message the message to fill
     */

    void fill_message_inspection_point
    (
        InspectionPoint_t *ipoint,
        OutputQuantity_t   output_quantity,
        Dimensions_t      *dimensions,
        Temperature_t     *temperatures,
        Source_t          *sources,
        NetworkMessage_t  *message
    ) ;

/******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* _3DICE_INSPECTION_POINT_H_ */
