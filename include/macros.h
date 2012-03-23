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

#ifndef _3DICE_MACROS_H_
#define _3DICE_MACROS_H_

/*! \file macros.h */

#ifdef __cplusplus
extern "C"
{
#endif

#include <math.h>
#include <stdlib.h>

    /*! \def PI
     *
     * Returns the constant \f$ \Pi \f$
     */

#   define PI 3.14159265358979323846



    /*! \def MAX(a,b)
     *
     * Returns the maximum value between \a a and \a b
     */

#   define MAX(a,b)  (((a) > (b)) ? (a) : (b))



    /*! \def MIN(a,b)
     *
     * Returns the minimum value between \a a and \a b
     */

#   define MIN(a,b)  (((a) < (b)) ? (a) : (b))



    /*! \def PARALLEL(x,y)
     *
     * Returns the equivalent resistance of \a x and \a y
     * connected in parallel
     */

#   define PARALLEL(x,y) (((x) * (y)) / ((x) + (y)))

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

    /*! \def FREE_POINTER(function,pointer)
     *
     * Applies \a function to \a pointer to free memory and sets
     * \a pointer to \c NULL
     */

#   define FREE_POINTER(function, pointer) \
                                           \
        do { function (pointer) ; pointer = NULL ; } while (0)



    /*! \def FREE_LIST(type,list,free_function)
     *
     * Frees a linked \a list of objects having type \a type using the function
     * \a free_function . The list must be built throught a pointer called
     * \c Next
     */

#   define FREE_LIST(type, list, free_function)      \
                                                     \
        do                                           \
        {                                            \
            type* next = NULL ;                      \
            for ( ; list != NULL ; list = next)      \
            {                                        \
                next = list->Next ;                  \
                FREE_POINTER (free_function, list) ; \
            }                                        \
        } while (0)                                  \



    /*! \def JOIN_ELEMENTS(first,second)
     *
     * Joins two elements to make a double-linked list. The elements \a first
     * and \a second will be connected through the pointers \c Next and \c Prev
     */

#   define JOIN_ELEMENTS(first, second) \
                                        \
        do { first->Next = second ; second->Prev = first ; } while (0)



    /*! \def FOR_EVERY_ELEMENT_IN_LIST_NEXT(type,index,list)
     *
     * Crosses a linked \a list of objects having type \a type
     * following the pointer \a Next.
     */

#   define FOR_EVERY_ELEMENT_IN_LIST_NEXT(type, index, list)      \
                                                                  \
        type* index ;                                             \
        for (index = list ; index != NULL ; index = index->Next)



    /*! \def FOR_EVERY_ELEMENT_IN_LIST_PREV(type,index,list)
     *
     * Crosses a linked \a list of objects having type \a type
     * following the pointer \a Prev.
     */

#   define FOR_EVERY_ELEMENT_IN_LIST_PREV(type, index, list)      \
                                                                  \
        type* index ;                                             \
        for (index = list ; index != NULL ; index = index->Prev)

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

    /*! \def FIRST_ROW_INDEX
     *
     * Returns the index of the first row of thermal cells in a layer.
     * If \a R rows are there, the first will be row \a 0 while the
     * last will be row \a R-1.
     */

#   define FIRST_ROW_INDEX (0u)



    /*! \def LAST_ROW_INDEX(dimensions)
     *
     * Returns the index of the last row of thermal cells in a layer.
     * If \a R rows are there, the first will be row \a 0 while the
     * last will be row \a R-1.
     */

#   define LAST_ROW_INDEX(dimensions) \
                                      \
        (get_number_of_rows (dimensions) - 1)



    /*! \def IS_FIRST_ROW(row)
     *
     * Returns \c TRUE if \a row is the first row index, \c FALSE otherwise.
     */

#   define IS_FIRST_ROW(row) \
                             \
        ((row) == FIRST_ROW_INDEX ? true : false)



    /*! \def IS_LAST_ROW(row,dimensions)
     *
     * Returns \c TRUE if \a row is the last row index, \c FALSE otherwise.
     */

#   define IS_LAST_ROW(row, dimensions) \
                                        \
        ((row) == LAST_ROW_INDEX(dimensions) ? true : false)


    /*! \def FOR_EVERY_ROW(index,dimensions)
     *
     * Declares an \a index to access in sequence all the rows of
     * thermal cells in a layer.
     */

#   define FOR_EVERY_ROW(index, dimensions)         \
                                                    \
        uint32_t index ;                            \
        for (index = FIRST_ROW_INDEX ;              \
             index <= LAST_ROW_INDEX(dimensions) ;  \
             index++)

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

    /*! \def FIRST_COLUMN_INDEX
     *
     * Returns the index of the first column of thermal cells in a layer.
     * If \a C columns are there, the first will be column \a 0 while the
     * last will be column \a C-1
     */

#   define FIRST_COLUMN_INDEX (0u)



    /*! \def LAST_COLUMN_INDEX(dimensions)
     *
     * Returns the index of the last column of thermal cells in a layer.
     * If \a C columns are there, the first will be column \a 0 while the
     * last will be column \a C-1.
     */

#   define LAST_COLUMN_INDEX(dimensions) \
                                         \
        (get_number_of_columns (dimensions) - 1)



    /*! \def IS_FIRST_COLUMN(column)
     *
     * Returns \c TRUE if \a column is the first column index,
     * \c FALSE otherwise
     */

#   define IS_FIRST_COLUMN(column) \
                                   \
        ((column) == FIRST_COLUMN_INDEX ? true : false)



    /*! \def IS_LAST_COLUMN(column,dimensions)
     *
     * Returns \c TRUE if \a column is the last column index,
     * \c FALSE otherwise
     */

#   define IS_LAST_COLUMN(column, dimensions) \
                                              \
        ((column) == LAST_COLUMN_INDEX(dimensions) ? true : false)



    /*! \def FOR_EVERY_COLUMN(index,dimensions)
     *
     * Declares an \a index to access in sequence all the columns
     * of thermal cells in a layer
     */

#   define FOR_EVERY_COLUMN(index, dimensions)          \
                                                        \
        uint32_t index ;                                \
        for (index = FIRST_COLUMN_INDEX ;               \
             index <= LAST_COLUMN_INDEX(dimensions) ;   \
             index++)



    /*! \def IS_CHANNEL_COLUMN(channel_model,column)
     *
     * Returns \c TRUE if \a column is the index of a channel column. If
     * \a channel_model is 4RM, then odd indeces are channel columns.
     *  Otherwise, all indexes are channels (2RM microchannel or pin fins).
     */

#   define IS_CHANNEL_COLUMN(channel_model, column) \
                                                    \
        (channel_model == TDICE_CHANNEL_MODEL_MC_4RM ? (column) & 1 : true)

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

    /*! \def FIRST_LAYER_INDEX
     *
     * Returns the index of the first layer of thermal cells in a stack.
     * If \a L layer are there, the first will be layer \a 0 while the
     * last will be layer \a L-1
     */

#   define FIRST_LAYER_INDEX (0u)



    /*! \def LAST_LAYER_INDEX(dimensions)
     *
     * Returns the index of the last layer of thermal cells in a stack.
     * If \a L layer are there, the first will be layer \a 0 while the
     * last will be layer \a L-1*/

#   define LAST_LAYER_INDEX(dimensions) \
                                        \
        (get_number_of_layers (dimensions) - 1)



    /*! \def IS_FIRST_LAYER(layer)
     *
     * Returns \c TRUE if \a layer is the first layer index,
     * \c FALSE otherwise
     */

#   define IS_FIRST_LAYER(layer) \
                                 \
        ((layer) == FIRST_LAYER_INDEX)



    /*! \def IS_LAST_LAYER(layer)
     *
     * Returns \c TRUE if \a layer is the last layer index,
     * \c FALSE otherwise
     */

#   define IS_LAST_LAYER(layer, dimensions) \
                                            \
        ((layer) == LAST_LAYER_INDEX(dimensions))



    /*! \def FOR_EVERY_LAYER(index,dimensions)
     *
     * Declares an \a index to access in sequence all the layers
     * of thermal cells in a stack.
     */

#   define FOR_EVERY_LAYER(index, dimensions)        \
                                                     \
        uint32_t index ;                             \
        for (index = FIRST_LAYER_INDEX ;             \
             index <= LAST_LAYER_INDEX(dimensions) ; \
             index++)

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

    /*! \def FIRST_IC_ELEMENT_ROW_INDEX(ic_el)
     *
     * Returns the index of the first row of thermal cells in an
     * ic element.
     */

#   define FIRST_IC_ELEMENT_ROW_INDEX(ic_el) (ic_el->SW_Row)



    /*! \def LAST_IC_ELEMENT_ROW_INDEX(ic_el)
     *
     * Returns the index of the last row of thermal cells in an
     * ic element.
     */

#   define LAST_IC_ELEMENT_ROW_INDEX(ic_el)  (ic_el->NE_Row)



    /*! \def FOR_EVERY_IC_ELEMENT_ROW(index,ic_el)
     *
     * Declares an \a index to access in sequence all the rows
     * of thermal cells in an ic element.
     */

#   define FOR_EVERY_IC_ELEMENT_ROW(index, ic_el)        \
                                                         \
        uint32_t index ;                                 \
        for (index = FIRST_IC_ELEMENT_ROW_INDEX(ic_el) ; \
             index <= LAST_IC_ELEMENT_ROW_INDEX(ic_el) ; \
             index++)



    /*! \def FIRST_IC_ELEMENT_COLUMN_INDEX(ic_el)
     *
     * Returns the index of the first column of thermal cells in an
     * ic element.
     */

#   define FIRST_IC_ELEMENT_COLUMN_INDEX(ic_el) (ic_el->SW_Column)



    /*! \def LAST_IC_ELEMENT_COLUMN_INDEX(ic_el)
     *
     * Returns the index of the last columns of thermal cells in an
     * ic element.
     */

#   define LAST_IC_ELEMENT_COLUMN_INDEX(ic_el)  (ic_el->NE_Column)



    /*! \def FOR_EVERY_IC_ELEMENT_COLUMN(index,ic_el)
     *
     * Declares an \a index to access in sequence all the columns
     * of thermal cells in an ic element.
     */

#   define FOR_EVERY_IC_ELEMENT_COLUMN(index, ic_el)        \
                                                            \
        uint32_t index ;                                    \
        for (index = FIRST_IC_ELEMENT_COLUMN_INDEX(ic_el) ; \
             index <= LAST_IC_ELEMENT_COLUMN_INDEX(ic_el) ; \
             index++)

/******************************************************************************/

    /*! \def CCONV_MC_4RM(nchannels,vhc,fr)
     *
     * Returns the \c C convective term for the 4RM model of microchannels
     *
     * FlowRatePerChannel [ um3 / sec ] = FlowRate             [ um3 / sec ]
     *                                    / \#ChannelColumns   [ ]
     *
     * CoolantVelocity     [ m / sec ]  = FlowRatePerChannel   [ um3 / sec ]
     *                                      * ( 1 / Ay )         [   1 / um2 ]
     *
     * Cconv         [ J / ( K . sec) ]  = CoolantVHC          [ J / ( um3 . K ) ]
     *                                       * CoolantVelocity   [ m / sec ]
     *                                       * ( Ay / 2 )        [ um2 ]
     * [ J / ( K . sec) ] = [ W / K ]
     *
     * CoolantVelocity = FlowRate / (\#ChannelColumns * Ay )
     *
     * Cconv           = (CoolantVHC * FlowRate) / (\#ChannelColumns * 2)
     */

#   define CCONV_MC_4RM(nchannels, vhc, fr) \
                                            \
        ((double) ((vhc * fr) / ((double) (nchannels * 2.0))))



    /*! \def CCONV_MC_2RM(nchannels,vhc,fr,porosity,cell_l,channel_l)
     *
     * Returns the \c C convective term for the 2RM model of microchannels
     *
     *
     * FlowRatePerChannel [ um3 / sec ] = FlowRate                            [ um3 / sec ]
     *                                    / \#Channels                        [ ]
     *
     * CoolantVelocity      [ m / sec ] = FlowRatePerChannel                  [ um3 / sec ]
     *                                    / (CavityHeight * ChannelLength)    [ um2 ]
     *
     * Cconv         [ J / ( K . sec) ] = CoolantVHC                          [ J / ( um3 . K ) ]
     *                                      * CoolantVelocity                   [ m / sec ]
     *                                      * ( Cavityheight * CellLength / 2 ) [ um2 ]
     *                                      * Porosity                          [ ]
     * [ J / ( K . sec) ] = [ W / K ]
     *
     * CoolantVelocity = FlowRate / (\#Channels * CavityHeight * ChannelLength)
     *
     * Cconv           = (CoolantVHC * FlowRate * Porosity * CellLength)
     *                   / (\#Channels * 2 * ChannelLength)
     */

#   define CCONV_MC_2RM(nchannels, vhc, fr, porosity, cell_l, channel_l)     \
                                                                             \
        ((double) (  (vhc * fr * porosity * cell_l)                          \
                   / ( (double) nchannels * 2.0 * channel_l) ))



    /*! \def CCONV_PF(vhc,darcy_velocity,cell_l,cavity_h)
     *
     * Returns the \c C convective term for the 2RM model of pin fins
     *
     * Cconv     [ J / ( K . sec) ] = CoolantVHC                             [ J / ( um3 . K ) ]
     *                                  * DarcyVelosity                        [ m / sec ]
     *                                  * ( CavityHeight * CellLength / 2 )    [ um2 ]
     *                                  * Porosity                             [ ]
     */

#   define CCONV_PF(vhc, darcy_velocity, cell_l, cavity_h) \
                                                           \
        ((double) ((vhc * darcy_velocity * cell_l * cavity_h) / 2.0))

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/



    /*! \def EFFECTIVE_HTC_PF_INLINE(darcy_velocity)
     *
     * Returns the Effective HTC for inline pin fins with 2RM model
     *
     * H_eff_inline = ((2.527E-5 / (DarcyVelosity + 1.35)^0.64 )+1.533E-6)^(-1) * 1E-12
     */

#   define EFFECTIVE_HTC_PF_INLINE(darcy_velocity)                             \
                                                                               \
        ((double) (  1e-12                                                     \
                   / (  2.527e-05                                              \
                      / pow((darcy_velocity/1e+06 + 1.35), 0.64) + 1.533e-06)))



    /*! \def EFFECTIVE_HTC_PF_STAGGERED(darcy_velocity)
     *
     * Returns the Effective HTC for staggered pin fins with 2RM model
     *
     * H_eff_stag = ((2.527E-5 / (DarcyVelosity + 1.35)^1.52 )+1.533E-6)^(-1) * 1E-12
     */

#   define EFFECTIVE_HTC_PF_STAGGERED(darcy_velocity)                          \
                                                                               \
        ((double) (  1e-12                                                     \
                   / (  2.527e-05                                              \
                      / pow((darcy_velocity/1e+06 + 1.35), 1.52) + 1.533e-06)))



    /*! \def FLOW_RATE_FROM_MLMIN_TO_UM3SEC(fr)
     *
     *  Converts the flow rate from \f$ \frac{ml}{min} \f$ to
     *  \f$ \frac{\mu m^3}{sec} \f$
     */

#   define FLOW_RATE_FROM_MLMIN_TO_UM3SEC(fr) (( fr * 1e+12 ) / 60.0)



    /*! \def FLOW_RATE_FROM_UM3SEC_TO_MLMIN(fr)
     *
     *  Converts the flow rate from \f$ \frac{\mu m^3}{sec} \f$ to
     *  \f$ \frac{ml}{min} \f$
     */

#   define FLOW_RATE_FROM_UM3SEC_TO_MLMIN(fr) (( fr * 60.0 ) / 1e+12)



    /*! \def POROSITY(diameter,pitch)
     *
     *  Returns the porosity of a 2RM pin fins
     */

#   define POROSITY(diameter, pitch) \
                                     \
        (1.0 - (PI * diameter * diameter / 4.0) / (pitch * pitch))



    /*! \def DIAMETER(porosity, pitch)
     *
     *  Returns the diameter of a 2RM pin fins
     */

#   define DIAMETER(porosity, pitch) \
                                     \
        (sqrt (( (1.0 - porosity) * pitch*pitch * 4.0 ) / PI ))

/******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* _3DICE_MACROS_H_ */
