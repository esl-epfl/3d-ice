/******************************************************************************
 * This file is part of 3D-ICE, version 2.2.4 .                               *
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

#ifndef _3DICE_COOLANT_H_
#define _3DICE_COOLANT_H_

/*! \file coolant.h */

#ifdef __cplusplus
extern "C"
{
#endif

#include "types.h"

/******************************************************************************/



    /*! \struct Coolant_t
     *
     *  \brief A collection of parameters describing the properties cooling fluid
     */

    struct Coolant_t
    {
        /*! The heat transfert coefficient in \f$ (W / ( \mu m^2 * K ) \f$
         *  between the liquid in the channel and the walls/pins
         */

        CoolantHTC_t HTCSide ;

        /*! The heat transfert coefficient in \f$ (W / ( \mu m^2 * K ) \f$
         *  between the liquid in the channel and the top wall
         */

        CoolantHTC_t HTCTop ;

        /*! The heat transfert coefficient in \f$ (W / ( \mu m^2 * K ) \f$
         *  between the liquid in the channel and the bottom wall
         */

        CoolantHTC_t HTCBottom ;

        /*! The volumetric heat capacity in \f$ J / ( \mu m^3 * K ) \f$ */

        CoolantVHC_t VHC ;

        /*! The flow rate per channel layer of the incolimg liquid.
         *  Specified in \f$ \frac{ml}{min} \f$ but stored in \f$ \frac{\mu m^3}{sec} \f$.
         *  Shared by all the channels for each layer in the 3DStack.
         */

        CoolantFR_t FlowRate ;

        /*! Darcy Velocity \f$ \frac{\mu m}{sec} \f$ */

        DarcyVelocity_t DarcyVelocity ;

        /*! The temperarute of the coolant at the channel inlet in \f$ K \f$ */

        Temperature_t TIn ;

    } ;

    /*! Definition of the type Coolant_t */

    typedef struct Coolant_t Coolant_t ;



/******************************************************************************/



    /*! Inits the fields of the \a coolant structure with default values
     *
     * \param coolant the address of the structure to initalize
     */

    void coolant_init    (Coolant_t *coolant) ;



    /*! Copies the structure \a src into \a dst , as an assignement
     *
     * The function destroys the content of \a dst and then makes the copy
     *
     * \param dst the address of the left term sructure (destination)
     * \param src the address of the right term structure (source)
     */

    void coolant_copy    (Coolant_t *dst, Coolant_t *src) ;



    /*! Destroys the content of the fields of the structure \a coolant
     *
     * The function releases any dynamic memory used by the structure and
     * resets its state calling \a coolant_init .
     *
     * \param coolant the address of the structure to destroy
     */

    void coolant_destroy (Coolant_t *coolant) ;

/******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* _3DICE_COOLANT_H_ */
