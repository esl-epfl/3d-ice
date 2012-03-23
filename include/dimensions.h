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

#ifndef _3DICE_DIMENSIONS_H_
#define _3DICE_DIMENSIONS_H_

/*! \file dimensions.h */

#ifdef __cplusplus
extern "C"
{
#endif

/******************************************************************************/

#include <stdio.h>

#include "types.h"

/******************************************************************************/

    /*! \struct CellDimensions
     *
     *  \brief Structure that collects the dimensions of a thermal cell
     *
     *  We store just the length (west-east) and the width (south-north) of a cell.
     *  The height depends on the layer the cell belongs to. The witdh is uniform
     *  along the south-north direction of the chip while the length can have four
     *  different values depending on the location. This is due to the 4RM thermal
     *  model for microchannels: this model forces the whole mesh of cells to have
     *  the same dimensions of walls and channels.
     *
     *  If the 2RM thermal model is used (microchannels or pin fins) of if the
     *  channel does not appear as a stack element then these four values store
     *  the same value.
     */

    struct CellDimensions
    {
        /*! The length of the first wall of a channel in \f$ \mu m \f$
         *  (west side of the 3D-IC). It corresponds to the length of all the
         *  cells in the first column */

        CellDimension_t FirstWallLength ;

        /*! The length of all the walls of a channel in \f$ \mu m \f$, except
         *  the first and the last. It corresponds to the length of all the
         *  cells with an even index */

        CellDimension_t WallLength ;

        /*! The length of all the channel cavity of a channel in \f$ \mu m \f$.
         *  It corresponds to the length of all the cells with an odd index */

        CellDimension_t ChannelLength ;

        /*! The length of the last wall of a channel in \f$ \mu m \f$ (east
         *  side of the 3D-IC). It corresponds to the length of all the cells
         *  in the last column */

        CellDimension_t LastWallLength ;

        /*! The width of all the cells in \f$ \mu m \f$*/

        CellDimension_t Width ;

    } ;

    /*! Definition of the type CellDimensions */

    typedef struct CellDimensions CellDimensions ;

/******************************************************************************/

    /*! \struct GridDimensions
     *
     *  \brief Structure that collects the dimensions of a 3d thermal grid of cells
     */

    struct GridDimensions
    {
        /*! The number of layers along the bottom-top direction.
         *
         *  The index of a layer increases from bottom to top.
         *
         *  This value is computed as the sum of the number of layers used
         *  by each stack element (die, channel or layers) */

        CellIndex_t NLayers ;

        /*! The number of rows along the south-north direction.
         *
         *  The index of a row increases from south to north.
         *
         *  This value is computed as the ratio between the width of the IC
         *  and the width of the thermal cell */

        CellIndex_t NRows ;

        /*! The number of columns along the west-east direction.
         *
         *  The index of a column increases from west to east.
         *
         *  If there are no channels in the stack or the 2RM thermal model
         *  is used, this value is computed as the ratio between the length
         *  of the IC and the length of the thermal cell. When the 4RM thermal
         *  model is used, instead, this value must be odd and at greather than
         *  three. Moreover, the length of the IC, together with the lengths
         *  of the cavity of the channel and the walls must be such that the
         *  resulting number of column is an integer value.
         */

        CellIndex_t NColumns ;

        /*! The number of thermal cells into which the IC is divided.
         *
         *  This value is computed as NRows times NColumns times NLayers. */

        CellIndex_t NCells ;

        /*! The number of connections between the thermal cells. */

        CellIndex_t NConnections ;
    } ;

    /*! The definition of the type GridDimensions */

    typedef struct GridDimensions GridDimensions ;

/******************************************************************************/

    /*! \struct ChipDimensions
     *
     *  \brief Structure that collects the main dimensions of the IC
     */

    struct ChipDimensions
    {
        /*! The length of the IC (west-east) in \f$ \mu m \f$ */

        ChipDimension_t Length ;

        /*! The width of the IC (south-north) in \f$ \mu m \f$ */

        ChipDimension_t Width ;
    } ;

    /*! Definition of the type ChipDimension */

    typedef struct ChipDimensions ChipDimensions ;

/******************************************************************************/

    /*! \struct Dimensions
     *  \brief Collections of all the structures that are needed for the thermal simulation
     */

    struct Dimensions
    {
        /*! The dimensions of the thermal cells */

        CellDimensions Cell ;

        /*! The dimensions of the grid of thermal cells */

        GridDimensions Grid ;

        /*! The dimensions of the IC */

        ChipDimensions Chip ;

    } ;

    /*! Definition of the type Dimensions */

    typedef struct Dimensions Dimensions ;

/******************************************************************************/



    /*! Sets all the fields of \a dimensions to a default value (zero).
     *
     * \param dimensions the address of the dimensions structure to initialize
     */

    void init_dimensions (Dimensions *dimensions) ;



    /*! Allocates a Dimensions structure in memory and sets its fields to their
     *  default value with #init_dimensions
     *
     * \return the pointer to a new Dimensions
     * \return \c NULL if the memory allocation fails
     */

    Dimensions *alloc_and_init_dimensions (void) ;



    /*! Frees the memory related to \a dimensions
     *
     * The parametrer \a dimensions must be a pointer previously obtained with
     * #alloc_and_init_dimensions
     *
     * \param dimensions the address of the dimensions structure to free
     */

    void free_dimensions (Dimensions *dimensions) ;



    /*! Prints the dimensions as they look in the stack file
     *
     * \param stream     the output stream (must be already open)
     * \param prefix     a string to be printed as prefix at the beginning of each line
     * \param dimensions the dimensions to print
     */

    void print_formatted_dimensions

        (FILE *stream, String_t prefix, Dimensions *dimensions) ;



    /*! Prints detailed information about all the fields of \a dimensions
     *
     * \param stream     the output stream (must be already open)
     * \param prefix     a string to be printed as prefix at the beginning of each line
     * \param dimensions the dimensions to print
     */

   void print_detailed_dimensions

        (FILE *stream, String_t prefix, Dimensions *dimensions) ;



    /*! Creates two text files containing the X and Y coordinates of the thermal cells
     *
     *  \param dimensions the address of the dimensions structure
     */

    void print_axes (Dimensions *dimensions) ;



    /*! Computes the number of connections between the thermal cells in the stack
     *
     *  Sets the content of the field Dimensions::NConnections
     *
     *  \param dimensions the address of the dimensions structure
     *  \param num_channels number of channels (as stack element) in the stack
     *  \param channel_model the model of the channel used in the stack
     */

    void compute_number_of_connections

        (Dimensions *dimensions, Quantity_t num_channels, ChannelModel_t channel_model) ;

    /*! Returns the length of a thermal cell
     *
     * \param dimensions   the address of the structure containing all the dimensions
     * \param column_index the column index of the thermal cell
     *
     * \return the length of the thermal cell in position \a coluimn_index
     */

    CellDimension_t get_cell_length (Dimensions *dimensions, CellIndex_t column_index) ;



    /*! Returns the width of a thermal cell
     *
     * \param dimensions the address of the structure containing all the dimensions
     * \param row_index  the row index of the thermal cell
     *
     * \return the width of the thermal cell in position \a row_index
     */

    CellDimension_t get_cell_width (Dimensions *dimensions, CellIndex_t row_index) ;



    /*! Returns the X coordinate of the center of the cell at \a column_index
     *
     * \param dimensions   the address of the structure containing all the dimensions
     * \param column_index the column index of the thermal cell
     *
     * \return the X coordinate of the center of thermal cell in position \a column_index
     */

    ChipDimension_t get_cell_center_x (Dimensions *dimensions, CellIndex_t column_index) ;



    /*! Returns the Y coordinate of the center of the cell at \a row_index
     *
     * \param dimensions the address of the structure containing all the dimensions
     * \param row_index  the row index of the thermal cell
     *
     * \return the Y coordinate of the center of thermal cell in position \a row_index
     */

    ChipDimension_t get_cell_center_y (Dimensions *dimensions, CellIndex_t row_index) ;



    /*! Returns the X coordinate of the south-west corner of the cell at \a column_index
     *
     * \param dimensions   the address of the structure containing all the dimensions
     * \param column_index the column index of the thermal cell
     *
     * \return the X coordinate of the south-west corner of thermal cell in position \a column_index
     */

    ChipDimension_t get_cell_location_x (Dimensions *dimensions, CellIndex_t column_index) ;



    /*! Returns the Y coordinate of the south-west corner of the cell at \a row_index
     *
     * \param dimensions the address of the structure containing all the dimensions
     * \param row_index  the row index of the thermal cell
     *
     * \return the X coordinate of the south-west corner of thermal cell in position \a row_index
     */

    ChipDimension_t get_cell_location_y (Dimensions *dimensions, CellIndex_t row_index) ;



    /*! Returns the number of layers in the 3d stack
     *
     * \param dimensions the address of the structure containing all the dimensions
     *
     * \return the number of layers
     */

    CellIndex_t get_number_of_layers (Dimensions *dimensions) ;



    /*! Returns the number of rows in the 3d stack
     *
     * \param dimensions the address of the structure containing all the dimensions
     *
     * \return the number of rows
     */

    CellIndex_t get_number_of_rows (Dimensions *dimensions) ;



    /*! Returns the number of columns in the 3d stack
     *
     * \param dimensions the address of the structure containing all the dimensions
     *
     * \return the number of columns
     */

    CellIndex_t get_number_of_columns (Dimensions *dimensions) ;



    /*! Returns the number of thermal cells in the 3d stack
     *
     * \param dimensions the address of the structure containing all the dimensions
     *
     * \return the number of thermal cells
     */

    CellIndex_t get_number_of_cells (Dimensions *dimensions) ;



    /*! Returns the number of connections between the thermal cells in the 3d stack
     *
     * \param dimensions the address of the structure containing all the dimensions
     *
     * \return the number of connections
     */

    CellIndex_t get_number_of_connections (Dimensions *dimensions) ;



    /*! Returns the number of thermal cells in a layer
     *
     * \param dimensions the address of the structure containing all the dimensions
     *
     * \return the number of thermal cells in a layer
     */

    CellIndex_t get_layer_area (Dimensions *dimensions) ;



    /*! Returns the index of a thermal cell within a layer
     *
     *  The index is computed as \f$ C \cdot r + c \f$, where \f$ C \f$
     *  is the number of columns in the 3d grid.
     *
     * \param dimensions   the address of the structure containing all the dimensions
     * \param row_index    the row index \f$ r \f$ of the thermal cell
     * \param column_index the column index \f$ c \f$ of the thermal cell
     *
     * \return the index of a thermal cell \f$ (r, c) \f$ in a layer
     */

    CellIndex_t get_cell_offset_in_layer

        (Dimensions *dimensions, CellIndex_t row_index, CellIndex_t column_index) ;



    /*! Returns the index of a thermal cell within the entire 3d stack
     *
     *  The index is computed as \f$ R \cdot C \cdot l + C \cdot r + c \f$,
     *  where \f$ R \f$ and \f$ C \f$ are, respectively, the number of rows and
     *  columns in the 3d grid
     *
     * \param dimensions   the address of the structure containing all the dimensions
     * \param layer_index  the layer index \f$ l \f$ of the thermal cell
     * \param row_index    the row index \f$ r \f$ of the thermal cell
     * \param column_index the column index \f$ c \f$ of the thermal cell
     *
     * \return the index of a thermal cell \f$ (l, r, c) \f$ in the 3d stack
     */

    CellIndex_t get_cell_offset_in_stack

        (Dimensions *dimensions,
         CellIndex_t layer_index, CellIndex_t row_index, CellIndex_t column_index) ;



    /*! Returns the length of the IC
     *
     * \param dimensions the address of the structure containing all the dimensions
     *
     * \return the length of the IC
     */

    ChipDimension_t get_chip_length (Dimensions *dimensions) ;



    /*! Returns the width of the IC
     *
     * \param dimensions the address of the structure containing all the dimensions
     *
     * \return the width of the IC
     */

    ChipDimension_t get_chip_width (Dimensions *dimensions) ;

/******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* _3DICE_DIMENSIONS_H_ */
