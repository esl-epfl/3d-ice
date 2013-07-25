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
 *          Thomas Brunschwiler                                               *
 *          Martino Ruggiero                                                  *
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

%code requires
{
    #include "types.h"
    #include "string_t.h"

    #include "material.h"
    #include "die.h"
    #include "stack_element.h"
    #include "inspection_point.h"
}

// The YYSTYPE union used to collect the types of tokens and rules

%union
{
    double                double_v ;
    String_t              string ;
    String_t             *string_p ;
    Material_t           *material_p ;
    Coolant_t             coolant_v ;
    ChannelModel_t        channel_model_v ;
    Die_t                *die_p ;
    Layer_t              *layer_p ;
    StackElement_t       *stack_element_p ;
    InspectionPoint_t    *inspection_point_p ;
    OutputInstant_t       output_instant_v ;
    OutputQuantity_t      output_quantity_v ;
}

%code
{
    #include <math.h>

    #include "analysis.h"
    #include "output.h"
    #include "channel.h"
    #include "heat_sink.h"
    #include "dimensions.h"
    #include "floorplan_element.h"
    #include "floorplan.h"
    #include "layer_list.h"
    #include "material_list.h"
    #include "die_list.h"
    #include "macros.h"
    #include "stack_description.h"

    #include "../flex/stack_description_scanner.h"

    void stack_description_error

        (StackDescription_t *stack, Analysis_t *analysis, Output_t *output,
         yyscan_t scanner, const char *message) ;

    #define STKERROR(m) stack_description_error (stkd, analysis, output, scanner, m)

    static char error_message [100] ;

    static CellDimension_t first_wall_length ;
    static CellDimension_t last_wall_length ;
    static CellDimension_t wall_length ;
    static CellDimension_t channel_length ;
    static Quantity_t      num_channels ;
    static Quantity_t      num_dies ;

    static Quantity_t  source_layer_offset ;
    static LayerList_t dielayers ;
}

%type <double_v>           first_wall_length
%type <double_v>           last_wall_length
%type <channel_model_v>    distribution
%type <coolant_v>          coolant_heat_transfer_coefficients_4rm
%type <coolant_v>          coolant_heat_transfer_coefficients_2rm
%type <material_p>         material
%type <die_p>              die
%type <layer_p>            layer
%type <layer_p>            die_layer
%type <layer_p>            die_layer_content
%type <stack_element_p>    stack_element
%type <inspection_point_p> inspection_point
%type <output_instant_v>   when
%type <output_quantity_v>  maxminavg
%type <string_p>           optional_layout

%token _2RM                  "keyword 2rm"
%token _4RM                  "keyword 4rm"
%token AVERAGE               "keyword average"
%token BOTTOM                "keyword bottom"
%token CAPACITY              "keyword capacity"
%token CELL                  "keyword cell"
%token CHANNEL               "keyword channel"
%token CHIP                  "keyword chip"
%token COEFFICIENT           "keyword coefficient"
%token CONDUCTIVITY          "keyword conductivity"
%token COOLANT               "keyword coolant"
%token DARCY                 "keyword darcy"
%token DIAMETER              "keyword diameter"
%token DIE                   "keyword die"
%token DIMENSIONS            "keyword dimensions"
%token DISTRIBUTION          "keyword distribution"
%token FINAL                 "keyword final"
%token FIRST                 "keyword first"
%token FLOORPLAN             "keyword floorplan"
%token FLOW                  "keyword flow"
%token GRADIENT              "keyword gradient"
%token HEAT                  "keyword heat"
%token HEIGHT                "keyword height"
%token INCOMING              "keyword incoming"
%token INITIAL_              "keyword initial"
%token INLINE                "keyword inline"
%token LAST                  "keyword last"
%token LAYER                 "keyword layer"
%token LAYOUT                "keyword layout"
%token LENGTH                "keyword length"
%token MATERIAL              "keyword material"
%token MAXIMUM               "keyword maximum"
%token MICROCHANNEL          "keyword microchannel"
%token MINIMUM               "keyword minimum"
%token OUTPUT                "keyword output"
%token PIN                   "keyword pin"
%token PINFIN                "keyword pinfin"
%token PITCH                 "keyword pitch"
%token PMAP                  "keyword Pmap"
%token RATE                  "keyword rate"
%token SIDE                  "keyword side"
%token SINK                  "keyword sink"
%token SLOT                  "keyword slot"
%token SOLVER                "keyword solver"
%token SOURCE                "keyword source"
%token STACK                 "keyword stack"
%token STAGGERED             "keyword staggered"
%token STATE                 "keyword state"
%token STEADY                "keyword steady"
%token STEP                  "keyword step"
%token TCELL                 "keyword T"
%token TCOOLANT              "keyword Tcoolant"
%token TEMPERATURE           "keyword temperature"
%token TFLP                  "keyword Tflp"
%token TFLPEL                "keyword Tflpel"
%token THERMAL               "keyword thermal"
%token TMAP                  "keyword Tmap"
%token TO                    "keyword to"
%token TOP                   "keyword top"
%token TRANSFER              "keyword transfer"
%token TRANSIENT             "keyword transient"
%token VELOCITY              "keyword velocity"
%token VOLUMETRIC            "keywork volumetric"
%token WALL                  "keyword wall"
%token WIDTH                 "keyword width"

%token <double_v> DVALUE     "double value"
%token <string>   IDENTIFIER "identifier"
%token <string>   PATH       "path to file"

%destructor { string_destroy (&$$) ; } <string>
%destructor { layer_free ($$) ;      } <layer_p>

%name-prefix "stack_description_"
%output      "stack_description_parser.c"

%pure-parser
%error-verbose

%parse-param { StackDescription_t *stkd     }
%parse-param { Analysis_t         *analysis }
%parse-param { Output_t           *output   }
%parse-param { yyscan_t            scanner  }

%lex-param   { yyscan_t scanner }

%initial-action
{
    first_wall_length = 0.0 ;
    last_wall_length  = 0.0 ;
    wall_length       = 0.0 ;
    channel_length    = 0.0 ;
    num_channels      = 0u ;
    num_dies          = 0u ;

    source_layer_offset = 0u ;

    layer_list_init (&dielayers) ;
} ;

%start stack_description_file

%%

/******************************************************************************/
/******************************* File content *********************************/
/******************************************************************************/

stack_description_file

  : materials_list
    heatsink_opt
    microchannel_opt
    dimensions
    layers_list_opt
    dies_list
    stack
    solver
    inspection_points
  ;

/******************************************************************************/
/******************************* Materials ************************************/
/******************************************************************************/

