/******************************************************************************
 * This file is part of 3D-ICE, version 3.1.0 .                               *
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
 *                             Copyright (C) 2021                             *
 *   Embedded Systems Laboratory - Ecole Polytechnique Federale de Lausanne   *
 *                            All Rights Reserved.                            *
 *                                                                            *
 * Authors: Arvind Sridhar              Alessandro Vincenzi                   *
 *          Giseong Bak                 Martino Ruggiero                      *
 *          Thomas Brunschwiler         Eder Zulian                           *
 *          Federico Terraneo           Darong Huang                          *
 *          Luis Costero                Marina Zapater                        *
 *          David Atienza                                                     *
 *                                                                            *
 * For any comment, suggestion or request  about 3D-ICE, please  register and *
 * write to the mailing list (see http://listes.epfl.ch/doc.cgi?liste=3d-ice) *
 * Any usage  of 3D-ICE  for research,  commercial or other  purposes must be *
 * properly acknowledged in the resulting products or publications.           *
 *                                                                            *
 * EPFL-STI-IEL-ESL                     Mail : 3d-ice@listes.epfl.ch          *
 * Batiment ELG, ELG 130                       (SUBSCRIPTION IS NECESSARY)    *
 * Station 11                                                                 *
 * 1015 Lausanne, Switzerland           Url  : http://esl.epfl.ch/3d-ice      *
 ******************************************************************************/

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

