/******************************************************************************
 * This file is part of 3D-ICE, version 2.2 .                                 *
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

%token _2RM                  "keyword 2rm"
%token _4RM                  "keyword 4rm"
%token AMBIENT               "keyword ambient"
%token AREA                  "keyword area"
%token AVERAGE               "keyword average"
%token BOTTOM                "keyword bottom"
%token CAPACITY              "keyword capacity"
%token CELL                  "keyword cell"
%token CHANNEL               "keyword channel"
%token CHIP                  "keyword chip"
%token COEFFICIENT           "keyword coefficient"
%token CONDUCTIVITY          "keyword conductivity"
%token CONNECTION            "keyword connection"
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
%token HEAT                  "keyword heat"
%token HEIGHT                "keyword height"
%token INCOMING              "keyword incoming"
%token INITIAL_              "keyword initial"
%token INLINE                "keyword inline"
%token LAST                  "keyword last"
%token LAYER                 "keyword layer"
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
%token SINK                  "keywork sink"
%token SLOT                  "keyword slot"
%token SOLVER                "keyword solver"
%token SOURCE                "keyword source"
%token SPREADER              "keyword spreader"
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

%destructor { free       ($$) ; } <string>
%destructor { layer_free ($$) ; } <layer_p>

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
    heat_sink_opt
    microchannel_opt
    layers_list_opt
    dies_list
    dimensions
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

            material_free ($2) ;

            STKERROR (error_message) ;

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
            free ($2) ;

            STKERROR ("Malloc material failed") ;

            YYABORT ;
        }

        material->Id                     = $2 ;
        material->ThermalConductivity    = (SolidTC_t) $6 ;
        material->VolumetricHeatCapacity = (SolidVHC_t) $11 ;
    }
  ;

/******************************************************************************/
/******************************* Heatsink *************************************/
/******************************************************************************/