materials_list

  : material                // $1 : pointer to the first material found
    {
        material_list_insert_end (&stkd->Materials, $1) ;

        material_free ($1) ;
    }
  | materials_list material // $1 : pointer to the last material in the list
                            // $2 : pointer to the material to add in the list
    {
        if (material_list_find (&stkd->Materials, $2) != NULL)
        {
            sprintf (error_message, "Material %s already declared", $2->Id) ;

            STKERROR (error_message) ;

            material_free ($2) ;

            YYABORT ;
        }

        material_list_insert_end (&stkd->Materials, $2) ;

        material_free ($2) ;
    }
  ;

material

  : MATERIAL IDENTIFIER ':'                     // $2
        THERMAL CONDUCTIVITY     DVALUE ';'     // $6
        VOLUMETRIC HEAT CAPACITY DVALUE ';'     // $11
    {
        Material_t *material = $$ = material_calloc () ;

        if (material == NULL)
        {
            STKERROR ("Malloc material failed") ;

            string_destroy (&$2) ;

            YYABORT ;
        }

        string_copy (&material->Id, &$2) ;

        material->ThermalConductivity    = (SolidTC_t) $6 ;
        material->VolumetricHeatCapacity = (SolidVHC_t) $11 ;

        string_destroy (&$2) ;
    }
  ;

/******************************************************************************/
/******************************* Heatsink *************************************/
/******************************************************************************/

heatsink_opt

  : // Declaring this section is not mandatory

  | topsink

  | bottomsink

  | topsink bottomsink

  ;

topsink

  : TOP HEAT SINK ':'
        HEAT TRANSFER COEFFICIENT DVALUE ';'  // $8
        TEMPERATURE               DVALUE ';'  // $11
    {
        stkd->TopHeatSink = heat_sink_calloc () ;

        if (stkd->TopHeatSink == NULL)
        {
            STKERROR ("Malloc top heat sink failed") ;

            YYABORT ;
        }

        stkd->TopHeatSink->SinkModel          = TDICE_HEATSINK_TOP ;
        stkd->TopHeatSink->AmbientHTC         = (AmbientHTC_t) $8 ;
        stkd->TopHeatSink->AmbientTemperature = $11 ;
    }

  ;

bottomsink

  : BOTTOM HEAT SINK ':'
        HEAT TRANSFER COEFFICIENT DVALUE ';'  // $8
        TEMPERATURE               DVALUE ';'  // $11
    {
        stkd->BottomHeatSink = heat_sink_calloc () ;

        if (stkd->BottomHeatSink == NULL)
        {
            STKERROR ("Malloc bottom heat sink failed") ;

            YYABORT ;
        }

        stkd->BottomHeatSink->SinkModel          = TDICE_HEATSINK_BOTTOM ;
        stkd->BottomHeatSink->AmbientHTC         = (AmbientHTC_t) $8 ;
        stkd->BottomHeatSink->AmbientTemperature = $11 ;
    }
  ;

/******************************************************************************/
/******************************* MicroChannel *********************************/
/******************************************************************************/

microchannel_opt

  : // Declaring the channel section is not mandatory

  |  MICROCHANNEL _4RM ':'
        HEIGHT DVALUE ';'                             //  $5
        CHANNEL LENGTH DVALUE ';'                     //  $9
        WALL    LENGTH DVALUE ';'                     //  $13
        first_wall_length                             //  $15
        last_wall_length                              //  $16
        WALL MATERIAL IDENTIFIER ';'                  //  $19
        COOLANT FLOW RATE DVALUE ';'                  //  $24
        coolant_heat_transfer_coefficients_4rm        //  $26
        COOLANT VOLUMETRIC HEAT CAPACITY DVALUE ';'   //  $31
        COOLANT INCOMING TEMPERATURE DVALUE ';'       //  $36
    {
        stkd->Channel = channel_calloc () ;

        if (stkd->Channel == NULL)
        {
            STKERROR ("Malloc channel failed") ;

            string_destroy (&$19) ;

            YYABORT ;
        }

        channel_length    = $9 ;
        wall_length       = $13 ;
        first_wall_length = ($15 != 0.0) ? $15 : $13 ;
        last_wall_length  = ($16 != 0.0) ? $16 : $13 ;

        stkd->Channel->ChannelModel      = TDICE_CHANNEL_MODEL_MC_4RM ;
        stkd->Channel->NLayers           = NUM_LAYERS_CHANNEL_4RM ;
        stkd->Channel->SourceLayerOffset = SOURCE_OFFSET_CHANNEL_4RM ;
        stkd->Channel->Height            = $5 ;

        string_copy (&stkd->Channel->WallMaterial.Id, &$19) ;

        stkd->Channel->Coolant.FlowRate  = FLOW_RATE_FROM_MLMIN_TO_UM3SEC ($24) ;
        stkd->Channel->Coolant.HTCSide   = $26.HTCSide ;
        stkd->Channel->Coolant.HTCTop    = $26.HTCTop ;
        stkd->Channel->Coolant.HTCBottom = $26.HTCBottom ;
        stkd->Channel->Coolant.VHC       = $31 ;
        stkd->Channel->Coolant.TIn       = $36 ;

        Material_t *tmp = material_list_find

            (&stkd->Materials, &stkd->Channel->WallMaterial) ;

        if (tmp == NULL)
        {
            sprintf (error_message, "Unknown material %s", $19) ;

            STKERROR (error_message) ;

            string_destroy (&$19) ;

            YYABORT ;
        }

        material_copy (&stkd->Channel->WallMaterial, tmp) ;

        string_destroy (&$19) ;
    }

  |  MICROCHANNEL _2RM ':'
        HEIGHT DVALUE ';'                            //  $5
        CHANNEL LENGTH DVALUE ';'                    //  $9
        WALL    LENGTH DVALUE ';'                    //  $13
        WALL MATERIAL IDENTIFIER ';'                 //  $17
        COOLANT FLOW RATE DVALUE ';'                 //  $22
        coolant_heat_transfer_coefficients_2rm       //  $24
        COOLANT VOLUMETRIC HEAT CAPACITY DVALUE ';'  //  $29
        COOLANT INCOMING TEMPERATURE DVALUE ';'      //  $34
    {
        stkd->Channel = channel_calloc () ;

        if (stkd->Channel == NULL)
        {
            STKERROR ("Malloc channel failed") ;

            string_destroy (&$17) ;

            YYABORT ;
        }

        stkd->Channel->ChannelModel      = TDICE_CHANNEL_MODEL_MC_2RM ;
        stkd->Channel->NLayers           = NUM_LAYERS_CHANNEL_2RM ;
        stkd->Channel->SourceLayerOffset = SOURCE_OFFSET_CHANNEL_2RM ;
        stkd->Channel->Height            = $5 ;
        stkd->Channel->Length            = $9 ;
        stkd->Channel->Pitch             = $13 + $9 ;
        stkd->Channel->Porosity          = stkd->Channel->Length / stkd->Channel->Pitch ;

        string_copy (&stkd->Channel->WallMaterial.Id, &$17) ;

        stkd->Channel->Coolant.FlowRate  = FLOW_RATE_FROM_MLMIN_TO_UM3SEC ($22) ;
        stkd->Channel->Coolant.HTCSide   = $24.HTCSide ;
        stkd->Channel->Coolant.HTCTop    = $24.HTCTop ;
        stkd->Channel->Coolant.HTCBottom = $24.HTCBottom ;
        stkd->Channel->Coolant.VHC       = $29 ;
        stkd->Channel->Coolant.TIn       = $34 ;

        Material_t *tmp = material_list_find

            (&stkd->Materials, &stkd->Channel->WallMaterial) ;

        if (tmp == NULL)
        {
            sprintf (error_message, "Unknown material %s", $17) ;

            STKERROR (error_message) ;

            string_destroy (&$17) ;

            YYABORT ;
        }

        material_copy (&stkd->Channel->WallMaterial, tmp) ;

        string_destroy (&$17) ;
    }

  |  PINFIN ':'
        HEIGHT DVALUE ';'                              //  $4
        PIN DIAMETER DVALUE ';'                        //  $8
        PIN PITCH  DVALUE ';'                          //  $12
        PIN DISTRIBUTION distribution ';'              //  $16
        PIN MATERIAL IDENTIFIER ';'                    //  $20
        DARCY VELOCITY DVALUE ';'                      //  $24
        COOLANT VOLUMETRIC HEAT CAPACITY DVALUE ';'    //  $30
        COOLANT INCOMING TEMPERATURE DVALUE ';'        //  $35
    {
        stkd->Channel = channel_calloc () ;

        if (stkd->Channel == NULL)
        {
            STKERROR ("Malloc channel failed") ;

            string_destroy (&$20) ;

            YYABORT ;
        }

        stkd->Channel->Height                = $4 ;
        stkd->Channel->Porosity              = POROSITY ($8, $12) ;
        stkd->Channel->Pitch                 = $12 ;
        stkd->Channel->ChannelModel          = $16 ;
        stkd->Channel->NLayers               = NUM_LAYERS_CHANNEL_2RM ;
        stkd->Channel->SourceLayerOffset     = SOURCE_OFFSET_CHANNEL_2RM ;

        string_copy (&stkd->Channel->WallMaterial.Id, &$20) ;

        stkd->Channel->Coolant.DarcyVelocity = $24 ;
        stkd->Channel->Coolant.HTCSide       = 0.0 ;
        stkd->Channel->Coolant.HTCTop        = 0.0 ;
        stkd->Channel->Coolant.HTCBottom     = 0.0 ;
        stkd->Channel->Coolant.VHC           = $30 ;
        stkd->Channel->Coolant.TIn           = $35 ;

        Material_t *tmp = material_list_find

            (&stkd->Materials, &stkd->Channel->WallMaterial) ;

        if (tmp == NULL)
        {
            sprintf (error_message, "Unknown material %s", $20) ;

            STKERROR (error_message) ;

            string_destroy (&$20) ;

            YYABORT ;
        }

        material_copy (&stkd->Channel->WallMaterial, tmp) ;

        string_destroy (&$20) ;
    }
  ;

