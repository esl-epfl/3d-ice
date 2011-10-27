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

#ifdef __cplusplus
extern "C"
{
#endif

/******************************************************************************/

#include "dimensions.h"
#include "thermal_cell.h"

/******************************************************************************/

  typedef struct
  {
    /* Compressed Column Storage (CCS):                           */
    /* the matrix stores non zero values as sequences of Columns. */

    /* Storage for the column pointers */

    SystemMatrixColumn_t* ColumnPointers ;

    /* Storage for the row indices. */

    SystemMatrixRow_t* RowIndices ;

    /* Storage for the nonzero entries. */

    SystemMatrixValue_t* Values ;

    Quantity_t Size ;
    Quantity_t NNz ;

  }  SystemMatrix ;

/******************************************************************************/

  void init_system_matrix (SystemMatrix* matrix) ;

/******************************************************************************/

  int alloc_system_matrix
  (
    SystemMatrix* matrix,
    Quantity_t    nvalues,
    Quantity_t    nnz
  ) ;

/******************************************************************************/

  void free_system_matrix (SystemMatrix* matrix) ;

/******************************************************************************/

  void print_system_matrix (String_t filename, SystemMatrix matrix) ;

/******************************************************************************/

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
        Dimensions      *dimensions,
        ThermalCell     *thermal_cells,

        GridDimension_t  layer_index,
        GridDimension_t  row_index,
        GridDimension_t  column_index,

        SystemMatrix     system_matrix
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
        Dimensions      *dimensions,
        ThermalCell     *thermal_cells,

        GridDimension_t  layer_index,
        GridDimension_t  row_index,
        GridDimension_t  column_index,

        SystemMatrix     system_matrix
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

    SystemMatrix add_liquid_column_mc_2rm
    (
        Dimensions      *dimensions,
        ThermalCell     *thermal_cells,

        GridDimension_t  layer_index,
        GridDimension_t  row_index,
        GridDimension_t  column_index,

        SystemMatrix     system_matrix
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

    SystemMatrix add_bottom_wall_column_mc_2rm
    (
        Dimensions      *dimensions,
        ThermalCell     *thermal_cells,

        GridDimension_t  layer_index,
        GridDimension_t  row_index,
        GridDimension_t  column_index,

        SystemMatrix     system_matrix
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

    SystemMatrix add_top_wall_column_mc_2rm
    (
        Dimensions      *dimensions,
        ThermalCell     *thermal_cells,

        GridDimension_t  layer_index,
        GridDimension_t  row_index,
        GridDimension_t  column_index,

        SystemMatrix     system_matrix
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

    SystemMatrix add_virtual_wall_column_mc_2rm
    (
        Dimensions      *dimensions,
        ThermalCell     *thermal_cells,

        GridDimension_t  layer_index,
        GridDimension_t  row_index,
        GridDimension_t  column_index,

        SystemMatrix     system_matrix
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
        Dimensions      *dimensions,
        ThermalCell     *thermal_cells,

        GridDimension_t  layer_index,
        GridDimension_t  row_index,
        GridDimension_t  column_index,

        SystemMatrix     system_matrix
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
        Dimensions      *dimensions,
        ThermalCell     *thermal_cells,

        GridDimension_t  layer_index,
        GridDimension_t  row_index,
        GridDimension_t  column_index,

        SystemMatrix     system_matrix
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
        Dimensions      *dimensions,
        ThermalCell     *thermal_cells,

        GridDimension_t  layer_index,
        GridDimension_t  row_index,
        GridDimension_t  column_index,

        SystemMatrix     system_matrix
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
        Dimensions      *dimensions,
        ThermalCell     *thermal_cells,

        GridDimension_t  layer_index,
        GridDimension_t  row_index,
        GridDimension_t  column_index,

        SystemMatrix     system_matrix
    ) ;

/******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* _3DICE_SYSTEM_MATRIX_ */