heat_sink_opt

  : // Declaring the heat sink section is not mandatory

  | HEAT SINK ':'
        SINK     HEIGHT   DVALUE     ',' // $6
                 AREA     DVALUE     ',' // $9
                 MATERIAL IDENTIFIER ';' // $12
        SPREADER HEIGHT   DVALUE     ',' // $16
                 AREA     DVALUE     ',' // $19
                 MATERIAL IDENTIFIER ';' // $22
        HEAT TRANSFER COEFFICIENT DVALUE ';'  // $27
        AMBIENT TEMPERATURE       DVALUE ';'  // $31
    {
        stkd->HeatSink = heat_sink_calloc () ;

        if (stkd->HeatSink == NULL)
        {
            STKERROR ("Malloc heat sink failed") ;

            YYABORT ;
        }

        stkd->HeatSink->SinkModel           = TDICE_HEATSINK_MODEL_TRADITIONAL ;
        stkd->HeatSink->NLayers             = NUM_LAYERS_HEATSINK_TRADITIONAL ;
        stkd->HeatSink->SourceLayerOffset   = SOURCE_OFFSET_HEATSINK_TRADITIONAL ;
        stkd->HeatSink->SinkHeight          = $6 ;
        stkd->HeatSink->SinkArea            = $9 ;
        stkd->HeatSink->SinkMaterial.Id     = $12 ;
        stkd->HeatSink->SpreaderHeight      = $16 ;
        stkd->HeatSink->SpreaderArea        = $19 ;
        stkd->HeatSink->SpreaderMaterial.Id = $22 ;
        stkd->HeatSink->AmbientHTC          = (AmbientHTC_t) $27 ;
        stkd->HeatSink->AmbientTemperature  = $31 ;

        if (stkd->HeatSink->SinkArea < stkd->HeatSink->SpreaderArea)
        {
            sprintf (error_message,
                "Error: the area of the spreader must be smaller than the sink") ;

            free ($12) ;
            free ($22) ;

            STKERROR (error_message) ;

            YYABORT ;
        }

        Material_t *tmp = material_list_find

            (&stkd->Materials, &stkd->HeatSink->SinkMaterial) ;

        if (tmp == NULL)
        {
            sprintf (error_message, "Unknown sink material %s", $12) ;

            STKERROR (error_message) ;

            YYABORT ;
        }

        material_copy (&stkd->HeatSink->SinkMaterial, tmp) ;

        tmp = material_list_find

            (&stkd->Materials, &stkd->HeatSink->SpreaderMaterial) ;

        if (tmp == NULL)
        {
            sprintf (error_message, "Unknown spreader material %s", $22) ;

            STKERROR (error_message) ;

            YYABORT ;
        }

        material_copy (&stkd->HeatSink->SpreaderMaterial, tmp) ;
    }

  | CONNECTION TO AMBIENT ':'
        HEAT TRANSFER COEFFICIENT DVALUE ';'  // $8
        AMBIENT TEMPERATURE       DVALUE ';'  // $12
    {
        stkd->HeatSink = heat_sink_calloc () ;

        if (stkd->HeatSink == NULL)
        {
            STKERROR ("Malloc heat sink failed") ;

            YYABORT ;
        }

        stkd->HeatSink->SinkModel          = TDICE_HEATSINK_MODEL_CONNECTION_TO_AMBIENT ;
        stkd->HeatSink->NLayers            = NUM_LAYERS_HEATSINK_CONNECTION_TO_AMBIENT ;
        stkd->HeatSink->SourceLayerOffset  = SOURCE_OFFSET_HEATSINK_CONNECTION_TO_AMBIENT ;
        stkd->HeatSink->AmbientHTC         = (AmbientHTC_t) $8 ;
        stkd->HeatSink->AmbientTemperature = $12 ;
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
            free ($19) ;

            STKERROR ("Malloc channel failed") ;

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
        stkd->Channel->WallMaterial.Id   = $19 ;
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

            YYABORT ;
        }

        material_copy (&stkd->Channel->WallMaterial, tmp) ;
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
            free ($17) ;

            STKERROR ("Malloc channel failed") ;

            YYABORT ;
        }

        stkd->Channel->ChannelModel      = TDICE_CHANNEL_MODEL_MC_2RM ;
        stkd->Channel->NLayers           = NUM_LAYERS_CHANNEL_2RM ;
        stkd->Channel->SourceLayerOffset = SOURCE_OFFSET_CHANNEL_2RM ;
        stkd->Channel->Height            = $5 ;
        stkd->Channel->Length            = $9 ;
        stkd->Channel->Pitch             = $13 + $9 ;
        stkd->Channel->Porosity          = stkd->Channel->Length / stkd->Channel->Pitch ;
        stkd->Channel->WallMaterial.Id   = $17 ;
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

            YYABORT ;
        }

        material_copy (&stkd->Channel->WallMaterial, tmp) ;
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
            free ($20) ;

            STKERROR ("Malloc channel failed") ;

            YYABORT ;
        }

        stkd->Channel->Height                = $4 ;
        stkd->Channel->Porosity              = POROSITY ($8, $12) ;
        stkd->Channel->Pitch                 = $12 ;
        stkd->Channel->ChannelModel          = $16 ;
        stkd->Channel->NLayers               = NUM_LAYERS_CHANNEL_2RM ;
        stkd->Channel->SourceLayerOffset     = SOURCE_OFFSET_CHANNEL_2RM ;
        stkd->Channel->WallMaterial.Id       = $20 ;
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

            YYABORT ;
        }

        material_copy (&stkd->Channel->WallMaterial, tmp) ;
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

            layer_free ($2) ;

            STKERROR (error_message) ;

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
  {
        Layer_t *layer = $$ = layer_calloc () ;

        if (layer == NULL)
        {
            free ($2) ;
            free ($8) ;

            STKERROR ("Malloc layer failed") ;

            YYABORT ;
        }

        layer->Height      = $5 ;
        layer->Id          = $2 ;
        layer->Material.Id = $8 ;

        Material_t *tmp = material_list_find

            (&stkd->Materials, &layer->Material) ;

        if (tmp == NULL)
        {
            sprintf (error_message, "Unknown material %s", $8) ;

            layer_free (layer) ;

            STKERROR (error_message) ;

            YYABORT ;
        }

        material_copy (&layer->Material, tmp) ;
  }

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
            free ($2) ;

            layer_list_destroy (&dielayers) ;

            STKERROR ("Malloc layer failed") ;

            YYABORT ;
        }

        layer->Height      = $1 ;
        layer->Id          = NULL ;
        layer->Material.Id = $2 ;

        Material_t *tmp = material_list_find (&stkd->Materials, &layer->Material) ;

        if (tmp == NULL)
        {
            sprintf (error_message, "Unknown material %s", $2) ;

            layer_list_destroy (&dielayers) ;

            layer_free (layer) ;

            STKERROR (error_message) ;

            YYABORT ;
        }

        material_copy (&layer->Material, tmp) ;
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

            die_free ($2) ;

            STKERROR (error_message) ;

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
            free ($2) ;

            layer_list_destroy (&dielayers) ;

            STKERROR ("Malloc die failed") ;

            YYABORT ;
        }

        die->Id                = $2 ;
        die->NLayers           = dielayers.Size ;
        die->SourceLayerOffset = source_layer_offset ;

        layer_list_copy (&die->Layers, &dielayers) ;

        layer_list_destroy (&dielayers) ;
        layer_list_init    (&dielayers) ;

        source_layer_offset = 0u ;
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

        // Check if the spreader is larger than the chip

        if (   stkd->HeatSink != NULL
            && stkd->HeatSink->SinkModel == TDICE_HEATSINK_MODEL_TRADITIONAL
            && stkd->HeatSink->SpreaderArea < get_chip_area (stkd->Dimensions))
        {
            sprintf (error_message,
                "Error: the area of the spreader must be smaller than the chip") ;

            STKERROR (error_message) ;

            YYABORT ;
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

        StackElement_t *bmost = stack_element_list_data (stack_element_list_end   (&stkd->StackElements)) ;
        StackElement_t *tmost = stack_element_list_data (stack_element_list_begin (&stkd->StackElements)) ;

        if (bmost->Type == TDICE_STACK_ELEMENT_CHANNEL)
        {
            STKERROR ("Error: cannot declare a channel as bottom-most stack element") ;

            YYABORT ;
        }

        if (stkd->HeatSink == NULL && stkd->Channel == NULL)

            fprintf (stderr, "Warning: neither heat sink nor channel has been declared\n") ;

        if (stkd->HeatSink != NULL)
        {
            if (stkd->HeatSink->SinkModel == TDICE_HEATSINK_MODEL_CONNECTION_TO_AMBIENT)
            {
                if (tmost->Type == TDICE_STACK_ELEMENT_LAYER)
                {
                    material_copy (
                         &stkd->HeatSink->SinkMaterial,
                         &tmost->Pointer.Layer->Material) ;
                }
                else
                {
                    material_copy (
                        &stkd->HeatSink->SinkMaterial,
                        &layer_list_data (layer_list_begin (&tmost->Pointer.Die->Layers))->Material) ;
                }
            }

            // Creates an extra stack element to be add in the 3d stack

            StackElement_t stack_element ;

            stack_element_init (&stack_element) ;

            stack_element.Id               = strdup ("Ambient") ;
            stack_element.Type             = TDICE_STACK_ELEMENT_HEATSINK ;
            stack_element.Pointer.HeatSink = stkd->HeatSink ;
            stack_element.NLayers          = stkd->HeatSink->NLayers ;

            stack_element_list_insert_begin (&stkd->StackElements, &stack_element) ;

            stack_element_destroy (&stack_element) ;
        }

        // Counts the number of layers and fix the layer offset starting from
        // the bottom most element in the stack. This operation can be done only
        // here since the parser processes elements in the stack from the top most.

        CellIndex_t layer_index = 0u ;

        StackElementListNode_t *stkeln ;

        for (stkeln  = stack_element_list_end (&stkd->StackElements) ;
             stkeln != NULL ;
             stkeln  = stack_element_list_prev (stkeln))
        {
            StackElement_t *stk_el = stack_element_list_data (stkeln) ;

            stk_el->Offset = layer_index ;
            layer_index   += stk_el->NLayers ;
        }

        tmost = stack_element_list_data (stack_element_list_begin (&stkd->StackElements)) ;

        if (   tmost->Type == TDICE_STACK_ELEMENT_HEATSINK
            && stkd->HeatSink->SinkModel == TDICE_HEATSINK_MODEL_CONNECTION_TO_AMBIENT)

            tmost->Offset-- ;

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

        layer_index  = 0u ;

        for (stkeln  = stack_element_list_end (&stkd->StackElements) ;
             stkeln != NULL ;
             stkeln  = stack_element_list_prev (stkeln))
        {
            StackElement_t *stk_el_ = stack_element_list_data (stkeln) ;

            if (stk_el_->Type == TDICE_STACK_ELEMENT_CHANNEL)
            {
                CellIndex_t tmp ;

                for (tmp = 0u ; tmp != stk_el_->Pointer.Channel->NLayers ; tmp++)

                    stkd->Dimensions->Cell.Heights [ layer_index++ ]

                        = stk_el_->Pointer.Channel->Height ;
            }
            else if (stk_el_->Type == TDICE_STACK_ELEMENT_LAYER)
            {
                stkd->Dimensions->Cell.Heights [ layer_index++ ]

                    = stk_el_->Pointer.Layer->Height ;
            }
            else if (stk_el_->Type == TDICE_STACK_ELEMENT_DIE)
            {
                LayerListNode_t *lnd ;

                for (lnd  = layer_list_end(&stk_el_->Pointer.Die->Layers) ;
                     lnd != NULL ;
                     lnd = layer_list_prev (lnd))
                {
                    stkd->Dimensions->Cell.Heights [ layer_index++ ] = layer_list_data(lnd)->Height ;
                }
            }
            else if (stk_el_->Type == TDICE_STACK_ELEMENT_HEATSINK)
            {
                if (stkd->HeatSink->SinkModel == TDICE_HEATSINK_MODEL_TRADITIONAL)
                {
                    stkd->Dimensions->Cell.Heights [ layer_index++ ]

                        = stkd->HeatSink->SpreaderHeight ;

                    stkd->Dimensions->Cell.Heights [ layer_index++ ]

                        = stkd->HeatSink->SinkHeight ;
                }
            }
            else
            {
                sprintf (error_message, "Unknown stack type %d", stk_el_->Type) ;

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

        HeatSinkModel_t hm = stkd->HeatSink == NULL    ?
                             TDICE_HEATSINK_MODEL_NONE :
                             stkd->HeatSink->SinkModel ;

        compute_number_of_connections

            (stkd->Dimensions, num_channels, cm, hm) ;
   }
  ;

stack_elements

  : stack_element
    {
        if (   stkd->StackElements.Size == 0

            && $1->Type == TDICE_STACK_ELEMENT_CHANNEL)
        {
            STKERROR ("Error: cannot declare a channel as top-most stack element") ;

            YYABORT ;
        }

        stack_element_list_insert_end (&stkd->StackElements, $1) ;

        stack_element_free ($1) ;
    }
  | stack_elements stack_element
    {
        if (stack_element_list_find (&stkd->StackElements, $2) != NULL)
        {
            sprintf (error_message, "Stack element %s already declared", $2->Id) ;

            stack_element_free ($2) ;

            STKERROR (error_message) ;

            YYABORT ;
        }

        StackElement_t *stkel = stack_element_list_data

            (stack_element_list_begin (&stkd->StackElements)) ;

        if (   stkel->Type == TDICE_STACK_ELEMENT_CHANNEL
            &&    $2->Type == TDICE_STACK_ELEMENT_CHANNEL)
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
            free ($2) ;
            free ($3) ;

            STKERROR ("Malloc stack element failed") ;

            YYABORT ;
        }

        Layer_t *layer = layer_calloc ( ) ;

        if (layer == NULL)
        {
            free ($2) ;
            free ($3) ;

            STKERROR ("Malloc layer for stack element failed") ;

            YYABORT ;
        }

        layer->Id = $3 ;

        Layer_t *tmp = layer_list_find (&stkd->Layers, layer) ;

        if (tmp == NULL)
        {
            sprintf (error_message, "Unknown layer %s", $3) ;

            layer_free (layer) ;

            free ($2) ;

            stack_element_free (stack_element) ;

            STKERROR (error_message) ;

            YYABORT ;
        }

        layer_copy (layer, tmp) ;

        stack_element->Type          = TDICE_STACK_ELEMENT_LAYER ;
        stack_element->Pointer.Layer = layer ;
        stack_element->Id            = $2 ;
        stack_element->NLayers       = 1 ;
    }

  | CHANNEL IDENTIFIER ';'  // $2 Identifier for the stack element
    {
        num_channels++ ;

        if (stkd->Channel == NULL)
        {
            STKERROR ("Error: channel used in stack but not declared") ;

            YYABORT ;
        }

        StackElement_t *stack_element = $$ = stack_element_calloc () ;

        if (stack_element == NULL)
        {
            free ($2) ;

            STKERROR ("Malloc stack element failed") ;

            YYABORT ;
        }

        stack_element->Type            = TDICE_STACK_ELEMENT_CHANNEL ;
        stack_element->Pointer.Channel = stkd->Channel ; // This might be NULL !!!
        stack_element->Id              = $2 ;
        stack_element->NLayers         = stkd->Channel->NLayers ;
    }

  | DIE IDENTIFIER IDENTIFIER FLOORPLAN PATH ';'  // $2 Identifier for the stack element
                                                  // $3 Identifier of the die
                                                  // $5 Path of the floorplan file
    {
        num_dies++ ;

        StackElement_t *stack_element = $$ = stack_element_calloc () ;

        if (stack_element == NULL)
        {
            free ($2) ;
            free ($3) ;
            free ($5) ;

            STKERROR ("Malloc stack element failed") ;

            YYABORT ;
        }

        Die_t *die = die_calloc ( ) ;

        if (die == NULL)
        {
            free ($2) ;
            free ($3) ;
            free ($5) ;

            stack_element_free (stack_element) ;

            STKERROR ("Malloc die for stack element failed") ;

            YYABORT ;
        }

        die->Id = $3 ;

        Die_t *tmp = die_list_find (&stkd->Dies, die) ;

        if (tmp == NULL)
        {
            sprintf (error_message, "Unknown die %s", $3) ;

            die_free (die) ;

            free ($2) ;
            free ($5) ;

            stack_element_free (stack_element) ;

            STKERROR (error_message) ;

            YYABORT ;
        }

        die_copy (die, tmp) ;

        if (   fill_floorplan (&die->Floorplan, stkd->Dimensions, $5)
            == TDICE_FAILURE)
        {
            free ($5) ;

            stack_description_destroy (stkd) ;

            YYABORT ; // CHECKME error messages printed in this case ....
        }

        free ($5) ;

        stack_element->Type        = TDICE_STACK_ELEMENT_DIE ;
        stack_element->Id          = $2 ;
        stack_element->Pointer.Die = die ;
        stack_element->NLayers     = stack_element->Pointer.Die->NLayers ;
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

        analysis->AnalysisType = TDICE_ANALYSIS_TYPE_TRANSIENT ;
        analysis->StepTime     = (Time_t) $5 ;
        analysis->SlotTime     = (Time_t) $8 ;
        analysis->SlotLength   = (Quantity_t) ( $8 / $5 ) ;


        analysis->InitialTemperature = (Temperature_t) $12;
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

        stack_element.Id = $3 ;

        StackElement_t *tmp = stack_element_list_find

            (&stkd->StackElements, &stack_element) ;

        if (tmp == NULL)
        {
            sprintf (error_message, "Unknown stack element %s", $3) ;

            STKERROR (error_message) ;

            stack_element_destroy (&stack_element) ;

            free ($9) ;

            YYABORT ;
        }

        stack_element_destroy (&stack_element) ;

        InspectionPoint_t *ipoint = $$ = inspection_point_calloc () ;

        if (ipoint == NULL)
        {
            free ($9) ;

            STKERROR ("Malloc inspection point failed") ;

            YYABORT ;
        }

        ipoint->Type          = TDICE_OUTPUT_TYPE_TCELL ;
        ipoint->Instant       = $10 ;
        ipoint->FileName      = $9 ;
        ipoint->StackElement  = tmp ;

        align_tcell (ipoint, $5, $7, stkd->Dimensions) ;
     }

  |  TFLP  '(' IDENTIFIER ',' PATH ',' maxminavg when ')'  ';'

     // $3 Identifier of the stack element (must be a die)
     // $5 Path of the output file
     // $7 temperature type
     // $8 when to generate output for this observation

     {
        StackElement_t stack_element ;

        stack_element_init (&stack_element) ;

        stack_element.Id = $3 ;

        StackElement_t *tmp = stack_element_list_find

            (&stkd->StackElements, &stack_element) ;

        if (tmp == NULL)
        {
            sprintf (error_message, "Unknown stack element %s", $3) ;

            STKERROR (error_message) ;

            stack_element_destroy (&stack_element) ;

            free ($5) ;

            YYABORT ;
        }

        if (tmp->Type != TDICE_STACK_ELEMENT_DIE)
        {
            sprintf (error_message, "The stack element %s must be a die", $3) ;

            STKERROR (error_message) ;

            stack_element_destroy (&stack_element) ;

            free ($5) ;

            YYABORT ;
        }

        stack_element_destroy (&stack_element) ;

        InspectionPoint_t *ipoint = $$ = inspection_point_calloc () ;

        if (ipoint == NULL)
        {
            free ($5) ;

            STKERROR ("Malloc inspection point failed") ;

            YYABORT ;
        }

        ipoint->Type         = TDICE_OUTPUT_TYPE_TFLP ;
        ipoint->FileName     = $5 ;
        ipoint->Quantity     = $7 ;
        ipoint->Instant      = $8 ;
        ipoint->StackElement = tmp ;
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

        stack_element.Id = $3 ;

        StackElement_t *tmp = stack_element_list_find

            (&stkd->StackElements, &stack_element) ;

        if (tmp == NULL)
        {
            sprintf (error_message, "Unknown stack element %s", $3) ;

            STKERROR (error_message) ;

            stack_element_destroy (&stack_element) ;

            free ($5) ;
            free ($7) ;

            YYABORT ;
        }

        if (tmp->Type != TDICE_STACK_ELEMENT_DIE)
        {
            sprintf (error_message, "The stack element %s must be a die", $3) ;

            STKERROR (error_message) ;

            stack_element_destroy (&stack_element) ;

            free ($5) ;
            free ($7) ;

            YYABORT ;
        }

        stack_element_destroy (&stack_element) ;

        FloorplanElement_t *flpel = get_floorplan_element (&tmp->Pointer.Die->Floorplan, $5) ;

        if (flpel == NULL)
        {
            sprintf (error_message, "Unknown floorplan element %s", $5) ;

            STKERROR (error_message) ;

            free ($5) ;
            free ($7) ;

            YYABORT ;
        }

        InspectionPoint_t *ipoint = $$ = inspection_point_calloc () ;

        if (ipoint == NULL)
        {
            free ($5) ;
            free ($7) ;

            STKERROR ("Malloc inspection point failed") ;

            YYABORT ;
        }

        ipoint->Type             = TDICE_OUTPUT_TYPE_TFLPEL ;
        ipoint->FloorplanElement = flpel ;
        ipoint->FileName         = $7 ;
        ipoint->Quantity         = $9 ;
        ipoint->Instant          = $10 ;
        ipoint->StackElement     = tmp ;

        free ($5) ;
     }

  |  TMAP '(' IDENTIFIER ',' PATH when ')' ';'

     // $3 Identifier of the stack element (layer, channel or die)
     // $5 Path of the output file
     // $6 when to generate output for this observation

     {
        StackElement_t stack_element ;

        stack_element_init (&stack_element) ;

        stack_element.Id = $3 ;

        StackElement_t *tmp = stack_element_list_find

            (&stkd->StackElements, &stack_element) ;

        if (tmp == NULL)
        {
            sprintf (error_message, "Unknown stack element %s", $3) ;

            STKERROR (error_message) ;

            stack_element_destroy (&stack_element) ;

            free ($5) ;

            YYABORT ;
        }

        stack_element_destroy (&stack_element) ;

        InspectionPoint_t *ipoint = $$ = inspection_point_calloc () ;

        if (ipoint == NULL)
        {
            free ($5) ;

            STKERROR ("Malloc inspection point failed") ;

            YYABORT ;
        }

        ipoint->Type         = TDICE_OUTPUT_TYPE_TMAP ;
        ipoint->Instant      = $6 ;
        ipoint->FileName     = $5 ;
        ipoint->StackElement = tmp ;
     }

  |  PMAP '(' IDENTIFIER ',' PATH when ')' ';'

     // $3 Identifier of the stack element (must be a die)
     // $5 Path of the output file
     // $6 when to generate output for this observation

    {
        StackElement_t stack_element ;

        stack_element_init (&stack_element) ;

        stack_element.Id = $3 ;

        StackElement_t *tmp = stack_element_list_find

            (&stkd->StackElements, &stack_element) ;

        if (tmp == NULL)
        {
            sprintf (error_message, "Unknown stack element %s", $3) ;

            STKERROR (error_message) ;

            stack_element_destroy (&stack_element) ;

            free ($5) ;

            YYABORT ;
        }

        stack_element_destroy (&stack_element) ;

        InspectionPoint_t *ipoint = $$ = inspection_point_calloc () ;

        if (ipoint == NULL)
        {
            free ($5) ;

            STKERROR ("Malloc inspection point failed") ;

            YYABORT ;
        }

        ipoint->Type         = TDICE_OUTPUT_TYPE_PMAP ;
        ipoint->Instant      = $6 ;
        ipoint->FileName     = $5 ;
        ipoint->StackElement = tmp ;
    }

  |  TCOOLANT '(' IDENTIFIER ',' PATH ',' maxminavg when ')' ';'

     // $3 Identifier of the stack element (must be a channel)
     // $5 Path of the output file
     // $7 temperature type
     // $8 when to generate output for this observation

     {
        StackElement_t stack_element ;

        stack_element_init (&stack_element) ;

        stack_element.Id = $3 ;

        StackElement_t *tmp = stack_element_list_find

            (&stkd->StackElements, &stack_element) ;

        if (tmp == NULL)
        {
            sprintf (error_message, "Unknown stack element %s", $3) ;

            STKERROR (error_message) ;

            stack_element_destroy (&stack_element) ;

            free ($5) ;

            YYABORT ;
        }

        if (tmp->Type != TDICE_STACK_ELEMENT_CHANNEL)
        {
            sprintf (error_message, "The stack element %s must be a channel", $3) ;

            STKERROR (error_message) ;

            stack_element_destroy (&stack_element) ;

            free ($5) ;

            YYABORT ;
        }

        stack_element_destroy (&stack_element) ;

        InspectionPoint_t *ipoint = $$ = inspection_point_calloc () ;

        if (ipoint == NULL)
        {
            free ($5) ;

            STKERROR ("Malloc inspection point failed") ;

            YYABORT ;
        }

        ipoint->Type         = TDICE_OUTPUT_TYPE_TCOOLANT ;
        ipoint->FileName     = $5 ;
        ipoint->Quantity     = $7 ;
        ipoint->Instant      = $8 ;
        ipoint->StackElement = tmp ;
     }
  ;

maxminavg

  :  MAXIMUM { $$ =  TDICE_OUTPUT_QUANTITY_MAXIMUM ; }
  |  MINIMUM { $$ =  TDICE_OUTPUT_QUANTITY_MINIMUM ; }
  |  AVERAGE { $$ =  TDICE_OUTPUT_QUANTITY_AVERAGE ; }
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
