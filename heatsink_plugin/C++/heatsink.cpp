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
 *                             Copyright (C) 2017                             *
 *   Embedded Systems Laboratory - Ecole Polytechnique Federale de Lausanne   *
 *                            All Rights Reserved.                            *
 *                                                                            *
 * Authors: Federico Terraneo                                                 *
 *          Arvind Sridhar                                                    *
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

// This is just a template, write your heatsink code here
// by implementing the HeatSink class

#include <stdexcept>
#include <iostream>
#include "heatsink.h"

using namespace std;

//
// class HeatSink
//

HeatSink::HeatSink(unsigned int nRows, unsigned int nCols,
                   double cellWidth,   double cellLength,
                   double initialTemperature,
                   double spreaderConductance,
                   double timeStep)
{
    /*
     * - nRows, nCols are the number of rows/columns of thermal cells of the
     *   spreader, while cellWidth and cellLength are their dimensions.
     *
     * - initialTemperature is the initial spreader temperature.
     *
     * - spreaderConductance is the conductance between the center of the
     *   spreader cell to its top face. NOTE: in most simulation cases you have
     *   to add the conductance between the top face of the spreader and the
     *   center of the bottommost cell of the heat sink.
     *
     * - timeStep is the simuation time step.
     */
    
    cout.setf(ios::fixed);
    cout.precision(3);
    cout<<endl;
    #define P(x) cout<<#x<<": "<<x<<endl
    P(nRows);
    P(nCols);
    P(cellWidth);
    P(cellLength);
    P(initialTemperature);
    P(spreaderConductance);
    P(timeStep);
    #undef P
    
    //Total ambient conductance in W/K, arbitrary value
    const double totalConductance=1;
        
    //Divide the ambient conductance evenly among the cells
    const double ambientConductance=totalConductance/nRows*nCols;
    
    this->conductance=parallel(spreaderConductance,ambientConductance);
    this->ambientTemperature=initialTemperature;
}

void HeatSink::simulateStep(const CellMatrix spreaderTemperatures,
                                  CellMatrix heatFlow)
{
    /*
     * spreaderTemperatures are the temperatures in Kelvin of the layer of cells
     * of the spreader computed by 3D-ICE.
     *
     * heatFlow is the heat flow in Watt between each cell of the heatsink that
     * are in direct contact with those of the spreader. You have to compute
     * those.
     * 
     * Last, but not least, if you encounter an error, you can signal it to
     * 3D-ICE by throwing a C++ exception.
     */
    
    // This example function simulates the absence of a heatsink, where
    // the spreader exchanges heat convectively to the ambient
    // In the absence of a heatsink, the cell above the spreader is the ambient
    for(unsigned int r=0;r<spreaderTemperatures.rows();r++)
        for(unsigned int c=0;c<spreaderTemperatures.cols();c++)
            heatFlow.at(r,c)=
                conductance*(spreaderTemperatures.at(r,c)-ambientTemperature);
}

HeatSink::~HeatSink() {}
