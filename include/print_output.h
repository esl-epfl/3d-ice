/******************************************************************************
 * This file is part of 3D-ICE, version 1.0.1 .                               *
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
 *          Martino Ruggiero                                                  *
 *          Thomas Brunschwiler                                               *
 *          David Atienza                                                     *
 *                                                                            *
 * For any comment, suggestion or request  about 3D-ICE, please  register and *
 * write to the mailing list (see http://listes.epfl.ch/doc.cgi?liste=3d-ice) *
 *                                                                            *
 * EPFL-STI-IEL-ESL                                                           *
 * Batiment ELG, ELG 130                Mail : 3d-ice@listes.epfl.ch          *
 * Station 11                                  (SUBSCRIPTION IS NECESSARY)    *
 * 1015 Lausanne, Switzerland           Url  : http://esl.epfl.ch/3d-ice.html *
 ******************************************************************************/

#ifndef _3DICE_PRINT_OUTPUT_H_
#define _3DICE_PRINT_OUTPUT_H_

/*! \file print_output.h */

#ifdef __cplusplus
extern "C"
{
#endif

/******************************************************************************/

#include <stdio.h>

#include "types.h"

#include "dimensions.h"
#include "stack_element.h"

/******************************************************************************/

    /*! \struct Tcell
     *
     *  \brief Temperature of an individual thermal cell
     */

    struct Tcell 
    {
        /*! The Stack Element Id (could be a layer, a die or a channel) */

        char *Id ;

        /*! X coordinate of the thermal cell as specified in the stack file */

        double Xval ;

        /*! Actual nearest X coordinate of the thermal cell */

        double ActualXval ;

        /*! Y coordnate of the thermal cell as specified in the stack file */

        double Yval ;

        /*! Actual nearest Y coordnate of the thermal cell */

        double ActualYval ;

        /*! Layer Index of the thermal cell */

        uint32_t LayerIndex ;

        /*! Row Index of the thermal cell */

        uint32_t RowIndex ;

        /*! Column Index of the thermal cell */

        uint32_t ColumnIndex ;
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

        (Tcell *tcell, double xval, double yval, Dimensions *dimensions) ;

/******************************************************************************/

    /*! \struct Tflp
     *
     *  \brief Temperature of all elements in a floorplan
     */

    struct Tflp 
    {
        /*! The Stack Element Id (it must be a die) */
        char *Id ;

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
     * \return the pointer to a new TFlp
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

/******************************************************************************/

    /*! \struct Tflpel
     *
     * \brief Temperature of a single floorplan element
     */

    struct Tflpel 
    {
        /*! The Stack Element Id (must be a die) */

        char *Id ;

        /*! The Floorplan Element Id (must exist in the flp file) */

        char *FlpId ;

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
     * \return the pointer to a new TFlp
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

/******************************************************************************/

    /*! \struct Tmap
     *
     * \brief Thermal map of a Stack Element
     */

    struct Tmap 
    {
        /*! The Stack Element Id (could be a layer, a die or a channel) */

        char *Id ;
    } ;

    /*! Definition of the type Tmap */

    typedef struct Tmap Tmap ;



    /*! Sets all the fields of \a tmap to a default value (zero or \c NULL ).
     *
     * \param tmap the address of the tmap to initialize
     */

    void init_tmap (Tmap *tmap) ;



    /*! Allocates a Tmap in memory and sets its fields to their default
     *  value with #init_tmap
     *
     * \return the pointer to a new TFlp
     * \return \c NULL if the memory allocation fails
     */

    Tmap *alloc_and_init_tmap (void) ;



    /*! Frees the memory related to \a tmap
     *
     * The parametrer \a tmap must be a pointer previously obtained with
     * #alloc_and_init_tmap
     *
     * \param tmap the address of the Tmap structure to free
     */

    void free_tmap (Tmap *tmap) ;

/******************************************************************************/

    /*! \union PrintOutput_p
     *
     *  \brief A union of pointers to types that can be an instance of PrintOutput
     */

    union PrintOutput_p
    {
        Tcell  *Tcell ;   /*!< Pointer to a Tcell */
        Tflp   *Tflp ;    /*!< Pointer to a Tflp */
        Tflpel *Tflpel ;  /*!< Pointer to a Tflpel */
        Tmap   *Tmap ;    /*!< Pointer to a Tmap */
    } ;

    /*! Definition of the type PrintOutput_p */

    typedef union PrintOutput_p PrintOutput_p ;

/******************************************************************************/

    /*! \struct PrintOutput
     *
     *  \brief Structure containing info about the output to be ptrinted while simulating
     */

    struct PrintOutput
    {
        /*! Type of Output requested */

        OutputType_t Type ;

        /*! The type of output instancing */

        OutputInstanceType_t InstanceType ;

        /*! The path of the file used to store the output */

        char *FileName ;

        /*! Pointer to a data structure representing the type of a PrintOutput.
         *  This pointer must be casted depending on the value stored in
         *  PrintOutput::Type */

        PrintOutput_p Pointer ;

        /*! To collect output instructions in a linked list */

        struct PrintOutput *Next ;

    } ;

    /*! definition of the type PrintOutput */

    typedef struct PrintOutput PrintOutput ;



    /*! Sets all the fields of \a print_output to a default value (zero or \c NULL ).
     *
     * \param print_output the address of the print output to initialize
     */

    void init_print_output (PrintOutput *print_output) ;



    /*! Allocates a PrintOutput in memory and sets its fields to their default
     *  value with #init_print_output
     *
     * \return the pointer to a new PrintOutput
     * \return \c NULL if the memory allocation fails
     */

    PrintOutput *alloc_and_init_print_output (void) ;



    /*! Frees the memory related to \a print_output
     *
     * The parametrer \a print_output must be a pointer previously obtained with
     * #alloc_and_init_print_output
     *
     * \param print_output the address of the PrintOutput structure to free
     */

    void free_print_output (PrintOutput *print_output) ;



    /*! Frees a list of print output
     *
     * If frees, calling #free_print_output, the print output pointed by the
     * parameter \a list and all the print outputs it finds following the
     * linked list throught the field PrintOutput::Next .
     *
     * \param list the pointer to the first elment in the list to be freed
     */

    void free_print_output_list (PrintOutput *list) ;



    /*! Prints a list of print outputs as they look in the stack file
     *
     * \param stream the output stream (must be already open)
     * \param prefix a string to be printed as prefix at the beginning of each line
     * \param list   the pointer to the first print output in the list
     */

    void print_formatted_print_output_list

        (FILE *stream, char *prefix, PrintOutput *list) ;



    /*! Prints a list of detailed information about all the fields of the print outputs
     *
     * \param stream the output stream (must be already open)
     * \param prefix a string to be printed as prefix at the beginning of each line
     * \param list   the pointer to the first print output in the list
     */

    void print_detailed_print_output_list

        (FILE *stream, char *prefix, PrintOutput *list) ;


    /*! Initializes the file in which a particular output has to be printed
     *
     *  \param print_output the address of the PrintOutput structure
     *  \param list pointer to the first (as a list) stack element the stack
     *
     *  \return FIXME
     */

    // FIXME: can we remove the list ?

    Error_t initialize_print_output (PrintOutput *print_output, StackElement *list) ;



    /*! Tells if there is at least one Tmap instruction in the \a list
     *
     *  \param list pointer to the first (list)) stack element the stack
     *
     *  \return \c true if there is a Tmap instance in \a list
     *  \return \c false otherwise
     */

    bool there_is_tmap_in_list (PrintOutput *list) ;

/******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* _3DICE_STACK_ELEMENT_H_ */
