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

#ifndef _3DICE_FLOORPLAN_MATRIX_
#define _3DICE_FLOORPLAN_MATRIX_

/*! \file floorplan_matrix.h */

#ifdef __cplusplus
extern "C"
{
#endif

/******************************************************************************/

#include <stdio.h>

#include "types.h"
#include "floorplan_element.h"
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


    /*! Sets all the fields of \a this to a default value (zero or \c NULL ).
     *
     * \param this the address of the floorplan matrix to initialize
     */

    void init_floorplan_matrix (FloorplanMatrix_t *this) ;



    /*! Allocates memory to store indexes and coefficients of a FloorplanMatrix
     *
     * \param this     the address of the floorplan matrix
     * \param nrows    the number of rows of the matrix
     * \param ncolumns the number of columns of the matrix
     * \param nnz      the number of nonzeroes coeffcients
     *
     * \return \c TDICE_SUCCESS if the memory allocation succeded
     * \return \c TDICE_FAILURE if the memory allocation fails
     */

    Error_t alloc_floorplan_matrix
    (
        FloorplanMatrix_t *this,
        CellIndex_t        nrows,
        CellIndex_t        ncolumns,
        CellIndex_t        nnz
    ) ;



    /*! Frees the memory used to store indexes and coefficients of a floorplan matrix
     *
     * \param this the address of the floorplan matrix structure
     */

    void free_floorplan_matrix (FloorplanMatrix_t *this) ;



    /*! Fills the floorplan matrix
     *
     *  The function fills, floorplan element by floorplan element, all
     *  the columns of the floorplan matrix.
     *
     *  \param this       pointer to the floorplan matrix to fill
     *  \param list       pointer to the list of floorplan elements
     *  \param dimensions pointer to the structure containing the dimensions of the IC
     */

    void fill_floorplan_matrix
    (
        FloorplanMatrix_t  *this,
        FloorplanElement_t *list,
        Dimensions_t       *dimensions
    ) ;



    /*! Generates a text file storing the nonzeroes coefficients
     *
     * The file will contain one row of the form row-column-value" for each
     * zero coefficient (COO format). The first row (or column) has index 1
     * (matlab compatibile)
     *
     * \param this      the floorplan matrix structure
     * \param file_name the name of the file to create
     */

    void print_floorplan_matrix (FloorplanMatrix_t this, String_t file_name) ;

/******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* _3DICE_FLOORPLAN_MATRIX_ */
