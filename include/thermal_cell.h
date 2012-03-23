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

#ifndef _3DICE_THERMALCELL_H_
#define _3DICE_THERMALCELL_H_

/*! \file thermal_cell.h */

#ifdef __cplusplus
extern "C"
{
#endif

/******************************************************************************/

#include "types.h"

/******************************************************************************/

    /*! \struct ThermalCell
     *
     *  \brief Structure used to store data about a single thermal cell
     *
     *  Each thermal cell has a length \a l, width \a w and height \a h, as
     *  shown in the image, and it is modeled as a node containing six
     *  resistances representing the conduction of heat in all the six directions
     *  (top, bottom, north, south, east and west), and a capacitance representing
     *  the heat storage inside the cell.
     *
     *  The conductance of the six resistors is stored as \f$ \frac{W}{K}\f$
     *  while the capacitance is stored as FIXME.
     *
     *  \image html fig_unit_cell.bmp "RC representation of a thermal cell"
     *
     */

    struct ThermalCell
    {
        /*! The conductance of the resistor in the \a Top direction */

        Conductance_t Top ;

        /*! The conductance of the resistor in the \a Bottom direction */

        Conductance_t Bottom ;

        /*! The conductance of the resistor in the \a North direction */

        Conductance_t North ;

        /*! The conductance of the resistor in the \a South direction */

        Conductance_t South ;

        /*! The conductance of the resistor in the \a East direction */

        Conductance_t East ;

        /*! The conductance of the resistor in the \a West direction */

        Conductance_t West ;

        /*! The capacity of the thermal cell */

        Capacity_t Capacity ;

    } ;

    /*! Definition of the type ThermalCell */

    typedef struct ThermalCell ThermalCell ;

/******************************************************************************/

    /*! Init a solid thermal cell using default values
     *
     * \param thermal_cell the address of the thermal cell to initialize
     */

    void init_thermal_cell (ThermalCell *thermal_cell) ;

    /*! Set to zero the capacity in a vector of thermal cells
     *
     * \param thermal_cell pointer to the first thermal cell in the vector
     * \param ncells number of thermal cells in the vector
     */

    void reset_capacities (ThermalCell *thermal_cell, CellIndex_t ncells) ;


    /*! Fill a solid thermal cell that belongs to the bottom-most layer of a stack
     *
     * \param thermal_cell the address of the thermal cell to fill
     * \param delta_time   the time step of the simulation
     * \param cell_length  the length \a l of the thermal cell
     * \param cell_width   the width \a w of the thermal cell
     * \param cell_height  the height \a h of the thermal cell
     * \param solid_tc     the thermal conductivity of the material
     * \param solid_vhc    the volumetric heat capacity of the material
     */

    void fill_solid_cell_bottom
    (
        ThermalCell     *thermal_cell,

        Time_t           delta_time,

        CellDimension_t  cell_length,
        CellDimension_t  cell_width,
        CellDimension_t  cell_height,

        SolidTC_t        solid_tc,
        SolidVHC_t       solid_vhc
    ) ;



    /*! Fill a solid thermal cell that belongs to a central layer in a stack
     *
     * \param thermal_cell the address of the thermal cell to fill
     * \param delta_time   the time step of the simulation
     * \param cell_length  the length \a l of the thermal cell
     * \param cell_width   the width \a w of the thermal cell
     * \param cell_height  the height \a h of the thermal cell
     * \param solid_tc     the thermal conductivity of the material
     * \param solid_vhc    the volumetric heat capacity of the material
     */

    void fill_solid_cell_central
    (
        ThermalCell     *thermal_cell,

        Time_t           delta_time,

        CellDimension_t  cell_length,
        CellDimension_t  cell_width,
        CellDimension_t  cell_height,

        SolidTC_t        solid_tc,
        SolidVHC_t       solid_vhc
    ) ;



    /*! Fill a solid thermal cell that belongs to the top-most layer of a stack
     *
     * \param thermal_cell the address of the thermal cell to fill
     * \param delta_time   the time step of the simulation
     * \param cell_length  the length \a l of the thermal cell
     * \param cell_width   the width \a w of the thermal cell
     * \param cell_height  the height \a h of the thermal cell
     * \param solid_tc     the thermal conductivity of the material
     * \param solid_vhc    the volumetric heat capacity of the material
     */

    void fill_solid_cell_top
    (
        ThermalCell     *thermal_cell,

        Time_t           delta_time,

        CellDimension_t  cell_length,
        CellDimension_t  cell_width,
        CellDimension_t  cell_height,

        SolidTC_t        solid_tc,
        SolidVHC_t       solid_vhc
    ) ;



    /*! Overwrites the content a solid thermal cell when the heat sink is used
     *
     * \param thermal_cell the address of the thermal cell to fill
     * \param cell_length  the length \a l of the thermal cell
     * \param cell_width   the width \a w of the thermal cell
     * \param cell_height  the height \a h of the thermal cell
     * \param solid_tc     the thermal conductivity of the material
     * \param ambient_htc  the heat transfer coefficient of the air (CHECKME)
     */

    void fill_solid_cell_conventional_heat_sink
    (
        ThermalCell     *thermal_cell,

        CellDimension_t  cell_length,
        CellDimension_t  cell_width,
        CellDimension_t  cell_height,

        SolidTC_t        solid_tc,
        AmbientHTC_t     ambient_htc
    ) ;



    /*! Fill a liquid thermal cell as if it belongs to a microchannel modeled with 4 resistances
     *
     * \param thermal_cell the address of the thermal cell to fill
     * \param delta_time   the time step of the simulation
     * \param cell_length  the length \a l of the thermal cell
     * \param cell_width   the width \a w of the thermal cell
     * \param cell_height  the height \a h of the thermal cell
     * \param nchannels    the number of microchannels in the channel layer
     * \param coolant      the thermal properties of the coolant
     */

    void fill_liquid_cell_mc_4rm
    (
        ThermalCell      *thermal_cell,

        Time_t           delta_time,

        CellDimension_t  cell_length,
        CellDimension_t  cell_width,
        CellDimension_t  cell_height,

        Quantity_t       nchannels,
        Coolant_t        coolant
    ) ;



    /*! Fill a liquid thermal cell as if it belongs to a microchannel modeled with 2 resistances
     *
     * \param thermal_cell   the address of the thermal cell to fill
     * \param delta_time     the time step of the simulation
     * \param cell_length    the length \a l of the thermal cell
     * \param cell_width     the width \a w of the thermal cell
     * \param cell_height    the height \a h of the thermal cell
     * \param nchannels      the number of microchannels in the channel layer
     * \param channel_length the length of the channel
     * \param porosity       FIXME
     * \param coolant        the thermal properties of the coolant
     */

    void fill_liquid_cell_mc_2rm
    (
        ThermalCell        *thermal_cell,

        Time_t              delta_time,

        CellDimension_t     cell_length,
        CellDimension_t     cell_width,
        CellDimension_t     cell_height,

        Quantity_t          nchannels,
        ChannelDimension_t  channel_length,
        ChannelDimension_t  porosity,
        Coolant_t           coolant
    ) ;



    /*! Fill a liquid thermal cell as if it belongs to a channel with pin fins
     *
     * \param thermal_cell     the address of the thermal cell to fill
     * \param delta_time       the time step of the simulation
     * \param cell_length      the length \a l of the thermal cell
     * \param cell_width       the width \a w of the thermal cell
     * \param cell_height      the height \a h of the thermal cell
     * \param pin_distribution the distribution of the pin fins
     * \param porosity         FIXME
     * \param coolant          the thermal properties of the coolant
     */

    void fill_liquid_cell_pf
    (
        ThermalCell       *thermal_cell,

        Time_t             delta_time,

        CellDimension_t    cell_length,
        CellDimension_t    cell_width,
        CellDimension_t    cell_height,

        ChannelModel_t     pin_distribution,
        ChannelDimension_t porosity,
        Coolant_t          coolant
    ) ;



    /*! Fill a solid cell that belongs to the virtual wall layer of a microchannel modeled with 2 resistances
     *
     * \param thermal_cell the address of the thermal cell to fill
     * \param delta_time   the time step of the simulation
     * \param cell_length  the length \a l of the thermal cell
     * \param cell_width   the width \a w of the thermal cell
     * \param cell_height  the height \a h of the thermal cell
     * \param porosity     FIXME
     * \param solid_tc     the thermal conductivity of the material composing the walls
     * \param solid_vhc    the volumetric heat capacity of the material composing the walls
     */

    void fill_virtual_wall_cell_mc_2rm
    (
        ThermalCell       *thermal_cell,

        Time_t             delta_time,

        CellDimension_t    cell_length,
        CellDimension_t    cell_width,
        CellDimension_t    cell_height,

        ChannelDimension_t porosity,
        SolidTC_t          solid_tc,
        SolidVHC_t         solid_vhc
    ) ;



    /*! Fill a solid cell that belongs to the virtual wall layer of a channel with pin fins
     *
     * \param thermal_cell the address of the thermal cell to fill
     * \param delta_time   the time step of the simulation
     * \param cell_length  the length \a l of the thermal cell
     * \param cell_width   the width \a w of the thermal cell
     * \param cell_height  the height \a h of the thermal cell
     * \param porosity     FIXME
     * \param solid_tc     the thermal conductivity of the material composing the walls
     * \param solid_vhc    the volumetric heat capacity of the material composing the walls
     */

    void fill_virtual_wall_cell_pf
    (
        ThermalCell       *thermal_cell,

        Time_t             delta_time,

        CellDimension_t    cell_length,
        CellDimension_t    cell_width,
        CellDimension_t    cell_height,

        ChannelDimension_t porosity,
        SolidTC_t          solid_tc,
        SolidVHC_t         solid_vhc
    ) ;

/******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* _3DICE_THERMALCELL_H_ */
