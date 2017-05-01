
#include <iostream>
#include "heatsink.h"

using namespace std;

static double ambientTemperature;

bool heatsink_init(unsigned int nrows, unsigned int ncols,
                   double cellwidth, double celllength,
                   double initialtemperature, double timestep)
{
    // This example function simply prints the given parameters
    cout.setf(ios::fixed);
    cout.precision(3);
    cout<<endl;

    #define P(x) cout<<#x<<": "<<x<<endl
    P(nrows);
    P(ncols);
    P(cellwidth);
    P(celllength);
    P(initialtemperature);
    P(timestep);
    #undef P

    ambientTemperature=initialtemperature;

    return true; //True means success
}

bool heatsink_simulate_step(const double *heatflow, double *temperatures,
                            unsigned int size)
{
    // This example function simulates a "brick wall" heatsink
    // whose temperature remains constant regardless of the
    // applied power
    double sum=0.0;
    for(unsigned int i=0;i<size;i++)
    {
        temperatures[i]=ambientTemperature;
        sum+=heatflow[i];
    }
    cout<<"sum of heat flow "<<sum<<endl;
    return true; //True means success
}