coolant_heat_transfer_coefficients_4rm

  : COOLANT HEAT TRANSFER COEFFICIENT DVALUE ';'
    {
        $$.HTCSide   = $5 ;
        $$.HTCTop    = $5 ;
        $$.HTCBottom = $5 ;
    }
  | COOLANT HEAT TRANSFER COEFFICIENT SIDE   DVALUE ','
                                      TOP    DVALUE ','
                                      BOTTOM DVALUE ';'
    {
        $$.HTCSide   = $6 ;
        $$.HTCTop    = $9 ;
        $$.HTCBottom = $12 ;
    }
  ;

coolant_heat_transfer_coefficients_2rm

  : COOLANT HEAT TRANSFER COEFFICIENT DVALUE ';'
    {
        $$.HTCSide   = 0.0 ;
        $$.HTCTop    = $5 ;
        $$.HTCBottom = $5 ;
    }
  | COOLANT HEAT TRANSFER COEFFICIENT TOP    DVALUE ','
                                      BOTTOM DVALUE ';'
    {
        $$.HTCSide   = 0.0 ;
        $$.HTCTop    = $6 ;
        $$.HTCBottom = $9 ;
    }
  ;

first_wall_length
  :                              { $$ = 0.0 ; } // Not mandatory
  | FIRST WALL LENGTH DVALUE ';' { $$ = $4 ;  }
  ;

last_wall_length
  :                             { $$ = 0.0 ; } // Not mandatory
  | LAST WALL LENGTH DVALUE ';' { $$ = $4 ;  }
  ;

distribution
  : INLINE                      { $$ = TDICE_CHANNEL_MODEL_PF_INLINE ;    }
  | STAGGERED                   { $$ = TDICE_CHANNEL_MODEL_PF_STAGGERED ; }
  ;

/******************************************************************************/
/******************************* Layers ***************************************/
/******************************************************************************/

layers_list_opt

  : // Declaring a list of layers is not mandatory

  | layers_list

  ;

layers_list

  : layer
    {
        layer_list_insert_end (&stkd->Layers, $1) ;

        layer_free ($1) ;
    }
  | layers_list layer  // $1 : pointer to the last layer in the list
                       // $2 : pointer to the layer to add in the list
    {

        // checks if layer id already exist

        if (layer_list_find (&stkd->Layers, $2) != NULL)
        {
            sprintf (error_message, "Layer %s already declared", $2->Id) ;

            STKERROR (error_message) ;

            layer_free ($2) ;

            YYABORT ;
        }

        layer_list_insert_end (&stkd->Layers, $2) ;

        layer_free ($2) ;
    }
  ;

