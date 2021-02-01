package NonlinearExample
  import Modelica.SIunits.*;
  import Modelica.Thermal.HeatTransfer.Sources.FixedHeatFlow;
  import Modelica.Thermal.HeatTransfer.Components.ThermalConductor;

  model NonlinearHeatsink    
    extends HeatsinkBlocks.PartialModels.Heatsink(
      bottomLength = L,
      bottomWidth  = L,
      bottomRows   = 2,
      bottomCols   = 2,
      cellBottomConductance = gz
    );
    //Copper
    parameter Length L = 0.04  "Plate length [m]";
    parameter Length t = 0.001 "Plate thickness [m]";
    parameter Density rho             = 8960  "[kg/m^3]";
    parameter SpecificHeatCapacity cp = 384.6 "[J/(kg.K)]";
    parameter ThermalConductivity k   = 401   "[W/(m.K)]";
    parameter HeatCapacity c = cp*rho*(L/2)^2*t;
    parameter ThermalConductance gz  = (L/2)^2/(t/2)*k;
    parameter ThermalConductance gxy = (L/2)*t/(L/4)*k;
    //Air
    parameter Temperature airTemperature = initialTemperature;
    parameter Velocity velocity = 5 "[m/s]";
    parameter Density rhoAir = 1.1;
    parameter SpecificHeatCapacity cpAir = 1020;
    parameter ThermalConductivity kAir = 0.016;
    parameter DynamicViscosity muAir = 1.182e-5;
    CoefficientOfHeatTransfer gamma;
    Real Re, Nu, Pr;
    Temperature T[2,2](each start = initialTemperature);
    Temperature Ttop[2,2] "Top plate temperature";
    Power Qair[2,2];
  equation
    c*der(T[1,1]) = (T[1,2] + T[2,1] - 2*T[1,1])*gxy
                  + bottom[1,1].Q_flow + Qair[1,1];
    c*der(T[1,2]) = (T[1,1] + T[2,2] - 2*T[1,2])*gxy
                  + bottom[1,2].Q_flow + Qair[1,2];
    c*der(T[2,1]) = (T[1,1] + T[2,2] - 2*T[2,1])*gxy
                  + bottom[2,1].Q_flow + Qair[2,1];
    c*der(T[2,2]) = (T[1,2] + T[2,1] - 2*T[2,2])*gxy
                  + bottom[2,2].Q_flow + Qair[2,2];
    Re = rhoAir*velocity*L/muAir "L = characteristic length";
    Nu = gamma*L/kAir;
    Pr = muAir*cpAir/kAir;
    Nu = 0.023*Re^0.8*Pr^0.4 "Dittus-Boelter";
    for i in 1:2 loop
      for j in 1:2 loop
        bottom[i,j].T = T[i,j];
        Qair[i,j] = (Ttop[i,j] - T[i,j])*gz;
        Qair[i,j] = gamma*(L/2)^2*(airTemperature - Ttop[i,j]);
      end for;
    end for;
  end NonlinearHeatsink;
  
  function T2c
    input Temperature T;
    input Real[:] polynomialCoeffs;
    input Temperature Tref;
    output SpecificHeatCapacity c;
algorithm
    c := polynomialCoeffs[1];
    for i in 2:size(polynomialCoeffs, 1) loop
      c := c + polynomialCoeffs[i] * (T - Tref) ^ (i - 1);
    end for;
end T2c;

  model NonlinearHeatsink_Interface3DICE
    extends HeatsinkBlocks.PartialModels.Interface3DICE(redeclare NonlinearHeatsink sink);
  end NonlinearHeatsink_Interface3DICE;

  package Tests
    model ConstantPower
      NonlinearHeatsink sink(initialTemperature = 20 + 273.15,
                             airTemperature     = 20 + 273.15);
      FixedHeatFlow heatSource(Q_flow = 2); //2W being dissipated
    protected
      //Heat sink exposes the center cells directly as the grid pitch
      //mapper requires so. So we need to add them ourselves when
      //simulating the sink in isolation from 3D-ICE
      ThermalConductor bottomConductances[sink.bottomRows, sink.bottomCols]
                                         (each G = sink.cellBottomConductance);
    equation
      for i in 1:sink.bottomRows loop
        for j in 1:sink.bottomCols loop
          // No grid pitch mapping needed, so do a one-to-one mapping
          connect(sink.bottom[i, j], bottomConductances[i, j].port_a);
          connect(bottomConductances[i, j].port_b, heatSource.port);
        end for;
      end for;
    end ConstantPower;
  end Tests;
end NonlinearExample;