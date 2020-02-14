package Heatsink
  model Heatsink
    parameter Modelica.SIunits.Temperature ambientTemperature(fixed=false) "Ambient temperature [K]";
    parameter Modelica.SIunits.ThermalConductance cellBottomConductance=1 "Thermal conductance between the center and bottom of one sink cell connected to the spreader [W/K]";
    parameter Modelica.SIunits.Length bottomLength=0.02 "Length of the heatsink (in the X axis) [m]";
    parameter Modelica.SIunits.Length bottomWidth=0.02 "Width of the heatsink  (in the Y axis) [m]";
    parameter Integer                 bottomRows=1 "Number of rows of cells in which the sink width is divided";
    parameter Integer                 bottomColumns=1 "Number of columns of cells in which the sink length is divided";
    
    Modelica.Thermal.HeatTransfer.Interfaces.HeatPort_a bottom[bottomRows, bottomColumns] "This is the heat port at the bottom of the sink" annotation(
      Placement(visible = true, transformation(origin = {1, -91}, extent = {{-17, -17}, {17, 17}}, rotation = 0), iconTransformation(origin = {1, -91}, extent = {{-17, -17}, {17, 17}}, rotation = 0)));
    Modelica.Thermal.HeatTransfer.Sources.FixedTemperature Ambient(T = ambientTemperature) annotation(
      Placement(visible = true, transformation(origin = {60, 0}, extent = {{10, -10}, {-10, 10}}, rotation = 0)));
    
  equation
    for i in 1:bottomRows loop
      for j in 1:bottomColumns loop
        connect(bottom[i,j],Ambient.port);
      end for;
    end for;
    annotation(
      Diagram,
      Icon(coordinateSystem(initialScale = 0.1), graphics = {Text(extent={{-150,150},{150,110}},textString="%name",lineColor={0,0,255}),Rectangle(fillColor = {255, 170, 0}, fillPattern = FillPattern.Solid, extent = {{-100, 100}, {100, -100}})}));
  end Heatsink;

  model Interface3DICE
    // The heatsink we are connecting to 3D-ICE
    Heatsink sink annotation(
      Placement(visible = true, transformation(origin = {30, 2}, extent = {{-10, -10}, {10, 10}}, rotation = 0)));
  
    // These parameters are assignd by 3D-ICE before the simulation starts.
    parameter String args "Plugin args (value coming from 3D-ICE DO NOT SET VALUES, THEY WILL BE OVERWRITTEN)";
    parameter Modelica.SIunits.Temperature initialTemperature "Initial simulation temperature [K] (value coming from 3D-ICE DO NOT SET VALUES, THEY WILL BE OVERWRITTEN)";
    
    // These parameters are read by 3D-ICE before the simulation starts.
    // Note that the plugin interface assumes that the thermal conductance between the center and bottom of the sink cells connected to the spreader is uniform for each cell and constant throughout the simulation.
    parameter Integer                 sinkRows   =sink.bottomRows    "Number of rows of cells in which the sink width is divided (read by 3D-ICE)";
    parameter Integer                 sinkColumns=sink.bottomColumns "Number of columns of cells in which the sink length is divided (read by 3D-ICE)";
    parameter Modelica.SIunits.ThermalConductance sinkCellBottomConductance=sink.cellBottomConductance "Conductance between the center and bottom of one sink cell connected to the spreader [W/K] (read by 3D-ICE)";
    parameter Modelica.SIunits.Length sinkLength =sink.bottomLength  "Length of the heatsink (in the X axis) [m] (read by 3D-ICE)";
    parameter Modelica.SIunits.Length sinkWidth  =sink.bottomWidth   "Width of the heatsink  (in the Y axis) [m] (read by 3D-ICE)";
    parameter Modelica.SIunits.Length spreaderX0(fixed = false)      "X axis distance between bottom left corner of sink and spreader [m] (read by 3D-ICE)";
    parameter Modelica.SIunits.Length spreaderY0(fixed = false)      "Y axis distance between bottom left corner of sink and spreader [m] (read by 3D-ICE)";
    
    // At each simulation step 3D-ICE will read the temperature values T and assign the heat flows Q_flow
    // A note on geometry, the port variable follows the 3D-ICE convention, so port[1,1] is the bottom left cell (x=0,y=0). Thus rows are swapped compared to a matrix.
    Modelica.Blocks.Interfaces.RealOutput T[sinkRows, sinkColumns] annotation(
      Placement(visible = true, transformation(origin = {-100, -60}, extent = {{22, -22}, {-22, 22}}, rotation = 0), iconTransformation(origin = {-100, -60}, extent = {{22, -22}, {-22, 22}}, rotation = 0)));
    Modelica.Blocks.Interfaces.RealInput Q_flow[sinkRows, sinkColumns] annotation(
      Placement(visible = true, transformation(origin = {-99, 61}, extent = {{-21, -21}, {21, 21}}, rotation = 0), iconTransformation(origin = {-99, 61}, extent = {{-21, -21}, {21, 21}}, rotation = 0)));
    CausalAdaptor adaptor[sinkRows,sinkColumns];
    
  initial algorithm
    sink.ambientTemperature := initialTemperature;
    (spreaderX0,spreaderY0) := Parse(args);
    Modelica.Utilities.Streams.print("args = " + args);
    Modelica.Utilities.Streams.print("initialTemperature = " + String(initialTemperature));
    Modelica.Utilities.Streams.print("spreaderX0 = "+String(spreaderX0));
    Modelica.Utilities.Streams.print("spreaderY0 = "+String(spreaderY0));
    
  equation
    for i in 1:sinkRows loop
      for j in 1:sinkColumns loop
      connect(adaptor[i,j].T,T[i,j]);
      connect(adaptor[i,j].Q_flow,Q_flow[i,j]);
      connect(adaptor[i,j].port,sink.bottom[i,j]);
      end for;
    end for;
    annotation(
      Diagram,
      Icon(graphics = {Text(extent={{-150,150},{150,110}},textString="%name",lineColor={0,0,255}), Rectangle(fillColor = {255, 170, 0}, fillPattern = FillPattern.Solid, extent = {{-100, 100}, {100, -100}})}));
  end Interface3DICE;
  
  model CausalAdaptor
    Modelica.Thermal.HeatTransfer.Interfaces.HeatPort_a port;
    Modelica.Blocks.Interfaces.RealInput Q_flow;
    Modelica.Blocks.Interfaces.RealOutput T;
  equation
    port.T=T;
    port.Q_flow=Q_flow;
  end CausalAdaptor;
  
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
end Heatsink;