layer

  : LAYER IDENTIFIER ':'       // $2
       HEIGHT DVALUE ';'       // $5
       MATERIAL IDENTIFIER ';' // $8
       optional_layout         // $10
  {
        Layer_t *layer = $$ = layer_calloc () ;

        if (layer == NULL)
        {
            STKERROR ("Malloc layer failed") ;

            string_destroy (&$2) ;
            string_destroy (&$8) ;

            if ($10 != NULL)

                string_destroy ($10) ;

            YYABORT ;
        }

        layer->Height = $5 ;

        string_copy (&layer->Id, &$2) ;
        string_copy (&layer->Material.Id, &$8) ;

        Material_t *tmp = material_list_find

            (&stkd->Materials, &layer->Material) ;

        if (tmp == NULL)
        {
            sprintf (error_message, "Unknown material %s", $8) ;

            STKERROR (error_message) ;

            layer_free (layer) ;

            string_destroy (&$2) ;
            string_destroy (&$8) ;

            if ($10 != NULL)

                string_destroy ($10) ;

            YYABORT ;
        }

        material_copy (&layer->Material, tmp) ;

        if ($10 != NULL
            &&  fill_layout (layer, stkd->Dimensions, &stkd->Materials, *$10) == TDICE_FAILURE)
        {
            layer_free (layer) ;

            string_destroy (&$2) ;
            string_destroy (&$8) ;

            if ($10 != NULL)

                string_destroy ($10) ;

           stack_description_destroy (stkd) ;

           YYABORT ; // CHECKME error messages printed in this case ....
        }

        string_destroy (&$2) ;
        string_destroy (&$8) ;

        if ($10 != NULL)

            string_destroy ($10) ;
  }

  ;

optional_layout

  : /* empty */

    {
        $$ = NULL ;
    }

  | LAYOUT PATH ';' // $2

    {
        $$ = &$2 ;
    }

  ;

/******************************************************************************/
/******************************* Dies *****************************************/
/******************************************************************************/

die_top_layers_list

  : // Declaring a "non active" layer over the source layer is not mandatory

  | die_top_layers_list die_layer
    {
        layer_list_insert_end (&dielayers, $2) ;

        layer_free ($2) ;
    }
  ;

die_bottom_layers_list

  : // Declaring a "non active" layer below the source layer is not mandatory

  | die_bottom_layers_list die_layer
    {
        layer_list_insert_end (&dielayers, $2) ;

        source_layer_offset ++ ;

        layer_free ($2) ;
    }
  ;

die_layer

  : LAYER die_layer_content
    {
        $$ = $2 ;
    }
  ;

die_source_layer

  : SOURCE die_layer_content
    {
        layer_list_insert_end (&dielayers, $2) ;

        layer_free ($2) ;
    }
  ;

die_layer_content

  : DVALUE IDENTIFIER ';' // $1 and $2
    {
        Layer_t *layer = $$ = layer_calloc () ;

        if (layer == NULL)
        {
            STKERROR ("Malloc layer failed") ;

            string_destroy (&$2) ;

            layer_list_destroy (&dielayers) ;

            YYABORT ;
        }

        layer->Height = $1 ;

        string_copy (&layer->Material.Id, &$2) ;

        Material_t *tmp = material_list_find (&stkd->Materials, &layer->Material) ;

        if (tmp == NULL)
        {
            sprintf (error_message, "Unknown material %s", $2) ;

            STKERROR (error_message) ;

            layer_list_destroy (&dielayers) ;

            string_destroy (&$2) ;

            layer_free (layer) ;

            YYABORT ;
        }

        material_copy (&layer->Material, tmp) ;

        string_destroy (&$2) ;
    }

  | IDENTIFIER ';' // $1 the id of a layer in the layer section
    {
        Layer_t *layer = $$ = layer_calloc () ;

        if (layer == NULL)
        {
            STKERROR ("Malloc layer failed") ;

            string_destroy (&$1) ;

            layer_list_destroy (&dielayers) ;

            YYABORT ;
        }

        string_copy (&layer->Id, &$1) ;

        Layer_t *tmp = layer_list_find (&stkd->Layers, layer) ;

        if (tmp == NULL)
        {
            sprintf (error_message, "Unknown layer %s", $1) ;

            STKERROR (error_message) ;

            layer_free (layer) ;

            string_destroy (&$1) ;

            layer_list_destroy (&dielayers) ;

            YYABORT ;
        }

        layer_copy (layer, tmp) ;

        string_destroy (&$1) ;
    }
  ;

dies_list

  : die                   // $1 : pointer to the first die found
    {
        die_list_insert_end (&stkd->Dies, $1) ;

        die_free ($1) ;
    }
  | dies_list die         // $1 : pointer to the last die in the list
                          // $2 : pointer to the die to add in the list
    {
        if (die_list_find (&stkd->Dies, $2) != NULL)
        {
            sprintf (error_message, "Die %s already declared", $2->Id) ;

            STKERROR (error_message) ;

            die_free ($2) ;

            YYABORT ;
        }

        die_list_insert_end (&stkd->Dies, $2) ;

        die_free ($2) ;
    }
  ;

die

  : DIE IDENTIFIER ':'          // $2 Die identifier
        die_top_layers_list     // $4 list of layers (above source -> top)
        die_source_layer        // $5 source layer
        die_bottom_layers_list  // $6 list of layers (bottom -> below source)
    {
        Die_t *die = $$ = die_calloc () ;

        if (die == NULL)
        {
            STKERROR ("Malloc die failed") ;

            string_destroy (&$2) ;

            layer_list_destroy (&dielayers) ;

            YYABORT ;
        }

        string_copy (&die->Id, &$2) ;

        die->NLayers           = dielayers.Size ;
        die->SourceLayerOffset = source_layer_offset ;

        layer_list_copy (&die->Layers, &dielayers) ;

        layer_list_destroy (&dielayers) ;
        layer_list_init    (&dielayers) ;

        source_layer_offset = 0u ;

        string_destroy (&$2) ;
    }
  ;

/******************************************************************************/
/******************************* Dimensions ***********************************/
/******************************************************************************/

