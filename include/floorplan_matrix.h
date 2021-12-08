/******************************************************************************
 * This file is part of 3D-ICE, version 3.1.0 .                               *
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
 *                             Copyright (C) 2021                             *
 *   Embedded Systems Laboratory - Ecole Polytechnique Federale de Lausanne   *
 *                            All Rights Reserved.                            *
 *                                                                            *
 * Authors: Arvind Sridhar              Alessandro Vincenzi                   *
 *          Giseong Bak                 Martino Ruggiero                      *
 *          Thomas Brunschwiler         Eder Zulian                           *
 *          Federico Terraneo           Darong Huang                          *
 *          Luis Costero                Marina Zapater                        *
 *          David Atienza                                                     *
 *                                                                            *
 * For any comment, suggestion or request  about 3D-ICE, please  register and *
 * write to the mailing list (see http://listes.epfl.ch/doc.cgi?liste=3d-ice) *
 * Any usage  of 3D-ICE  for research,  commercial or other  purposes must be *
 * properly acknowledged in the resulting products or publications.           *
 *                                                                            *
 * EPFL-STI-IEL-ESL                     Mail : 3d-ice@listes.epfl.ch          *
 * Batiment ELG, ELG 130                       (SUBSCRIPTION IS NECESSARY)    *
 * Station 11                                                                 *
 * 1015 Lausanne, Switzerland           Url  : http://esl.epfl.ch/3d-ice      *
 ******************************************************************************/

#ifndef _3DICE_FLOORPLAN_MATRIX_
#define _3DICE_FLOORPLAN_MATRIX_

/*! \file floorplan_matrix.h */

#ifdef __cplusplus
extern "C"
{
#endif

/******************************************************************************/

#include "types.h"
#include "string_t.h"

#include "floorplan_element_list.h"
#include "dimensions.h"

/******************************************************************************/

    /*! \struct FloorplanMatrix_t
     *
     *  \brief Structure representing the matrix storing the coefficients of the
     *         floorplans tha scales power values to sources
     *
     * Compressed Column Storage (CCS): the matrix stores non zero values as
     * sequences of columns.
     */

    struct FloorplanMatrix_t
    {

        /*! Pointer to the array storing the column pointers.
         *  If the matrix is nxm, then m+1 column pointers are needed.
         */

        CellIndex_t *ColumnPointers ;

        /*! Pointer to the array storing the row indexes
         *  If the matrix has nnz elements, then nnz row indexes are needed */

        CellIndex_t *RowIndices ;

        /*! Pointer to the array storing the non zeroes coefficient */

        Source_t *Values ;

        /*! The number of rows (i.e. the number of thermal cells on a layer) */

        CellIndex_t NRows ;

        /*! The number of columns (i.e. the number of floorplan elements) */

        CellIndex_t NColumns ;

        /*! The number of nonzeroes coefficients */

        CellIndex_t NNz ;
    } ;

    /*! Definition of the type FloorplanMatrix_t */

    typedef struct FloorplanMatrix_t FloorplanMatrix_t ;



/******************************************************************************/



    /*! Inits the fields of the \a flpmatrix structure with default values
     *
     * \param flpmatrix the address of the structure to initalize
     */

    void floorplan_matrix_init (FloorplanMatrix_t *flpmatrix) ;



    /*! Copies the structure \a src into \a dst , as an assignement
     *
     * The function destroys the content of \a dst and then makes the copy
     *
     * \param dst the address of the left term sructure (destination)
     * \param src the address of the right term structure (source)
     */

    void floorplan_matrix_copy

        (FloorplanMatrix_t *dst, FloorplanMatrix_t *src) ;



    /*! Allocates memory to store indexes and coefficients of a FloorplanMatrix
     *
     * \param flpmatrix     the address of the floorplan matrix
     * \param nrows    the number of rows of the matrix
     * \param ncolumns the number of columns of the matrix
     * \param nnz      the number of nonzeroes coeffcients
     *
     * \return \c TDICE_SUCCESS if the memory allocation succeded
     * \return \c TDICE_FAILURE if the memory allocation fails
     */

    Error_t floorplan_matrix_build
    (
        FloorplanMatrix_t *flpmatrix,
        CellIndex_t        nrows,
        CellIndex_t        ncolumns,
        CellIndex_t        nnz
    ) ;



    /*! Destroys the content of the fields of the structure \a flpmatrix
     *
     * The function releases any dynamic memory used by the structure and
     * resets its state calling \a floorplan_matrix_init .
     *
     * \param flpmatrix the address of the structure to destroy
     */

    void floorplan_matrix_destroy (FloorplanMatrix_t *flpmatrix) ;



    /*! Fills the floorplan matrix
     *
     *  The function fills, floorplan element by floorplan element, all
     *  the columns of the floorplan matrix.
     *
     *  \param flpmatrix  pointer to the floorplan matrix to fill
     *  \param list       pointer to the list of floorplan elements
     *  \param dimensions the address of the dimension structure
     */

    void floorplan_matrix_fill
    (
        FloorplanMatrix_t      *flpmatrix,
        FloorplanElementList_t *list,
        Dimensions_t           *dimensions
    ) ;



    /*! Performs a Matrix-Vector Multiplication x = Ab
     *
     * The multiplication corresponds to the routine DGEMV
     * \c y := \c alpha * \c A* \c x + \c beta * \c y where \c alpha = 1.0 and
     * \c beta = 1.0 .
     *
     * \param flpmatrix pointer to the (floorplan) matrix \a A
     * \param x    pointer to the output vector \a x
     * \param b    pointer to the input vector \a b
     *
     */

    void floorplan_matrix_multiply

        (FloorplanMatrix_t *flpmatrix, Source_t *x, Source_t *b) ;



    /*! Generates a text file storing the sparse matrix
     *
     * The file will contain one row of the form row-column-value" for each
     * zero coefficient (COO format). The first row (or column) has index 1
     * (matlab compatibile)
     *
     * \param flpmatrix the floorplan matrix structure
     * \param file_name the name of the file to create
     */

    void floorplan_matrix_print

        (FloorplanMatrix_t flpmatrix, String_t file_name) ;

/******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* _3DICE_FLOORPLAN_MATRIX_ */
