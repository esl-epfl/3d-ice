/********************** ********************************************************
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

#include <stdlib.h>

#ifdef PRINT_THERMAL_CELLS
#include <stdio.h>
#endif

#include "thermal_cell.h"
#include "macros.h"

/******************************************************************************/

void init_thermal_cell (ThermalCell *thermal_cell)
{
    thermal_cell->Top      = 0.0 ;
    thermal_cell->Bottom   = 0.0 ;
    thermal_cell->North    = 0.0 ;
    thermal_cell->South    = 0.0 ;
    thermal_cell->East     = 0.0 ;
    thermal_cell->West     = 0.0 ;
    thermal_cell->Capacity = 0.0 ;
}

/******************************************************************************/

void reset_capacities (ThermalCell *thermal_cell, uint32_t ncells)
{
    uint32_t index = 0u ;

    while (index++ != ncells) thermal_cell++->Capacity = 0.0 ;
}

/******************************************************************************/

void fill_solid_cell_bottom
(
    ThermalCell *thermal_cell,
    Time_t       delta_time,
    double       cell_length,
    double       cell_width,
    double       cell_height,
    double       solid_tc,
    double       solid_vhc
)
{
    thermal_cell->North = thermal_cell->South
        = (solid_tc * cell_length * cell_height )
          / ( cell_width / 2.0);

    thermal_cell->East = thermal_cell->West
        = (solid_tc * cell_width * cell_height )
          / (cell_length / 2.0) ;

    thermal_cell->Bottom = 0.0 ;

    thermal_cell->Top = (solid_tc * cell_length * cell_width) / cell_height ;

    thermal_cell->Capacity
        = ((cell_length * cell_width * cell_height) * solid_vhc) / delta_time ;

#ifdef PRINT_THERMAL_CELLS
    fprintf (stderr,
        "| l %6.1f w %6.1f h %6.1f "  \
        "| N % .5e  S % .5e  E % .5e  W % .5e  T % .5e  B % .5e | C %.5e\n",
        cell_length, cell_width, cell_height,
        thermal_cell->North, thermal_cell->South, thermal_cell->East,
        thermal_cell->West, thermal_cell->Top, thermal_cell->Bottom,
        thermal_cell->Capacity) ;
#endif
}

/******************************************************************************/

void fill_solid_cell_central
(
    ThermalCell *thermal_cell,
    Time_t       delta_time,
    double       cell_length,
    double       cell_width,
    double       cell_height,
    double       solid_tc,
    double       solid_vhc
)
{
    thermal_cell->North = thermal_cell->South
        = (solid_tc * cell_length * cell_height )
          / ( cell_width / 2.0);

    thermal_cell->East = thermal_cell->West
        = (solid_tc * cell_width * cell_height )
          / (cell_length / 2.0) ;

    thermal_cell->Top = thermal_cell->Bottom
        = (solid_tc * cell_length * cell_width )
          / (cell_height / 2.0);

    thermal_cell->Capacity
        = ((cell_length * cell_width * cell_height) * solid_vhc) / delta_time ;

#ifdef PRINT_THERMAL_CELLS
    fprintf (stderr,
        "| l %6.1f w %6.1f h %6.1f "  \
        "| N % .5e  S % .5e  E % .5e  W % .5e  T % .5e  B % .5e | C %.5e\n",
        cell_length, cell_width, cell_height,
        thermal_cell->North, thermal_cell->South, thermal_cell->East,
        thermal_cell->West, thermal_cell->Top, thermal_cell->Bottom,
        thermal_cell->Capacity) ;
#endif
}

/******************************************************************************/

void fill_solid_cell_top
(
    ThermalCell *thermal_cell,
    Time_t       delta_time,
    double       cell_length,
    double       cell_width,
    double       cell_height,
    double       solid_tc,
    double       solid_vhc
)
{
    thermal_cell->North = thermal_cell->South
        = (solid_tc * cell_length * cell_height )
          / ( cell_width / 2.0);

    thermal_cell->East = thermal_cell->West
        = (solid_tc * cell_width * cell_height )
          / (cell_length / 2.0) ;

    thermal_cell->Bottom = (solid_tc * cell_length * cell_width) / cell_height ;

    thermal_cell->Top = 0.0 ;

    thermal_cell->Capacity
        = ((cell_length * cell_width * cell_height) * solid_vhc) / delta_time ;

#ifdef PRINT_THERMAL_CELLS
    fprintf (stderr,
        "| l %6.1f w %6.1f h %6.1f "  \
        "| N % .5e  S % .5e  E % .5e  W % .5e  T % .5e  B % .5e | C %.5e\n",
        cell_length, cell_width, cell_height,
        thermal_cell->North, thermal_cell->South, thermal_cell->East,
        thermal_cell->West, thermal_cell->Top, thermal_cell->Bottom,
        thermal_cell->Capacity) ;
#endif
}

