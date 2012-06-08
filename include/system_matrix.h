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

#ifndef _3DICE_SYSTEM_MATRIX_
#define _3DICE_SYSTEM_MATRIX_

/*! \file system_matrix.h */

#ifdef __cplusplus
extern "C"
{
#endif

/******************************************************************************/

#include <stdio.h>

#include "types.h"

#include "dimensions.h"
#include "thermal_grid.h"

/******************************************************************************/

    /*! \struct SystemMatrix_t
     *
     *  \brief Structure representing the squared matrix storing the coefficients of the
     *         linear system that is solved tu run the thermal simulation
     *
     * Compressed Column Storage (CCS): the matrix stores non zero values as
     * sequences of columns.
     */

    struct SystemMatrix_t
    {

        /*! Pointer to the array storing the column pointers.
         *  If the matrix is nxn, then n+1 column pointer are needed.
         */

        CellIndex_t *ColumnPointers ;

        /*! Pointer to the array storing the row indexes
         *  If the matrix has nnz elements, then nnz row indexes are needed */

        CellIndex_t *RowIndices ;

        /*! Pointer to the array storing the non zeroes coefficient */

        SystemMatrixCoeff_t* Values ;

        /*! The dimension n of the squared matrix nxn */

        CellIndex_t Size ;

        /*! The number of nonzeroes coefficients */

        CellIndex_t NNz ;

    } ;

    /*! Definition of the type SystemMatrix_t */

    typedef struct SystemMatrix_t SystemMatrix_t ;

/******************************************************************************/


    /*! Sets all the fields of \a system_matrix to a default value (zero or \c NULL ).
     *
     * \param this the address of the system matrix to initialize
     */

    void init_system_matrix (SystemMatrix_t *this) ;



    /*! Allocates memory to store indexes and coefficients of a SystemMatrix
     *
     * \param this the address of the system matrix
     * \param size the dimension of the matrix
     * \param nnz  the number of nonzeroes coeffcients
     *
     * \return \c TDICE_SUCCESS if the memory allocation succeded
     * \return \c TDICE_FAILURE if the memory allocation fails
     */

    Error_t alloc_system_matrix

        (SystemMatrix_t *this, CellIndex_t size, CellIndex_t nnz) ;



    /*! Frees the memory used to store indexes and coefficients of a system matrix
     *
     * \param this the address of the system matrix structure
     */

    void free_system_matrix (SystemMatrix_t *this) ;



    /*! Fills the system matrix
     *
     *  The function fills, layer by layer, all the columns
     *  of the system matrix.
     *
     *  \param this         pointer to the system matrix to fill
     *  \param thermal_grid pointer to the thermal grid structure
     *  \param dimensions   pointer to the structure containing the dimensions of the IC
     */

    void fill_system_matrix
    (
        SystemMatrix_t *this,
        ThermalGrid_t  *thermal_grid,
        Dimensions_t   *dimensions
    ) ;



    /*! Generates a text file storing the nonzeroes coefficients
     *
     * The file will contain one row of the form row-column-value" for each
     * zero coefficient (COO format). The first row (or column) has index 1
     * (matlab compatibile)
     *
     * \param file_name the name of the file to create
     * \param this      the system matrix structure
     */

    void print_system_matrix (String_t file_name, SystemMatrix_t this) ;

/******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* _3DICE_SYSTEM_MATRIX_ */
