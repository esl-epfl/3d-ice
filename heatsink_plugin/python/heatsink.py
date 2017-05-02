
# This is just a template, write your heatsink code here
# by implementing the heatsinkInit and heatsinkSimulateStep functions

ambientTemperature=0;

def heatsinkInit(nRows,nCols,cellWidth,cellLength,initialTemperature,timeStep):
    # This example function simply prints the given parameters
    print(nRows)
    print(nCols)
    print(cellWidth)
    print(cellLength)
    print(initialTemperature)
    print(timeStep)
    global ambientTemperature
    ambientTemperature=initialTemperature

def heatsinkSimulateStep(heatFlows):
    # This example function simulates a "brick wall" heatsink whose
    # temperature remains constant regardless of the applied power.
    # If an error is found, raise an exception to halt the simulation
    print("sum of heat flow %.3f" % sum(heatFlows))
    result=[]
    for i in range(len(heatFlows)):
        result.append(ambientTemperature)
    return result