/******************************************************************************/

void fill_solid_cell_conventional_heat_sink
(
    ThermalCell *thermal_cell,
    double       cell_length,
    double       cell_width,
    double       cell_height,
    double       solid_tc,
    double       ambient_htc
)
{
    thermal_cell->Bottom
        = (solid_tc * cell_length * cell_width)
          / (cell_height / 2.0) ;

    thermal_cell->Top
        = (
            2.0 * solid_tc * ambient_htc * cell_length * cell_width
          )
          /
          (
            cell_height * ambient_htc + 2.0 * solid_tc
          ) ;

#ifdef PRINT_THERMAL_CELLS
    fprintf (stderr,
        "| l %6.1f w %6.1f h %6.1f "  \
        "|                                     T % .5e  B % .5e |\n",
        cell_length, cell_width, cell_height,
        thermal_cell->Top, thermal_cell->Bottom) ;
#endif
}

/******************************************************************************/

void fill_liquid_cell_mc_4rm
(
    ThermalCell *thermal_cell,
    Time_t       delta_time,
    double       cell_length,
    double       cell_width,
    double       cell_height,
    uint32_t     nchannels,
    Coolant_t    coolant,
    double       coolant_fr
)
{
    double C = CCONV_MC_4RM (nchannels, coolant.VHC, coolant_fr) ;

    thermal_cell->North =  C ;
    thermal_cell->South = -C ;

    thermal_cell->East = thermal_cell->West
        = coolant.HTCSide * cell_width * cell_height ;

    thermal_cell->Top = coolant.HTCTop * cell_width * cell_length ;

    thermal_cell->Bottom = coolant.HTCBottom * cell_width * cell_length ;

    thermal_cell->Capacity
        = ((cell_length * cell_width * cell_height) * coolant.VHC) / delta_time ;

#ifdef PRINT_THERMAL_CELLS
    fprintf (stderr,
        "| l %6.1f w %6.1f h %6.1f "  \
        "| N % .5e  S % .5e  E % .5e  W % .5e  T % .5e  B % .5e | C %.5e\n",
        cell_length, cell_width, cell_height,
        thermal_cell->North, thermal_cell->South, thermal_cell->East,
        thermal_cell->West, thermal_cell->Top, thermal_cell->Bottom,
        thermal_cell->Capacity) ;
#endif
}

/******************************************************************************/

void fill_liquid_cell_mc_2rm
(
    ThermalCell *thermal_cell,
    Time_t       delta_time,
    double       cell_length,
    double       cell_width,
    double       cell_height,
    uint32_t     nchannels,
    double       channel_length,
    double       porosity,
    Coolant_t    coolant,
    double       coolant_fr
)
{
    double C = CCONV_MC_2RM (nchannels, coolant.VHC, coolant_fr,
                             porosity, cell_length, channel_length);

    thermal_cell->North =  C;
    thermal_cell->South = -C;

    thermal_cell->East = thermal_cell->West = 0.0 ;

    thermal_cell->Top    = coolant.HTCTop    * cell_width * cell_length ;

    thermal_cell->Bottom = coolant.HTCBottom * cell_width * cell_length ;

    thermal_cell->Capacity
        = ((cell_length * cell_width * cell_height) * coolant.VHC) * porosity
          / delta_time ;

#ifdef PRINT_THERMAL_CELLS
    fprintf (stderr,
        "| l %6.1f w %6.1f h %6.1f "  \
        "| N % .5e  S % .5e  E % .5e  W % .5e  T % .5e  B % .5e | C %.5e\n",
        cell_length, cell_width, cell_height,
        thermal_cell->North, thermal_cell->South, thermal_cell->East,
        thermal_cell->West, thermal_cell->Top, thermal_cell->Bottom,
        thermal_cell->Capacity) ;
#endif

}


