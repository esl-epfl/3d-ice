/******************************************************************************
 * This file is part of 3D-ICE, version 1.0.2 .                               *
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

#ifndef _3DICE_SYSTEM_MATRIX_
#define _3DICE_SYSTEM_MATRIX_

/*! \file system_matrix.h */

#ifdef __cplusplus
extern "C"
{
#endif

/******************************************************************************/

#include "stdint.h"

#include "dimensions.h"
#include "thermal_cell.h"

/******************************************************************************/

    /*! \struct SystemMatrix
     *
     *  \brief Structure representing the squared matrix storing the coefficients of the
     *         linear system that is solved tu run the thermal simulation
     *
     * Compressed Column Storage (CCS): the matrix stores non zero values as
     * sequences of columns.
     */

    struct SystemMatrix
    {

        /*! Pointer to the array storing the column pointers.
         *  If the matrix is nxn, then n+1 column pointer are needed.
         */

        uint32_t *ColumnPointers ;

        /*! Pointer to the array storing the row indexes
         *  If the matrix has nnz elements, then nnz row indexes are needed */

        uint32_t* RowIndices ;

        /*! Pointer to the array storing the non zeroes coefficient */

        double* Values ;

        /*! The dimension n of the squared matrix nxn */

        uint32_t Size ;

        /*! The number of nonzeroes coefficients */

        uint32_t NNz ;

    } ;

    /*! Definition of the type SystemMAtrix */

    typedef struct SystemMatrix SystemMatrix ;

/******************************************************************************/


    /*! Sets all the fields of \a system_matrix to a default value (zero or \c NULL ).
     *
     * \param system_matrix the address of the system matrix to initialize
     */

    void init_system_matrix (SystemMatrix *system_matrix) ;



    /*! Allocates memory to store indexes and coefficients of a SystemMatrix
     *
     * \param system_matrix the address of the system matrix
     * \param size          the dimension of the matrix
     * \param nnz           the number of nonzeroes coeffcients
     *
     * \return \c TDICE_SUCCESS if the memory allocation succeded
     * \return \c TDICE_FAILURE if the memory allocation fails
     */

    Error_t alloc_system_matrix

        (SystemMatrix *system_matrix, uint32_t size, uint32_t nnz) ;



    /*! Frees the memory used to store indexes and coefficients of a system matrix
     *
     * \param system_matrix the address of the system matrix structure
     */

    void free_system_matrix (SystemMatrix *system_matrix) ;



    /*! Generates a text file storing the nonzeroes coefficients
     *
     * The file will contain one row of the form row-column-value" for each
     * zero coefficient (COO format). The first row (or column) has index 1
     * (matlab compatibile)
     *
     * \param file_name     the name of the file to create
     * \param system_matrix the system matrix structure
     */

    void print_system_matrix (char *file_name, SystemMatrix system_matrix) ;



    /*! Fills a column of the system matrix
     *
     *  The column corresponds to a thermal cell in a solid layer
     *
     *  \param dimensions    pointer to the structure storing the dimensions
     *  \param thermal_cells pointer to the first thermal cell in the 3d stack
     *  \param layer_index   layer index of the thermal cell
     *  \param row_index     row index of the thermal cell
     *  \param column_index  column index of the thermal cell
     *  \param system_matrix copy of the system matrix structure
     *
     *  \return A matrix partially filled (FIXME)
     */

    SystemMatrix add_solid_column
    (
        Dimensions   *dimensions,
        ThermalCell  *thermal_cells,

        uint32_t      layer_index,
        uint32_t      row_index,
        uint32_t      column_index,

        SystemMatrix  system_matrix
    ) ;



    /*! Fills a column of the system matrix
     *
     *  The column corresponds to a thermal cell in a channel layer (4rm model)
     *
     *  \param dimensions    pointer to the structure storing the dimensions
     *  \param thermal_cells pointer to the first thermal cell in the 3d stack
     *  \param layer_index   layer index of the thermal cell
     *  \param row_index     row index of the thermal cell
     *  \param column_index  column index of the thermal cell
     *  \param system_matrix copy of the system matrix structure
     *
     *  \return A matrix partially filled (FIXME)
     */

    SystemMatrix add_liquid_column_mc_4rm
    (
        Dimensions   *dimensions,
        ThermalCell  *thermal_cells,

        uint32_t      layer_index,
        uint32_t      row_index,
        uint32_t      column_index,

        SystemMatrix  system_matrix
    ) ;



    /*! Fills a column of the system matrix
     *
     *  The column corresponds to a thermal cell in a liquid layer (2rm model)
     *
     *  \param dimensions    pointer to the structure storing the dimensions
     *  \param thermal_cells pointer to the first thermal cell in the 3d stack
     *  \param layer_index   layer index of the thermal cell
     *  \param row_index     row index of the thermal cell
     *  \param column_index  column index of the thermal cell
     *  \param system_matrix copy of the system matrix structure
     *
     *  \return A matrix partially filled (FIXME)
     */

    SystemMatrix add_liquid_column_mc_rm2
    (
        Dimensions   *dimensions,
        ThermalCell  *thermal_cells,

        uint32_t      layer_index,
        uint32_t      row_index,
        uint32_t      column_index,

        SystemMatrix  system_matrix
    ) ;



    /*! Fills a column of the system matrix
     *
     *  The column corresponds to a thermal cell in a bottom wall layer (2rm model)
     *
     *  \param dimensions    pointer to the structure storing the dimensions
     *  \param thermal_cells pointer to the first thermal cell in the 3d stack
     *  \param layer_index   layer index of the thermal cell
     *  \param row_index     row index of the thermal cell
     *  \param column_index  column index of the thermal cell
     *  \param system_matrix copy of the system matrix structure
     *
     *  \return A matrix partially filled (FIXME)
     */

    SystemMatrix add_bottom_wall_column_mc_rm2
    (
        Dimensions   *dimensions,
        ThermalCell  *thermal_cells,

        uint32_t      layer_index,
        uint32_t      row_index,
        uint32_t      column_index,

        SystemMatrix  system_matrix
    ) ;



    /*! Fills a column of the system matrix
     *
     *  The column corresponds to a thermal cell in a top wall layer (2rm model)
     *
     *  \param dimensions    pointer to the structure storing the dimensions
     *  \param thermal_cells pointer to the first thermal cell in the 3d stack
     *  \param layer_index   layer index of the thermal cell
     *  \param row_index     row index of the thermal cell
     *  \param column_index  column index of the thermal cell
     *  \param system_matrix copy of the system matrix structure
     *
     *  \return A matrix partially filled (FIXME)
     */

    SystemMatrix add_top_wall_column_mc_rm2
    (
        Dimensions   *dimensions,
        ThermalCell  *thermal_cells,

        uint32_t      layer_index,
        uint32_t      row_index,
        uint32_t      column_index,

        SystemMatrix  system_matrix
    ) ;


    /*! Fills a column of the system matrix
     *
     *  The column corresponds to a thermal cell in a virtual wall layer (2rm model)
     *
     *  \param dimensions    pointer to the structure storing the dimensions
     *  \param thermal_cells pointer to the first thermal cell in the 3d stack
     *  \param layer_index   layer index of the thermal cell
     *  \param row_index     row index of the thermal cell
     *  \param column_index  column index of the thermal cell
     *  \param system_matrix copy of the system matrix structure
     *
     *  \return A matrix partially filled (FIXME)
     */

    SystemMatrix add_virtual_wall_column_mc_rm2
    (
        Dimensions   *dimensions,
        ThermalCell  *thermal_cells,

        uint32_t      layer_index,
        uint32_t      row_index,
        uint32_t      column_index,

        SystemMatrix  system_matrix
    ) ;



    /*! Fills a column of the system matrix
     *
     *  The column corresponds to a thermal cell in a liquid layer (pin fin model)
     *
     *  \param dimensions    pointer to the structure storing the dimensions
     *  \param thermal_cells pointer to the first thermal cell in the 3d stack
     *  \param layer_index   layer index of the thermal cell
     *  \param row_index     row index of the thermal cell
     *  \param column_index  column index of the thermal cell
     *  \param system_matrix copy of the system matrix structure
     *
     *  \return A matrix partially filled (FIXME)
     */

    SystemMatrix add_liquid_column_pf
    (
        Dimensions   *dimensions,
        ThermalCell  *thermal_cells,

        uint32_t      layer_index,
        uint32_t      row_index,
        uint32_t      column_index,

        SystemMatrix  system_matrix
    ) ;



    /*! Fills a column of the system matrix
     *
     *  The column corresponds to a thermal cell in a bottom wall layer (pin fin model)
     *
     *  \param dimensions    pointer to the structure storing the dimensions
     *  \param thermal_cells pointer to the first thermal cell in the 3d stack
     *  \param layer_index   layer index of the thermal cell
     *  \param row_index     row index of the thermal cell
     *  \param column_index  column index of the thermal cell
     *  \param system_matrix copy of the system matrix structure
     *
     *  \return A matrix partially filled (FIXME)
     */

    SystemMatrix add_bottom_wall_column_pf
    (
        Dimensions   *dimensions,
        ThermalCell  *thermal_cells,

        uint32_t      layer_index,
        uint32_t      row_index,
        uint32_t      column_index,

        SystemMatrix  system_matrix
    ) ;



    /*! Fills a column of the system matrix
     *
     *  The column corresponds to a thermal cell in a top wall layer (pin fin model)
     *
     *  \param dimensions    pointer to the structure storing the dimensions
     *  \param thermal_cells pointer to the first thermal cell in the 3d stack
     *  \param layer_index   layer index of the thermal cell
     *  \param row_index     row index of the thermal cell
     *  \param column_index  column index of the thermal cell
     *  \param system_matrix copy of the system matrix structure
     *
     *  \return A matrix partially filled (FIXME)
     */

    SystemMatrix add_top_wall_column_pf
    (
        Dimensions   *dimensions,
        ThermalCell  *thermal_cells,

        uint32_t      layer_index,
        uint32_t      row_index,
        uint32_t      column_index,

        SystemMatrix  system_matrix
    ) ;



    /*! Fills a column of the system matrix
     *
     *  The column corresponds to a thermal cell in a virtual wall layer (pin fin model)
     *
     *  \param dimensions    pointer to the structure storing the dimensions
     *  \param thermal_cells pointer to the first thermal cell in the 3d stack
     *  \param layer_index   layer index of the thermal cell
     *  \param row_index     row index of the thermal cell
     *  \param column_index  column index of the thermal cell
     *  \param system_matrix copy of the system matrix structure
     *
     *  \return A matrix partially filled (FIXME)
     */

    SystemMatrix add_virtual_wall_column_pf
    (
        Dimensions   *dimensions,
        ThermalCell  *thermal_cells,

        uint32_t      layer_index,
        uint32_t      row_index,
        uint32_t      column_index,

        SystemMatrix  system_matrix
    ) ;

/******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* _3DICE_SYSTEM_MATRIX_ */
