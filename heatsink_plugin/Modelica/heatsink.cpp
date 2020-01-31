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
    // This example function simply prints the given parameters
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

    this->ambientTemperature=initialTemperature;
    this->spreaderConductance=spreaderConductance;
}

Conductances HeatSink::simulateStep(const CellMatrix spreaderTemperatures,
                                          CellMatrix sinkTemperatures,
                                          CellMatrix thermalConductances)
{
    /*
     * spreaderTemperatures are the temperatures in K of the layer of cells of
     * the spreader computed by 3D-ICE. This is a readonly parameter.
     * 
     * sinkTemperatures are the temperatures in K of the layer of cells of the
     * heatsink that are in direct contact with those of the spreader.
     * You have to compute those. This is an inout parameter, as the previous
     * values are preserved across function calls.
     * 
     * thermalConductances are the thermal conductances in W/K between each
     * cell of the spreader and the one of the sink directly above. You have to
     * compute those at least once the first time this function is called.
     * The way to compute them is by paralleling the conductance passed to the
     * constructor (which is the conductance from the center of the spreader
     * cell to its top face) with a conductance that you have to compute, which
     * is the conductance from the bottom face of the sink to the center of the
     * sink cell. Although the spreader conductances coming from 3D-ICE are
     * uniform, the sink can have different conductances for each cell, that's
     * why it's a matrix. Also, you can update the conductances values during
     * the simulation to simulate e.g: two phase materials. Just remeber, every
     * time you update the conductances (including the first one!), you have
     * to return Conductances::CHANGED from this function! This is an inout
     * parameter, as the previous values are preserved across function calls.
     * 
     * Last, but not least, if you encounter an error, you can signal it by
     * throwing a C++ exception.
     */
    
    // This example function simulates the absence of a heatsink, where
    // the spreader exchanges heat convectively to the ambient
    Conductances result;
    if(firstCall)
    {
        //First time called, compute the thermal conductances
        firstCall=false;
        
        double ambientConductance=1; //total ambient conductance in W/K, arbitrary value
        
        //Divide the ambient conductance evenly among the cells
        ambientConductance/=thermalConductances.rows()*thermalConductances.cols();
        double conductance=parallel(spreaderConductance,ambientConductance);
        
        for(unsigned int r=0;r<thermalConductances.rows();r++)
            for(unsigned int c=0;c<thermalConductances.cols();c++)
                thermalConductances.at(r,c)=conductance;
        result=Conductances::CHANGED;
    } else result=Conductances::NOT_CHANGED;
    
    //In the absence of a heatsink, the cell above the spreader is the ambient
    for(unsigned int r=0;r<sinkTemperatures.rows();r++)
        for(unsigned int c=0;c<sinkTemperatures.cols();c++)
            sinkTemperatures.at(r,c)=ambientTemperature;

    return result;
}

HeatSink::~HeatSink() {}
