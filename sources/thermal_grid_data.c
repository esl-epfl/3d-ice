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

#include <stdlib.h>

#include "thermal_grid_data.h"
#include "macros.h"

/******************************************************************************/

void init_thermal_grid_data (ThermalGridData* thermalgriddata)
{
  thermalgriddata->LayersData  = NULL ;
  thermalgriddata->CoolantVHC  = COOLANTVHC_I ;
  thermalgriddata->CoolantHTCs = COOLANTHTCS_I ;
  thermalgriddata->CoolantFR   = COOLANTFR_I ;
  thermalgriddata->DeltaTime   = TIME_I ;
}

/******************************************************************************/

int alloc_thermal_grid_data
(
  ThermalGridData* thermalgriddata,
  Quantity_t       quantity,
  CoolantVHC_t     coolant_vhc,
  CoolantHTCs_t    coolant_htcs,
  CoolantFR_t      coolant_fr,
  AmbientHTC_t     ambient_htc,
  Time_t           delta_time
)
{
  PerLayerData* layersdata

    = (PerLayerData*) malloc ( sizeof(PerLayerData) * quantity) ;

  if (layersdata == NULL) return 1 ;

  while (--quantity >= 0)
  {
    layersdata[quantity].Type                = TDICE_LAYER_NONE ;
    layersdata[quantity].ThermalConductivity = SOLIDTC_I ;
    layersdata[quantity].SolidVHC            = SOLIDVHC_I ;
    layersdata[quantity].Height              = CELLDIMENSION_I ;
  }

  thermalgriddata->LayersData  = layersdata ;
  thermalgriddata->CoolantVHC  = coolant_vhc ;
  thermalgriddata->CoolantHTCs = coolant_htcs ;
  thermalgriddata->CoolantFR   = coolant_fr ;
  thermalgriddata->AmbientHTC  = ambient_htc ;
  thermalgriddata->DeltaTime   = delta_time ;

#ifdef PRINT_THERMAL_GRID_DATA
  fprintf (stderr, "CoolantVHC    %e\n", thermalgriddata->CoolantVHC) ;
  fprintf (stderr, "CoolantHTCs t %e\n", thermalgriddata->CoolantHTCs.Top) ;
  fprintf (stderr, "            b %e\n", thermalgriddata->CoolantHTCs.Bottom) ;
  fprintf (stderr, "            s %e\n", thermalgriddata->CoolantHTCs.Side) ;
  fprintf (stderr, "CoolantFR     %e\n", thermalgriddata->CoolantFR) ;
  fprintf (stderr, "ambientHTC    %e\n", thermalgriddata->AmbientHTC) ;
  fprintf (stderr, "DeltaTime     %.4f\n", thermalgriddata->DeltaTime) ;
#endif

  return 0 ;
}

/******************************************************************************/

void free_thermal_grid_data (ThermalGridData* thermalgriddata)
{
  free (thermalgriddata->LayersData) ;
}

/******************************************************************************/

void fill_thermal_grid_data
(
  ThermalGridData* thermalgriddata,
  GridDimension_t  layer_index,
  Layer_t          type,
  SolidTC_t        thermal_conductivity,
  SolidVHC_t       solid_vhc,
  CellDimension_t  height
)
{
  PerLayerData* layersdata = thermalgriddata->LayersData + layer_index ;

  layersdata->Type                = type ;
  layersdata->ThermalConductivity = thermal_conductivity ;
  layersdata->SolidVHC            = solid_vhc ;
  layersdata->Height              = height ;

#ifdef PRINT_THERMAL_GRID_DATA
  fprintf (stderr,
    "l #%d Type                <- %d\n",
    layer_index, layersdata->Type) ;
  fprintf (stderr,
    "l #%d ThermalConductivity <- %e\n",
    layer_index, layersdata->ThermalConductivity) ;
  fprintf (stderr,
    "l #%d SolidVHC            <- %e\n",
    layer_index, layersdata->SolidVHC) ;
  fprintf (stderr,
    "l #%d Height              <- %.2f\n",
    layer_index, layersdata->Height) ;
#endif

}

/******************************************************************************/

Capacity_t get_capacity
(
  ThermalGridData* thermalgriddata,
  Dimensions*      dimensions,
  GridDimension_t  layer_index,
  GridDimension_t  row_index,
  GridDimension_t  column_index
)
{
  (void) row_index ;

  PerLayerData* layerdata = thermalgriddata->LayersData + layer_index ;

  /* Liquid column */

  if (   layerdata->Type == TDICE_LAYER_CHANNEL
      && IS_CHANNEL_COLUMN(column_index) )

  return ( ( get_cell_length(dimensions, column_index)
             * get_cell_width(dimensions)
             * layerdata->Height
           )
           * thermalgriddata->CoolantVHC
         ) / thermalgriddata->DeltaTime ;

  /* Solid layer or channel wall column */

  return ( ( get_cell_length(dimensions, column_index)
             * get_cell_width(dimensions)
             * layerdata->Height
           )
           * layerdata->SolidVHC
         ) / thermalgriddata->DeltaTime ;
}