dimensions

  : DIMENSIONS ':'
        CHIP LENGTH DVALUE ',' WIDTH DVALUE ';'    // $5  $8
        CELL LENGTH DVALUE ',' WIDTH DVALUE ';'    // $12 $15
    {
        stkd->Dimensions = dimensions_calloc () ;

        if (stkd->Dimensions == NULL)
        {
            STKERROR ("Malloc dimensions failed") ;

            YYABORT ;
        }

        stkd->Dimensions->Chip.Length = $5 ;
        stkd->Dimensions->Chip.Width  = $8 ;

        stkd->Dimensions->Cell.ChannelLength   = $12 ;
        stkd->Dimensions->Cell.FirstWallLength = $12 ;
        stkd->Dimensions->Cell.LastWallLength  = $12 ;
        stkd->Dimensions->Cell.WallLength      = $12 ;

        stkd->Dimensions->Cell.Width  = $15 ;

        stkd->Dimensions->Grid.NRows    = ($8 / $15) ;
        stkd->Dimensions->Grid.NColumns = ($5 / $12) ;


        if (stkd->Channel != NULL)
        {
            if (stkd->Channel->ChannelModel == TDICE_CHANNEL_MODEL_MC_4RM)
            {
                stkd->Dimensions->Cell.ChannelLength   = channel_length ;
                stkd->Dimensions->Cell.FirstWallLength = first_wall_length ;
                stkd->Dimensions->Cell.LastWallLength  = last_wall_length ;
                stkd->Dimensions->Cell.WallLength      = wall_length ;

                CellDimension_t ratio
                    = ($5 - first_wall_length - last_wall_length -channel_length)
                    /
                    (channel_length + wall_length) ;

                if ( ratio - (int) ratio != 0)
                {
                    STKERROR ("Error: cell dimensions does not fit the chip length correctly") ;

                    YYABORT ;
                }

                stkd->Dimensions->Grid.NColumns = 2 * ratio + 3 ;

                if ((stkd->Dimensions->Grid.NColumns & 1) == 0)
                {
                    STKERROR ("Error: column number must be odd") ;

                    YYABORT ;
                }

                // Check the number of columns

                if (stkd->Dimensions->Grid.NColumns < 3)
                {
                    STKERROR ("Error: not enough columns") ;

                    YYABORT ;
                }

                stkd->Channel->NChannels = ((stkd->Dimensions->Grid.NColumns - 1 )  / 2) ;
            }
            else if (stkd->Channel->ChannelModel == TDICE_CHANNEL_MODEL_MC_2RM)
            {
                stkd->Channel->NChannels = (($5 / stkd->Channel->Pitch) + 0.5) ; // round function
            }
        }
    }
  ;

/******************************************************************************/
/******************************* Stack ****************************************/
/******************************************************************************/

stack

  : STACK ':'
        stack_elements
    {
        if (num_dies == 0u)
        {
            STKERROR ("Error: stack must contain at least one die") ;

            YYABORT ;
        }

        StackElement_t *bmost = stack_element_list_data (stack_element_list_end (&stkd->StackElements)) ;

        if (bmost->SEType == TDICE_STACK_ELEMENT_CHANNEL)
        {
            STKERROR ("Error: cannot declare a channel as bottom-most stack element") ;

            YYABORT ;
        }

        StackElement_t *tmost = stack_element_list_data (stack_element_list_begin (&stkd->StackElements)) ;

        if (tmost->SEType == TDICE_STACK_ELEMENT_CHANNEL)
        {
            STKERROR ("Error: cannot declare a channel as top-most stack element") ;

            YYABORT ;
        }

        if (   stkd->TopHeatSink    == NULL
            && stkd->BottomHeatSink == NULL
            && stkd->Channel        == NULL)

            fprintf (stderr, "Warning: no dissipation has been declared\n") ;

        if (stkd->TopHeatSink != NULL)

            tmost->TopSink = heat_sink_clone (stkd->TopHeatSink) ;

        if (stkd->BottomHeatSink != NULL)

            bmost->BottomSink = heat_sink_clone (stkd->BottomHeatSink) ;

        // Counts the number of layers and fix the layer offset starting from
        // the bottom most element in the stack. This operation can be done only
        // here since the parser processes elements in the stack from the top most.

        CellIndex_t layer_index = first_layer (stkd->Dimensions) ;

        StackElementListNode_t *stkeln ;

        for (stkeln  = stack_element_list_end (&stkd->StackElements) ;
             stkeln != NULL ;
             stkeln  = stack_element_list_prev (stkeln))
        {
            StackElement_t *stk_el = stack_element_list_data (stkeln) ;

            stk_el->Offset = layer_index ;
            layer_index   += stk_el->NLayers ;
        }

        stkd->Dimensions->Grid.NLayers = layer_index ;

        // Stores the vertical profile of cell heights

        stkd->Dimensions->Cell.NHeights = layer_index ;
        stkd->Dimensions->Cell.Heights =

            (CellDimension_t *) malloc (layer_index * sizeof (CellDimension_t)) ;

        if (stkd->Dimensions->Cell.Heights == NULL)
        {
            STKERROR ("Malloc heights failed") ;

            YYABORT ;
        }

        layer_index  = first_layer (stkd->Dimensions) ;

        for (stkeln  = stack_element_list_end (&stkd->StackElements) ;
             stkeln != NULL ;
             stkeln  = stack_element_list_prev (stkeln))
        {
            StackElement_t *stk_el_ = stack_element_list_data (stkeln) ;

            switch (stk_el_->SEType)
            {
                case TDICE_STACK_ELEMENT_CHANNEL :
                {
                    CellIndex_t tmp ;

                    for (tmp = 0u ; tmp != stk_el_->Pointer.Channel->NLayers ; tmp++)

                        stkd->Dimensions->Cell.Heights [ layer_index++ ]

                            = stk_el_->Pointer.Channel->Height ;

                    break ;
                }
                case TDICE_STACK_ELEMENT_LAYER :

                    stkd->Dimensions->Cell.Heights [ layer_index++ ]

                        = stk_el_->Pointer.Layer->Height ;

                    break ;

                case TDICE_STACK_ELEMENT_DIE :
                {
                    LayerListNode_t *lnd ;

                    for (lnd  = layer_list_end(&stk_el_->Pointer.Die->Layers) ;
                         lnd != NULL ;
                         lnd = layer_list_prev (lnd))

                        stkd->Dimensions->Cell.Heights [ layer_index++ ] = layer_list_data(lnd)->Height ;

                    break ;
                }
                case TDICE_STACK_ELEMENT_NONE :

                    sprintf (error_message, "Unset stack type %d", stk_el_->SEType) ;

                    STKERROR (error_message) ;

                    YYABORT ;

                    break ;

                default :

                    sprintf (error_message, "Unknown stack type %d", stk_el_->SEType) ;

                    STKERROR (error_message) ;

                    YYABORT ;
            }
        }

        // Evaluate the number of cells and nonzero elements

        uint64_t ncells =   (uint64_t) get_number_of_layers (stkd->Dimensions)
                          * (uint64_t) get_number_of_rows (stkd->Dimensions)
                          * (uint64_t) get_number_of_columns (stkd->Dimensions) ;

        if (ncells > INT32_MAX)
        {
            sprintf (error_message,
                "%lu are too many cells ... (SuperLU uses 'int')",
                ncells) ;

            STKERROR (error_message) ;

            YYABORT ;
        }

        stkd->Dimensions->Grid.NCells = (CellIndex_t) ncells ;

        ChannelModel_t cm =  stkd->Channel == NULL       ?
                             TDICE_CHANNEL_MODEL_NONE    :
                             stkd->Channel->ChannelModel ;

        compute_number_of_connections (stkd->Dimensions, num_channels, cm) ;
   }
  ;

