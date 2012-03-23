/******************************************************************************
 * This file is part of 3D-ICE, version 2.1 .                                 *
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

#ifndef _3DICE_TYPES_H_
#define _3DICE_TYPES_H_

/*! \file types.h */

#ifdef __cplusplus
extern "C"
{
#endif

#include <string.h>
#include <stdint.h>
#include <stdbool.h>

/******************************************************************************/

    /*! Definition of the primitive type String_t */

    typedef char* String_t ;



    /*! Definition of the type Quantity_t
     *
     *  For small quantities as \#channels or \#floorplan elements */

    typedef uint32_t Quantity_t ;



    /*! Definition of the primitive type Time_t */

    typedef double Time_t ;



    /*! Definition of the primitive type Temperature_t */

    typedef double Temperature_t ;



    /*! Definition of the primitive type Source_t */

    typedef double Source_t ;



    /*! Definition of the primitive type Power_t */

    typedef double Power_t ;



    /*! Definition of the type AmbientHTC_t
     *
     *  Ambient heat transfer coefficient */

    typedef double AmbientHTC_t ;



    /*! Definition of the type SolidVHC_t
     *
     *  Solid volumetric heat capacity */

    typedef double SolidVHC_t ;



    /*! Definition of the type SolidTC_t
     *
     * Solid thermal conductivity */

    typedef double SolidTC_t ;



    /*! Definition of the type Conductance_t */

    typedef double Conductance_t ;



    /*! Definition of the type Capacity_t */

    typedef double Capacity_t ;



    /*! Definition of the type SystemMatrixCoeff_t */

    typedef double SystemMatrixCoeff_t ;



    /*! Definition of the type CoolantHTC_t
     *
     * Coolant heat transfer coefficient */

    typedef double CoolantHTC_t ;



    /*! Definition of the type CoolantVHC_t
     *
     * Coolant volumetric heat capacity */

    typedef double CoolantVHC_t ;



    /*! Definition of the type CoolantFR_t
     *
     *  Coolant flow rate */

    typedef double CoolantFR_t ;



    /*! Definition of the type DarcyVelocity_t
     *
     *  Cooland darcy veloocity */

    typedef double DarcyVelocity_t ;



    /*! Definition of the type Cconv_t
     *
     * C convection coefficient */

    typedef double Cconv_t ;



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



    /*! Definition of the type CellDimension_t
     *
     * For cell length, height, width */

    typedef double CellDimension_t ;



    /*! Definitio of the type ChipDimension_t
     *
     * For chip length and with and coordinates to a point on its surface.
     * Also for floorplan elments dimensions ...
     */

    typedef double ChipDimension_t ;



    /*! Definitio of the type ChannelDimension_t
     *
     * Channel length, pitch and porosity
     */

    typedef double ChannelDimension_t ;



    /*! Definition of the type CellIndex_t
     *
     * For cell, layer, row, columns indexes and for counting too */

    typedef uint32_t CellIndex_t ;


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



    /*! \enum SimResult_t
     *
     * Collection of possible values returned bt the eulation functions
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



    /*! Definition of the type SimResult_t */

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
        TDICE_OUTPUT_TYPE_TMAP,       //!< The thermal map of a stack element
        TDICE_OUTPUT_TYPE_TCOOLANT    //!< The coolant leaving the cavity
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
        TDICE_OUTPUT_INSTANT_FINAL,     //!< At the end of the simulation
        TDICE_OUTPUT_INSTANT_SLOT,      //!< At the end of each time slot
        TDICE_OUTPUT_INSTANT_STEP       //!< At every internal time step
    } ;



    /*! Definition of the type OutputInstant_t */

    typedef enum OutputInstant_t OutputInstant_t ;

/******************************************************************************/

    /*! Definition of the type NetworkSocket_t */

    typedef int NetworkSocket_t ;



    /*! Definition of the type PortNumber_t */

    typedef uint16_t PortNumber_t ;



    /*! \enum MessageType_t
     *
     * Enumeration to collect all the types of messages that the client
     * can send as a request to the server
     */

    enum MessageType_t
    {
        /*! \brief Forces the server to terminate.
         *
         * The client sends a message without payload :
         *
         * | 2 | TDICE_EXIT_SIMULATION |
         *
         * The server terminates without a reply
         */

        TDICE_EXIT_SIMULATION = 0,

        /*! \brief Reset thermal state to initial temperature
         *
         * The client sends a message without payload :
         *
         * | 2 | TDICE_RESET_THERMAL_STATE |
         *
         * The client sends a message without payload
         * The server resets the thermal state and does not reply
         */

        TDICE_RESET_THERMAL_STATE,

        /*! \brief Request thermal results at a specific instant
         *
         * The client sends a message with the following payload
         *
         * | 4 | TDICE_RESET_THERMAL_STATE | OutputInstant_t instant | OutputType_t type |
         *
         * The server will process all the inspection point matching the parameters and
         * will send back the following message
         *
         * | length | TDICE_RESET_THERMAL_STATE | n_matching_inspection_points | ip 0 | ... | ip n-1 |
         *
         * The message will contain at word 0 the number of inspection points matching the
         * request. The remaining payloas will contain temperatures according to the type of
         * inspection point.
         */

        TDICE_THERMAL_RESULTS,

        /*! \brief Request for the total number of florplan elements in the stack
         *
         * The client sends a message without payload :
         *
         * | 2 | TDICE_TOTAL_NUMBER_OF_FLOORPLAN_ELEMENTS |
         *
         * The server will send back the number n of floorplan elements in the stack
         *
         * | 3 | TDICE_TOTAL_NUMBER_OF_FLOORPLAN_ELEMENTS | n |
         */

        TDICE_TOTAL_NUMBER_OF_FLOORPLAN_ELEMENTS,

        /*! \brief Insert a slot of power values into the queues and simulate the slot
         *
         * The client sends a message with the following payload
         *
         * | n + 3 | TDICE_INSERT_POWERS_AND_SIMULATE_SLOT | n | power0 | ... | power n-1 |
         *
         * where n is the total number of floorplan elements in the stack. The server
         * will use the power values to simulate a slot and it will reply sending back the
         * state after the simulation:
         *
         * | 3 | TDICE_INSERT_POWERS_AND_SIMULATE_SLOT | SimResult_t |
         */

        TDICE_INSERT_POWERS_AND_SIMULATE_SLOT
    } ;



    /*! Definition of the type MessageType_t */

    typedef enum MessageType_t MessageType_t ;



    /*! Definition of the type MessageWord_t */

    typedef uint32_t MessageWord_t ;

/******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* _3DICE_TYPES_H_ */
