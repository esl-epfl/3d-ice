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
     *  \brief Structure containing info about the output to be printed while simulating
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

    void inspection_point_init (InspectionPoint_t *ipoint) ;

    void inspection_point_copy (InspectionPoint_t *dst, InspectionPoint_t *src) ;

    void inspection_point_destroy (InspectionPoint_t *ipoint) ;


    InspectionPoint_t *inspection_point_calloc (void) ;

    InspectionPoint_t *inspection_point_clone (InspectionPoint_t *ipoint) ;

    void inspection_point_free (InspectionPoint_t *ipoint) ;

    bool inspection_point_same_filename (InspectionPoint_t *ipoint, InspectionPoint_t *other) ;

    void inspection_point_print (InspectionPoint_t *ipoint, FILE *stream, String_t prefix) ;


    /*! Aligns the Tcell inspection point to the grid of thermal cells
     *
     *  The function computes the values ActualXval, ActualYval,
     *  RowIndex and ColumnIndex while XVal and YVal are set to \a xval and \a yval
     *
     *  \param ipoint  the pointer to the Tcell inspection point to align
     *  \param xval  the requested X coordinate of the cell
     *  \param yval  the requested Y coordinate of the cell
     *  \param dimensions pointer to the structure containing the dimensions of the IC
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

    /*! Generates the file in which a particular inspection point will be printed
     *
     * \param ipoint       the address of the InspectionPoint structure
     * \param dimensions pointer to the structure containing the dimensions of the IC
     * \param prefix string to be printed as suffix for every line in the header
     *
     *  \return FIXME
     */

    Error_t generate_inspection_point_header

        (InspectionPoint_t *ipoint, Dimensions_t *dimensions, String_t prefix) ;



    /*! Generates the output implemented by the inspection point
     *
     * \param ipoint the address of the InspectionPoint structure
     * \param dimensions pointer to the structure containing the dimensions of the IC
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
     * \param dimensions pointer to the structure containing the dimensions of the IC
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
