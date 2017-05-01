
#ifndef HEATSINK_H
#define HEATSINK_H

// This is the public interface that a heatsink plugin has to
// implement. Functions need to have C linkage to be callable
// from 3D-ICE
extern "C" {
bool heatsink_init(unsigned int nrows, unsigned int ncols,
                   double cellwidth, double celllength,
                   double initialtemperature, double timestep);
bool heatsink_simulate_step(const double *heatflow, double *temperatures,
                            unsigned int size);
}

#endif //HEATSINK_H
