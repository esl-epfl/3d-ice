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

#include <stdlib.h>
#include "IceWrapper.h"

IceWrapper::IceWrapper(std::string serverIp, unsigned int portNumber)
{
    SC_REPORT_INFO("3D-ICE","Starting thermal simulation");
    // Configure Connection
    serverIP = serverIp;
    serverPort = portNumber;

    if(openConnection() == false)
    {
        SC_REPORT_FATAL("3D-ICE","Cannot connect to thermal simulation");
        exit(EXIT_FAILURE);
    }

    // Get information about system architecture:
    numberOfFloorplanElements = getNumberOfFloorplanElements();
}

IceWrapper::~IceWrapper()
{
    if(closeConnection() == false)
    {
        SC_REPORT_FATAL("3D-ICE","Cannot close connection to thermal simulation");
        exit(EXIT_FAILURE);
    }
}

bool IceWrapper::openConnection()
{
    socket_init(&client_socket);
    if (open_client_socket(&client_socket) != TDICE_SUCCESS)
    {
        return false ;
    }

    // Convert string to char*:
    char * ip = new char[serverIP.size() + 1];
    std::copy(serverIP.begin(), serverIP.end(), ip);
    ip[serverIP.size()] = '\0';

    if (connect_client_to_server(&client_socket, ip , serverPort) != TDICE_SUCCESS)
    {
        return false ;
    }
    return true;
}

bool IceWrapper::closeConnection()
{
    return (socket_close(&client_socket) == TDICE_SUCCESS);
}

unsigned int IceWrapper::getNumberOfFloorplanElements()
{
    unsigned int value;

    network_message_init(&client_nflp) ;
    build_message_head(&client_nflp, TDICE_TOTAL_NUMBER_OF_FLOORPLAN_ELEMENTS);
    send_message_to_socket(&client_socket, &client_nflp);
    receive_message_from_socket(&client_socket, &client_nflp);
    extract_message_word(&client_nflp, &value, 0);
    network_message_destroy(&client_nflp);

    // Debug
    cout << "Number of Floorplan Elements: " << value << endl;
    return value;
}

void IceWrapper::sendPowerValues(std::vector<float> * powerValues)
{
    if(powerValues->size() != numberOfFloorplanElements)
    {
        SC_REPORT_FATAL("3D-ICE","Wrong number of power numbers");
    }
    network_message_init (&client_powers) ;
    build_message_head   (&client_powers, TDICE_INSERT_POWERS) ;
    insert_message_word  (&client_powers, &numberOfFloorplanElements) ;

    float power;
    for (unsigned int i = 0 ; i != numberOfFloorplanElements ; i++)
    {
        power = powerValues->at(i);
        insert_message_word (&client_powers, &power) ;
    }

    send_message_to_socket (&client_socket, &client_powers);
    network_message_destroy (&client_powers);

    // Get result from Thermal Simulator (BLOCKING)
    network_message_init (&server_reply);
    receive_message_from_socket (&client_socket, &server_reply);
    Error_t error;
    extract_message_word (&server_reply, &error, 0);

    if (error != TDICE_SUCCESS)
    {
        network_message_destroy (&server_reply);
        closeConnection();
        SC_REPORT_FATAL("3D-ICE","Cannot send power values");
    }
    network_message_destroy (&server_reply);
}

void IceWrapper::simulate()
{
    network_message_init (&client_simulate) ;
    build_message_head   (&client_simulate, TDICE_SIMULATE_SLOT) ;
    send_message_to_socket (&client_socket, &client_simulate) ;
    network_message_destroy (&client_simulate) ;

    // Wait for Simulation Result (BLOCKING)
    network_message_init (&server_reply) ;
    receive_message_from_socket (&client_socket, &server_reply) ;
    SimResult_t sim_result ;
    extract_message_word (&server_reply, &sim_result, 0) ;

    if (sim_result != TDICE_SLOT_DONE)
    {
        network_message_destroy (&server_reply) ;
        closeConnection();
        SC_REPORT_FATAL("3D-ICE","Cannot send power values");
    }
    network_message_destroy (&server_reply) ;
}

