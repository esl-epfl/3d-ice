
#ifndef PYTHONWRAPPER_H
#define PYTHONWRAPPER_H

#include <Python.h>

class PythonWrapper
{
public:
    PythonWrapper(unsigned int nRows, unsigned int nCols,
                  double cellWidth,   double cellLength,
                  double initialTemperature,
                  double spreaderConductance,
                  double timeStep);

    int simulateStep(const double *spreaderTemperatures,
                           double *sinkTemperatures,
                           double *thermalConductances);

    ~PythonWrapper();

private:
    PyObject *check(PyObject *object);

    unsigned int size;
    void *so;
    PyObject *hSimulateStep;
    PyObject *cachedConductances=nullptr;
};

#endif //PYTHONWRAPPER_H
