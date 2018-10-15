
//This code is just for testing

#include <iostream>
#include "power_reader.h"
#include "power_mapper.h"

using namespace std;

int main()
{
    auto pd=shared_ptr<PowerData>(new FilePowerData("test/testp.csv"));
    auto td=shared_ptr<TemperatureData>(new FileTemperatureData("/dev/null"));
    int gridsize=2;
    AreaProportionalPowerMapper appm(pd,td,"test/test.flp",gridsize);
    cout<<appm.getxChipSize()<<" "<<appm.getyChipSize()<<endl<<endl;
    ModelicaMatrixDouble mmd(gridsize,gridsize);
    for(;;)
    {
        appm.getPowerGrid(mmd);
        for(int r=0;r<gridsize;r++)
        {
            for(int c=0;c<gridsize;c++) cout<<mmd.at(r,c)<<" ";
            cout<<endl;
        }
        cout<<"---"<<endl;
    }
}