stack_elements

  : stack_element
    {
        stack_element_list_insert_end (&stkd->StackElements, $1) ;

        stack_element_free ($1) ;
    }
  | stack_elements stack_element
    {
        if (stack_element_list_find (&stkd->StackElements, $2) != NULL)
        {
            sprintf (error_message, "Stack element %s already declared", $2->Id) ;

            STKERROR (error_message) ;

            stack_element_free ($2) ;

            YYABORT ;
        }

        StackElement_t *stkel = stack_element_list_data

            (stack_element_list_begin (&stkd->StackElements)) ;

        if (   stkel->SEType == TDICE_STACK_ELEMENT_CHANNEL
            &&    $2->SEType == TDICE_STACK_ELEMENT_CHANNEL)
        {
            STKERROR ("Error: cannot declare two consecutive channels") ;

            YYABORT ;
        }

        stack_element_list_insert_end (&stkd->StackElements, $2) ;

        stack_element_free ($2) ;
    }
  ;

stack_element

  : LAYER IDENTIFIER IDENTIFIER ';'    // $2 Identifier for the stack element
                                       // $3 Identifier of the layer
    {
        StackElement_t *stack_element = $$ = stack_element_calloc () ;

        if (stack_element == NULL)
        {
            STKERROR ("Malloc stack element failed") ;

            string_destroy (&$2) ;
            string_destroy (&$3) ;

            YYABORT ;
        }

        Layer_t *layer = layer_calloc ( ) ;

        if (layer == NULL)
        {
            STKERROR ("Malloc layer for stack element failed") ;

            string_destroy (&$2) ;
            string_destroy (&$3) ;

            YYABORT ;
        }

        string_copy (&layer->Id, &$3) ;

        Layer_t *tmp = layer_list_find (&stkd->Layers, layer) ;

        if (tmp == NULL)
        {
            sprintf (error_message, "Unknown layer %s", $3) ;

            STKERROR (error_message) ;

            layer_free (layer) ;

            string_destroy (&$2) ;
            string_destroy (&$3) ;

            stack_element_free (stack_element) ;

            YYABORT ;
        }

        layer_copy (layer, tmp) ;

        stack_element->SEType        = TDICE_STACK_ELEMENT_LAYER ;
        stack_element->Pointer.Layer = layer ;
        stack_element->NLayers       = 1 ;

        string_copy (&stack_element->Id, &$2) ;

        string_destroy (&$2) ;
        string_destroy (&$3) ;
    }

  | CHANNEL IDENTIFIER ';'  // $2 Identifier for the stack element
    {
        num_channels++ ;

        if (stkd->Channel == NULL)
        {
            STKERROR ("Error: channel used in stack but not declared") ;

            string_destroy (&$2) ;

            YYABORT ;
        }

        StackElement_t *stack_element = $$ = stack_element_calloc () ;

        if (stack_element == NULL)
        {
            STKERROR ("Malloc stack element failed") ;

            string_destroy (&$2) ;

            YYABORT ;
        }

        stack_element->SEType          = TDICE_STACK_ELEMENT_CHANNEL ;
        stack_element->Pointer.Channel = stkd->Channel ; // This might be NULL !!!
        stack_element->NLayers         = stkd->Channel->NLayers ;

        string_copy (&stack_element->Id, &$2) ;

        string_destroy (&$2) ;
    }

  | DIE IDENTIFIER IDENTIFIER FLOORPLAN PATH ';'  // $2 Identifier for the stack element
                                                  // $3 Identifier of the die
                                                  // $5 Path of the floorplan file
    {
        num_dies++ ;

        StackElement_t *stack_element = $$ = stack_element_calloc () ;

        if (stack_element == NULL)
        {
            STKERROR ("Malloc stack element failed") ;

            string_destroy (&$2) ;
            string_destroy (&$3) ;
            string_destroy (&$5) ;

            YYABORT ;
        }

        Die_t *die = die_calloc ( ) ;

        if (die == NULL)
        {
            STKERROR ("Malloc die for stack element failed") ;

            string_destroy (&$2) ;
            string_destroy (&$3) ;
            string_destroy (&$5) ;

            stack_element_free (stack_element) ;

            YYABORT ;
        }

        string_copy (&die->Id, &$3) ;

        Die_t *tmp = die_list_find (&stkd->Dies, die) ;

        if (tmp == NULL)
        {
            sprintf (error_message, "Unknown die %s", $3) ;

            STKERROR (error_message) ;

            die_free (die) ;

            string_destroy (&$2) ;
            string_destroy (&$3) ;
            string_destroy (&$5) ;

            stack_element_free (stack_element) ;

            YYABORT ;
        }

        die_copy (die, tmp) ;

        if (   fill_floorplan (&die->Floorplan, stkd->Dimensions, $5)
            == TDICE_FAILURE)
        {
            string_destroy (&$2) ;
            string_destroy (&$3) ;
            string_destroy (&$5) ;

            stack_description_destroy (stkd) ;

            YYABORT ; // CHECKME error messages printed in this case ....
        }

        stack_element->SEType      = TDICE_STACK_ELEMENT_DIE ;
        stack_element->Pointer.Die = die ;
        stack_element->NLayers     = stack_element->Pointer.Die->NLayers ;

        string_copy (&stack_element->Id, &$2) ;

        string_destroy (&$2) ;
        string_destroy (&$3) ;
        string_destroy (&$5) ;
    }
  ;

/******************************************************************************/
/******************************* Solevr type **********************************/
/******************************************************************************/

solver

  : SOLVER ':'
        STEADY ';'
        INITIAL_ TEMPERATURE DVALUE ';' // $7

    {
        // StepTime is set to 1 to avoid division by zero when computing
        // the capacitance of a thermal cell

        analysis->AnalysisType = TDICE_ANALYSIS_TYPE_STEADY ;
        analysis->StepTime     = (Time_t) 1.0 ;
        analysis->SlotTime     = (Time_t) 0.0 ;
        analysis->SlotLength   = 1u ; // CHECKME

        analysis->InitialTemperature = (Temperature_t) $7;
    }

  | SOLVER ':'
        TRANSIENT STEP DVALUE ',' SLOT DVALUE ';'  // $5 StepTime
                                                   // $8 SlotTime
        INITIAL_ TEMPERATURE DVALUE ';'            // $12 Initial temperature
    {
        if ($8 < $5)
        {
            STKERROR ("Slot time must be higher than StepTime") ;

            YYABORT ;
        }

        if ($5 <= 0.0)
        {
            STKERROR ("StepTime must be a positive value") ;

            YYABORT ;
        }

        if ($8 <= 0.0)
        {
            STKERROR ("SlotTime must be a positive value") ;

            YYABORT ;
        }

        analysis->AnalysisType       = TDICE_ANALYSIS_TYPE_TRANSIENT ;
        analysis->StepTime           = (Time_t) $5 ;
        analysis->SlotTime           = (Time_t) $8 ;
        analysis->InitialTemperature = (Temperature_t) $12 ;

        // Execute correct division Slot / Step avoiding floating point issues
        // i.e. both slot and step are mutiplied by 10 until the decimal part
        // is removed. Then run division ...

        Time_t sl_int, sl_dec, sl = analysis->SlotTime ;
        Time_t st_int, st_dec, st = analysis->StepTime ;

        sl_dec = modf (sl, &sl_int) ;
        st_dec = modf (st, &st_int) ;

        while (sl_dec != 0.0 || st_dec != 0.0)
        {
            sl *= 10.0 ; st *= 10.0 ;

            sl_dec = modf (sl, &sl_int) ;
            st_dec = modf (st, &st_int) ;
        }

        analysis->SlotLength   = (Quantity_t) sl_int / (Quantity_t) st_int ;
    }
  ;

