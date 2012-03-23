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

        (SystemMatrix *system_matrix, CellIndex_t size, CellIndex_t nnz) ;



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

    void print_system_matrix (String_t file_name, SystemMatrix system_matrix) ;



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

        CellIndex_t   layer_index,
        CellIndex_t   row_index,
        CellIndex_t   column_index,

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

    SystemMatrix add_liquid_column_4rm
    (
        Dimensions   *dimensions,
        ThermalCell  *thermal_cells,

        CellIndex_t   layer_index,
        CellIndex_t   row_index,
        CellIndex_t   column_index,

        SystemMatrix  system_matrix
    ) ;



    /*! Fills a column of the system matrix
     *
     *  The column corresponds to a thermal cell in a liquid layer
     *  in the 2rm model (microchannels and pin fins)
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

    SystemMatrix add_liquid_column_2rm
    (
        Dimensions   *dimensions,
        ThermalCell  *thermal_cells,

        CellIndex_t   layer_index,
        CellIndex_t   row_index,
        CellIndex_t   column_index,

        SystemMatrix  system_matrix
    ) ;



    /*! Fills a column of the system matrix
     *
     *  The column corresponds to a thermal cell in a bottom wall layer
     *  in the 2rm model (microchannels and pin fins)
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

    SystemMatrix add_bottom_wall_column_2rm
    (
        Dimensions   *dimensions,
        ThermalCell  *thermal_cells,

        CellIndex_t   layer_index,
        CellIndex_t   row_index,
        CellIndex_t   column_index,

        SystemMatrix  system_matrix
    ) ;



    /*! Fills a column of the system matrix
     *
     *  The column corresponds to a thermal cell in a top wall layer
     *  in the 2rm model (microchannels and pin fins)
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

    SystemMatrix add_top_wall_column_2rm
    (
        Dimensions   *dimensions,
        ThermalCell  *thermal_cells,

        CellIndex_t   layer_index,
        CellIndex_t   row_index,
        CellIndex_t   column_index,

        SystemMatrix  system_matrix
    ) ;


    /*! Fills a column of the system matrix
     *
     *  The column corresponds to a thermal cell in a virtual wall layer (2rm model)
     *
     *  \param dimensions    pointer to the structure storing the dimensions
     *  \param thermal_cells pointer to the first thermal cell in the 3d stack
     *  \param channel_model the model of the channel, to distinguish between
     *                       2rm microchannel or 2rm pinfins
     *  \param layer_index   layer index of the thermal cell
     *  \param row_index     row index of the thermal cell
     *  \param column_index  column index of the thermal cell
     *  \param system_matrix copy of the system matrix structure
     *
     *  \return A matrix partially filled (FIXME)
     */

    SystemMatrix add_virtual_wall_column_2rm
    (
        Dimensions    *dimensions,
        ThermalCell   *thermal_cells,

        ChannelModel_t channel_model,

        CellIndex_t    layer_index,
        CellIndex_t    row_index,
        CellIndex_t    column_index,

        SystemMatrix   system_matrix
    ) ;

/******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* _3DICE_SYSTEM_MATRIX_ */
