/******************************************************************************
 * This file is part of 3D-ICE, version 2.2.7 .                               *
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
 *          Federico Terraneo                                                 *
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

#ifndef _3DICE_HEAT_SINK_H_
#define _3DICE_HEAT_SINK_H_

/*! \file heat_sink.h */

#ifdef __cplusplus
extern "C"
{
#endif

/******************************************************************************/

#include <stdio.h> // For the file type FILE

#include "types.h"
#include "string_t.h"

#include "dimensions.h"
#include "material.h"
#include "analysis.h"

/******************************************************************************/

     /*! \struct HeatSink_t
     *
     *  \brief Structure used to store data about the heat dissipation
     *         through the top or bottom surfaces of the 2D/3D stack
     */

    struct HeatSink_t
    {
        /*! The type of the heastink */

        HeatSinkModel_t SinkModel ;

        /*! The heat transfert coefficient (from 3d stack to the environment).
            ignored by pluggable heatsink */

        AmbientHTC_t AmbientHTC ;

        /*! The temperarute of the environment in \f$ K \f$,
            ignored by pluggable heatsink */

        Temperature_t AmbientTemperature ;

        /*! Spreader dimensions, only for pluggable heatsink */

        SpreaderDimension_t SpreaderLength, SpreaderWidth, SpreaderHeight;
        
        /*! Spreader material, only for pluggable heatsink */

        Material_t SpreaderMaterial;

        /*! Plugin file name, only for pluggable heatsink */
        
        String_t Plugin;
        
        /*! The length of a spreader cell, only for pluggable heatsink */
        
        CellDimension_t CellLength;
        
        /*! The width of a spreader cell, only for pluggable heatsink */
        
        CellDimension_t CellWidth;
        
        /*! The number of rows of cells of the spreader, only for pluggable heatsink */

        CellIndex_t NRows;

        /*! The number of columns of cells of the spreader, only for pluggable heatsink */

        CellIndex_t NColumns;
        
        /*! The number of rows of cells of the spreader border, which is the
            part of the spreader not in contact with the chip,
            only for pluggable heatsink */
        
        CellIndex_t NumRowsBorder;
        
        /*! The number of columns of cells of the spreader border, which is the
            part of the spreader not in contact with the chip,
            only for pluggable heatsink */
        
        CellIndex_t NumColumnsBorder;
        
        /*! Current temperature returned from the pluggable heatsink callback */
        
        double *CurrentSinkHeatFlows;
        
        /*! The pluggable heatsink initialization callback */
        int (*PluggableHeatsinkInit)(unsigned int nrows, unsigned int ncols,
                                     double cellwidth,    double celllength,
                                     double initialtemperature,
                                     double spreaderconductance,
                                     double timestep,
                                     const char *args);
        
        /*! The pluggable heatsink callback */
        int (*PluggableHeatsink)(const double *spreadertemperatures,
                                       double *sinkheatflows);
     };

    /*! Definition of the type HeatSink_t */

    typedef struct HeatSink_t HeatSink_t ;



/******************************************************************************/



    /*! Inits the fields of the \a hsink structure with default values
     *
     * \param hsink the address of the structure to initalize
     */

    void heat_sink_init (HeatSink_t *hsink) ;



    /*! Copies the structure \a src into \a dst , as an assignement
     *
     * The function destroys the content of \a dst and then makes the copy
     *
     * \param dst the address of the left term sructure (destination)
     * \param src the address of the right term structure (source)
     */

    void heat_sink_copy (HeatSink_t *dst, HeatSink_t *src) ;



    /*! Destroys the content of the fields of the structure \a hsink
     *
     * The function releases any dynamic memory used by the structure and
     * resets its state calling \a heat_sink_init .
     *
     * \param hsink the address of the structure to destroy
     */

    void heat_sink_destroy (HeatSink_t *hsink) ;



    /*! Allocates memory for a structure of type HeatSink_t
     *
     * The content of the new structure is set to default values
     * calling \a heat_sink_init
     *
     * \return the pointer to the new structure
     * \return \c NULL if the memory allocation fails
     */

    HeatSink_t *heat_sink_calloc (void) ;



    /*! Allocates memory for a new copy of the structure \a hsink
     *
     * \param hsink the address of the structure to clone
     *
     * \return a pointer to a new structure
     * \return \c NULL if the memory allocation fails
     * \return \c NULL if the parameter \a hsink is \c NULL
     */

    HeatSink_t *heat_sink_clone (HeatSink_t *hsink) ;



    /*! Frees the memory space pointed by \a hsink
     *
     * The function destroys the structure \a hsink and then frees
     * its memory. The pointer \a hsink must have been returned by
     * a previous call to \a heat_sink_calloc or \a heat_sink_clone .
     *
     * If \a hsink is \c NULL, no operation is performed.
     *
     * \param hsink the pointer to free
     */

    void heat_sink_free (HeatSink_t *hsink) ;



    /*! Returns the conductance of the heat sink in a given position
     *
     * \param hsink        the address of the heat sink structure
     * \param dimensions   pointer to the structure storing the dimensions
     * \param row_index    the index of the row
     * \param column_index the index of the column
     *
     * \return the top conductance of the thermal cell in position
     *         (\a layer_index , \a row_index , \a column_index ).
     */

    Conductance_t heat_sink_conductance
    (
        HeatSink_t    *hsink,
        Dimensions_t  *dimensions,
        CellIndex_t    row_index,
        CellIndex_t    column_index
    ) ;



    /*! Prints the heat sink declaration as it looks in the stack file
     *
     * \param hsink the address of the structure to print
     * \param stream the output stream (must be already open)
     * \param prefix a string to be printed as prefix at the
     *               beginning of each line
     */

    void heat_sink_print (HeatSink_t *hsink, FILE *stream, String_t prefix) ;
    
    /*! Computes the spreader number of cells based on the chip dimensions
     *
     * \param hsink the heatsink
     * \param chip the chip dimensions
     */
    Error_t initialize_heat_spreader(HeatSink_t *hsink, Dimensions_t *chip);
    
    /*! Initializies the pluggable heatsink
     *
     * \param hsink the heatsink
     * \param analysis the analysis
     */
    Error_t initialize_pluggable_heatsink(HeatSink_t *hsink, Analysis_t *analysis);

    /*! \return the thermal capacity of a cell in the heat spreader */
    Capacity_t get_spreader_capacity(HeatSink_t *hsink);
    
    /*! \return the thermal conductance along the x axis of a heat spreader cell */
    Conductance_t get_spreader_conductance_east_west(HeatSink_t *hsink);
    
    /*! \return the thermal conductance along the y axis of a heat spreader cell */
    Conductance_t get_spreader_conductance_north_south(HeatSink_t *hsink);
    
    /*! \return the thermal conductance along the z axis of a heat spreader cell */
    Conductance_t get_spreader_conductance_top_bottom(HeatSink_t *hsink);

/******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* _3DICE_HEAT_SINK_H_ */
