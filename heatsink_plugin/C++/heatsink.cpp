
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
     * The way to compute them is by paralleling the conductance passed in
     * heatsinkInit() (which is the conductance from the center of the spreader
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
    
    // This example function simulates a "brick wall" heatsink whose
    // temperature remains constant regardless of the applied power
    Conductances result;
    if(firstCall)
    {
        firstCall=false;
        //First time called, compute the thermal conductances
        for(unsigned int r=0;r<thermalConductances.rows();r++)
        {
            for(unsigned int c=0;c<thermalConductances.cols();c++)
            {
                thermalConductances.at(r,c)=spreaderConductance;
            }
        }
        result=Conductances::CHANGED;
    } else result=Conductances::NOT_CHANGED;
    
    for(unsigned int r=0;r<sinkTemperatures.rows();r++)
    {
        for(unsigned int c=0;c<sinkTemperatures.cols();c++)
        {
            sinkTemperatures.at(r,c)=ambientTemperature;
        }
    }
    return result;
}

HeatSink::~HeatSink() {}
