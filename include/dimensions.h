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

    /*! \struct CellDimensions_t
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

    struct CellDimensions_t
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

        /*! The number of values stored by the vector \a Heights */

        Quantity_t NHeights ;

        /*! The heights of all the cells in \f$ \mu m \f$. The vector stores
         *  as many values as layers in the 3D-IC */

        CellDimension_t *Heights ;

    } ;

    /*! Definition of the type CellDimension_t */

    typedef struct CellDimensions_t CellDimensions_t ;



    /*! Sets all the fields to a default value (zero or \c NULL ).
     *
     * \param this the address of the cell dimensions structure to initialize
     */

    void init_cell_dimensions (CellDimensions_t *this) ;

/******************************************************************************/

    /*! \struct GridDimensions_t
     *
     *  \brief Structure that collects the dimensions of a 3d thermal grid of cells
     */

    struct GridDimensions_t
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

    /*! The definition of the type GridDimensions_t */

    typedef struct GridDimensions_t GridDimensions_t ;



    /*! Sets all the fields to a default value (zero or \c NULL ).
     *
     * \param this the address of the grid dimensions structure to initialize
     */

    void init_grid_dimensions (GridDimensions_t *this) ;

/******************************************************************************/

    /*! \struct ChipDimensions_t
     *
     *  \brief Structure that collects the main dimensions of the IC
     */

    struct ChipDimensions_t
    {
        /*! The length of the IC (west-east) in \f$ \mu m \f$ */

        ChipDimension_t Length ;

        /*! The width of the IC (south-north) in \f$ \mu m \f$ */

        ChipDimension_t Width ;
    } ;

    /*! Definition of the type ChipDimension */

    typedef struct ChipDimensions_t ChipDimensions_t ;



    /*! Sets all the fields to a default value (zero or \c NULL ).
     *
     * \param this the address of the chip dimensions structure to initialize
     */

    void init_chip_dimensions (ChipDimensions_t *this) ;

/******************************************************************************/

    /*! \struct Dimensions_t
     *  \brief Collections of all the structures that are needed for the thermal simulation
     */

    struct Dimensions_t
    {
        /*! The dimensions of the thermal cells */

        CellDimensions_t Cell ;

        /*! The dimensions of the grid of thermal cells */

        GridDimensions_t Grid ;

        /*! The dimensions of the IC */

        ChipDimensions_t Chip ;

    } ;

    /*! Definition of the type Dimensions_t */

    typedef struct Dimensions_t Dimensions_t ;