/******************************************************************************/
/****************************** Desired Output ********************************/
/******************************************************************************/

inspection_points

  :  // Declaring this section is not mandatory

  |  OUTPUT ':' inspection_points_list

  ;

inspection_points_list

  :  inspection_point
     {
        add_inspection_point (output, $1) ;

        inspection_point_free ($1) ;
     }

  |  inspection_points_list inspection_point
     {
        add_inspection_point (output, $2) ;

        inspection_point_free ($2) ;
     }
  ;

inspection_point

  :  TCELL '(' IDENTIFIER ',' DVALUE ',' DVALUE ',' PATH when ')' ';'

     // $3       Identifier of the stack element (layer, channel or die)
     // ($5, $7) Coordinates of the cell to monitor
     // $9       Path of the output file
     // $10      when to generate output for this observation

     {
        StackElement_t stack_element ;

        stack_element_init (&stack_element) ;

        string_copy (&stack_element.Id, &$3) ;

        StackElement_t *tmp = stack_element_list_find

            (&stkd->StackElements, &stack_element) ;

        if (tmp == NULL)
        {
            sprintf (error_message, "Unknown stack element %s", $3) ;

            STKERROR (error_message) ;

            string_destroy (&$3) ;
            string_destroy (&$9) ;

            stack_element_destroy (&stack_element) ;

            YYABORT ;
        }

        stack_element_destroy (&stack_element) ;

        InspectionPoint_t *ipoint = $$ = inspection_point_calloc () ;

        if (ipoint == NULL)
        {
            STKERROR ("Malloc inspection point failed") ;

            string_destroy (&$3) ;
            string_destroy (&$9) ;

            YYABORT ;
        }

        ipoint->OType         = TDICE_OUTPUT_TYPE_TCELL ;
        ipoint->Instant       = $10 ;
        ipoint->StackElement  = tmp ;

        string_copy (&ipoint->FileName, &$9) ;

        align_tcell (ipoint, $5, $7, stkd->Dimensions) ;

        string_destroy (&$3) ;
        string_destroy (&$9) ;
     }

  |  TFLP  '(' IDENTIFIER ',' PATH ',' maxminavg when ')'  ';'

     // $3 Identifier of the stack element (must be a die)
     // $5 Path of the output file
     // $7 temperature type
     // $8 when to generate output for this observation

     {
        StackElement_t stack_element ;

        stack_element_init (&stack_element) ;

        string_copy (&stack_element.Id, &$3) ;

        StackElement_t *tmp = stack_element_list_find

            (&stkd->StackElements, &stack_element) ;

        if (tmp == NULL)
        {
            sprintf (error_message, "Unknown stack element %s", $3) ;

            STKERROR (error_message) ;

            string_destroy (&$3) ;
            string_destroy (&$5) ;

            stack_element_destroy (&stack_element) ;

            YYABORT ;
        }

        if (tmp->SEType != TDICE_STACK_ELEMENT_DIE)
        {
            sprintf (error_message, "The stack element %s must be a die", $3) ;

            STKERROR (error_message) ;

            string_destroy (&$3) ;
            string_destroy (&$5) ;

            stack_element_destroy (&stack_element) ;

            YYABORT ;
        }

        stack_element_destroy (&stack_element) ;

        InspectionPoint_t *ipoint = $$ = inspection_point_calloc () ;

        if (ipoint == NULL)
        {
            STKERROR ("Malloc inspection point failed") ;

            string_destroy (&$3) ;
            string_destroy (&$5) ;

            YYABORT ;
        }

        ipoint->OType        = TDICE_OUTPUT_TYPE_TFLP ;
        ipoint->Quantity     = $7 ;
        ipoint->Instant      = $8 ;
        ipoint->StackElement = tmp ;

        string_copy (&ipoint->FileName, &$5) ;

        string_destroy (&$3) ;
        string_destroy (&$5) ;
     }

  |  TFLPEL '(' IDENTIFIER '.' IDENTIFIER ',' PATH ',' maxminavg when ')' ';'

     // $3  Identifier of the stack element (must be a die)
     // $5  Identifier of the floorplan element
     // $7  Path of the output file
     // $9  temperature type
     // $10 when to generate output for this observation

     {
        StackElement_t stack_element ;

        stack_element_init (&stack_element) ;

        string_copy (&stack_element.Id, &$3) ;

        StackElement_t *tmp = stack_element_list_find

            (&stkd->StackElements, &stack_element) ;

        if (tmp == NULL)
        {
            sprintf (error_message, "Unknown stack element %s", $3) ;

            STKERROR (error_message) ;

            string_destroy (&$3) ;
            string_destroy (&$5) ;
            string_destroy (&$7) ;

            stack_element_destroy (&stack_element) ;

            YYABORT ;
        }

        if (tmp->SEType != TDICE_STACK_ELEMENT_DIE)
        {
            sprintf (error_message, "The stack element %s must be a die", $3) ;

            STKERROR (error_message) ;

            string_destroy (&$3) ;
            string_destroy (&$5) ;
            string_destroy (&$7) ;

            stack_element_destroy (&stack_element) ;

            YYABORT ;
        }

        stack_element_destroy (&stack_element) ;

        FloorplanElement_t *flpel = get_floorplan_element (&tmp->Pointer.Die->Floorplan, $5) ;

        if (flpel == NULL)
        {
            sprintf (error_message, "Unknown floorplan element %s", $5) ;

            STKERROR (error_message) ;

            string_destroy (&$3) ;
            string_destroy (&$5) ;
            string_destroy (&$7) ;

            YYABORT ;
        }

        InspectionPoint_t *ipoint = $$ = inspection_point_calloc () ;

        if (ipoint == NULL)
        {
            STKERROR ("Malloc inspection point failed") ;

            string_destroy (&$3) ;
            string_destroy (&$5) ;
            string_destroy (&$7) ;

            YYABORT ;
        }

        ipoint->OType            = TDICE_OUTPUT_TYPE_TFLPEL ;
        ipoint->FloorplanElement = flpel ;
        ipoint->Quantity         = $9 ;
        ipoint->Instant          = $10 ;
        ipoint->StackElement     = tmp ;

        string_copy (&ipoint->FileName, &$7) ;

        string_destroy (&$3) ;
        string_destroy (&$5) ;
        string_destroy (&$7) ;
     }

  |  TMAP '(' IDENTIFIER ',' PATH when ')' ';'

     // $3 Identifier of the stack element (layer, channel or die)
     // $5 Path of the output file
     // $6 when to generate output for this observation

     {
        StackElement_t stack_element ;

        stack_element_init (&stack_element) ;

        string_copy (&stack_element.Id, &$3) ;

        StackElement_t *tmp = stack_element_list_find

            (&stkd->StackElements, &stack_element) ;

        if (tmp == NULL)
        {
            sprintf (error_message, "Unknown stack element %s", $3) ;

            STKERROR (error_message) ;

            string_destroy (&$3) ;
            string_destroy (&$5) ;

            stack_element_destroy (&stack_element) ;

            YYABORT ;
        }

        stack_element_destroy (&stack_element) ;

        InspectionPoint_t *ipoint = $$ = inspection_point_calloc () ;

        if (ipoint == NULL)
        {
            STKERROR ("Malloc inspection point failed") ;

            string_destroy (&$3) ;
            string_destroy (&$5) ;

            YYABORT ;
        }

        ipoint->OType        = TDICE_OUTPUT_TYPE_TMAP ;
        ipoint->Instant      = $6 ;
        ipoint->StackElement = tmp ;

        string_copy (&ipoint->FileName, &$5) ;

        string_destroy (&$3) ;
        string_destroy (&$5) ;
     }

  |  PMAP '(' IDENTIFIER ',' PATH when ')' ';'

     // $3 Identifier of the stack element (must be a die)
     // $5 Path of the output file
     // $6 when to generate output for this observation

    {
        StackElement_t stack_element ;

        stack_element_init (&stack_element) ;

        string_copy (&stack_element.Id, &$3) ;

        StackElement_t *tmp = stack_element_list_find

            (&stkd->StackElements, &stack_element) ;

        if (tmp == NULL)
        {
            sprintf (error_message, "Unknown stack element %s", $3) ;

            STKERROR (error_message) ;

            string_destroy (&$3) ;
            string_destroy (&$5) ;

            stack_element_destroy (&stack_element) ;

            YYABORT ;
        }

        stack_element_destroy (&stack_element) ;

        InspectionPoint_t *ipoint = $$ = inspection_point_calloc () ;

        if (ipoint == NULL)
        {
            STKERROR ("Malloc inspection point failed") ;

            string_destroy (&$3) ;
            string_destroy (&$5) ;

            YYABORT ;
        }

        ipoint->OType        = TDICE_OUTPUT_TYPE_PMAP ;
        ipoint->Instant      = $6 ;
        ipoint->StackElement = tmp ;

        string_copy (&ipoint->FileName, &$5) ;

        string_destroy (&$3) ;
        string_destroy (&$5) ;
    }

  |  TCOOLANT '(' IDENTIFIER ',' PATH ',' maxminavg when ')' ';'

     // $3 Identifier of the stack element (must be a channel)
     // $5 Path of the output file
     // $7 temperature type
     // $8 when to generate output for this observation

     {
        StackElement_t stack_element ;

        stack_element_init (&stack_element) ;

        string_copy (&stack_element.Id, &$3) ;

        StackElement_t *tmp = stack_element_list_find

            (&stkd->StackElements, &stack_element) ;

        if (tmp == NULL)
        {
            sprintf (error_message, "Unknown stack element %s", $3) ;

            STKERROR (error_message) ;

            string_destroy (&$3) ;
            string_destroy (&$5) ;

            stack_element_destroy (&stack_element) ;

            YYABORT ;
        }

        if (tmp->SEType != TDICE_STACK_ELEMENT_CHANNEL)
        {
            sprintf (error_message, "The stack element %s must be a channel", $3) ;

            STKERROR (error_message) ;

            string_destroy (&$3) ;
            string_destroy (&$5) ;

            stack_element_destroy (&stack_element) ;

            YYABORT ;
        }

        stack_element_destroy (&stack_element) ;

        InspectionPoint_t *ipoint = $$ = inspection_point_calloc () ;

        if (ipoint == NULL)
        {
            STKERROR ("Malloc inspection point failed") ;

            string_destroy (&$3) ;
            string_destroy (&$5) ;

            YYABORT ;
        }

        ipoint->OType        = TDICE_OUTPUT_TYPE_TCOOLANT ;
        ipoint->Quantity     = $7 ;
        ipoint->Instant      = $8 ;
        ipoint->StackElement = tmp ;

        string_copy (&ipoint->FileName, &$5) ;

        string_destroy (&$3) ;
        string_destroy (&$5) ;
     }
  ;

