package Test
  model Heatsink
    // 3D-ICE interface -- begin
  
    // These parameters are assignd by 3D-ICE before the simulation starts. DO NOT SET VALUES, THEY WILL BE OVERWRITTEN.
    parameter String args "Plugin args (value coming from 3D-ICE)";
    parameter Modelica.SIunits.Temperature initialTemperature "Initial simulation temperature [K] (value coming from 3D-ICE)";
    
    // These parameters are read by 3D-ICE before the simulation starts.
    // Note that the plugin interface assumes that the thermal conductance between the center and bottom of the sink cells connected to the spreader is uniform for each cell and constant throughout the simulation.
    parameter Modelica.SIunits.ThermalConductance sinkCellBottomConductance = 8.333 "Thermal conductance between the center and bottom of the sink cells connected to the spreader [W/K]";
    parameter Modelica.SIunits.Length sinkLength = 0.03 "Length of the heatsink (in the X axis) [m]";
    parameter Modelica.SIunits.Length sinkWidth  = 0.02 "Width of the heatsink (in the Y axis) [m]";
    parameter Modelica.SIunits.Length spreaderX0(fixed=false) "X axis distance between bottom left corner of sink and spreader [m]";
    parameter Modelica.SIunits.Length spreaderY0(fixed=false) "Y axis distance between bottom left corner of sink and spreader [m]";
    parameter Integer sinkRows = 2    "Number of rows of cells in which the sink width is divided (the whole width, not just the one overlapping with the spreader) [m]";
    parameter Integer sinkColumns = 3 "Number of columns of cells in which the sink length is divided (the whole length, not just the one overlapping with the spreader) [m]";
  
    // This is the heat port of the bottom of the sink (the whole bottom, not just the one overlapping with the spreader)
    Modelica.Thermal.HeatTransfer.Interfaces.HeatPort_b port[sinkRows,sinkColumns] "Bottom face of the heat sink (the overlapping part is connected to 3D-ICE)" annotation(
      Placement(visible = true, transformation(origin = {-58, 0}, extent = {{-10, -10}, {10, 10}}, rotation = 0), iconTransformation(origin = {-90, 0}, extent = {{-10, -10}, {10, 10}}, rotation = 0)));
    
    // 3D-ICE interface -- end
    
    Modelica.Thermal.HeatTransfer.Sources.FixedTemperature Ambient(T = initialTemperature)  annotation(
      Placement(visible = true, transformation(origin = {42, 0}, extent = {{10, -10}, {-10, 10}}, rotation = 0))); 
  
  initial algorithm
    (spreaderX0,spreaderY0) := Parse(args);
    Modelica.Utilities.Streams.print("args = "+args);
    Modelica.Utilities.Streams.print("initialTemperature = "+String(initialTemperature));
    Modelica.Utilities.Streams.print("spreaderX0 = "+String(spreaderX0));
    Modelica.Utilities.Streams.print("spreaderY0 = "+String(spreaderY0));
  
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

  function Parse
    input String args;
    output Real x0;
    output Real y0;
  protected
    Integer nextIndex;
  algorithm
    nextIndex      := Modelica.Utilities.Strings.find(args," ");
    (x0,nextIndex) := Modelica.Utilities.Strings.scanReal(args,nextIndex);
    (y0,nextIndex) := Modelica.Utilities.Strings.scanReal(args,nextIndex);
  end Parse;
  annotation(
    uses(Modelica(version = "3.2.2")));
end Test;
