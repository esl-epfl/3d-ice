/******************************************************************************
 * This file is part of 3D-ICE, version 3.0.0 .                               *
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
 *          Federico Terraneo                                                 *
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

#include <stdint.h>

/******************************************************************************/



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



/******************************************************************************/



    /*! Definition of the type CellDimension_t
     *
     * For cell length, height, width */

    typedef double CellDimension_t ;



    /*! Definition of the type ChipDimension_t
     *
     * For chip length and with and coordinates to a point on its surface.
     * Also for floorplan elments dimensions ...
     */

    typedef double ChipDimension_t ;



    /*! Definition of the type SpreaderDimension_t
     *
     * For pluggable heatsink, heat spreader length, height, width
     */

    typedef double SpreaderDimension_t ;



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

    /*! \enum HeatSinkModel_t
     *
     *   Enumeration to collect the supported model of the heat sink
     */

    enum HeatSinkModel_t
    {
        TDICE_HEATSINK_NONE = 0,     //!< Undefined type
        TDICE_HEATSINK_TOP,          //!< Top heat sink       (top-most layer)
        TDICE_HEATSINK_BOTTOM,       //!< Bottom heat sink (bottom-most layer)
        TDICE_HEATSINK_TOP_PLUGGABLE //!< Top pluggable heat sink
    } ;

    /*! The definition of the type HeatSinkModel_t */

    typedef enum HeatSinkModel_t HeatSinkModel_t ;

/******************************************************************************/

    /*! \enum StackLayerType_t
     *
     * Enumeration to collect the types of layer
     */

    enum StackLayerType_t
    {
        /*! Undefined type */

        TDICE_LAYER_NONE = 0,

        /*! Solid layer (passive) */

        TDICE_LAYER_SOLID,

        /*! Solid layer (active) */

        TDICE_LAYER_SOURCE,

        /*! Solid layer (passive) connected to the environment to dissipate heat */

        TDICE_LAYER_SOLID_CONNECTED_TO_AMBIENT,
        
        /*! Solid layer (passive) connected to heat spreader to dissipate heat */

        TDICE_LAYER_SOLID_CONNECTED_TO_SPREADER,

        /*! Solid layer (passive) connected to the pcb to dissipate heat */

        TDICE_LAYER_SOLID_CONNECTED_TO_PCB,

        /*! Solid layer (active) connected to the environment to dissipate heat */

        TDICE_LAYER_SOURCE_CONNECTED_TO_AMBIENT,
        
        /*! Solid layer (active) connected to heat spreader to dissipate heat */

        TDICE_LAYER_SOURCE_CONNECTED_TO_SPREADER,

        /*! Solid layer (active) connected to the pcb to dissipate heat */

        TDICE_LAYER_SOURCE_CONNECTED_TO_PCB,

        /*! Liquid layer in Channel 4 resistors model */

        TDICE_LAYER_CHANNEL_4RM,

        /*! Liquid layer in Channel 2 resistors model */

        TDICE_LAYER_CHANNEL_2RM,

        /*! Liquid layer in inline PinFins 2 resistors model */

        TDICE_LAYER_PINFINS_INLINE,

        /*! Liquid layer in staggered PinFins 2 resistors model */

        TDICE_LAYER_PINFINS_STAGGERED,

        /*! Virtual wall layer in Channel 2rm */

        TDICE_LAYER_VWALL_CHANNEL,

        /*! Virtual wall layer in inline/staggered PinFins 2 rm */

        TDICE_LAYER_VWALL_PINFINS,

        /*! Top wall layer in Channel/PinFins 2 rm */

        TDICE_LAYER_TOP_WALL,

        /*! Bottom wall layer in Channel/PinFins 2 rm */

        TDICE_LAYER_BOTTOM_WALL
    } ;

    /*! The definition of the type StackLayerType_t */

    typedef enum StackLayerType_t StackLayerType_t ;

