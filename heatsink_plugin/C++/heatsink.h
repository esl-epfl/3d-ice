
#ifndef HEATSINK_H
#define HEATSINK_H

#include "entrypoint.h"

enum class Conductances
{
    CHANGED,
    NOT_CHANGED
};

inline double parallel(double x, double y)
{
    return x*y/(x+y);
}

class HeatSink
{
public:
    HeatSink(unsigned int nRows, unsigned int nCols,
             double cellWidth,   double cellLength,
             double initialTemperature,
             double spreaderConductance,
             double timeStep);

    Conductances simulateStep(const CellMatrix spreaderTemperatures,
                                    CellMatrix sinkTemperatures,
                                    CellMatrix thermalConductances);

    ~HeatSink();

private:
    double ambientTemperature;
    double spreaderConductance;
    bool firstCall=true;
};

#endif //HEATSINK_H
