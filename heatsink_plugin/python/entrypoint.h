
#ifndef ENTRYPOINT_H
#define ENTRYPOINT_H

#include <chrono>

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
