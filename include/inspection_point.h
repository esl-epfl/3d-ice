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

    /*! \struct Tcell
     *
     *  \brief Temperature of an individual thermal cell
     */

    struct Tcell
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

    /*! Definition of the type Tcell */

    typedef struct Tcell Tcell ;



    /*! Sets all the fields of \a tcell to a default value (zero or \c NULL ).
     *
     * \param tcell the address of the tcell to initialize
     */

    void init_tcell (Tcell *tcell) ;



    /*! Allocates a Tcell in memory and sets its fields to their default
     *  value with #init_tcell
     *
     * \return the pointer to a new Tcell
     * \return \c NULL if the memory allocation fails
     */

    Tcell *alloc_and_init_tcell (void) ;



    /*! Frees the memory related to \a tcell
     *
     * The parametrer \a tcell must be a pointer previously obtained with
     * #alloc_and_init_tcell
     *
     * \param tcell the address of the Tcell structure to free
     */

    void free_tcell (Tcell *tcell) ;



    /*! Prints a list of detailed information about all the fields of \a tcell
     *
     * \param stream the output stream (must be already open)
     * \param prefix a string to be printed as prefix at the beginning of each line
     * \param tcell  the address of the Tcell structure to print
     */

    void print_detailed_tcell

        (FILE *stream, String_t prefix, Tcell *tcell) ;



    /*! Aligns the thermal cell to the grid of thermal cells
     *
     *  The function computes the values Tcell:ActualXval, Tcell:ActualYval,
     *  Tcell:RowIndex and Tcell:ColumnIndex while Tcell:XVal and Tcell:YVal
     *  are set as \a xval and \a yval
     *
     *  \param tcell the pointer to the Tcell to align
     *  \param xval  the requested X coordinate of the cell
     *  \param yval  the requested Y coordinate of the cell
     *  \param dimensions pointer to the structure containing the dimensions of the IC
     */

    void align_tcell
    (
        Tcell           *tcell,
        ChipDimension_t  xval,
        ChipDimension_t  yval,
        Dimensions      *dimensions
    ) ;

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

    /*! \struct Tflp
     *
     *  \brief Temperature of all elements in a floorplan
     */

    struct Tflp
    {
        /*! The kind of quantity to be measured */

        OutputQuantity_t Quantity ;
    } ;

    /*! Definition of the type Tflp */

    typedef struct Tflp Tflp ;



    /*! Sets all the fields of \a tflp to a default value (zero or \c NULL ).
     *
     * \param tflp the address of the tflp to initialize
     */

    void init_tflp (Tflp *tflp) ;



    /*! Allocates a Tflp in memory and sets its fields to their default
     *  value with #init_tflp
     *
     * \return the pointer to a new Tflp
     * \return \c NULL if the memory allocation fails
     */

    Tflp *alloc_and_init_tflp (void) ;



    /*! Frees the memory related to \a tflp
     *
     * The parametrer \a tflp must be a pointer previously obtained with
     * #alloc_and_init_tflp
     *
     * \param tflp the address of the Tflp structure to free
     */

    void free_tflp (Tflp *tflp) ;



    /*! Prints a list of detailed information about all the fields of \a tflp
     *
     * \param stream the output stream (must be already open)
     * \param prefix a string to be printed as prefix at the beginning of each line
     * \param tflp   the address of the Tflp structure to print
     */

    void print_detailed_tflp

        (FILE *stream, String_t prefix, Tflp *tflp) ;

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

    /*! \struct Tflpel
     *
     * \brief Temperature of a single floorplan element
     */

    struct Tflpel
    {
        /*! Pointer to the Floorplan Element */

        FloorplanElement *FloorplanElement ;

        /*! The kind of quantity to be measured */

        OutputQuantity_t Quantity ;
    } ;

    /*! Definition of the type Tflpel */

    typedef struct Tflpel Tflpel ;



    /*! Sets all the fields of \a tflpel to a default value (zero or \c NULL ).
     *
     * \param tflpel the address of the tflpel to initialize
     */

    void init_tflpel (Tflpel *tflpel) ;



    /*! Allocates a Tflpel in memory and sets its fields to their default
     *  value with #init_tflpel
     *
     * \return the pointer to a new Tflpel
     * \return \c NULL if the memory allocation fails
     */

    Tflpel *alloc_and_init_tflpel (void) ;



    /*! Frees the memory related to \a tflpel
     *
     * The parametrer \a tflpel must be a pointer previously obtained with
     * #alloc_and_init_tflpel
     *
     * \param tflpel the address of the Tflpel structure to free
     */

    void free_tflpel (Tflpel *tflpel) ;



    /*! Prints a list of detailed information about all the fields of \a tflpel
     *
     * \param stream the output stream (must be already open)
     * \param prefix a string to be printed as prefix at the beginning of each line
     * \param tflpel the address of the Tflpel structure to print
     */

    void print_detailed_tflpel

        (FILE *stream, String_t prefix, Tflpel *tflpel) ;

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

    /*! \struct Tcoolant
     *
     *  \brief Temperature of the coolant when leaving a cavity
     */

    struct Tcoolant
    {
        /*! The kind of quantity to be measured */

        OutputQuantity_t Quantity ;

    } ;

    /*! Definition of the type Tcoolant */

    typedef struct Tcoolant Tcoolant ;



    /*! Sets all the fields of \a tcoolant to a default value (zero or \c NULL ).
     *
     * \param tcoolant the address of the tcoolant to initialize
     */

    void init_tcoolant (Tcoolant *tcoolant) ;



    /*! Allocates a Tcoolant in memory and sets its fields to their default
     *  value with #init_tcoolant
     *
     * \return the pointer to a new Tcoolant
     * \return \c NULL if the memory allocation fails
     */

    Tcoolant *alloc_and_init_tcoolant (void) ;



    /*! Frees the memory related to \a tcoolant
     *
     * The parametrer \a tcoolant must be a pointer previously obtained with
     * #alloc_and_init_tcoolant
     *
     * \param tcoolant the address of the Tcoolant structure to free
     */

    void free_tcoolant (Tcoolant *tcoolant) ;



    /*! Prints a list of detailed information about all the fields of \a tcoolant
     *
     * \param stream the output stream (must be already open)
     * \param prefix a string to be printed as prefix at the beginning of each line
     * \param tcoolant the address of the Tcoolant structure to print
     */

    void print_detailed_tcoolant

        (FILE *stream, String_t prefix, Tcoolant *tcoolant) ;

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

    /*! \union InspectionPoint_p
     *
     *  \brief A union of pointers to types that can be an instance of InspectionPoint
     */

    union InspectionPoint_p
    {
        Tcell    *Tcell ;     /*!< Pointer to a Tcell */
        Tflp     *Tflp ;      /*!< Pointer to a Tflp */
        Tflpel   *Tflpel ;    /*!< Pointer to a Tflpel */
        Tcoolant *Tcoolant ;  /*!< Pointer to a Tcoolant */
    } ;

    /*! Definition of the type InspectionPoint_p */

    typedef union InspectionPoint_p InspectionPoint_p ;

