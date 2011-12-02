/******************************************************************************
 * This file is part of 3D-ICE, version 1.0.2 .                               *
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
 *          Martino Ruggiero                                                  *
 *          Thomas Brunschwiler                                               *
 *          David Atienza                                                     *
 *                                                                            *
 * For any comment, suggestion or request  about 3D-ICE, please  register and *
 * write to the mailing list (see http://listes.epfl.ch/doc.cgi?liste=3d-ice) *
 *                                                                            *
 * EPFL-STI-IEL-ESL                                                           *
 * Batiment ELG, ELG 130                Mail : 3d-ice@listes.epfl.ch          *
 * Station 11                                  (SUBSCRIPTION IS NECESSARY)    *
 * 1015 Lausanne, Switzerland           Url  : http://esl.epfl.ch/3d-ice.html *
 ******************************************************************************/

#ifndef _3DICE_TYPES_H_
#define _3DICE_TYPES_H_

/*! \file types.h */

#ifdef __cplusplus
extern "C"
{
#endif

#include "stdint.h"
#include "stdbool.h"

/******************************************************************************/

  /* Boolean value */

//   typedef int Bool_t ;
// # define      FALSE_V (0)
// # define      TRUE_V  (!FALSE_V)
//
//   /* Quantity */
//
//   typedef int Quantity_t ;
// # define      QUANTITY_I 0
//
//   /* String */
//
//   typedef char* String_t ;
// # define        STRING_I NULL

/******************************************************************************/

  /* Conductance */

//   typedef double Conductance_t ;
// # define         CONDUCTANCE_I 0.0
//
//   /* Capacity */
//
//   typedef double Capacity_t ;
// # define         CAPACITY_I 0.0
//
//   /* Source */
//
//   typedef double Source_t ;
// # define         SOURCE_I 0.0
//
//   /* Power */
//
//   typedef double Power_t ;
// # define         POWER_I 0.0
//
//   /* Temperature */
//
//   typedef double Temperature_t ;
// # define         TEMPERATURE_I 0.0
//
//   /* Time */
//
//   typedef double Time_t ;
// # define         TIME_I 0.0

/******************************************************************************/

  /* Solid volumetric heat capacity */

//   typedef double SolidVHC_t ;
// # define         SOLIDVHC_I 0.0
//
//   /* Solid thermal conductivity */
//
//   typedef double SolidTC_t ;
// # define         SOLIDTC_I 0.0

/******************************************************************************/

  /* Ambient heat transfer coefficient */

//   typedef double AmbientHTC_t ;
// # define         AMBIENTHTC_I 0.0

/******************************************************************************/

  /* Coolant heat transfer coefficient */

//   typedef double CoolantHTC_t ;
// # define         COOLANTHTC_I 0.0
//
//   /* Coolant volumetric heat capacity */
//
//   typedef double CoolantVHC_t ;
// # define         COOLANTVHC_I 0.0

    /*! \struct Coolant_t
     * A collection of parameters describing the properties cooling fluid
     */

    struct Coolant_t
    {
        /*! The heat transfert coefficient in \f$ (W / ( \mu m^2 * K ) \f$
         *  between the liquid in the channel and the walls/pins
         */

        double HTCSide ;

        /*! The heat transfert coefficient in \f$ (W / ( \mu m^2 * K ) \f$
         *  between the liquid in the channel and the top wall
         */

        double HTCTop ;

        /*! The heat transfert coefficient in \f$ (W / ( \mu m^2 * K ) \f$
         *  between the liquid in the channel and the bottom wall
         */

        double HTCBottom ;

        /*! The volumetric heat capacity in \f$ J / ( \mu m^3 * K ) \f$ */

        double VHC ;

        /*! The temperarute of the coolant at the channel inlet in \f$ K \f$ */

        double TIn ;

    } ;

    /*! Definition of the type Coolant_t */

    typedef struct Coolant_t Coolant_t ;


//#   define COOLANT_I ((Coolant_t) { double, double, double, double, double })
//
//  /* Coolant flow rate */
//
//   typedef double CoolantFR_t ;
// # define         COOLANTFR_I 0.0
//
//   /* Darcy Velocity  */
//
//   typedef double DarcyVelocity_t ;
// # define         DARCYVELOCITY_I 0.0
//
//   /* C convection coefficient */
//
//   typedef double Cconv_t ;
// # define         CCONV_I 0.0
//
//   /* Porosity */
//
//   typedef double Porosity_t ;
// # define         POROSITY_I 0.0

/******************************************************************************/

  /* Grid dimension (number and index of layer, row, column) */

//   typedef int GridDimension_t ;
// # define      GRIDDIMENSION_I 0
//
//   /* Cell dimension (for cell length, height, width) */
//
//   typedef double CellDimension_t ;
// # define         CELLDIMENSION_I 0.0
//
//   /* Chip dimension (for chip length, width) */
//
// # define ChipDimension_t CellDimension_t
// # define CHIPDIMENSION_I CELLDIMENSION_I

/******************************************************************************/

  /* System matrix coefficients */

//   typedef double SystemMatrixValue_t ;
// # define         SYSTEMMATRIXVALUE_I 0.0
//
//   /* System matrix column indexes */
//
//   typedef int SystemMatrixColumn_t ;
// # define      SYSTEMMATRIXCOLUMN_I 0
//
//   /* System matrix row indexes */
//
//   typedef int SystemMatrixRow_t ;
// # define      SYSTEMMATRIXROW_I 0

/******************************************************************************/

    /*! \enum StackElement_t
     *
     * Enumeration to collect the types of a stack element
     */

    enum StackElement_t
    {
        TDICE_STACK_ELEMENT_NONE = 0, //!< Undefined type
        TDICE_STACK_ELEMENT_LAYER   , //!< Layer
        TDICE_STACK_ELEMENT_CHANNEL , //!< Channel
        TDICE_STACK_ELEMENT_DIE       //!< Die
    } ;

    /*! The definition of the type StackElement_t */

    typedef enum StackElement_t StackElement_t ;

//# define                      STACKELEMENT_I TDICE_STACK_ELEMENT_NONE

/******************************************************************************/

    /*! \enum ChannelModel_t
     *
     * Enumeration to collect the available thermal models that can be used
     * to implement the microchannel. If no channels are declared in the
     * stack, the 4 resistors model is used for solid only stacks.
     */

    enum ChannelModel_t
    {
        TDICE_CHANNEL_MODEL_NONE = 0,      //!< Undefined type
        TDICE_CHANNEL_MODEL_MC_4RM,        //!< Microchannel - 4 Resistors model
        TDICE_CHANNEL_MODEL_MC_2RM,        //!< Microchannel - 2 Resistors model
        TDICE_CHANNEL_MODEL_PF_INLINE,     //!< Inline pin fins - 2 Resistors model
        TDICE_CHANNEL_MODEL_PF_STAGGERED   //!< Staggered pin fins - 2 Resistors model
    } ;

    /*! The definition of the type ChannelModel_t */

    typedef enum ChannelModel_t ChannelModel_t ;

//# define                      CHANNELMODEL_I TDICE_CHANNEL_MODEL_NO_CHANNEL

    /*! \def NUM_LAYERS_CHANNEL_4RM
     *
     *  The number of layers of thermal cells needed to model a microchannel
     *  stack elenment according to the 4 resistors model
     */

#   define NUM_LAYERS_CHANNEL_4RM 1

    /*! \def NUM_LAYERS_CHANNEL_2RM
     *
     *  The number of layers of thermal cells needed to model a microchannel
     *  or pin fins stack elenment according to the 2 resistors model
     */

#   define NUM_LAYERS_CHANNEL_2RM 4

    /*! \def SOURCE_OFFSET_CHANNEL_4RM
     *
     *  The offset (\# layers) within a channel stack elenment modeled with
     *  4 resistors to be added to locate the source layer
     */

#   define SOURCE_OFFSET_CHANNEL_4RM 0

    /*! \def SOURCE_OFFSET_CHANNEL_2RM
     *
     *  The offset (\# layers) within a channel stack elenment modeled with
     *  4 resistors to be added to locate the source layer
     */

#   define SOURCE_OFFSET_CHANNEL_2RM 2

/******************************************************************************/

    /*! \enum Error_t
     *
     *  Enumeration to collect the possible values returned by functions in
     *  the library to indicate errors or success.
     */

    enum Error_t
    {
        TDICE_SUCCESS = 0,  //!< The function returns with success
        TDICE_FAILURE       //!< The function retuerns with a generic error
    } ;

    /*! Definition of the type Error_t */

    typedef enum Error_t Error_t ;



    /*! \enum
     *
     */

    enum SimResult_t
    {
        /*! The power values are over */

        TDICE_END_OF_SIMULATION = 0,

        /*! The emulation function does not match the configuration */

        TDICE_WRONG_CONFIG,

        /*! The solver routine failed */

        TDICE_SOLVER_ERROR,

        /*! The time step has been simulated correctly */

        TDICE_STEP_DONE,

        /*! The time slot has been simulated correctly */

        TDICE_SLOT_DONE
    } ;

    typedef enum SimResult_t SimResult_t ;

/******************************************************************************/

    /*! \enum AnalysisType_t
     *
     *  Enumeration to collect the possible types of thermal analysis
     *  that can be run with 3D-ICE.
     */

    enum AnalysisType_t
    {
        TDICE_ANALYSIS_TYPE_NONE = 0,  //!< Undefined analysis type
        TDICE_ANALYSIS_TYPE_TRANSIENT, //!< Transient analysis
        TDICE_ANALYSIS_TYPE_STEADY     //!< Steady state analysis
    } ;



    /*! the definition of the type AnalysisType_t */

    typedef enum AnalysisType_t AnalysisType_t ;



    /*! \enum OutputQuantity_t
     *
     *  The "type" of temperature measurement that can be reported with
     *  an inspection point during a thermal simulation. It is used when
     *  the measurement is related to a surface.
     */

    enum OutputQuantity_t
    {
        TDICE_OUTPUT_QUANTITY_NONE = 0,  //!< Undefined type
        TDICE_OUTPUT_QUANTITY_AVERAGE,   //!< Average temperature
        TDICE_OUTPUT_QUANTITY_MAXIMUM,   //!< Maximum temperature
        TDICE_OUTPUT_QUANTITY_MINIMUM    //!< Minimum temperature
    } ;



    /*! Definition of the type OutputQuantity_t */

    typedef enum OutputQuantity_t OutputQuantity_t ;



    /*! \enum OutputType_t
     *
     * The "stack object" that can be monitored with an ispection point
     * during a thermal simulation.
     */

    enum OutputType_t
    {
        TDICE_OUTPUT_TYPE_NONE = 0,   //!< Undefined object
        TDICE_OUTPUT_TYPE_TCELL,      //!< Single thermal cell
        TDICE_OUTPUT_TYPE_TFLP,       //!< All the element in a floorplan
        TDICE_OUTPUT_TYPE_TFLPEL,     //!< A single floorplan element
        TDICE_OUTPUT_TYPE_TMAP        //!< The thermal map of a stack element
    } ;



    /*! Definition of the type OutputType_t */

    typedef enum OutputType_t OutputType_t ;



    /*! \enum OutputInstant_t
     *
     * Enumeration to collect the possible istant of time at which the
     * inspection points generate the output
     */

    enum OutputInstant_t
    {
        TDICE_OUTPUT_INSTANT_NONE = 0,  //!< Undefined instant
        TDICE_OUTPUT_FINAL,             //!< At the end of the simulation
        TDICE_OUTPUT_SLOT,              //!< At the end of each time slot
        TDICE_OUTPUT_STEP               //!< At every internal time step
    } ;



    /*! Definition of the type OutputInstant_t */

    typedef enum OutputInstant_t OutputInstant_t ;

/******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* _3DICE_TYPES_H_ */
