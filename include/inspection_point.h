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

    /*! \struct Tcell_t
     *
     *  \brief Temperature of an individual thermal cell
     */

    struct Tcell_t
    {
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
    } ;

    /*! Definition of the type Tcell_t */

    typedef struct Tcell_t Tcell_t ;



    /*! Sets all the fields to a default value (zero or \c NULL ).
     *
     * \param this the address of the tcell structure to initialize
     */

    void init_tcell (Tcell_t *this) ;



    /*! Allocates and inits memory for a structure of type Tcell_t
     *
     * \return a pointer to the allocated memory.
     * \return \c NULL in case of error
     */

    Tcell_t *calloc_tcell (void) ;



    /*! Frees the memory space pointed to by \a this
     *
     * The pointer \a this must have been returned by a previous call
     * to \a calloc_tcell . If \a this is \c NULL, no operation is performed.
     *
     * \param this the address to free
     */

    void free_tcell (Tcell_t *this) ;



    /*! Aligns the thermal cell to the grid of thermal cells
     *
     *  The function computes the values Tcell:ActualXval, Tcell:ActualYval,
     *  Tcell:RowIndex and Tcell:ColumnIndex while Tcell:XVal and Tcell:YVal
     *  are set as \a xval and \a yval
     *
     *  \param this  the pointer to the Tcell to align
     *  \param xval  the requested X coordinate of the cell
     *  \param yval  the requested Y coordinate of the cell
     *  \param dimensions pointer to the structure containing the dimensions of the IC
     */

    void align_tcell
    (
        Tcell_t         *this,
        ChipDimension_t  xval,
        ChipDimension_t  yval,
        Dimensions_t    *dimensions
    ) ;

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

    /*! \struct Tflp_t
     *
     *  \brief Temperature of all elements in a floorplan
     */

    struct Tflp_t
    {
        /*! The kind of quantity to be measured */

        OutputQuantity_t Quantity ;
    } ;

    /*! Definition of the type Tflp_t */

    typedef struct Tflp_t Tflp_t ;



    /*! Sets all the fields to a default value (zero or \c NULL ).
     *
     * \param this the address of the tflp structure to initialize
     */

    void init_tflp (Tflp_t *this) ;



    /*! Allocates and inits memory for a structure of type Tflp_t
     *
     * \return a pointer to the allocated memory.
     * \return \c NULL in case of error
     */

    Tflp_t *calloc_tflp (void) ;



    /*! Frees the memory space pointed to by \a this
     *
     * The pointer \a this must have been returned by a previous call
     * to \a calloc_tflp . If \a this is \c NULL, no operation is performed.
     *
     * \param this the address to free
     */

    void free_tflp (Tflp_t *this) ;

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

    /*! \struct Tflpel_t
     *
     * \brief Temperature of a single floorplan element
     */

    struct Tflpel_t
    {
        /*! Pointer to the Floorplan Element */

        FloorplanElement_t *FloorplanElement ;

        /*! The kind of quantity to be measured */

        OutputQuantity_t Quantity ;
    } ;

    /*! Definition of the type Tflpel_t */

    typedef struct Tflpel_t Tflpel_t ;



    /*! Sets all the fields to a default value (zero or \c NULL ).
     *
     * \param this the address of the tflpel structure to initialize
     */

    void init_tflpel (Tflpel_t *this) ;



    /*! Allocates and inits memory for a structure of type Tflpel_t
     *
     * \return a pointer to the allocated memory.
     * \return \c NULL in case of error
     */

    Tflpel_t *calloc_tflpel (void) ;



    /*! Frees the memory space pointed to by \a this
     *
     * The pointer \a this must have been returned by a previous call
     * to \a calloc_tflpel . If \a this is \c NULL, no operation is performed.
     *
     * \param this the address to free
     */

    void free_tflpel (Tflpel_t *this) ;

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

    /*! \struct Tcoolant_t
     *
     *  \brief Temperature of the coolant when leaving a cavity
     */

    struct Tcoolant_t
    {
        /*! The kind of quantity to be measured */

        OutputQuantity_t Quantity ;

    } ;

    /*! Definition of the type Tcoolant_t */

    typedef struct Tcoolant_t Tcoolant_t ;



    /*! Sets all the fields to a default value (zero or \c NULL ).
     *
     * \param this the address of the tcoolant structure to initialize
     */

    void init_tcoolant (Tcoolant_t *this) ;



    /*! Allocates and inits memory for a structure of type Tcoolant_t
     *
     * \return a pointer to the allocated memory.
     * \return \c NULL in case of error
     */

    Tcoolant_t *calloc_tcoolant (void) ;



    /*! Frees the memory space pointed to by \a this
     *
     * The pointer \a this must have been returned by a previous call
     * to \a calloc_tcoolant . If \a this is \c NULL, no operation is performed.
     *
     * \param this the address to free
     */

    void free_tcoolant (Tcoolant_t *this) ;

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

    /*! \union InspectionPoint_p
     *
     *  \brief A union of pointers to types that can be an instance of InspectionPoint
     */

    union InspectionPoint_p
    {
        Tcell_t    *Tcell ;     /*!< Pointer to a Tcell */
        Tflp_t     *Tflp ;      /*!< Pointer to a Tflp */
        Tflpel_t   *Tflpel ;    /*!< Pointer to a Tflpel */
        Tcoolant_t *Tcoolant ;  /*!< Pointer to a Tcoolant */
    } ;

    /*! Definition of the type InspectionPoint_p */

    typedef union InspectionPoint_p InspectionPoint_p ;

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

        /*! Pointer to a data structure representing the type of a InspectionPoint.
         *  This pointer must be casted depending on the value stored in
         *  InspectionPoint::Type */

        InspectionPoint_p Pointer ;

        /*! Pointer to the StackElement that will be used to print the outputs.
         *  The inspection point will be applied to this stack element */

        StackElement_t *StackElement ;

        /*! To collect inspection points in a linked list */

        struct InspectionPoint_t *Next ;

    } ;

    /*! definition of the type InspectionPoint_t */

    typedef struct InspectionPoint_t InspectionPoint_t ;



    /*! Sets all the fields to a default value (zero or \c NULL ).
     *
     * \param this the address of the inspection point structure to initialize
     */

    void init_inspection_point (InspectionPoint_t *this) ;



    /*! Allocates and inits memory for a structure of type InspectionPoint_t
     *
     * \return a pointer to the allocated memory.
     * \return \c NULL in case of error
     */

    InspectionPoint_t *calloc_inspection_point (void) ;



    /*! Frees the memory space pointed to by \a this
     *
     * The pointer \a this must have been returned by a previous call
     * to \a calloc_inspection_point . If \a this is \c NULL, no
     * operation is performed.
     *
     * \param this the address to free
     */

    void free_inspection_point (InspectionPoint_t *this) ;



    /*! Frees a list of inspection points
     *
     * If frees, calling #free_inspection_point, the inspection point pointed by the
     * parameter \a list and all the inspection points it finds following the
     * linked list throught the field InspectionPoint::Next .
     *
     * \param list the pointer to the first elment in the list to be freed
     */

    void free_inspection_point_list (InspectionPoint_t *list) ;



    /*! Prints a list of inspection points as they look in the stack file
     *
     * \param list   the pointer to the first inspection point in the list
     * \param stream the output stream (must be already open)
     * \param prefix a string to be printed as prefix at the beginning of each line
     */

    void print_inspection_point_list

        (InspectionPoint_t *list, FILE *stream, String_t prefix) ;


    /*! Checks if the inspection point has a specific set up
     *
     * \param this     the address of the InspectionPoint structure
     * \param type     the type of the inspection point (tcell, tmap, ...)
     * \param quantity the quantity to be measured (max, min, avg)
     *
     * \return \c true if \a this has type \a type and quantity
     *         \a quantity, \c false otherwise
     */

    bool is_inspection_point
    (
        InspectionPoint_t *this,
        OutputType_t       type,
        OutputQuantity_t   quantity
    ) ;

    /*! Generates the file in which a particular inspection point will be printed
     *
     * \param this       the address of the InspectionPoint structure
     * \param dimensions pointer to the structure containing the dimensions of the IC
     * \param prefix string to be printed as suffix for every line in the header
     *
     *  \return FIXME
     */

    Error_t generate_inspection_point_header

        (InspectionPoint_t *this, Dimensions_t *dimensions, String_t prefix) ;



    /*! Generates the output implemented by the inspection point
     *
     * \param this the address of the InspectionPoint structure
     * \param dimensions pointer to the structure containing the dimensions of the IC
     * \param temperatures pointer to the first element of the temparature array
     * \param sources      pointer to the first element of the source array
     * \param current_time time instant of the measurement
     *
     * \return FIXME
     */

    Error_t generate_inspection_point_output
    (
        InspectionPoint_t *this,
        Dimensions_t      *dimensions,
        Temperature_t     *temperatures,
        Source_t          *sources,
        Time_t             current_time
    ) ;



    /*! Fills a message with the output implemented by the inspection point
     *
     * \param this the address of the InspectionPoint structure
     * \param output_quantity the quantity to report (max, min, avg)
     * \param dimensions pointer to the structure containing the dimensions of the IC
     * \param temperatures pointer to the first element of the temparature array
     * \param sources      pointer to the first element of the source array
     * \param message the message to fill

     */

    void fill_message_inspection_point
    (
        InspectionPoint_t *this,
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
