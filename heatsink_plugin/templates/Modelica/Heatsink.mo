package Heatsink
  model TestHeatsink
    extends HeatsinkBlocks.PartialModels.Heatsink(cellBottomConductance = 1 / (bottomRows * bottomCols), bottomLength = 0.02, bottomWidth = 0.02);
    Modelica.Thermal.HeatTransfer.Sources.FixedTemperature Ambient(T = initialTemperature) annotation(
      Placement(visible = true, transformation(origin = {60, 0}, extent = {{10, -10}, {-10, 10}}, rotation = 0)));
  equation
    for i in 1:bottomRows loop
      for j in 1:bottomCols loop
        connect(bottom[i, j], Ambient.port);
      end for;
    end for;
  end TestHeatsink;

  model TestHeatsink_Interface3DICE
extends HeatsinkBlocks.PartialModels.Interface3DICE(redeclare TestHeatsink sink);
  end TestHeatsink_Interface3DICE;
end Heatsink;