/******************************************************************************/

    /*! \enum StackElementType_t
     *
     * Enumeration to collect the types of a stack element
     */

    enum StackElementType_t
    {
        TDICE_STACK_ELEMENT_NONE = 0,     //!< Undefined type
        TDICE_STACK_ELEMENT_LAYER   ,     //!< Layer
        TDICE_STACK_ELEMENT_CHANNEL ,     //!< Channel
        TDICE_STACK_ELEMENT_DIE           //!< Die
    } ;

    /*! The definition of the type StackElementType_t */

    typedef enum StackElementType_t StackElementType_t ;

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
        TDICE_OUTPUT_QUANTITY_MINIMUM,   //!< Minimum temperature
        TDICE_OUTPUT_QUANTITY_GRADIENT   //!< Maximum - Minimum temperature
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
        TDICE_OUTPUT_TYPE_PMAP,       //!< The power map of a die (its floorplan)
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
         * The server resets the thermal state and does not reply
         */

        TDICE_RESET_THERMAL_STATE,



        /*! \brief Request a message with the thermal state at a specific instant
         *
         * The client sends a message with the following payload
         *
         * | 4 | TDICE_SEND_OUTPUT | OutputInstant_t | OutputType_t | OutputQuantity_t |
         *
         * The server will process all the inspection point (declared in the
         * output section) matching the three parameters and will send back the
         * following message
         *
         * | length | TDICE_SEND_OUTPUT | nip | ip 1 | ... | ip n |
         *
         * The message will contain at word 0 the number nip of inspection points
         * matching the request. All the ips will be inserted in the message
         * following the same order with which they appear in the stack file.
         * If nip is zero, the message should be discarded. Otherwise, the
         * remaining payloas will contain temperatures according to the type
         * of inspection point that has been requested.
         *
         * The content of the ips could be
         *
         * | Temperature |
         *
         * if OutputType_t is Tcell, Tflpel or Tcoolant.
         *
         * | nflp | T 1 | ... | T nflp |
         *
         * if OutputType_t is Tflp (nflp is its number of floorplan elements).
         *
         * | nrows | ncolumns | T 1 | ... | T (nrows x ncolumns) |
         *
         * if OutputType_t is Tmap.
         *
         * | nrows | ncolumns | S 1 | ... | S (nrows x ncolumns) |
         *
         * if OutputType_t is Pmap.
         */

        TDICE_SEND_OUTPUT,



        /*! \brief Request to print thermal state at a specific instant
         *
         * The client sends a message with the following payload
         *
         * | 4 | TDICE_PRINT_OUTPUT | OutputInstant_t |
         *
         * The server will process all the inspection point (declared in the
         * output section) matching the output instant and will generate the output
         * in their corresponding text file. The server does not reply.
         */

        TDICE_PRINT_OUTPUT,



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



        /*! \brief Insert a slot of power values into the power queues
         *
         * The client sends a message with the following payload
         *
         * | n + 3 | TDICE_INSERT_POWERS | n | power0 | ... | power n-1 |
         *
         * where n is the total number of floorplan elements in the stack.
         * The server will access the power vaues received and put them
         * into the power queue of each floorplan element. The server will
         * respond with the message:
         *
         * | 3 | TDICE_INSERT_POWERS | Error_t |
         */

        TDICE_INSERT_POWERS,



        /*! \brief Tells the server to run a slot
         *
         * The client sends a message with the following payload
         *
         * | 2 | TDICE_SIMULATE_SLOT |
         *
         * The server will execute emulate_slot (power values should be in the
         * queues) and returns the state got after the simulation:
         *
         * | 3 | TDICE_SIMULATE_SLOT | SimResult_t |
         */

        TDICE_SIMULATE_SLOT,



        /*! \brief Tells the server to run a step
         *
         * The client sends a message with the following payload
         *
         * | 2 | TDICE_SIMULATE_STEP |
         *
         * The server will execute emulate_step and returns the state got
         * after the simulation:
         *
         * | 3 | TDICE_SIMULATE_STEP | SimResult_t |
         */

        TDICE_SIMULATE_STEP,
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
