
#ifndef PYTHONWRAPPER_H
#define PYTHONWRAPPER_H

#include <Python.h>

class PythonWrapper
{
public:
    PythonWrapper();

    void heatsinkInit(unsigned int nRows, unsigned int nCols,
                      double cellWidth, double cellLength,
                      double initialTemperature, double timeStep);

    void heatsinkSimulateStep(const double *heatFlows, double *temperatures,
                              unsigned int size);

    ~PythonWrapper();

private:
    PyObject *check(PyObject *object);

    PyObject *init, *simulateStep;
};

#endif //PYTHONWRAPPER_H
