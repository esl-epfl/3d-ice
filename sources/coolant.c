/******************************************************************************
 * This file is part of 3D-ICE, version 2.2.2 .                               *
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

#include "coolant.h"

/******************************************************************************/

void coolant_init (Coolant_t *coolant)
{
    coolant->HTCSide       = (CoolantHTC_t) 0.0 ;
    coolant->HTCTop        = (CoolantHTC_t) 0.0 ;
    coolant->HTCBottom     = (CoolantHTC_t) 0.0 ;
    coolant->VHC           = (CoolantVHC_t) 0.0 ;
    coolant->FlowRate      = (CoolantFR_t) 0.0 ;
    coolant->DarcyVelocity = (DarcyVelocity_t) 0.0 ;
    coolant->TIn           = (Temperature_t) 0.0 ;
}

/******************************************************************************/

void coolant_copy (Coolant_t *dst, Coolant_t *src)
{
    coolant_destroy (dst) ;

    dst->HTCSide       = src->HTCSide ;
    dst->HTCTop        = src->HTCTop ;
    dst->HTCBottom     = src->HTCBottom ;
    dst->VHC           = src->VHC ;
    dst->FlowRate      = src->FlowRate ;
    dst->DarcyVelocity = src->DarcyVelocity ;
    dst->TIn           = src->TIn ;
}

/******************************************************************************/

void coolant_destroy (Coolant_t *coolant)
{
    // Nothing to do ...

    coolant_init (coolant) ;
}

/******************************************************************************/
