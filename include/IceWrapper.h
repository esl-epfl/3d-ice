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

/*! \file IceWrapper.h
 * This file contains a SystemC/TLM-2.0 wrapper for 3D-ICE.
 */

#ifndef ICEWRAPPER_H
#define ICEWRAPPER_H

#include <iostream>
#include <systemc.h>
#include <string>
#include <fstream>

#include "network_socket.h"
#include "network_message.h"

/*! \class IceWrapper IceWrapper.h "include/IceWrapper.h"
 *  \brief SystemC/TLM-2.0 wrapper for 3D-ICE
 *
 * This class provides support to bind 3D-ICE with simulation tools based on
 * SystemC/TLM2.0 like <a href="http://www.uni-kl.de/3d-dram/tools/dramsys/">DRAMSys</a>,
 * <a href="http://www.es.ele.tue.nl/drampower/">DRAMPower</a>, and many others.
 */
class IceWrapper
{
  private:
    // Network Related Variables:
    std::string serverIP;
    unsigned int serverPort;
    Socket_t client_socket;
    NetworkMessage_t client_nflp,
                     client_powers,
                     client_simulate,
                     client_tmap,
                     client_temperatures,
                     server_reply;

    // 3D Structure related:
    unsigned int numberOfFloorplanElements;

  public:
    /*! IceWrapper constructor
     *
     * Establishes a connection with the 3D-ICE server and gets information
     * about system architecture. The server must be up and running and
     * waiting for a connection.
     *
     * \param serverIp the IP address of the server
     * \param portNumber the port number of the server
     *
     */
    IceWrapper(std::string serverIp, unsigned int portNumber);

    /*! IceWrapper destructor
     *
     * Closes the connection with the 3D-ICE server and releases previously
     * allocated resources.
     */
    ~IceWrapper();

  private:
    /*! Opens the connection between client and server
     *
     * \return \c true in case of success
     * \return \c false otherwise
     */
    bool openConnection();

    /*! Closes the connection between client and server
     *
     * \return \c true in case of success
     * \return \c false otherwise
     */
    bool closeConnection();

    /*! Generates an output file containing values that correspond to a
     * thermal map of a stack element or the power map of a die accordingly to
     * the \p type passed as parameter.
     *
     * \param type TDICE_OUTPUT_TYPE_TMAP for thermal map of a stack element, TDICE_OUTPUT_TYPE_PMAP for power map of a die
     * \param filename name of the output file
     */
    void getMap(OutputType_t type, std::string filename);

  public:
    /*! Gets the number of floorplan elements
     *
     * \return the number of floorplan elements
     */
    unsigned int getNumberOfFloorplanElements();

    /*! Sends power information to the 3D-ICE server
     *
     * Typically the power information is dynamically generated during the
     * simulation and then passed to the server periodically. The server uses
     * this information among others to calculate the temperatures of the
     * system.
     *
     * \param powerValues a vector containing the power values of all floorplan elements
     */
    void sendPowerValues(std::vector<float> *powerValues);

    /*! Requests the simulation of a time slot to the server.
     *
     * This function sends and receives messages using network sockets and it
     * blocks until waiting for the simulation result.
     */
    void simulate();

    /*! Gets the temperature values regarding the last thermal simulation step
     *
     * \param TemperatureValues buffer to be filled with the temperature values
     * \param instant instant of time at which the inspection points generate the output
     * \param type inspection point of interest
     * \param quantity which of temperature records (e.g., average, maximum, minimum, gradient) shall be provided
     */
    void getTemperature(std::vector<float> &TemperatureValues, OutputInstant_t instant, OutputType_t type, OutputQuantity_t quantity);

    /*! Generates an output file containing values that correspond to a
     * thermal map of the stack element.
     *
     * \param filename name of the output file
     */
    void getTemperatureMap(std::string filename);

    /*! Generates an output file containing values that correspond to a power
     * map of the die.
     *
     * \param filename name of the output file
     */
    void getPowerMap(std::string filename);
};

#endif /* ICEWRAPPER_H */

