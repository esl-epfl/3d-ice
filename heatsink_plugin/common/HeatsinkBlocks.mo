package HeatsinkBlocks
/******************************************************************************
 * This file is part of 3D-ICE, version 2.2.7 .                               *
 *                                                                            *
 * 3D-ICE is free software: you can  redistribute it and/or  modify it  under *
 * the terms of the  GNU General  Public  License as  published by  the  Free *
 * Software  Foundation, either  version  3  of  the License,  or  any  later *
 * version.                                                                   *
 *                                                                            *
 * 3D-ICE is  distributed  in the hope  that it will  be useful, but  WITHOUT *
 * ANY  WARRANTY; without  even the  implied warranty  of MERCHANTABILITY  or *
 * FITNESS  FOR A PARTICULAR  PURPOSE. See the GNU General Public License for *
 * more details.                                                              *
 *                                                                            *
 * You should have  received a copy of  the GNU General  Public License along *
 * with 3D-ICE. If not, see <http://www.gnu.org/licenses/>.                   *
 *                                                                            *
 *                             Copyright (C) 2020                             *
 *   Embedded Systems Laboratory - Ecole Polytechnique Federale de Lausanne   *
 *                            All Rights Reserved.                            *
 *                                                                            *
 * Authors: Federico Terraneo                                                 *
 *          Arvind Sridhar                                                    *
 *          Alessandro Vincenzi                                               *
 *          Giseong Bak                                                       *
 *          Martino Ruggiero                                                  *
 *          Thomas Brunschwiler                                               *
 *          David Atienza                                                     *
 *                                                                            *
 * For any comment, suggestion or request  about 3D-ICE, please  register and *
 * write to the mailing list (see http://listes.epfl.ch/doc.cgi?liste=3d-ice) *
 * Any usage  of 3D-ICE  for research,  commercial or other  purposes must be *
 * properly acknowledged in the resulting products or publications.           *
 *                                                                            *
 * EPFL-STI-IEL-ESL                                                           *
 * Batiment ELG, ELG 130                Mail : 3d-ice@listes.epfl.ch          *
 * Station 11                                  (SUBSCRIPTION IS NECESSARY)    *
 * 1015 Lausanne, Switzerland           Url  : http://esl.epfl.ch/3d-ice.html *
 ******************************************************************************/
  package PartialModels
    partial model Fin
    extends ThermalBlocks.PartialModels.Layer;
      // Model of a fin oriented in the width direction (y axis).
      // Note: the parameters of the fin have to be configured as-if the fin has been rotated 90° clockwise
      // around the y axis, "flattening" it in the xy plane, like any other layer.
      // This has been done for modeling convenience, but also means that
      // - the "unflattened" fin height is instead the fin length
      // - the "unflattened" fin length is instead the fin height
      // - the fin width remains unchanged.
      
      ThermalBlocks.Connectors.HeatPortVector bottom(n = rows) annotation(
        Placement(transformation(extent = {{-100, -100}, {-80, -80}}, rotation = 0)));
    
      input Modelica.SIunits.Velocity fluidVelocity "Fluid velocity [m/s]";
      input Modelica.SIunits.Temperature inletFluidTemperature "Inlet fluid temperature [K]";
      
      replaceable partial function convectionCorrelation
        input Modelica.SIunits.Velocity fluidVelocity;
        output Modelica.SIunits.CoefficientOfHeatTransfer htc;
      end convectionCorrelation;
      
      Modelica.SIunits.CoefficientOfHeatTransfer htc;
      Modelica.SIunits.ThermalConductance gf;
    
    equation
      htc = convectionCorrelation(fluidVelocity);
      gf = 2 * (htc * l * w * gz) / (htc * l * w + gz); //2 because both fin faces exchange convectively
    
      for i in 1:rows loop
        bottom.port[i].Q_flow = 2 * gx * (bottom.port[i].T - T[i, 1]);
      end for;
      
      if rows > 1 and cols > 1 then
        c * der(T[1, 1]) = gx * (T[1, 2] - T[1, 1]) 
                         + gy * (T[2, 1] - T[1, 1])
                         + gf * (inletFluidTemperature - T[1, 1])
                         + bottom.port[1].Q_flow;
        
        c * der(T[1, cols]) = gx * (T[1, cols - 1] - T[1, cols]) 
                            + gy * (T[2, cols] - T[1, cols]) 
                            + gf * (inletFluidTemperature - T[1, cols]);
        
        c * der(T[rows, 1]) = gx * (T[rows, 2] - T[rows, 1])
                            + gy * (T[rows - 1, 1] - T[rows, 1])
                            + gf * (inletFluidTemperature - T[rows, 1])
                            + bottom.port[rows].Q_flow;
      
        c * der(T[rows, cols]) = gx * (T[rows, cols - 1] - T[rows, cols])
                               + gy * (T[rows - 1, cols] - T[rows, cols])
                               + gf * (inletFluidTemperature - T[rows, cols]);
      elseif rows > 1 /*and cols == 1*/ then
        c * der(T[1, 1]) = gy * (T[2, 1] - T[1, 1])
                         + gf * (inletFluidTemperature - T[1, 1])
                         + bottom.port[1].Q_flow;
      
        c * der(T[rows, 1]) = gy * (T[rows - 1, 1] - T[rows, 1])
                            + gf * (inletFluidTemperature - T[rows, 1])
                            + bottom.port[rows].Q_flow;
      elseif cols > 1 /*and rows == 1*/ then
        c * der(T[1, 1]) = gx * (T[1, 2] - T[1, 1]) 
                         + gf * (inletFluidTemperature - T[1, 1])
                         + bottom.port[1].Q_flow;
                         
        c * der(T[1, cols]) = gx * (T[1, cols - 1] - T[1, cols]) 
                            + gf * (inletFluidTemperature - T[1, cols]);
      else /* cols == 1 and rows == 1 */
        c * der(T[1, 1]) = + gf * (inletFluidTemperature - T[1, 1])
                           + bottom.port[1].Q_flow;
      end if;
      
      for i in 2:rows - 1 loop
        if cols > 1 then
          c * der(T[i, 1]) = gx * (T[i, 2] - T[i, 1])
                           + gy * (T[i - 1, 1] + T[i + 1, 1] - 2 * T[i, 1])
                           + gf * (inletFluidTemperature - T[i, 1])
                           + bottom.port[i].Q_flow;
                         
          c * der(T[i, cols]) = gx * (T[i, cols - 1] - T[i, cols])
                              + gy * (T[i - 1, cols] + T[i + 1, cols] - 2 * T[i, cols])
                              + gf * (inletFluidTemperature - T[i, cols]);
        else /* cols == 1 */
          c * der(T[i, 1]) = gy * (T[i - 1, 1] + T[i + 1, 1] - 2 * T[i, 1])
                           + gf * (inletFluidTemperature - T[i, 1])
                           + bottom.port[i].Q_flow;
        end if;
      end for;
      
      for j in 2:cols - 1 loop
        if rows > 1 then
          c * der(T[1, j]) = gx * (T[1, j - 1] + T[1, j + 1] - 2 * T[1, j])
                           + gy * (T[2, j] - T[1, j])
                           + gf * (inletFluidTemperature - T[1, j]);
          c * der(T[rows, j]) = gx * (T[rows, j - 1] + T[rows, j + 1] - 2 * T[rows, j])
                              + gy * (T[rows - 1, j] - T[rows, j])
                              + gf * (inletFluidTemperature - T[rows, j]);
        else /* rows == 1 */
          c * der(T[1, j]) = gx * (T[1, j - 1] + T[1, j + 1] - 2 * T[1, j])
                           + gf * (inletFluidTemperature - T[1, j]);
        end if;
      end for;
      
      for i in 2:rows - 1 loop
        for j in 2:cols - 1 loop
          c * der(T[i, j]) = gx * (T[i, j - 1] + T[i, j + 1] - 2 * T[i, j])
                           + gy * (T[i - 1, j] + T[i + 1, j] - 2 * T[i, j])
                           + gf * (inletFluidTemperature - T[i, j]);
        end for;
      end for;
    end Fin;
  
    partial model Heatsink
      parameter Modelica.SIunits.Temperature initialTemperature(fixed=false) "Initial simulation temperature [K]";
      parameter Modelica.SIunits.ThermalConductance cellBottomConductance "Thermal conductance between the center and bottom of one sink cell connected to the spreader [W/K]";
      parameter Modelica.SIunits.Length bottomLength "Length of the heatsink bottom (in the X axis) [m]";
      parameter Modelica.SIunits.Length bottomWidth "Width of the heatsink bottom (in the Y axis) [m]";
      parameter Integer                 bottomRows=1 "Number of rows of cells in which the sink bottom width is divided";
      parameter Integer                 bottomCols=1 "Number of columns of cells in which the sink bottom length is divided";
    
      Modelica.Thermal.HeatTransfer.Interfaces.HeatPort_a bottom[bottomRows, bottomCols] "This is the heat port at the bottom of the sink. Note that these heat ports map to the center of the bottom cells of the sink, without including the bottom conductance (cellBottomConductance). This has been done to allow grid pitch adaptation." annotation(
        Placement(visible = true, transformation(origin = {1, -91}, extent = {{-17, -17}, {17, 17}}, rotation = 0), iconTransformation(origin = {1, -91}, extent = {{-17, -17}, {17, 17}}, rotation = 0)));
      
      annotation(
        Diagram,
        Icon(coordinateSystem(initialScale = 0.1), graphics = {Text(extent={{-150,150},{150,110}},textString="%name",lineColor={0,0,255}),Rectangle(fillColor = {255, 170, 0}, fillPattern = FillPattern.Solid, extent = {{-100, 100}, {100, -100}})}));
    end Heatsink;

    partial model Interface3DICE
    // The heatsink we are connecting to 3D-ICE
      replaceable Heatsink sink annotation(
        Placement(visible = true, transformation(origin = {30, 2}, extent = {{-10, -10}, {10, 10}}, rotation = 0)));
        
      // These parameters are assignd by 3D-ICE before the simulation starts.
      parameter String args "Plugin args (value coming from 3D-ICE DO NOT SET VALUES, THEY WILL BE OVERWRITTEN)";
      parameter Modelica.SIunits.Temperature initialTemperature "Initial simulation temperature [K] (value coming from 3D-ICE DO NOT SET VALUES, THEY WILL BE OVERWRITTEN)";
        
      // These parameters are read by 3D-ICE before the simulation starts.
      // Note that the plugin interface assumes that the thermal conductance between the center and bottom of the sink cells connected to the spreader is uniform for each cell and constant throughout the simulation.
      parameter Integer sinkRows = sink.bottomRows "Number of rows of cells in which the sink width is divided (read by 3D-ICE)";
      parameter Integer sinkColumns = sink.bottomCols "Number of columns of cells in which the sink length is divided (read by 3D-ICE)";
      parameter Modelica.SIunits.ThermalConductance sinkCellBottomConductance = sink.cellBottomConductance "Conductance between the center and bottom of one sink cell connected to the spreader [W/K] (read by 3D-ICE)";
      parameter Modelica.SIunits.Length sinkLength = sink.bottomLength "Length of the heatsink (in the X axis) [m] (read by 3D-ICE)";
      parameter Modelica.SIunits.Length sinkWidth = sink.bottomWidth "Width of the heatsink  (in the Y axis) [m] (read by 3D-ICE)";
      parameter Modelica.SIunits.Length spreaderX0(fixed = false) "X axis distance between bottom left corner of sink and spreader [m] (read by 3D-ICE)";
      parameter Modelica.SIunits.Length spreaderY0(fixed = false) "Y axis distance between bottom left corner of sink and spreader [m] (read by 3D-ICE)";
        
      // At each simulation step 3D-ICE will read the temperature values T and assign the heat flows Q_flow
      // A note on geometry, the port variable follows the 3D-ICE convention, so port[1,1] is the bottom left cell (x=0,y=0). Thus rows are swapped compared to a matrix.
      Modelica.Blocks.Interfaces.RealOutput T[sinkRows, sinkColumns] annotation(
        Placement(visible = true, transformation(origin = {-100, -60}, extent = {{22, -22}, {-22, 22}}, rotation = 0), iconTransformation(origin = {-100, -60}, extent = {{22, -22}, {-22, 22}}, rotation = 0)));
      Modelica.Blocks.Interfaces.RealInput Q_flow[sinkRows, sinkColumns] annotation(
        Placement(visible = true, transformation(origin = {-99, 61}, extent = {{-21, -21}, {21, 21}}, rotation = 0), iconTransformation(origin = {-99, 61}, extent = {{-21, -21}, {21, 21}}, rotation = 0)));
      CausalAdaptor adaptor[sinkRows, sinkColumns];
    
      model CausalAdaptor
        Modelica.Thermal.HeatTransfer.Interfaces.HeatPort_a port;
        Modelica.Blocks.Interfaces.RealInput Q_flow;
        Modelica.Blocks.Interfaces.RealOutput T;
      equation
        port.T = T;
        port.Q_flow = - Q_flow;
      end CausalAdaptor;
    
      function Parse
        input String args;
        output Real x0;
        output Real y0;
      protected
        Integer nextIndex;
      algorithm
        nextIndex := Modelica.Utilities.Strings.find(args, " ");
        (x0, nextIndex) := Modelica.Utilities.Strings.scanReal(args, nextIndex);
        (y0, nextIndex) := Modelica.Utilities.Strings.scanReal(args, nextIndex);
      end Parse;
    
    initial algorithm
      sink.initialTemperature := initialTemperature;
      // Assign the position of the spreader underneath the heatsink. Incidentally, this shows how you can pass parameters to Modelica from the 3D-ICE stk file.
      (spreaderX0, spreaderY0) := Parse(args);
      Modelica.Utilities.Streams.print("args = " + args);
      Modelica.Utilities.Streams.print("initialTemperature = " + String(initialTemperature));
      Modelica.Utilities.Streams.print("spreaderX0 = " + String(spreaderX0));
      Modelica.Utilities.Streams.print("spreaderY0 = " + String(spreaderY0));
      
    equation  
      for i in 1:sinkRows loop
        for j in 1:sinkColumns loop
          connect(adaptor[i, j].T, T[i, j]);
          connect(adaptor[i, j].Q_flow, Q_flow[i, j]);
          connect(adaptor[i, j].port, sink.bottom[i, j]);
        end for;
      end for;
      annotation(
        Diagram,
        Icon(graphics = {Text(extent = {{-150, 150}, {150, 110}}, textString = "%name", lineColor = {0, 0, 255}), Rectangle(fillColor = {255, 170, 0}, fillPattern = FillPattern.Solid, extent = {{-100, 100}, {100, -100}})}));
    end Interface3DICE;
    
  end PartialModels;
  
  model LayerOptimized
  extends ThermalBlocks.PartialModels.Layer;
  extends ThermalBlocks.Icons.Layer;
    
    ThermalBlocks.Connectors.HeatPortMatrix top(rows = rows, cols = cols) annotation(
      Placement(transformation(extent = {{-100, 80}, {-80, 100}}, rotation = 0)));
    ThermalBlocks.Connectors.HeatPortMatrix pGen(rows = rows, cols = cols) annotation(
      Placement(transformation(extent = {{80, -100}, {100, -80}}, rotation = 0)));
  
  equation
    for i in 1:rows loop
      for j in 1:cols loop
        top.port[i, j].Q_flow = 2 * gz * (top.port[i, j].T - T[i, j]);
        pGen.port[i, j].T = T[i, j];
      end for;
    end for;
    
    if rows > 1 and cols > 1 then
      c * der(T[1, 1]) = gx * (T[1, 2] - T[1, 1]) 
                       + gy * (T[2, 1] - T[1, 1])
                       + top.port[1, 1].Q_flow
                       + pGen.port[1, 1].Q_flow;
      
      c * der(T[1, cols]) = gx * (T[1, cols - 1] - T[1, cols]) 
                          + gy * (T[2, cols] - T[1, cols]) 
                          + top.port[1, cols].Q_flow 
                          + pGen.port[1, cols].Q_flow;
      
      c * der(T[rows, 1]) = gx * (T[rows, 2] - T[rows, 1])
                          + gy * (T[rows - 1, 1] - T[rows, 1])
                          + top.port[rows, 1].Q_flow
                          + pGen.port[rows, 1].Q_flow;
    
      c * der(T[rows, cols]) = gx * (T[rows, cols - 1] - T[rows, cols])
                             + gy * (T[rows - 1, cols] - T[rows, cols])
                             + top.port[rows, cols].Q_flow
                             + pGen.port[rows, cols].Q_flow;
    elseif rows > 1 /*and cols == 1*/ then
      c * der(T[1, 1]) = gy * (T[2, 1] - T[1, 1])
                       + top.port[1, 1].Q_flow
                       + pGen.port[1, 1].Q_flow;
    
      c * der(T[rows, 1]) = gy * (T[rows - 1, 1] - T[rows, 1])
                          + top.port[rows, 1].Q_flow
                          + pGen.port[rows, 1].Q_flow;
    elseif cols > 1 /*and rows == 1*/ then
      c * der(T[1, 1]) = gx * (T[1, 2] - T[1, 1]) 
                       + top.port[1, 1].Q_flow
                       + pGen.port[1, 1].Q_flow;
                       
      c * der(T[1, cols]) = gx * (T[1, cols - 1] - T[1, cols]) 
                          + top.port[1, cols].Q_flow 
                          + pGen.port[1, cols].Q_flow;
    else /* cols == 1 and rows == 1 */
      c * der(T[1, 1]) = pGen.port[1, 1].Q_flow;
    end if;
    
    for i in 2:rows - 1 loop
      if cols > 1 then
        c * der(T[i, 1]) = gx * (T[i, 2] - T[i, 1])
                         + gy * (T[i - 1, 1] + T[i + 1, 1] - 2 * T[i, 1])
                         + top.port[i, 1].Q_flow
                         + pGen.port[i, 1].Q_flow;
                       
        c * der(T[i, cols]) = gx * (T[i, cols - 1] - T[i, cols])
                            + gy * (T[i - 1, cols] + T[i + 1, cols] - 2 * T[i, cols])
                            + top.port[i, cols].Q_flow
                            + pGen.port[i, cols].Q_flow;
      else /* cols == 1 */
        c * der(T[i, 1]) = gy * (T[i - 1, 1] + T[i + 1, 1] - 2 * T[i, 1])
                         + top.port[i, 1].Q_flow
                         + pGen.port[i, 1].Q_flow;
      end if;
    end for;
    
    for j in 2:cols - 1 loop
      if rows > 1 then
        c * der(T[1, j]) = gx * (T[1, j - 1] + T[1, j + 1] - 2 * T[1, j])
                         + gy * (T[2, j] - T[1, j])
                         + top.port[1, j].Q_flow
                         + pGen.port[1, j].Q_flow;
        c * der(T[rows, j]) = gx * (T[rows, j - 1] + T[rows, j + 1] - 2 * T[rows, j])
                            + gy * (T[rows - 1, j] - T[rows, j])
                            + top.port[rows, j].Q_flow
                            + pGen.port[rows, j].Q_flow;
      else /* rows == 1 */
        c * der(T[1, j]) = gx * (T[1, j - 1] + T[1, j + 1] - 2 * T[1, j])
                         + top.port[1, j].Q_flow
                         + pGen.port[1, j].Q_flow;
      end if;
    end for;
    
    for i in 2:rows - 1 loop
      for j in 2:cols - 1 loop
        c * der(T[i, j]) = gx * (T[i, j - 1] + T[i, j + 1] - 2 * T[i, j])
                         + gy * (T[i - 1, j] + T[i + 1, j] - 2 * T[i, j])
                         + top.port[i, j].Q_flow
                         + pGen.port[i, j].Q_flow;
      end for;
    end for;
  end LayerOptimized;

  block Fan
    input Modelica.SIunits.AngularVelocity fanSpeed "Fan speed [rad/s] (divide RPM by 9.55 to get rad/s)";
    output Modelica.SIunits.VolumeFlowRate airFlowRate "Air volumetric flow rate [m^3/s]";
    output Modelica.SIunits.Velocity airVelocity "Air velocity [m/s]";
  
    parameter Real flowRateCoefficient = 1e-5 "Coefficient of proportionality from fan speed to maximum air volumetric flow rate (assuming deltaP=0)";
    parameter Real operatingPoint = 1 "Fan operating point (1 means flow=max,deltaP=0, 0 means flow=0,deltaP=max)";
    parameter Modelica.SIunits.Area area = 1e-3 "Area of the airflow [m^2]";
    
  equation
    airFlowRate = fanSpeed * flowRateCoefficient * operatingPoint "Air volumetric flow rate is linearly proportional to fan speed";
    airVelocity = airFlowRate / area "Air velocity is linearly proportional to volumetric flow rate";
  end Fan;

  function RPMtoRads
    input Real speedRPM;
    output Modelica.SIunits.AngularVelocity speedRads;
  algorithm
    speedRads := speedRPM / 9.55; //9.55 is the conversion factor from RPM to rad/s
  end RPMtoRads;
  
  function LPMtoM3s
    input Real flowRateLPM;
    output Modelica.SIunits.VolumeFlowRate flowRateM3s;
  algorithm
    flowRateM3s := flowRateLPM / 6e4; //6e4 is the conversion factor from liters per minute to cubic meters per second
  end LPMtoM3s;
  annotation(
    uses(Modelica(version = "3.2.3")));
end HeatsinkBlocks;