/******************************************************************************/

void fill_liquid_cell_pf
(
    ThermalCell   *thermal_cell,
    Time_t         delta_time,
    double         cell_length,
    double         cell_width,
    double         cell_height,
    ChannelModel_t pin_distribution,
    double         porosity,
    Coolant_t      coolant,
    double         darcy_velocity
)
{
    double C = CCONV_PF (coolant.VHC, darcy_velocity, cell_length, cell_height);

    thermal_cell->North =   C;
    thermal_cell->South =  -C;

    thermal_cell->East = thermal_cell->West = 0.0 ;

    double eff_htc ;

    if (pin_distribution == TDICE_CHANNEL_MODEL_PF_INLINE)

        eff_htc = EFFECTIVE_HTC_PF_INLINE(darcy_velocity) ;

    else

        eff_htc = EFFECTIVE_HTC_PF_STAGGERED(darcy_velocity) ;

    thermal_cell->Top = thermal_cell->Bottom
        = eff_htc * cell_width * cell_length ;

    thermal_cell->Capacity
        = ((cell_length * cell_width * cell_height) * coolant.VHC) * porosity / delta_time ;

#ifdef PRINT_THERMAL_CELLS
    fprintf (stderr,
        "| l %6.1f w %6.1f h %6.1f "  \
        "| N % .5e  S % .5e  E % .5e  W % .5e  T % .5e  B % .5e | C %.5e\n",
        cell_length, cell_width, cell_height,
        thermal_cell->North, thermal_cell->South, thermal_cell->East,
        thermal_cell->West, thermal_cell->Top, thermal_cell->Bottom,
        thermal_cell->Capacity) ;
#endif
}

/******************************************************************************/

void fill_virtual_wall_cell_mc_2rm
(
    ThermalCell *thermal_cell,
    Time_t       delta_time,
    double       cell_length,
    double       cell_width,
    double       cell_height,
    double       porosity,
    double       solid_tc,
    double       solid_vhc
)
{
    thermal_cell->East = thermal_cell->West = 0.0 ;

    thermal_cell->Top = thermal_cell->Bottom
        = (solid_tc * cell_length * cell_width )
          / (cell_height / 2.0)
          * (1.0 - porosity) ;

    thermal_cell->North = thermal_cell->South
        = (solid_tc * cell_length * cell_height )
          / (cell_width / 2.0)
          * (1.0 - porosity) ;

    thermal_cell->Capacity
        = (solid_vhc * (cell_length * cell_width * cell_height) * (1 - porosity)) / delta_time ;

#ifdef PRINT_THERMAL_CELLS
    fprintf (stderr,
        "| l %6.1f w %6.1f h %6.1f "  \
        "| N % .5e  S % .5e  E % .5e  W % .5e  T % .5e  B % .5e | C %.5e\n",
        cell_length, cell_width, cell_height,
        thermal_cell->North, thermal_cell->South, thermal_cell->East,
        thermal_cell->West, thermal_cell->Top, thermal_cell->Bottom,
        thermal_cell->Capacity) ;
#endif
}


/******************************************************************************/

void fill_virtual_wall_cell_pf
(
    ThermalCell *thermal_cell,
    Time_t       delta_time,
    double       cell_length,
    double       cell_width,
    double       cell_height,
    double       porosity,
    double       solid_tc,
    double       solid_vhc
)
{
    thermal_cell->North = thermal_cell->South = 0.0 ;
    thermal_cell->East  = thermal_cell->West  = 0.0 ;

    thermal_cell->Top = thermal_cell->Bottom
        = (solid_tc * cell_length * cell_width )
          / (cell_height / 2.0)
          * (1.0 - porosity) ;

    thermal_cell->Capacity
        = (solid_vhc * (cell_length * cell_width * cell_height) * (1 - porosity)) / delta_time ;

#ifdef PRINT_THERMAL_CELLS
    fprintf (stderr,
        "| l %6.1f w %6.1f h %6.1f "  \
        "| N % .5e  S % .5e  E % .5e  W % .5e  T % .5e  B % .5e | C %.5e\n",
        cell_length, cell_width, cell_height,
        thermal_cell->North, thermal_cell->South, thermal_cell->East,
        thermal_cell->West, thermal_cell->Top, thermal_cell->Bottom,
        thermal_cell->Capacity) ;
#endif
}

/******************************************************************************/