/******************************************************************************/

Conductance_t get_conductance
(
  ThermalGridData* thermalgriddata,
  Dimensions*      dimensions,
  GridDimension_t  layer_index,
  GridDimension_t  row_index,
  GridDimension_t  column_index,
  Conductance_d    direction
)
{
  (void) row_index ;

  PerLayerData* layerdata = thermalgriddata->LayersData + layer_index ;

  /* For a liquid cell ... */

  if (   layerdata->Type == TDICE_LAYER_CHANNEL
      && IS_CHANNEL_COLUMN(column_index) )
  {
    switch (direction)
    {
      case TDICE_CONDUCTANCE_NORTH :

        return   CCONV (get_number_of_columns (dimensions),
                        thermalgriddata->CoolantVHC,
                        thermalgriddata->CoolantFR) ;

      case TDICE_CONDUCTANCE_SOUTH :

        return - CCONV (get_number_of_columns (dimensions),
                        thermalgriddata->CoolantVHC,
                        thermalgriddata->CoolantFR) ;

      case TDICE_CONDUCTANCE_EAST :
      case TDICE_CONDUCTANCE_WEST :

        return thermalgriddata->CoolantHTCs.Side
               * get_cell_width (dimensions)
               * layerdata->Height ;

      case TDICE_CONDUCTANCE_TOP :

        return thermalgriddata->CoolantHTCs.Top
               * get_cell_width (dimensions)
               * get_cell_length (dimensions, column_index) ;

      case TDICE_CONDUCTANCE_BOTTOM :

        return thermalgriddata->CoolantHTCs.Bottom
               * get_cell_width (dimensions)
               * get_cell_length (dimensions, column_index) ;

      default :

        fprintf(stderr, "Unknown direction %d\n", direction) ;

        return (Conductance_t) 0 ;
    }
  }

  /* For all the solid cell ... */

  switch (direction)
  {
    case TDICE_CONDUCTANCE_NORTH :
    case TDICE_CONDUCTANCE_SOUTH :

      return ( layerdata->ThermalConductivity
               * get_cell_length (dimensions, column_index)
               * layerdata->Height )
             / ( get_cell_width (dimensions)
                 / (CellDimension_t) 2 ) ;

    case TDICE_CONDUCTANCE_EAST :
    case TDICE_CONDUCTANCE_WEST :

      return ( layerdata->ThermalConductivity
               * get_cell_width (dimensions)
               * layerdata->Height
             ) / ( get_cell_length (dimensions, column_index)
                   / (CellDimension_t) 2 ) ;

    case TDICE_CONDUCTANCE_TOP :

      if (IS_FIRST_LAYER(layer_index))
      {
        return ( layerdata->ThermalConductivity
                 * get_cell_length (dimensions, column_index)
                 * get_cell_width (dimensions)
               ) / layerdata->Height ;
      }
      else if (IS_LAST_LAYER(layer_index, dimensions))
      {
        if (layerdata->Type == TDICE_LAYER_CHS)
        {
          return ( (Conductance_t) 2
                   * layerdata->ThermalConductivity
                   * thermalgriddata->AmbientHTC
                   * get_cell_length (dimensions, column_index)
                   * get_cell_width (dimensions)
                 ) / ( layerdata->Height * thermalgriddata->AmbientHTC
                       +
                       (Conductance_t) 2 * layerdata->ThermalConductivity ) ;
        }
        else
        {
          return (Conductance_t) 0 ;
        }
      }
      else
      {
        return ( layerdata->ThermalConductivity
                 * get_cell_length (dimensions, column_index)
                 * get_cell_width (dimensions)
               ) / ( layerdata->Height
                     / (CellDimension_t) 2 ) ;
      }

    case TDICE_CONDUCTANCE_BOTTOM :

      if (IS_FIRST_LAYER(layer_index))
      {
        return (Conductance_t) 0 ;
      }
      else if (   IS_LAST_LAYER(layer_index, dimensions)
               && layerdata->Type != TDICE_LAYER_CHS)
      {
        return ( layerdata->ThermalConductivity
                 * get_cell_length (dimensions, column_index)
                 * get_cell_width (dimensions)
               ) / layerdata->Height ;
      }
      else
      {
        return ( layerdata->ThermalConductivity
                 * get_cell_length (dimensions, column_index)
                 * get_cell_width (dimensions)
               ) / ( layerdata->Height
                     / (CellDimension_t) 2) ;
      }

    default :

      fprintf(stderr, "Unknown direction %d\n", direction) ;

      return (Conductance_t) 0 ;
  }
}

/******************************************************************************/
