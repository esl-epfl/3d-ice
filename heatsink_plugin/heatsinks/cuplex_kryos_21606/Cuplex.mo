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
  extends ThermalBlocks.PartialModels.Layer;
    // Thermal model of one fin of the cuplex kryos NEXT water block from aqua computer, part number 21606
    
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
    
    function convectionCorrelation
      input Modelica.SIunits.Velocity fluidVelocity;
      output Modelica.SIunits.CoefficientOfHeatTransfer htc;
      
    protected
      // Cubic fit of the water velocity to heat transfer coefficient
      // for cuplex kryos NEXT water block, fitted from experimental data
      // Only valid for water velocities in the range 0.01 to 0.04m/s
      parameter Real c0 = -4634;
      parameter Real c1 = 738.4e3;
      parameter Real c2 = -33.58e6;
      parameter Real c3 = 5.371e8;
  
    algorithm
      assert(fluidVelocity >= 0.014 and fluidVelocity <= 0.031, "Assertion failed: HTC correlation outside fitting range");
      htc := c0 + c1 * fluidVelocity + c2 * fluidVelocity ^ 2 + c3 * fluidVelocity ^ 3;
      
      //htc := 1800 "for 0.12l/min 0.03m/s";
      //htc := 1230 "for 0.1l/min 0.025m/s";
      //htc := 1000 "for 0.08l/min 0.02m/s";
      //htc :=  700 "for 0.06l/min 0.015m/s";
      
    end convectionCorrelation;
    
    Modelica.SIunits.CoefficientOfHeatTransfer htc;
    Modelica.SIunits.ThermalConductance gf[rows];
    //Index                          1   2   3   4   5   6   7   8   9  10  11  12  13  14  15  16  17
    parameter Real htcPattern[17]={ -1, -1, -1, -1, -1, -1, -1,  0,  1,  1,  1,  1,  1,  1,  1,  1,  1};
    parameter Real htcFactor=0.5;
    parameter Real htcMultiplier=1;
  
  equation
    assert(rows==17, "htc nonuniform mapping only computed for rows==17");
    htc = htcMultiplier * convectionCorrelation(fluidVelocity);
    
    for i in 1:rows loop
      gf[i] = 2 * (htc * (1 + htcPattern[i] * htcFactor) * l * w * gz) / (htc * (1 + htcPattern[i] * htcFactor) * l * w + gz); //2 because both fin faces exchange convectively
      bottom.port[i].Q_flow = 2 * gx * (bottom.port[i].T - T[i, 1]);
    end for;
    
    if rows > 1 and cols > 1 then
      c * der(T[1, 1]) = gx * (T[1, 2] - T[1, 1]) 
                       + gy * (T[2, 1] - T[1, 1])
                       + gf[1] * (inletFluidTemperature - T[1, 1])
                       + bottom.port[1].Q_flow;
      
      c * der(T[1, cols]) = gx * (T[1, cols - 1] - T[1, cols]) 
                          + gy * (T[2, cols] - T[1, cols]) 
                          + gf[1] * (inletFluidTemperature - T[1, cols]);
      
      c * der(T[rows, 1]) = gx * (T[rows, 2] - T[rows, 1])
                          + gy * (T[rows - 1, 1] - T[rows, 1])
                          + gf[rows] * (inletFluidTemperature - T[rows, 1])
                          + bottom.port[rows].Q_flow;
    
      c * der(T[rows, cols]) = gx * (T[rows, cols - 1] - T[rows, cols])
                             + gy * (T[rows - 1, cols] - T[rows, cols])
                             + gf[rows] * (inletFluidTemperature - T[rows, cols]);
    elseif rows > 1 /*and cols == 1*/ then
      c * der(T[1, 1]) = gy * (T[2, 1] - T[1, 1])
                       + gf[1] * (inletFluidTemperature - T[1, 1])
                       + bottom.port[1].Q_flow;
    
      c * der(T[rows, 1]) = gy * (T[rows - 1, 1] - T[rows, 1])
                          + gf[rows] * (inletFluidTemperature - T[rows, 1])
                          + bottom.port[rows].Q_flow;
    elseif cols > 1 /*and rows == 1*/ then
      c * der(T[1, 1]) = gx * (T[1, 2] - T[1, 1]) 
                       + gf[1] * (inletFluidTemperature - T[1, 1])
                       + bottom.port[1].Q_flow;
                       
      c * der(T[1, cols]) = gx * (T[1, cols - 1] - T[1, cols]) 
                          + gf[1] * (inletFluidTemperature - T[1, cols]);
    else /* cols == 1 and rows == 1 */
      c * der(T[1, 1]) = + gf[1] * (inletFluidTemperature - T[1, 1])
                         + bottom.port[1].Q_flow;
    end if;
    
    for i in 2:rows - 1 loop
      if cols > 1 then
        c * der(T[i, 1]) = gx * (T[i, 2] - T[i, 1])
                         + gy * (T[i - 1, 1] + T[i + 1, 1] - 2 * T[i, 1])
                         + gf[i] * (inletFluidTemperature - T[i, 1])
                         + bottom.port[i].Q_flow;
                       
        c * der(T[i, cols]) = gx * (T[i, cols - 1] - T[i, cols])
                            + gy * (T[i - 1, cols] + T[i + 1, cols] - 2 * T[i, cols])
                            + gf[i] * (inletFluidTemperature - T[i, cols]);
      else /* cols == 1 */
        c * der(T[i, 1]) = gy * (T[i - 1, 1] + T[i + 1, 1] - 2 * T[i, 1])
                         + gf[i] * (inletFluidTemperature - T[i, 1])
                         + bottom.port[i].Q_flow;
      end if;
    end for;
    
    for j in 2:cols - 1 loop
      if rows > 1 then
        c * der(T[1, j]) = gx * (T[1, j - 1] + T[1, j + 1] - 2 * T[1, j])
                         + gy * (T[2, j] - T[1, j])
                         + gf[1] * (inletFluidTemperature - T[1, j]);
        c * der(T[rows, j]) = gx * (T[rows, j - 1] + T[rows, j + 1] - 2 * T[rows, j])
                            + gy * (T[rows - 1, j] - T[rows, j])
                            + gf[rows] * (inletFluidTemperature - T[rows, j]);
      else /* rows == 1 */
        c * der(T[1, j]) = gx * (T[1, j - 1] + T[1, j + 1] - 2 * T[1, j])
                         + gf[1] * (inletFluidTemperature - T[1, j]);
      end if;
    end for;
    
    for i in 2:rows - 1 loop
      for j in 2:cols - 1 loop
        c * der(T[i, j]) = gx * (T[i, j - 1] + T[i, j + 1] - 2 * T[i, j])
                         + gy * (T[i - 1, j] + T[i + 1, j] - 2 * T[i, j])
                         + gf[i] * (inletFluidTemperature - T[i, j]);
      end for;
    end for;
  end Cuplex21606_Fin;

  model Cuplex21606_Heatsink
    extends HeatsinkBlocks.PartialModels.Heatsink(cellBottomConductance = 2 * base.gz, bottomLength = baseLength, bottomWidth = baseWidth, bottomRows = baseRows, bottomCols = baseCols);
    // Thermal model of cuplex kryos NEXT water block from aqua computer, part number 21606
    //
    // The water block has been fitted in the range 0.06 to 0.12 l/min.
    // Power to be dissipated should be limited to less than:
    //  20W, for a 0.06l/min water flow
    //  70W, for a 0.08l/min water flow
    //  80W, for higher water flow rates
    // DO NOT exceed the fitting range.
    //
    // The fins are oriented in the y (width) direction.
    
    HeatsinkBlocks.LayerOptimized base(cp = cp, rho = rho, k = k, alpha = alpha, length = baseLength, width = baseWidth, height = baseHeight, rows = baseRows, cols = baseCols, Tstart = initialTemperature) annotation(
      Placement(visible = true, transformation(origin = {-10, -30}, extent = {{-10, -10}, {10, 10}}, rotation = 0)));
    
    Cuplex21606_Fin fins[finXelements](each cp = cp, each rho = rho, each k = k, each alpha = alpha, each length = finHeight, each width = finWidth, each height = finThickness * numFinsPerGroup, each rows = finYelements, each cols = finZelements, each Tstart = initialTemperature, each htcMultiplier = numFinsPerGroup) annotation(
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
    parameter Modelica.SIunits.Length baseHeight = 0.65 * (basePlateMass / rho ) / (baseLength * baseWidth) "base has a complex shape, computing average height from the base mass, adjusted as there are difficult to measure deep cuts in the base";
    parameter Modelica.SIunits.Length totalHeight = 0.0025 "measured";
    
    parameter Integer numFins = 68 "measured";
    parameter Modelica.SIunits.Length finZoneLength = 0.0272 "length of the center zone with all the fins, measured";
    parameter Modelica.SIunits.Length finWidth = 0.025 "width of one fin, measured";
    parameter Modelica.SIunits.Length finHeight = totalHeight - baseHeight "height of one fin";
    parameter Modelica.SIunits.Length finThickness = 0.0002 "thickness of one microfin";
    parameter Modelica.SIunits.Length finPitch = finZoneLength / numFins "pitch of the microfins";
    
    parameter Modelica.SIunits.Mass basePlateMass = 0.0458 "mass of baseplate including fins, measured";
    
    parameter Modelica.SIunits.Length finToWaterBlockDistance = 0.0005 "free space on top of fins for water to flow, fitted from experimental data";
    parameter Modelica.SIunits.Area waterPassageCrossSection = 2 * ((numFins + 1) * (finPitch - finThickness) * finHeight + finZoneLength * finToWaterBlockDistance) "by the water block design, water is injected at the fin middle and divides in two streams";
    
    parameter Integer baseRows = 39 "discretization of sink base in the y direction";
    parameter Integer baseCols = 33 "discretization of sink base in the x direction";
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
