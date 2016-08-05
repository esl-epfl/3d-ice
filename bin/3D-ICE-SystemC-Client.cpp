/*
 * Copyright (c) 2015, University of Kaiserslautern
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER
 * OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Authors: Matthias Jung
 *          Éder F. Zulian
 */

#include <iostream>
#include <systemc.h>
#include <string>
#include <fstream>
#include <stdlib.h>

#include "network_socket.h"
#include "network_message.h"
#include "IceWrapper.h"

#define NARGC           3
#define EXE_NAME        argv[0]
#define SERVER_IP       argv[1]
#define SERVER_PORT     argv[2]

static std::string ServerIp;
static unsigned int ServerPort;

SC_MODULE(YourSimulator)
{
    IceWrapper *thermalSimulation;
    SC_CTOR(YourSimulator) {
        thermalSimulation = new IceWrapper(ServerIp, ServerPort);
        SC_THREAD(process);
    }

    void process()
    {
        std::vector<float> powerValues = {0, // 1: CPUs
                                          0, // 2: GPU
                                          1, // 3: BASEBAND1
                                          0, // 4: BASEBAND2
                                          0, // 5: LLCACHE
                                          0, // 6: DRAMCTRL1
                                          0, // 7: DRAMCTRL2
                                          0, // 8: TSVs
                                          0, // 9: ACELLERATORS
                                          1, //10: C0 
                                          0, //11: C1
                                          0, //12: C2
                                          0, //13: C3
                                          0, //14: TSVs
                                          0, //10: C0 
                                          0, //11: C1
                                          0, //12: C2
                                          0, //13: C3
                                          0, //14: TSVs
                                          0, //10: C0 
                                          0, //11: C1
                                          0, //12: C2
                                          0, //13: C3
                                          0, //14: TSVs
                                          0, //10: C0 
                                          0, //11: C1
                                          0, //12: C2
                                          0, //13: C3
                                          0  //14: TSVs
                                         };
        while(true)
        {
            wait(sc_time(50,SC_MS));

            if(sc_time_stamp() >= sc_time(1,SC_SEC))
            {
                powerValues.at(1) = 5;
            } 
            if(sc_time_stamp() >= sc_time(2,SC_SEC))
            {
                sc_stop();
            }

            std::vector<float> temperatureValues;

            thermalSimulation->sendPowerValues(&powerValues);
            thermalSimulation->simulate();
            thermalSimulation->getTemperature(temperatureValues,
                                              TDICE_OUTPUT_INSTANT_SLOT,
                                              TDICE_OUTPUT_TYPE_TCELL,
                                              TDICE_OUTPUT_QUANTITY_NONE);

            // Visualize this file within octave:
            //      imagesc(flipud(load("temperature_map.txt")))
            thermalSimulation->getTemperatureMap("temperature_map.txt");
            thermalSimulation->getPowerMap("power_map.txt");

            std::cout << "@" << sc_time_stamp() 
                      << "\tTemperature=" << temperatureValues.at(0) << endl;

        }
    }

};

int sc_main(int argc, char *argv[])
{
    std::cout << "\n\t2015 University of Kaiserslautern" << std::endl;

    if (argc != NARGC) {
        std::cerr << "\n\tUsage: " << EXE_NAME << " <server IP> <server port>" << std::endl;
        exit(EXIT_FAILURE);
    }

    ServerIp = SERVER_IP;
    ServerPort = atoi(SERVER_PORT);

    YourSimulator("YourSimulator");
    sc_start();

    return 0;
}

