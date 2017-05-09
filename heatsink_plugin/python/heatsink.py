
# This is just a template, write your heatsink code here
# by implementing the heatsinkInit and heatsinkSimulateStep functions

ambientTemperature=0;
gSpreaderConductance=0;
first=True

def parallel(x,y):
    return x*y/(x+y)

def heatsinkInit(nRows,nCols,cellWidth,cellLength,initialTemperature,spreaderConductance,timeStep):
    # This example function simply prints the given parameters
    print(nRows)
    print(nCols)
    print(cellWidth)
    print(cellLength)
    print(initialTemperature)
    print(spreaderConductance)
    print(timeStep)
    global ambientTemperature
    global gSpreaderConductance
    ambientTemperature=initialTemperature
    gSpreaderConductance=spreaderConductance

def heatsinkSimulateStep(spreaderTemperatures,thermalConductances):
    # See the C++ plugin comments for this
    global first
    conductancesChanged=False
    if first:
        first=False
        ambientConductance=1/len(thermalConductances)
        conductance=parallel(gSpreaderConductance,ambientConductance)
        for i in range(len(thermalConductances)):
            thermalConductances[i]=conductance
        conductancesChanged=True
    
    # This example function simulates the absence of a heatsink, where
    # the spreader exchanges heat convectively to the ambient
    # If an error is found, raise an exception to halt the simulation
    heatsinkTemperature=[]
    for i in range(len(spreaderTemperatures)):
        heatsinkTemperature.append(ambientTemperature)
    return (heatsinkTemperature,conductancesChanged)
