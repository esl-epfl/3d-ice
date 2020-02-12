package Heatsink
  model Heatsink
    // 3D-ICE interface -- begin
  
    // These parameters are assignd by 3D-ICE before the simulation starts. DO NOT SET VALUES, THEY WILL BE OVERWRITTEN.
    parameter String args(fixed=false) "Plugin args (value coming from 3D-ICE)";
    parameter Modelica.SIunits.Temperature initialTemperature(fixed=false) "Initial simulation temperature [K] (value coming from 3D-ICE)";
    
    // These parameters are read by 3D-ICE before the simulation starts.
    // Note that the plugin interface assumes that the thermal conductance between the center and bottom of the sink cells connected to the spreader is uniform for each cell and constant throughout the simulation.
    parameter Modelica.SIunits.ThermalConductance sinkCellBottomConductance = 1 "Thermal conductance between the center and bottom of the sink cells connected to the spreader [W/K]";
    parameter Modelica.SIunits.Length sinkLength = 0.01 "Length of the heatsink (in the X axis) [m]";
    parameter Modelica.SIunits.Length sinkWidth  = 0.01 "Width of the heatsink (in the Y axis) [m]";
    parameter Modelica.SIunits.Length spreaderX0 = 0    "X axis distance between bottom left corner of sink and spreader [m]";
    parameter Modelica.SIunits.Length spreaderY0 = 0    "Y axis distance between bottom left corner of sink and spreader [m]";
    parameter Integer sinkRows = 1    "Number of rows of cells in which the sink width is divided (the whole width, not just the one overlapping with the spreader) [m]";
    parameter Integer sinkColumns = 1 "Number of columns of cells in which the sink length is divided (the whole length, not just the one overlapping with the spreader) [m]";

    // This is the heat port of the bottom of the sink (the whole bottom, not just the one overlapping with the spreader)
    Modelica.Thermal.HeatTransfer.Interfaces.HeatPort_b port[sinkRows,sinkColumns] "Bottom face of the heat sink (the overlapping part is connected to 3D-ICE)" annotation(
      Placement(visible = true, transformation(origin = {-58, 0}, extent = {{-10, -10}, {10, 10}}, rotation = 0), iconTransformation(origin = {-90, 0}, extent = {{-10, -10}, {10, 10}}, rotation = 0)));
    
    // 3D-ICE interface -- end
    
    Modelica.Thermal.HeatTransfer.Sources.FixedTemperature Ambient(T = initialTemperature)  annotation(
      Placement(visible = true, transformation(origin = {42, 0}, extent = {{10, -10}, {-10, 10}}, rotation = 0))); 
  
  initial algorithm
    Modelica.Utilities.Streams.print("args = "+args);
    Modelica.Utilities.Streams.print("initialTemperature = "+String(initialTemperature));
  
  equation
    for i in 1:sinkRows loop
      for j in 1:sinkColumns loop
        connect(port[i,j], Ambient.port);
      end for;
    end for;
    annotation(
      Diagram,
      Icon(coordinateSystem(initialScale = 0.1)));
  end Heatsink;
  annotation(
    uses(Modelica(version = "3.2.2")));
end Heatsink;