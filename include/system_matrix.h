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
#include "analysis.h"

#include "slu_ddefs.h"

/******************************************************************************/

    /*! \struct SystemMatrix_t
     *
     *  \brief Structure representing the squared matrix storing the
     *         coefficients of the linear system that is solved tu run
     *         the thermal simulation
     *
     * Compressed Column Storage (CCS): the matrix stores non zero values as
     * sequences of columns.
     */

    struct SystemMatrix_t
    {

        /*! Pointer to the array storing the column pointers.
         *  If the matrix is nxn, then n+1 column pointers are needed.
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

        /*! SuperLU matrix A (wrapper arount our SystemMatrix SM_A )*/

        SuperMatrix SLUMatrix_A ;

        /*! SuperLU matrix A after the permutation */

        SuperMatrix SLUMatrix_A_Permuted ;

        /*! SuperLU matrix L after the A=LU factorization */

        SuperMatrix SLUMatrix_L ;

        /*! SuperLU matrix U after the A=LU factorization */

        SuperMatrix SLUMatrix_U ;

        /*! SuperLU structure for statistics */

        SuperLUStat_t SLU_Stat ;

        /*! SuperLU structure for factorization options */

        superlu_options_t SLU_Options ;

        /*! SuperLU integer to code the result of the SLU routines */

        int  SLU_Info ;

        /*! SuperLU matrix R for permutation RAC = LU. */

        int* SLU_PermutationMatrixR ;

        /*! SuperLU matrix C for permutation RAC = LU. */

        int* SLU_PermutationMatrixC ;

        /*! SuperLU elimination tree */

        int* SLU_Etree ;
    } ;

    /*! Definition of the type SystemMatrix_t */

    typedef struct SystemMatrix_t SystemMatrix_t ;



/******************************************************************************/



    /*! Inits the fields of the \a sysmatrix structure with default values
     *
     * \param sysmatrix the address of the structure to initalize
     */

    void system_matrix_init (SystemMatrix_t *sysmatrix) ;



    /*! Allocates memory to store indexes and coefficients of a SystemMatrix
     *
     * \param sysmatrix the address of the system matrix
     * \param size the dimension of the matrix
     * \param nnz  the number of nonzeroes coeffcients
     *
     * \return \c TDICE_SUCCESS if the memory allocation succeded
     * \return \c TDICE_FAILURE if the memory allocation fails
     */

    Error_t system_matrix_build

        (SystemMatrix_t *sysmatrix, CellIndex_t size, CellIndex_t nnz) ;



    /*! Destroys the content of the fields of the structure \a sysmatrix
     *
     * The function releases any dynamic memory used by the structure and
     * resets its state calling \a system_matrix_init .
     *
     * \param sysmatrix the address of the structure to destroy
     */

    void system_matrix_destroy (SystemMatrix_t *sysmatrix) ;



    /*! Fills the system matrix
     *
     *  The function fills, layer by layer, all the columns
     *  of the system matrix.
     *
     *  \param sysmatrix         pointer to the system matrix to fill
     *  \param thermal_grid pointer to the thermal grid structure
     *  \param analysis     pointer to the structure containing info
     *                      about the type of thermal analysis
     *  \param dimensions   pointer to the structure containing the
     *                      dimensions of the IC
     */

    void fill_system_matrix
    (
        SystemMatrix_t *sysmatrix,
        ThermalGrid_t  *thermal_grid,
        Analysis_t     *analysis,
        Dimensions_t   *dimensions
    ) ;



    /*! Perform the A=LU decomposition on the system matrix
     *
     * \param sysmatrix pointer to the (system) matrix \a A to factorize
     *
     * \return \c TDICE_SUCCESS if the factorization succeded
     * \return \c TDICE_FAILURE if some error occured
     */

    Error_t do_factorization (SystemMatrix_t *sysmatrix) ;



    /*! Solve the linear system b = A/b
     *
     * \param sysmatrix pointer to the (system) matrix \a A
     * \param b    pointer to the input vector \a b
     *
     * \return \c TDICE_SUCCESS if the solution b has been found
     * \return \c TDICE_FAILURE if some error occured
     */

    Error_t solve_sparse_linear_system (SystemMatrix_t *sysmatrix, SuperMatrix *b) ;



    /*! Generates a text file storing the sparse matrix
     *
     * The file will contain one row of the form row-column-value" for each
     * zero coefficient (COO format). The first row (or column) has index 1
     * (matlab compatibile)
     *
     * \param sysmatrix      the system matrix structure
     * \param file_name the name of the file to create
     */

    void system_matrix_print (SystemMatrix_t sysmatrix, String_t file_name) ;

/******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* _3DICE_SYSTEM_MATRIX_ */
