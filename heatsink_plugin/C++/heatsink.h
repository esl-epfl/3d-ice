
#ifndef HEATSINK_H
#define HEATSINK_H

#include "entrypoint.h"

class HeatSink
{
public:
    HeatSink();

    void heatsinkInit(unsigned int nRows, unsigned int nCols,
                      double cellWidth, double cellLength,
                      double initialTemperature, double timeStep);

    void heatsinkSimulateStep(const CellMatrix heatFlows, CellMatrix temperatures);

    ~HeatSink();

private:
    double ambientTemperature;
};

#endif //HEATSINK_H
