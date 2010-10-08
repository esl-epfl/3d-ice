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

#ifndef _3DICE_SYSTEM_MATRIX_
#define _3DICE_SYSTEM_MATRIX_

#ifdef __cplusplus
extern "C"
{
#endif

#include "stack_description.h"

/******************************************************************************/

  typedef struct
  {
    /* Compressed Column Storage (CCS):                           */
    /* the matrix stores non zero values as sequences of Columns. */

    /* Storage for the column pointers */

    ColumnIndex_t*       ColumnPointers ;

    /* Storage for the row indices. */

    RowIndex_t*          RowIndices ;

    /* Storage for the nonzero entries. */

    SystemMatrixValue_t* Values ;

    Quantity_t Size ;
    Quantity_t NNz ;

  }  SystemMatrix ;

/******************************************************************************/

  int alloc_system_matrix
  (
    SystemMatrix* matrix,
    Quantity_t nvalues,
    Quantity_t nnz
  ) ;

/******************************************************************************/

  void free_system_matrix (SystemMatrix* matrix) ;

/******************************************************************************/

  Quantity_t add_solid_column
  (
    Dimensions*           dimensions,
    Conductances*         conductances,
    Capacity_t*           capacities,
    ConventionalHeatSink* conventionalheatsink,
    LayerIndex_t          current_layer,
    RowIndex_t            current_row,
    ColumnIndex_t         current_column,
    ColumnIndex_t*        column_pointers,
    RowIndex_t*           row_indices,
    SystemMatrixValue_t*  values
  ) ;

/******************************************************************************/

  Quantity_t add_liquid_column
  (
    Dimensions*          dimensions,
    Conductances*        conductances,
    Capacity_t*          capacities,
    LayerIndex_t         current_layer,
    RowIndex_t           current_row,
    ColumnIndex_t        current_column,
    ColumnIndex_t*       column_pointers,
    RowIndex_t*          row_indices,
    SystemMatrixValue_t* values
  ) ;

/******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* _3DICE_SYSTEM_MATRIX_ */
