package HS483
  package Tests
    model ConstantPowerNaturalConvection
      HS483_P14752_Heatsink sink(initialTemperature = 20 + 273.15, airTemperature = 20 + 273.15);
      Modelica.Thermal.HeatTransfer.Sources.FixedHeatFlow heatSource(Q_flow = 10);
    protected
      Modelica.Thermal.HeatTransfer.Components.ThermalConductor bottomConductances[sink.bottomRows,sink.bottomCols](each G = sink.cellBottomConductance);
    equation
      sink.fanSpeed = 0; // Natural convection
      for i in 1:sink.bottomRows loop
        for j in 1:sink.bottomCols loop
          // No grid pitch mapping needed, so do a one-to-one mapping
          connect(sink.bottom[i, j], bottomConductances[i, j].port_a);
          connect(bottomConductances[i, j].port_b, heatSource.port);
        end for;
      end for;
      annotation(
        experiment(StartTime = 0, StopTime = 4000, Tolerance = 1e-6, Interval = 10));
    end ConstantPowerNaturalConvection;

    model ConstantPowerForcedConvection
      HS483_P14752_Heatsink sink(initialTemperature = 20 + 273.15, airTemperature = 20 + 273.15);
      Modelica.Thermal.HeatTransfer.Sources.FixedHeatFlow heatSource(Q_flow = 75);
    protected
       Modelica.Thermal.HeatTransfer.Components.ThermalConductor bottomConductances[sink.bottomRows,sink.bottomCols](each G = sink.cellBottomConductance);
    equation
      sink.fanSpeed = HeatsinkBlocks.RPMtoRads(2700);
      for i in 1:sink.bottomRows loop
        for j in 1:sink.bottomCols loop
          // No grid pitch mapping needed, so do a one-to-one mapping
          connect(sink.bottom[i, j], bottomConductances[i, j].port_a);
          connect(bottomConductances[i, j].port_b, heatSource.port);
        end for;
      end for;
      annotation(
        experiment(StartTime = 0, StopTime = 400, Tolerance = 1e-6, Interval = 1));
    end ConstantPowerForcedConvection;

    model ConstantPowerVariableSpeed
      Modelica.Blocks.Sources.CombiTimeTable fanSpeedTable(smoothness = Modelica.Blocks.Types.Smoothness.ConstantSegments,
        table = [   0,    0;
                  100, 1500;
                  300, 2000;
                  600, 2500;
                 1000, 2500]);
      HS483_P14752_Heatsink sink(initialTemperature = 20 + 273.15, airTemperature = 20 + 273.15);
      Modelica.Thermal.HeatTransfer.Sources.FixedHeatFlow heatSource(Q_flow = 75);
    protected
      Modelica.Thermal.HeatTransfer.Components.ThermalConductor bottomConductances[sink.bottomRows,sink.bottomCols](each G = sink.cellBottomConductance);
    equation
      sink.fanSpeed = HeatsinkBlocks.RPMtoRads(fanSpeedTable.y[1]);
      for i in 1:sink.bottomRows loop
        for j in 1:sink.bottomCols loop
          // No grid pitch mapping needed, so do a one-to-one mapping
          connect(sink.bottom[i, j], bottomConductances[i, j].port_a);
          connect(bottomConductances[i, j].port_b, heatSource.port);
        end for;
      end for;
      annotation(
        experiment(StartTime = 0, StopTime = 1000, Tolerance = 1e-6, Interval = 2));
    end ConstantPowerVariableSpeed;
  end Tests;

  model HS483_Fin
    extends HeatsinkBlocks.PartialModels.Fin;
    // Thermal model of one fin of the copper heatsink model number HS483-ND

    redeclare function extends convectionCorrelation
        input Modelica.SIunits.Velocity fluidVelocity;
        output Modelica.SIunits.CoefficientOfHeatTransfer htc;

      protected
        // Quadratic fit of the air velocity to heat transfer coefficient
        // for copper heatsink model number HS483-ND, fitted from datasheet data
        // Only valid for air velocities in the range 1 to 4.1m/s.
        parameter Real c0 = 11.8;
        parameter Real c1 = 9.33;
        parameter Real c2 = -1.09;

      algorithm
        assert(fluidVelocity == 0 or fluidVelocity >= 1 and fluidVelocity <= 4.1, "Assertion failed: HTC correlation outside fitting range");
        if fluidVelocity == 0 then
          htc := 2.62 "HS483-ND natural convection heat transfer coefficient, from datasheet";
        else
          htc := c0 + c1 * fluidVelocity + c2 * fluidVelocity ^ 2;
        end if;
    end convectionCorrelation;
  end HS483_Fin;

  model HS483_P14752_Heatsink
    extends HeatsinkBlocks.PartialModels.Heatsink(cellBottomConductance = 2 * base.gz, bottomLength = baseLength, bottomWidth = baseWidth, bottomRows = baseRows, bottomCols = baseCols);
    // Thermal model of copper heatsink model number HS483-ND
    // with attached fan model number P14752-ND.
    //
    // To simulate natural convection set fanSpeed to 0. This can be done
    // also in the middle of a simulation to simulate turning off the fan.
    //
    // Fan and heatsink have been fitted in the range 900 to 3500RPM
    // and tested experimentally in range 1500 to 2700RPM (157 to 283rad/s).
    // DO NOT exceed the fitting range.
    //
    // The silicone thermal pad at the bottom of the heatsink is not simulated
    // in Modelica, but in 3D-ICE as the heat spreader layer.
    //
    // The fins are oriented in the y (width) direction.
    
    HeatsinkBlocks.LayerOptimized base(cp = cp, rho = rho, k = k, alpha = alpha, length = baseLength, width = baseWidth, height = baseHeight, rows = baseRows, cols = baseCols, Tstart = initialTemperature) annotation(
      Placement(visible = true, transformation(origin = {-10, -30}, extent = {{-10, -10}, {10, 10}}, rotation = 0)));
    
    HS483_Fin fins[baseCols](each cp = cp, each rho = rho, each k = k, each alpha = alpha, each length = finHeight, each width = baseWidth, each height = finThickness, each rows = baseRows, each cols = finZelements, each Tstart = initialTemperature) annotation(
      Placement(visible = true, transformation(origin = {-10, 10}, extent = {{-10, -10}, {10, 10}}, rotation = 0)));
    
    HeatsinkBlocks.Fan fan(flowRateCoefficient = fanFlowRateCoefficient, operatingPoint = operatingPoint, area = fanArea) annotation(
      Placement(visible = true, transformation(origin = {-10, 50}, extent = {{-10, -10}, {10, 10}}, rotation = 0)));
    
    input Modelica.SIunits.Temperature airTemperature "Air temperature in [K]";
    input Modelica.SIunits.AngularVelocity fanSpeed "Fan speed in rad/s";
    
  protected
    parameter Modelica.SIunits.SpecificHeatCapacity cp = 384.6 "copper properties";
    parameter Modelica.SIunits.Density rho = 8960 "copper properties";
    parameter Modelica.SIunits.ThermalConductivity k = 401 "copper properties";
    parameter Real alpha = 1 "fitted from experimental data";
    parameter Modelica.SIunits.Length baseLength = 0.06 "HS483 dimensions";
    parameter Modelica.SIunits.Length baseWidth = 0.06 "HS483 dimensions";
    parameter Modelica.SIunits.Length baseHeight = 0.0028 "HS483 dimensions";
    parameter Modelica.SIunits.Length finHeight = 0.0192 "HS483 dimensions";
    parameter Modelica.SIunits.Length finThickness = 0.0005 "HS483 dimensions";
    parameter Integer baseRows = 23 "discretization of sink base in the y direction";
    parameter Integer baseCols = 23 "discretization of sink base in the x direction, also number of fins";
    parameter Integer finZelements = 1 "discretization of fin elements in the z direction";
    parameter Real fanFlowRateCoefficient = 5.98e-5 "P14752 parameter, fitted from datasheet";
    parameter Real operatingPoint = 0.9 "P14752 - HS483 interaction, fitted from experimental data";
    parameter Modelica.SIunits.Area fanArea = 4.82e-3 "P14752 parameter, fitted from experimental data starting from datasheet";
  
  equation
    for i in 1:baseRows loop
      for j in 1:baseCols loop
        connect(bottom[i, j], base.pGen.port[i, j]);
        connect(base.top.port[i, j], fins[j].bottom.port[i]);
      end for;
    end for;
    for j in 1:baseCols loop
      fins[j].fluidVelocity = fan.airVelocity;
      fins[j].inletFluidTemperature = airTemperature;
    end for;
    fan.fanSpeed = fanSpeed;
  end HS483_P14752_Heatsink;

  model HS483_P14752_ConstantFanSpeed_Interface3DICE
    extends HeatsinkBlocks.PartialModels.Interface3DICE(redeclare HS483_P14752_Heatsink sink);
    // This interface models makes the HS483_P14752_Heatsink available to 3D-ICE as an FMU.
    // The fan speed is constant throughout the entire simulation and is set in the 3D-ICE stk file
    // as the fourth parameter of the heatisnk.
    //
    // Set the fan speed to 0 to simulate natural convection.

    function ParseFanSpeed
      input String args;
      output Real temperature;
      output Real fanSpeed;
    protected
      Integer nextIndex;
      Real unused;
    algorithm
      nextIndex := Modelica.Utilities.Strings.find(args, " "); // Skip past the plugin name
      (unused, nextIndex) := Modelica.Utilities.Strings.scanReal(args, nextIndex); // Skip past the spreader x0
      (unused, nextIndex) := Modelica.Utilities.Strings.scanReal(args, nextIndex); // Skip past the spreader y0
      (temperature, nextIndex) := Modelica.Utilities.Strings.scanReal(args, nextIndex); // Get the temperature
      (fanSpeed, nextIndex) := Modelica.Utilities.Strings.scanReal(args, nextIndex); // Get the fan speed
    end ParseFanSpeed;

    parameter Real constantAirTemperature(fixed = false);
    parameter Real constantFanSpeed(fixed = false);
    
  initial algorithm
    (constantAirTemperature, constantFanSpeed) := ParseFanSpeed(args);
    Modelica.Utilities.Streams.print("air temperature = " + String(constantAirTemperature));
    Modelica.Utilities.Streams.print("fanSpeed = " + String(constantFanSpeed));
    
  equation
    sink.airTemperature = constantAirTemperature;
    sink.fanSpeed = HeatsinkBlocks.RPMtoRads(constantFanSpeed);
  end HS483_P14752_ConstantFanSpeed_Interface3DICE;

  model HS483_P14752_VariableFanSpeed_Interface3DICE
    extends HeatsinkBlocks.PartialModels.Interface3DICE(redeclare HS483_P14752_Heatsink sink);
    // This interface models makes the HS483_P14752_Heatsink available to 3D-ICE as an FMU.
    // The fan speed is read from a table file that in the following format:
    // time0 fanSpeedRPM0
    // time1 fanSpeedRPM1
    // ...
    // Make sure the first time point is 0, and the last is greater than the desired simulation time.
    //
    // The fan can be turned off during the simulation by setting the fan speed to 0 to simulate natural convection.

    function ParseAirTemperature
      input String args;
      output Real temperature;
    protected
      Integer length;
      Integer nextIndex;
      Real unused;
    algorithm
      length := Modelica.Utilities.Strings.length(args);
      nextIndex := Modelica.Utilities.Strings.find(args, " "); // Skip past the plugin name
      (unused, nextIndex) := Modelica.Utilities.Strings.scanReal(args, nextIndex); // Skip past the spreader x0
      (unused, nextIndex) := Modelica.Utilities.Strings.scanReal(args, nextIndex); // Skip past the spreader y0
      (temperature, nextIndex) := Modelica.Utilities.Strings.scanReal(args, nextIndex); // Get the temperature
    end ParseAirTemperature;
  
    function ParseFanSpeedFilename
      input String args;
      output String fanSpeedFilename;
    protected
      Integer length;
      Integer nextIndex;
      Real unused;
    algorithm
      length := Modelica.Utilities.Strings.length(args);
      nextIndex := Modelica.Utilities.Strings.find(args, " "); // Skip past the plugin name
      (unused, nextIndex) := Modelica.Utilities.Strings.scanReal(args, nextIndex); // Skip past the spreader x0
      (unused, nextIndex) := Modelica.Utilities.Strings.scanReal(args, nextIndex); // Skip past the spreader y0
      (unused, nextIndex) := Modelica.Utilities.Strings.scanReal(args, nextIndex); // Skip past the temperature
      fanSpeedFilename := Modelica.Utilities.Strings.substring(args, nextIndex+1, length); // Get the fan speed filename
    end ParseFanSpeedFilename;

    parameter Real constantAirTemperature(fixed = false);
    parameter String fanSpeedFilename(fixed = false);
    Modelica.Blocks.Sources.CombiTimeTable fanSpeedTable(tableOnFile = true, fileName = fanSpeedFilename, tableName = "fanSpeed", smoothness = Modelica.Blocks.Types.Smoothness.ConstantSegments);
    
  initial algorithm
    constantAirTemperature := ParseAirTemperature(args);
    fanSpeedFilename := ParseFanSpeedFilename(args);
    Modelica.Utilities.Streams.print("air temperature = " + String(constantAirTemperature));
    Modelica.Utilities.Streams.print("fanSpeedFileName = " + fanSpeedFilename);
    
  equation
    sink.airTemperature = constantAirTemperature;
    sink.fanSpeed = HeatsinkBlocks.RPMtoRads(fanSpeedTable.y[1]);
  end HS483_P14752_VariableFanSpeed_Interface3DICE;
end HS483;