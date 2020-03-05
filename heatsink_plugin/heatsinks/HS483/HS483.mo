package HS483
  package PartialModels
    partial model HS483
    extends HeatsinkBlocks.PartialModels.Heatsink(cellBottomConductance = 2 * base.gz, bottomLength = baseLength, bottomWidth = baseWidth, bottomRows = baseRows, bottomCols = baseCols);
      // Base class from which both the natural and forced convection models
      // for the copper heatsink model number HS483-ND derive.
      HeatsinkBlocks.LayerOptimized base(cp = cp, rho = rho, k = k, alpha = alpha, length = baseLength, width = baseWidth, height = baseHeight, rows = baseRows, cols = baseCols, Tstart = ambientTemperature) annotation(
        Placement(visible = true, transformation(origin = {-10, -30}, extent = {{-10, -10}, {10, 10}}, rotation = 0)));
      
    protected
      parameter Modelica.SIunits.SpecificHeatCapacity cp = 384.6 "copper properties";
      parameter Modelica.SIunits.Density rho = 8960 "copper properties";
      parameter Modelica.SIunits.ThermalConductivity k = 401 "copper properties";
      parameter Real alpha = 1;
      parameter Modelica.SIunits.Length baseLength = 0.06 "HS483 dimensions";
      parameter Modelica.SIunits.Length baseWidth = 0.06 "HS483 dimensions";
      parameter Modelica.SIunits.Length baseHeight = 0.0028 "HS483 dimensions";
      parameter Modelica.SIunits.Length finHeight = 0.0192 "HS483 dimensions";
      parameter Modelica.SIunits.Length finThickness = 0.0005 "HS483 dimensions";
      parameter Integer baseRows = 23 "discretization of sink base in the y direction";
      parameter Integer baseCols = 23 "discretization of sink base in the x direction, also number of fins";
      parameter Integer finZelements = 2/*1*/ "discretization of fin elements in the z direction";
    
    equation
      for i in 1:baseRows loop
        for j in 1:baseCols loop
          connect(bottom[i, j], base.pGen.port[i, j]);
        end for;
      end for;
    end HS483;
  end PartialModels;

  package Specializations
    model HS483_FinForcedConvection
    extends HeatsinkBlocks.PartialModels.Fin;

      redeclare function extends convectionCorrelation
          input Modelica.SIunits.Velocity airVelocity;
          output Modelica.SIunits.CoefficientOfHeatTransfer htc;

        protected
          // Quadratic fit of the air velocity to heat transfer coefficient
          // for copper heatsink model number HS483-ND, fitted from datasheet data
          // Only valid for air velocities in the range 1 to 4.1m/s.
          parameter Real c0 = 11.8;
          parameter Real c1 = 9.33;
          parameter Real c2 = -1.09;

        algorithm
          assert(airVelocity >= 1 and airVelocity<= 4.1, "Assertion failed: HTC correlation outside fitting range");
          htc := c0 + c1 * airVelocity + c2 * airVelocity ^ 2;
      end convectionCorrelation;
    end HS483_FinForcedConvection;

    model HS483_FinNaturalConvection
    extends HeatsinkBlocks.PartialModels.Fin;
    
      redeclare function extends convectionCorrelation
          input Modelica.SIunits.Velocity airVelocity;
          output Modelica.SIunits.CoefficientOfHeatTransfer htc;
    
        algorithm
          htc := 2.62 "HS483-ND natural convection heat transfer coefficient, from datasheet";
      end convectionCorrelation;

    end HS483_FinNaturalConvection;

    model HS483_P14752_ForcedConvection_Interface3DICE
    extends HeatsinkBlocks.PartialModels.Interface3DICE(redeclare HS483_P14752_ForcedConvection sink);
    
      function ParseFanSpeed
        input String args;
        output Real fanSpeed;
      protected
        Integer nextIndex;
        Real unused;
      algorithm
        nextIndex := Modelica.Utilities.Strings.find(args, " ");                       // Skip past the plugin name
        (unused, nextIndex)   := Modelica.Utilities.Strings.scanReal(args, nextIndex); // Skip past the spreader x0
        (unused, nextIndex)   := Modelica.Utilities.Strings.scanReal(args, nextIndex); // Skip past the spreader y0
        (fanSpeed, nextIndex) := Modelica.Utilities.Strings.scanReal(args, nextIndex); // Get the fan speed
      end ParseFanSpeed;
    
      parameter Real constantFanSpeed(fixed = false);
    
    initial algorithm
      constantFanSpeed := ParseFanSpeed(args); // Get fan speed form 3D-ICE
      Modelica.Utilities.Streams.print("fanSpeed = " + String(constantFanSpeed));
    
equation
      sink.fanSpeed = constantFanSpeed / 9.55; //9.55 is the conversion factor from RPM to rad/s
    end HS483_P14752_ForcedConvection_Interface3DICE;

    model HS483_NaturalConvection_Interface3DICE