maxminavg

  :  MAXIMUM { $$ =  TDICE_OUTPUT_QUANTITY_MAXIMUM ; }
  |  MINIMUM { $$ =  TDICE_OUTPUT_QUANTITY_MINIMUM ; }
  |  AVERAGE { $$ =  TDICE_OUTPUT_QUANTITY_AVERAGE ; }
  |  GRADIENT { $$ =  TDICE_OUTPUT_QUANTITY_GRADIENT ; }
  ;

when

  :  // Declaring the instance option is not mandatory (final is assumed)
                { $$ =  TDICE_OUTPUT_INSTANT_FINAL ; }
  |  ',' STEP   { $$ =  TDICE_OUTPUT_INSTANT_STEP ;  }
  |  ',' SLOT   { $$ =  TDICE_OUTPUT_INSTANT_SLOT ;  }
  |  ',' FINAL  { $$ =  TDICE_OUTPUT_INSTANT_FINAL ; }
  ;

%%

/******************************************************************************/

void stack_description_error
(
    StackDescription_t *stkd,
    Analysis_t         *analysis,
    Output_t           *output,
    yyscan_t            scanner,
    const char         *message
)
{
    fprintf (stack_description_get_out (scanner),
             "%s:%d: %s\n",
            stkd->FileName, stack_description_get_lineno (scanner), message) ;

    stack_description_destroy (stkd) ;
    analysis_destroy          (analysis) ;
    output_destroy            (output) ;
}

/******************************************************************************/
