
#ifndef ENTRYPOINT_H
#define ENTRYPOINT_H

#include <chrono>
#include <stdexcept>

// This is the public interface that a heatsink plugin has to
// implement. Functions need to have C linkage to be callable
// from 3D-ICE
extern "C" {
bool heatsink_init(unsigned int nrows, unsigned int ncols,
                   double cellwidth, double celllength,
                   double initialtemperature, double timestep);
bool heatsink_simulate_step(const double *heatflows, double *temperatures,
                            unsigned int size);
}

// Cooment out to disable bound checking in CellMatrix
// #define CELLMATRIX_BOUND_CHECK

/**
 * A simple non-owning wrapper over a double* to turn it into a more
 * understandable representation. 3D-ICE stores information about cells
 * in an matrix layout, where the cell (0,0) is at the top left.
 */
class CellMatrix
{
public:
    CellMatrix(double *data, unsigned int nRows, unsigned int nCols)
        : data(data), nRows(nRows), nCols(nCols) {}
    
    double& at(unsigned int row, unsigned int col)
    {
        #ifdef CELLMATRIX_BOUND_CHECK
        if(row>nRows || col>nCols)
            throw std::range_error("CellMatrix out of bounds");
        #endif //CELLMATRIX_BOUND_CHECK
        return data[row*nCols+col];
    }
    
    double at(unsigned int row, unsigned int col) const
    {
        #ifdef CELLMATRIX_BOUND_CHECK
        if(row>nRows || col>nCols)
            throw std::range_error("CellMatrix out of bounds");
        #endif //CELLMATRIX_BOUND_CHECK
        return data[row*nCols+col];
    }
    
    unsigned int rows() const { return nRows; }
    
    unsigned int cols() const { return nCols; }
    
private:
    double *data; // Non-owning pointer
    unsigned int nRows, nCols;
};

// Comment out to silence profiler
#define PRINT_PROFILER_STATS

class Profiler
{
public:
    Profiler();
    
    void enterPlugin();
    
    void exitPlugin();
    
    void printStats();
    
    ~Profiler();
    
private:
    std::chrono::high_resolution_clock::time_point last;
    std::chrono::nanoseconds mainTime;
    std::chrono::nanoseconds pluginTime;
};

class ProfileFunction
{
public:
    ProfileFunction(Profiler& p) : p(p) { p.enterPlugin(); }
    ~ProfileFunction()                  { p.exitPlugin();  }
private:
    Profiler& p;
};

#endif //ENTRYPOINT_H
