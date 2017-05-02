
// This is just a template, write your heatsink code here
// by implementing the HeatSink class

#include <stdexcept>
#include <iostream>
#include "heatsink.h"

using namespace std;

//
// class HeatSink
//

HeatSink::HeatSink() {}

void HeatSink::heatsinkInit(unsigned int nRows, unsigned int nCols,
                            double cellWidth, double cellLength,
                            double initialTemperature, double timeStep)
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
    P(timeStep);
    #undef P

    ambientTemperature=initialTemperature;
}

void HeatSink::heatsinkSimulateStep(const CellMatrix heatFlows, CellMatrix temperatures)
{
    // This example function simulates a "brick wall" heatsink whose
    // temperature remains constant regardless of the applied power
    // If an error is found, throw an exception to halt the simulation
    double sum=0.0;
    for(unsigned int r=0;r<heatFlows.rows();r++)
    {
        for(unsigned int c=0;c<heatFlows.cols();c++)
        {
            temperatures.at(r,c)=ambientTemperature;
            sum+=heatFlows.at(r,c);
        }
    }
    cout<<"sum of heat flow "<<sum<<endl;
}

HeatSink::~HeatSink() {}