extends HeatsinkBlocks.PartialModels.Interface3DICE(redeclare HS483_NaturalConvection sink);
    end HS483_NaturalConvection_Interface3DICE;
  end Specializations;

  package Tests
    model SteadyStateNaturalConvection
      HS483_NaturalConvection sink(ambientTemperature = 20 + 273.15);
      Modelica.Thermal.HeatTransfer.Sources.FixedHeatFlow heatSource(Q_flow = 10);
    equation
      for i in 1:sink.bottomRows loop
        for j in 1:sink.bottomCols loop
          connect(sink.bottom[i, j], heatSource.port);
        end for;
      end for;
      annotation(
        experiment(StartTime = 0, StopTime = 4000, Tolerance = 1e-6, Interval = 10));
    end SteadyStateNaturalConvection;
    
    model SteadyStateForcedConvection
  HS483_P14752_ForcedConvection sink(ambientTemperature = 20 + 273.15);
      Modelica.Thermal.HeatTransfer.Sources.FixedHeatFlow heatSource(Q_flow = 75);
    equation
      sink.fanSpeed = 2700 / 9.55; //9.55 is the conversion factor from RPM to rad/s
      for i in 1:sink.bottomRows loop
        for j in 1:sink.bottomCols loop
          connect(sink.bottom[i, j], heatSource.port);
        end for;
      end for;
      annotation(
        experiment(StartTime = 0, StopTime = 400, Tolerance = 1e-6, Interval = 1));
    end SteadyStateForcedConvection;

    model VariableSpeedForcedConvection
      Modelica.Blocks.Sources.CombiTimeTable fanSpeedTable(smoothness=Modelica.Blocks.Types.Smoothness.ConstantSegments,
       table=[   0, 1500;
               300, 2000;
               600, 2500;
              1000, 2500]);
      HS483_P14752_ForcedConvection sink(ambientTemperature = 20 + 273.15);
      Modelica.Thermal.HeatTransfer.Sources.FixedHeatFlow heatSource(Q_flow = 75);
    equation
      sink.fanSpeed = fanSpeedTable.y[1] / 9.55; //9.55 is the conversion factor from RPM to rad/s
      for i in 1:sink.bottomRows loop
        for j in 1:sink.bottomCols loop
          connect(sink.bottom[i, j], heatSource.port);
        end for;
      end for;
    annotation(
        experiment(StartTime = 0, StopTime = 1000, Tolerance = 1e-6, Interval = 2));
    end VariableSpeedForcedConvection;
  end Tests;

  model HS483_P14752_ForcedConvection
    extends PartialModels.HS483;
    // Thermal model of copper heatsink model number HS483-ND
    // with attached fan model number P14752-ND.
    // Fan and heatsink have been fitted in the range 900 to 3500RPM
    // and tested experimentally in range 1500 to 2700RPM (157 to 283rad/s).
    // DO NOT exceed the fitting range.
    // The silicone thermal pad at the bottom of the heatsink is not simulated
    // in Modelica, but in 3D-ICE as the heat spreader layer.
    // The fins are oriented in the y (width) direction.
    Specializations.HS483_FinForcedConvection fins[baseCols](each cp = cp, each rho = rho, each k = k, each alpha = alpha, each length = finHeight, each width = baseWidth, each height = finThickness, each rows = baseRows, each cols = finZelements, each Tstart = ambientTemperature, each inletFluidTemperature = ambientTemperature) annotation(
      Placement(visible = true, transformation(origin = {-10, 10}, extent = {{-10, -10}, {10, 10}}, rotation = 0)));
    HeatsinkBlocks.Fan fan(flowRateCoefficient = fanFlowRateCoefficient, operatingPoint = operatingPoint, area = fanArea) annotation(
      Placement(visible = true, transformation(origin = {-10, 50}, extent = {{-10, -10}, {10, 10}}, rotation = 0)));
    input Modelica.SIunits.AngularVelocity fanSpeed;
  protected
    parameter Real fanFlowRateCoefficient = 5.98e-5 "P14752 parameter, fitted from datasheet";
    parameter Real operatingPoint = 0.9 "P14752 - HS483 interaction, fitted from experimental data";
    parameter Modelica.SIunits.Area fanArea = 4.82e-3 "P14752 parameter, fitted from experimental data starting from datasheet";
  equation
    for i in 1:baseRows loop
      for j in 1:baseCols loop
        connect(base.top.port[i, j], fins[j].bottom.port[i]);
      end for;
    end for;
    for j in 1:baseCols loop
      fins[j].fluidVelocity = fan.airVelocity;
    end for;
    fan.fanSpeed = fanSpeed;
  end HS483_P14752_ForcedConvection;

  model HS483_NaturalConvection
    extends PartialModels.HS483;
    // Thermal model of copper heatsink model number HS483-ND dissipating through natural convection.
    // The silicone thermal pad at the bottom of the heatsink is not simulated
    // in Modelica, but in 3D-ICE as the heat spreader layer.
    // The fins are oriented in the y (width) direction.
    Specializations.HS483_FinNaturalConvection fins[baseCols](each cp = cp, each rho = rho, each k = k, each alpha = alpha, each length = finHeight, each width = baseWidth, each height = finThickness, each rows = baseRows, each cols = finZelements, each Tstart = ambientTemperature, each inletFluidTemperature = ambientTemperature) annotation(
      Placement(visible = true, transformation(origin = {-10, 10}, extent = {{-10, -10}, {10, 10}}, rotation = 0)));
  equation
    for i in 1:baseRows loop
      for j in 1:baseCols loop
        connect(base.top.port[i, j], fins[j].bottom.port[i]);
      end for;
    end for;
    for j in 1:baseCols loop
      fins[j].fluidVelocity = 0;
    end for;
  
  end HS483_NaturalConvection;

end HS483;