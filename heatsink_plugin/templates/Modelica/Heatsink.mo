package Heatsink
  model Heatsink
    parameter Modelica.SIunits.Temperature initialTemperature(fixed=false) "Initial simulation temperature [K] (value coming from 3D-ICE)";
    parameter Modelica.SIunits.ThermalConductance spreaderConductance(fixed=false) "Thermal conductance from the center to the face of the spreader cells [W/K] (value coming from 3D-ICE)";
    parameter String args(fixed=false) "Plugin args (value coming from 3D-ICE)";
    Modelica.Thermal.HeatTransfer.Interfaces.HeatPort_b port "Bottom face of the heat sink (the overlapping part is connected to 3D-ICE)" annotation(
      Placement(visible = true, transformation(origin = {-58, 0}, extent = {{-10, -10}, {10, 10}}, rotation = 0), iconTransformation(origin = {-58, 0}, extent = {{-10, -10}, {10, 10}}, rotation = 0)));
    
    parameter Modelica.SIunits.ThermalConductance ambientConductance = 1 "[W/K]";
    parameter Modelica.SIunits.ThermalConductance fullConductance = ambientConductance * spreaderConductance / (ambientConductance + spreaderConductance);
    Modelica.Thermal.HeatTransfer.Components.ThermalConductor Conductance(G = fullConductance)  annotation(
      Placement(visible = true, transformation(origin = {-10, 0}, extent = {{10, -10}, {-10, 10}}, rotation = 0)));
    Modelica.Thermal.HeatTransfer.Sources.FixedTemperature Ambient(T = initialTemperature)  annotation(
      Placement(visible = true, transformation(origin = {42, 0}, extent = {{10, -10}, {-10, 10}}, rotation = 0))); 
  equation
    connect(Ambient.port, Conductance.port_a) annotation(
      Line(points = {{32, 0}, {0, 0}, {0, 0}, {0, 0}}, color = {191, 0, 0}));
    connect(Conductance.port_b, port) annotation(
      Line(points = {{-20, 0}, {-54, 0}, {-54, 0}, {-58, 0}}, color = {191, 0, 0}));
    annotation(
      Diagram,
      Icon);
  end Heatsink;
  annotation(
    uses(Modelica(version = "3.2.2")));
end Heatsink;