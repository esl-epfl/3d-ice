/******************************************************************************
 * This file is part of 3D-ICE, version 2.0 .                                 *
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
 *                             Copyright (C) 2010                             *
 *   Embedded Systems Laboratory - Ecole Polytechnique Federale de Lausanne   *
 *                            All Rights Reserved.                            *
 *                                                                            *
 * Authors: Arvind Sridhar                                                    *
 *          Alessandro Vincenzi                                               *
 *          Giseong Bak                                                       *
 *          Martino Ruggiero                                                  *
 *          Thomas Brunschwiler                                               *
 *          David Atienza                                                     *
 *                                                                            *
 * For any comment, suggestion or request  about 3D-ICE, please  register and *
 * write to the mailing list (see http://listes.epfl.ch/doc.cgi?liste=3d-ice) *
 * Any usage  of 3D-ICE  for research,  commercial or other  purposes must be *
 * properly acknowledged in the resulting products or publications.           *
 *                                                                            *
 * EPFL-STI-IEL-ESL                                                           *
 * Batiment ELG, ELG 130                Mail : 3d-ice@listes.epfl.ch          *
 * Station 11                                  (SUBSCRIPTION IS NECESSARY)    *
 * 1015 Lausanne, Switzerland           Url  : http://esl.epfl.ch/3d-ice.html *
 ******************************************************************************/

#ifndef _3DICE_CLIENT_H_
#define _3DICE_CLIENT_H_

/*! \file ni_client.h */

#ifdef __cplusplus
extern "C"
{
#endif

/******************************************************************************/

#include "types.h"

/******************************************************************************/

    /*! Initializes a socket in the unix domain
     *
     * \param socket_id   the address of the socket to initialize
     * \param socket_name the name of the socket to inizialize
     *
     * \return \c TDICE_SUCCESS if the initialization succeeded
     * \return \c TDICE_FAILURE if the initialization fails. A message will be
     *                          printed on standard error
     */

    Error_t init_client_unix_socket

        (UnixSocket_t *socket_id, String_t socket_name) ;



    /*! Initializes a socket in the network domain
     *
     * \param socket_id the address of the socket to initialize
     * \param host_name the ip address of the server
     * \param port_number the port number of the server
     *
     * \return \c TDICE_SUCCESS if the initialization succeeded
     * \return \c TDICE_FAILURE if the initialization fails. A message will be
     *                          printed on standard error
     */

    Error_t init_client_network_socket

        (NetworkSocket_t *socket_id, String_t host_name, int port_number) ;



    /*! Closes a socket in the unix domain
     *
     * \param socket_id the address of the socket to close
     *
     * \return \c TDICE_SUCCESS if the closure succeeded
     * \return \c TDICE_FAILURE if the closure fails. A message will be
     *                          printed on standard error
     */

    Error_t close_client_unix_socket (UnixSocket_t *socket_id) ;



    /*! Closes a socket in the network domain
     *
     * \param socket_id the address of the socket to close
     *
     * \return \c TDICE_SUCCESS if the closure succeeded
     * \return \c TDICE_FAILURE if the closure fails. A message will be
     *                          printed on standard error
     */

    Error_t close_client_network_socket (NetworkSocket_t *socket_id) ;

#ifdef __cplusplus
}
#endif

#endif /* _3DICE_CLIENT_H_ */