void IceWrapper::getTemperature(std::vector<float> &TemperatureValues, OutputInstant_t instant, OutputType_t type, OutputQuantity_t quantity)
{
    network_message_init(&client_temperatures) ;
    build_message_head(&client_temperatures, TDICE_SEND_OUTPUT) ;
    insert_message_word(&client_temperatures, &instant) ;
    insert_message_word(&client_temperatures, &type) ;
    insert_message_word(&client_temperatures, &quantity) ;

    send_message_to_socket (&client_socket, &client_temperatures) ;

    network_message_destroy (&client_temperatures) ;

    // Receive Temperatures:

    network_message_init (&server_reply) ;

    receive_message_from_socket (&client_socket, &server_reply) ;

    double time = 0;
    unsigned int nresults;

    extract_message_word (&server_reply, &time,     0) ;
    extract_message_word (&server_reply, &nresults, 1) ;

    // Check if the SystemC time is almost aligned (+-1ns) with 3D-ICE time 
    if(   sc_time(time, SC_SEC) >= (sc_time_stamp()+sc_time(1, SC_NS))
            && sc_time(time, SC_SEC) <= (sc_time_stamp()-sc_time(1, SC_NS)))
    {
        fprintf (stdout, "%5.9e sec : \t", time) ;
        std::cout << " @" << sc_time_stamp() << ":" << sc_time(time, SC_SEC) << endl;
        SC_REPORT_FATAL("3D-ICE","The time of 3D-ICE is not in sync with the current SystemC time");
    }

    nresults += 2;
    for(unsigned int i = 2; i != nresults ; i++)
    {
        float temperature = 0;
        extract_message_word (&server_reply, &temperature, i) ;
        TemperatureValues.push_back(temperature);
    }
}

void IceWrapper::getMap(OutputType_t type, std::string filename)
{
    // Send request to 3D-ICE:

    OutputInstant_t instant = TDICE_OUTPUT_INSTANT_SLOT ;
    OutputQuantity_t quantity = TDICE_OUTPUT_QUANTITY_NONE ;

    network_message_init (&client_tmap) ;
    build_message_head   (&client_tmap, TDICE_SEND_OUTPUT) ;
    insert_message_word  (&client_tmap, &instant) ;
    insert_message_word  (&client_tmap, &type) ;
    insert_message_word  (&client_tmap, &quantity) ;

    send_message_to_socket (&client_socket, &client_tmap) ;

    network_message_destroy (&client_tmap) ;

    // Open file
    ofstream myfile;
    myfile.open (filename, std::fstream::in | std::fstream::out | std::fstream::app);

    // Receives the map:

    float temperature = 0;
    unsigned int nresults;
    unsigned int nrows;
    unsigned int ncolumns;
    double time = 0;

    network_message_init (&server_reply) ;

    receive_message_from_socket (&client_socket, &server_reply) ;

    extract_message_word (&server_reply, &time,     0) ;
    extract_message_word (&server_reply, &nresults, 1) ;
    extract_message_word (&server_reply, &nrows,    2) ;
    extract_message_word (&server_reply, &ncolumns, 3) ;

    unsigned int index = 4;
    for (unsigned int i = 0; i < nresults; i++)
    {
        for (unsigned int r = 0; r < nrows; r++)
        {
            for (unsigned int c = 0; c < ncolumns; c++)
            {
                index++;
                extract_message_word (&server_reply, &temperature, index) ;
                myfile << temperature << " ";
            }
            myfile << endl;
        }
        myfile << endl;
    }

    network_message_destroy (&server_reply) ;
    myfile.close();
}

void IceWrapper::getTemperatureMap(std::string filename)
{
    getMap(TDICE_OUTPUT_TYPE_TMAP, filename);
}

void IceWrapper::getPowerMap(std::string filename)
{
    getMap(TDICE_OUTPUT_TYPE_PMAP, filename);
}

