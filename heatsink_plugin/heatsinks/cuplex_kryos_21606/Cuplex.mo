package Cuplex
  package Tests

    model ConstantPowerConstantFlowRate
      Cuplex21606_Heatsink sink(initialTemperature = 20 + 273.15, waterTemperature = 20 + 273.15);
      Modelica.Thermal.HeatTransfer.Sources.FixedHeatFlow heatSource(Q_flow = 75);
    protected
       Modelica.Thermal.HeatTransfer.Components.ThermalConductor bottomConductances[sink.bottomRows,sink.bottomCols](each G = sink.cellBottomConductance);
    equation
      sink.waterFlowRate = HeatsinkBlocks.LPMtoM3s(0.1);
      for i in 1:sink.bottomRows loop
        for j in 1:sink.bottomCols loop
          // No grid pitch mapping needed, so do a one-to-one mapping
          connect(sink.bottom[i, j], bottomConductances[i, j].port_a);
          connect(bottomConductances[i, j].port_b, heatSource.port);
        end for;
      end for;
      annotation(
        experiment(StartTime = 0, StopTime = 100, Tolerance = 1e-6, Interval = 1));
    end ConstantPowerConstantFlowRate;

    model ConstantPowerVariableFlowRate
      Modelica.Blocks.Sources.CombiTimeTable flowRateTable(smoothness = Modelica.Blocks.Types.Smoothness.ConstantSegments,
        table = [   0,  0.10;
                  300,  0.06;
                  600,  0.12;
                 1000,  0.12]);
      Cuplex21606_Heatsink sink(initialTemperature = 20 + 273.15, waterTemperature = 20 + 273.15);
      Modelica.Thermal.HeatTransfer.Sources.FixedHeatFlow heatSource(Q_flow = 75);
    protected
       Modelica.Thermal.HeatTransfer.Components.ThermalConductor bottomConductances[sink.bottomRows,sink.bottomCols](each G = sink.cellBottomConductance);
    equation
      sink.waterFlowRate = HeatsinkBlocks.LPMtoM3s(flowRateTable.y[1]);
      for i in 1:sink.bottomRows loop
        for j in 1:sink.bottomCols loop
          // No grid pitch mapping needed, so do a one-to-one mapping
          connect(sink.bottom[i, j], bottomConductances[i, j].port_a);
          connect(bottomConductances[i, j].port_b, heatSource.port);
        end for;
      end for;
      annotation(
        experiment(StartTime = 0, StopTime = 1000, Tolerance = 1e-6, Interval = 2));
    end ConstantPowerVariableFlowRate;
  end Tests;

  model Cuplex21606_Fin
    extends HeatsinkBlocks.PartialModels.Fin;
    // Thermal model of one fin of the cuplex kryos NEXT water block from aqua computer, part number 21606

    redeclare function extends convectionCorrelation
        input Modelica.SIunits.Velocity fluidVelocity;
        output Modelica.SIunits.CoefficientOfHeatTransfer htc;

      algorithm
        htc := 600 "TODO: compute the correlation, this formula is only here to make the sink compile!!!";
    end convectionCorrelation;
  end Cuplex21606_Fin;

  model Cuplex21606_Heatsink
    extends HeatsinkBlocks.PartialModels.Heatsink(cellBottomConductance = 2 * base.gz, bottomLength = baseLength, bottomWidth = baseWidth, bottomRows = baseRows, bottomCols = baseCols);
    // Thermal model of cuplex kryos NEXT water block from aqua computer, part number 21606
    //
    // The water block has been fitted in the range TODO
    // and tested experimentally in range TODO l/min.
    // DO NOT exceed the fitting range.
    //
    // The fins are oriented in the y (width) direction.
    
    HeatsinkBlocks.LayerOptimized base(cp = cp, rho = rho, k = k, alpha = alpha, length = baseLength, width = baseWidth, height = baseHeight, rows = baseRows, cols = baseCols, Tstart = initialTemperature) annotation(
      Placement(visible = true, transformation(origin = {-10, -30}, extent = {{-10, -10}, {10, 10}}, rotation = 0)));
    
    Cuplex21606_Fin fins[finXelements](each cp = cp, each rho = rho, each k = k, each alpha = alpha, each length = numFinsPerGroup * finHeight, each width = finWidth, each height = finThickness, each rows = finYelements, each cols = finZelements, each Tstart = initialTemperature) annotation(
      Placement(visible = true, transformation(origin = {-10, 10}, extent = {{-10, -10}, {10, 10}}, rotation = 0)));
  
    input Modelica.SIunits.Temperature waterTemperature "Water temperature in [K]";
    input Modelica.SIunits.VolumeFlowRate waterFlowRate "Water flow rate [m^3/s]";
    
  protected
    parameter Modelica.SIunits.SpecificHeatCapacity cp = 384.6 "copper properties";
    parameter Modelica.SIunits.Density rho = 8960 "copper properties";
    parameter Modelica.SIunits.ThermalConductivity k = 401 "copper properties";
    parameter Real alpha = 1 "fitted from experimental data";
    
    parameter Modelica.SIunits.Length baseLength = 0.0528 "measured";
    parameter Modelica.SIunits.Length baseWidth = 0.059 "measured";
    parameter Modelica.SIunits.Length baseHeight = ((basePlateMass - finsMass) / rho ) / (baseLength * baseWidth) "base has a complex shape, computing average height from the base mass";
    
    parameter Integer numFins = 68 "measured";
    parameter Modelica.SIunits.Length finZoneLength = 0.0272 "length of the center zone with all the fins, measured";
    parameter Modelica.SIunits.Length finWidth = 0.025 "width of one fin, measured";
    parameter Modelica.SIunits.Length finHeight = 0.001 "measured";
    parameter Modelica.SIunits.Length finThickness = 0.0002 "thickness of one microfin";
    parameter Modelica.SIunits.Length finPitch = finZoneLength / numFins "pitch of the microfins";
    
    parameter Modelica.SIunits.Mass finsMass = numFins * rho * finHeight * finWidth * finThickness "mass of all fins, computed from dimensions";
    parameter Modelica.SIunits.Mass basePlateMass = 0.0458 "mass of baseplate including fins, measured";
    
    parameter Modelica.SIunits.Length finToWaterBlockDistance = 0.0005 "free space on top of fins for water to flow, fitted from experimental data";
    parameter Modelica.SIunits.Area waterPassageCrossSection = 2 * ((numFins + 1) * (finPitch - finThickness) * finHeight + finZoneLength * finToWaterBlockDistance) "by the water block design, water is injected at the fin middle and divides in two streams";
    
    parameter Integer baseRows = 39 /*59*/ "discretization of sink base in the y direction";
    parameter Integer baseCols = 33 /*66*/ "discretization of sink base in the x direction";
    parameter Integer finXelemApprox = integer(floor(finZoneLength / baseLength * baseCols)) "used to compute number of simulated fins";
    parameter Integer finXelements = if(mod(baseCols - finXelemApprox, 2) == 0) then finXelemApprox else finXelemApprox + 1 "number of simulated fins (adjusted to center the fin zone)";
    parameter Integer numFinsPerGroup = div(numFins, finXelements) "to reduce the model complexity we simulate microfins in groups";
    parameter Integer finYelemApprox = integer(floor(finWidth / baseWidth * baseRows)) "used to compute discretization of fin elements in the y direction";
    parameter Integer finYelements = if(mod(baseRows - finYelemApprox, 2) == 0) then finYelemApprox else finYelemApprox + 1 "discretization of fin elements in the y direction (adjusted for centering)";
    parameter Integer finZelements = 1 "discretization of fin elements in the z direction";
    parameter Integer finXoffset = div((baseCols - finXelements), 2);
    parameter Integer finYoffset = div((baseRows - finYelements), 2);
  
    Modelica.SIunits.Velocity waterVelocity "velocity of water through the fins";
  
  equation
    assert(mod(numFins, finXelements) == 0, "X discretization must divide the number of fins");
    assert(finThickness < finPitch, "Fin too thick for pitch");
    
    waterVelocity = waterFlowRate / waterPassageCrossSection;
    for i in 1:baseRows loop
      for j in 1:baseCols loop
        connect(bottom[i, j], base.pGen.port[i, j]);
      end for;
    end for;
    for i in 1:finYelements loop
      for j in 1:finXelements loop
        connect(base.top.port[i + finYoffset, j + finXoffset], fins[j].bottom.port[i]);
      end for;
    end for;
    for j in 1:finXelements loop
      fins[j].fluidVelocity = waterVelocity;
      fins[j].inletFluidTemperature = waterTemperature;
    end for;
  end Cuplex21606_Heatsink;

  model Cuplex21606_ConstantFlowRate_Interface3DICE
    extends HeatsinkBlocks.PartialModels.Interface3DICE(redeclare Cuplex21606_Heatsink sink);
    // This interface models makes the Cuplex21606_Heatsink available to 3D-ICE as an FMU.
    // The water inlet temperature and flow rate are constant throughout the entire simulation and are set
    // in the 3D-ICE stk file as the fourth (temperature) and fifth (flow rate) parameters of the heatisnk.
    // The water flow rate in the table file is in liters per minute, while the inlet temperature in K.

    function ParseWaterData
      input String args;
      output Real temperature;
      output Real flowRate;
    protected
      Integer nextIndex;
      Real unused;
    algorithm
      nextIndex := Modelica.Utilities.Strings.find(args, " "); // Skip past the plugin name
      (unused, nextIndex) := Modelica.Utilities.Strings.scanReal(args, nextIndex); // Skip past the spreader x0
      (unused, nextIndex) := Modelica.Utilities.Strings.scanReal(args, nextIndex); // Skip past the spreader y0
      (temperature, nextIndex) := Modelica.Utilities.Strings.scanReal(args, nextIndex); // Get the temperature
      (flowRate, nextIndex) := Modelica.Utilities.Strings.scanReal(args, nextIndex); // Get the flow rate
    end ParseWaterData;

    parameter Real constantWaterTemperature(fixed = false);
    parameter Real constantWaterFlowRate(fixed = false);
    
  initial algorithm
    (constantWaterTemperature, constantWaterFlowRate) := ParseWaterData(args);
    Modelica.Utilities.Streams.print("water temperature = " + String(constantWaterTemperature));
    Modelica.Utilities.Streams.print("water flow rate = " + String(constantWaterFlowRate));
    
  equation
    sink.waterTemperature = constantWaterTemperature;
    sink.waterFlowRate = HeatsinkBlocks.LPMtoM3s(constantWaterFlowRate);
  end Cuplex21606_ConstantFlowRate_Interface3DICE;

  model Cuplex21606_VariableFlowRate_Interface3DICE