/******************************************************************************/

    /*! \struct InspectionPoint
     *
     *  \brief Structure containing info about the output to be ptrinted while simulating
     */

    struct InspectionPoint
    {
        /*! The path of the file used to store the output */

        String_t FileName ;

        /*! The type of output instance (final, slot, step) */

        OutputInstant_t Instant ;

        /*! Type of Output requested (cell, flp, flpel, map) */

        OutputType_t Type ;

        /*! Pointer to a data structure representing the type of a InspectionPoint.
         *  This pointer must be casted depending on the value stored in
         *  InspectionPoint::Type */

        InspectionPoint_p Pointer ;

        /*! Pointer to the StackElement that will be used to print the outputs.
         *  The inspection point will be applied to this stack element */

        StackElement *StackElement ;

        /*! To collect inspection points in a linked list */

        struct InspectionPoint *Next ;

    } ;

    /*! definition of the type InspectionPoint */

    typedef struct InspectionPoint InspectionPoint ;



    /*! Sets all the fields of \a inspection_point to a default value (zero or \c NULL ).
     *
     * \param inspection_point the address of the inspection point to initialize
     */

    void init_inspection_point (InspectionPoint *inspection_point) ;



    /*! Allocates a InspectionPoint in memory and sets its fields to their default
     *  value with #init_inspection_point
     *
     * \return the pointer to a new InspectionPoint
     * \return \c NULL if the memory allocation fails
     */

    InspectionPoint *alloc_and_init_inspection_point (void) ;



    /*! Frees the memory related to \a inspection_point
     *
     * The parametrer \a inspection_point must be a pointer previously obtained with
     * #alloc_and_init_inspection_point
     *
     * \param inspection_point the address of the InspectionPoint structure to free
     */

    void free_inspection_point (InspectionPoint *inspection_point) ;



    /*! Frees a list of inspection points
     *
     * If frees, calling #free_inspection_point, the inspection point pointed by the
     * parameter \a list and all the inspection points it finds following the
     * linked list throught the field InspectionPoint::Next .
     *
     * \param list the pointer to the first elment in the list to be freed
     */

    void free_inspection_point_list (InspectionPoint *list) ;



    /*! Prints a list of inspection points as they look in the stack file
     *
     * \param stream the output stream (must be already open)
     * \param prefix a string to be printed as prefix at the beginning of each line
     * \param list   the pointer to the first inspection point in the list
     */

    void print_formatted_inspection_point_list

        (FILE *stream, String_t prefix, InspectionPoint *list) ;



    /*! Prints a list of detailed information about all the fields of the inspection points
     *
     * \param stream the output stream (must be already open)
     * \param prefix a string to be printed as prefix at the beginning of each line
     * \param list   the pointer to the first inspection point in the list
     */

    void print_detailed_inspection_point_list

        (FILE *stream, String_t prefix, InspectionPoint *list) ;


    /*! Generates the file in which a particular inspection point will be printed
     *
     * \param inspection_point the address of the InspectionPoint structure
     * \param dimensions pointer to the structure containing the dimensions of the IC
     * \param prefix string to be printed as suffix for every line in the header
     *
     *  \return FIXME
     */

    Error_t generate_inspection_point_header

        (InspectionPoint *inspection_point, Dimensions *dimensions, String_t prefix) ;



    /*! Generates the output implemented by the inspection point
     *
     * \param inspection_point the address of the InspectionPoint structure
     * \param dimensions pointer to the structure containing the dimensions of the IC
     * \param temperatures pointer to the first element of the temparature array
     * \param current_time time instant of the measurement
     *
     * \return FIXME
     */

    Error_t generate_inspection_point_output
    (
        InspectionPoint *inspection_point,
        Dimensions      *dimensions,
        Temperature_t   *temperatures,
        Time_t           current_time
    ) ;



    /*! Fills a message with the output implemented by the inspection point
     *
     * \param inspection_point the address of the InspectionPoint structure
     * \param dimensions pointer to the structure containing the dimensions of the IC
     * \param temperatures pointer to the first element of the temparature array
     * \param message the message to fill

     */

    void fill_message_inspection_point
    (
        InspectionPoint *inspection_point,
        Dimensions      *dimensions,
        Temperature_t   *temperatures,
        NetworkMessage  *message
    ) ;

/******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* _3DICE_INSPECTION_POINT_H_ */