/******************************************************************************/



    /*! Sets all the fields to a default value (zero or \c NULL ).
     *
     * \param this the address of the dimensions structure to initialize
     */

    void init_dimensions (Dimensions_t *this) ;



    /*! Allocates and inits memory for a structure of type Dimensions_t
     *
     * \return a pointer to the allocated memory.
     * \return \c NULL in case of error
     */

    Dimensions_t *calloc_dimensions (void) ;



    /*! Frees the memory space pointed to by \a this
     *
     * The pointer \a this must have been returned by a previous call
     * to \a calloc_dimensions . If \a this is \c NULL, no operation is performed.
     *
     * \param this the address to free
     */

    void free_dimensions (Dimensions_t *this) ;



    /*! Prints the dimensions as they look in the stack file
     *
     * \param this   the dimensions to print
     * \param stream the output stream (must be already open)
     * \param prefix a string to be printed as prefix at the beginning of each line
     */

    void print_dimensions (Dimensions_t *this, FILE *stream, String_t prefix) ;



    /*! Creates two text files containing the X and Y coordinates of the thermal cells
     *
     *  \param this the address of the dimensions structure
     */

    void print_axes (Dimensions_t *this) ;



    /*! Computes the number of connections between the thermal cells in the stack
     *
     *  Sets the content of the field Dimensions::NConnections
     *
     *  \param this          the address of the dimensions structure
     *  \param num_channels  number of channels (as stack element) in the stack
     *  \param channel_model the model of the channel used in the stack
     *  \param sink_model    the model of the heat sink used in the stack
     */

    void compute_number_of_connections
    (
        Dimensions_t   *this,
        Quantity_t      num_channels,
        ChannelModel_t  channel_model,
        HeatSinkModel_t sink_model
    ) ;



    /*! Returns the length of a thermal cell
     *
     *  The function prints a warning on stderr if \a column_index is
     *  oustside of its correct range of values, i.e. [0, ncolumns].
     *
     * \param this         the address of the structure containing all the dimensions
     * \param column_index the column index of the thermal cell
     *
     * \return the length of the thermal cell in position \a column_index
     * \return \c 0 if \a column_index is out of range
     */

    CellDimension_t get_cell_length (Dimensions_t *this, CellIndex_t column_index) ;



    /*! Returns the width of a thermal cell
     *
     *  The function prints a warning on stderr if \a row_index is
     *  oustside of its correct range of values, i.e. [0, nrows].
     *
     * \param this the address of the structure containing all the dimensions
     * \param row_index  the row index of the thermal cell
     *
     * \return the width of the thermal cell in position \a row_index
     * \return \c 0 if \a row_index is out of range
     */

    CellDimension_t get_cell_width (Dimensions_t *this, CellIndex_t row_index) ;



    /*! Returns the height of a thermal cell
     *
     *  The function prints a warning on stderr if \a layer_index is
     *  oustside of its correct range of values, i.e. [0, nlayers] or if
     *  the array of heights has not been created.
     *
     * \param this        the address of the structure containing all the dimensions
     * \param layer_index the layer index of the thermal cell
     *
     * \return the height of the thermal cell in position \a layer_index
     * \return \c 0 if \a layer_index is out of range or if the array of
     *         heights has not been created.
     */

    CellDimension_t get_cell_height (Dimensions_t *this, CellIndex_t layer_index) ;



    /*! Returns the X coordinate of the center of the cell at \a column_index
     *
     * \param this   the address of the structure containing all the dimensions
     * \param column_index the column index of the thermal cell
     *
     * \return the X coordinate of the center of thermal cell in position \a column_index
     */

    ChipDimension_t get_cell_center_x (Dimensions_t *this, CellIndex_t column_index) ;



    /*! Returns the Y coordinate of the center of the cell at \a row_index
     *
     * \param this      the address of the structure containing all the dimensions
     * \param row_index the row index of the thermal cell
     *
     * \return the Y coordinate of the center of thermal cell in position \a row_index
     */

    ChipDimension_t get_cell_center_y (Dimensions_t *this, CellIndex_t row_index) ;



    /*! Returns the X coordinate of the south-west corner of the cell at \a column_index
     *
     * \param this         the address of the structure containing all the dimensions
     * \param column_index the column index of the thermal cell
     *
     * \return the X coordinate of the south-west corner of thermal cell in position \a column_index
     */

    ChipDimension_t get_cell_location_x (Dimensions_t *this, CellIndex_t column_index) ;



    /*! Returns the Y coordinate of the south-west corner of the cell at \a row_index
     *
     * \param this       the address of the structure containing all the dimensions
     * \param row_index  the row index of the thermal cell
     *
     * \return the X coordinate of the south-west corner of thermal cell in position \a row_index
     */

    ChipDimension_t get_cell_location_y (Dimensions_t *this, CellIndex_t row_index) ;



    /*! Returns the number of layers in the 3d stack
     *
     * \param this the address of the structure containing all the dimensions
     *
     * \return the number of layers
     */

    CellIndex_t get_number_of_layers (Dimensions_t *this) ;



    /*! Returns the number of rows in the 3d stack
     *
     * \param this the address of the structure containing all the dimensions
     *
     * \return the number of rows
     */

    CellIndex_t get_number_of_rows (Dimensions_t *this) ;



    /*! Returns the number of columns in the 3d stack
     *
     * \param this the address of the structure containing all the dimensions
     *
     * \return the number of columns
     */

    CellIndex_t get_number_of_columns (Dimensions_t *this) ;



    /*! Returns the number of thermal cells in the 3d stack
     *
     * \param this the address of the structure containing all the dimensions
     *
     * \return the number of thermal cells
     */

    CellIndex_t get_number_of_cells (Dimensions_t *this) ;



    /*! Returns the number of connections between the thermal cells in the 3d stack
     *
     * \param this the address of the structure containing all the dimensions
     *
     * \return the number of connections
     */

    CellIndex_t get_number_of_connections (Dimensions_t *this) ;



    /*! Returns the number of thermal cells in a layer
     *
     * \param this the address of the structure containing all the dimensions
     *
     * \return the number of thermal cells in a layer
     */

    CellIndex_t get_layer_area (Dimensions_t *this) ;



    /*! Returns the index of a thermal cell within a layer
     *
     *  The index is computed as \f$ C \cdot r + c \f$, where \f$ C \f$
     *  is the number of columns in the 3d grid.
     *
     * \param this         the address of the structure containing all the dimensions
     * \param row_index    the row index \f$ r \f$ of the thermal cell
     * \param column_index the column index \f$ c \f$ of the thermal cell
     *
     * \return the index of a thermal cell \f$ (r, c) \f$ in a layer
     */

    CellIndex_t get_cell_offset_in_layer

        (Dimensions_t *this, CellIndex_t row_index, CellIndex_t column_index) ;



    /*! Returns the index of a thermal cell within the entire 3d stack
     *
     *  The index is computed as \f$ R \cdot C \cdot l + C \cdot r + c \f$,
     *  where \f$ R \f$ and \f$ C \f$ are, respectively, the number of rows and
     *  columns in the 3d grid
     *
     * \param this         the address of the structure containing all the dimensions
     * \param layer_index  the layer index \f$ l \f$ of the thermal cell
     * \param row_index    the row index \f$ r \f$ of the thermal cell
     * \param column_index the column index \f$ c \f$ of the thermal cell
     *
     * \return the index of a thermal cell \f$ (l, r, c) \f$ in the 3d stack
     */

    CellIndex_t get_cell_offset_in_stack

        (Dimensions_t *this,
         CellIndex_t layer_index, CellIndex_t row_index, CellIndex_t column_index) ;



    /*! Returns the length of the IC
     *
     * \param this the address of the structure containing all the dimensions
     *
     * \return the length of the IC
     */

    ChipDimension_t get_chip_length (Dimensions_t *this) ;



    /*! Returns the width of the IC
     *
     * \param this the address of the structure containing all the dimensions
     *
     * \return the width of the IC
     */

    ChipDimension_t get_chip_width (Dimensions_t *this) ;



    /*! Returns the area of the IC in \f$ \mu m^2 \f$
     *
     * \param this the address of the structure containing all the dimensions
     *
     * \return the area of the IC
     */

    ChipDimension_t get_chip_area (Dimensions_t *this) ;

/******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* _3DICE_DIMENSIONS_H_ */