extends HeatsinkBlocks.PartialModels.Interface3DICE(redeclare Cuplex21606_Heatsink sink);
    // This interface models makes the Cuplex21606_Heatsink available to 3D-ICE as an FMU.
    // The fourth parameter in the stk file is the (constant) water inlet temperature, the fifth one is
    // the table file name for the water flow rate.
    // The water flow rate is read from a table file that in the following format:
    // time0 waterFlowRate0
    // time1 waterFlowRate1
    // ...
    // Make sure the first time point is 0, and the last is greater than the desired simulation time.
    // The water flow rate in the table file is in liters per minute, while the inlet temperature in K.
  
    function ParseWaterTemperature
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
    end ParseWaterTemperature;
  
    function ParseWaterData
      input String args;
      output String flowRateFilename;
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
      flowRateFilename := Modelica.Utilities.Strings.substring(args, nextIndex+1, length); // Get the flow rate filename
    end ParseWaterData;
  
    parameter Real constantWaterTemperature(fixed = false);
    parameter String flowRateFilename(fixed = false);
    Modelica.Blocks.Sources.CombiTimeTable flowRateTable(tableOnFile = true, fileName = flowRateFilename, tableName = "flowRate", smoothness = Modelica.Blocks.Types.Smoothness.ConstantSegments);
    
  initial algorithm
    constantWaterTemperature := ParseWaterTemperature(args);
    flowRateFilename := ParseWaterData(args);
    Modelica.Utilities.Streams.print("water temperature = " + String(constantWaterTemperature));
    Modelica.Utilities.Streams.print("flow rate file name = " + flowRateFilename);
    
  equation
    sink.waterTemperature = constantWaterTemperature;
    sink.waterFlowRate = HeatsinkBlocks.LPMtoM3s(flowRateTable.y[1]);
  end Cuplex21606_VariableFlowRate_Interface3DICE;
end Cuplex;