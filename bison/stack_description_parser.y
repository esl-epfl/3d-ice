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
    #include "layer.h"
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
}

%type <double_v>           first_wall_length
%type <double_v>           last_wall_length
%type <channel_model_v>    distribution
%type <coolant_v>          coolant_heat_transfer_coefficients_4rm
%type <coolant_v>          coolant_heat_transfer_coefficients_2rm
%type <material_p>         material
%type <material_p>         materials_list
%type <die_p>              die
%type <die_p>              dies_list
%type <layer_p>            layers_list
%type <layer_p>            layer
%type <layer_p>            die_layers_list
%type <layer_p>            die_layer
%type <layer_p>            die_source_layer
%type <layer_p>            die_layer_content
%type <stack_element_p>    stack_element
%type <stack_element_p>    stack_elements
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

%destructor { FREE_POINTER (free,             $$) ; } <string>
%destructor { FREE_POINTER (free_layers_list, $$) ; } <layer_p>

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
        stkd->MaterialsList = $1 ;

        $$ = $1 ;           // $1 will be the new last element in the list
    }
  | materials_list material // $1 : pointer to the last material in the list
                            // $2 : pointer to the material to add in the list
    {

        if (find_material_in_list (stkd->MaterialsList, $2->Id) != NULL)
        {
            sprintf (error_message, "Material %s already declared", $2->Id) ;

            FREE_POINTER (free_material, $2) ;

            STKERROR (error_message) ;

            YYABORT ;
        }

        $1->Next = $2 ;     // insert $2 at the end of the list
        $$ = $2 ;           // $2 will be the new last element in the list
    }
  ;

material

  : MATERIAL IDENTIFIER ':'                     // $2
        THERMAL CONDUCTIVITY     DVALUE ';'     // $6
        VOLUMETRIC HEAT CAPACITY DVALUE ';'     // $11
    {
        Material_t *material = $$ = calloc_material () ;

        if (material == NULL)
        {
            FREE_POINTER (free, $2) ;

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
        stkd->HeatSink = alloc_and_init_heat_sink () ;

        if (stkd->HeatSink == NULL)
        {
            STKERROR ("Malloc heat sink failed") ;

            YYABORT ;
        }

        stkd->HeatSink->SinkModel          = TDICE_HEATSINK_MODEL_TRADITIONAL ;
        stkd->HeatSink->NLayers            = NUM_LAYERS_HEATSINK_TRADITIONAL ;
        stkd->HeatSink->SourceLayerOffset  = SOURCE_OFFSET_HEATSINK_TRADITIONAL ;
        stkd->HeatSink->SinkHeight         = $6 ;
        stkd->HeatSink->SinkArea           = $9 ;
        stkd->HeatSink->SpreaderHeight     = $16 ;
        stkd->HeatSink->SpreaderArea       = $19 ;
        stkd->HeatSink->AmbientHTC         = (AmbientHTC_t) $27 ;
        stkd->HeatSink->AmbientTemperature = $31 ;

        if (stkd->HeatSink->SinkArea < stkd->HeatSink->SpreaderArea)
        {
            sprintf (error_message,
                "Error: the area of the spreader must be smaller than the sink") ;

            FREE_POINTER (free, $12) ;
            FREE_POINTER (free, $22) ;

            STKERROR (error_message) ;

            YYABORT ;
        }

        stkd->HeatSink->SinkMaterial =

            find_material_in_list (stkd->MaterialsList, $12) ;

        if (stkd->HeatSink->SinkMaterial == NULL)
        {
            sprintf (error_message, "Unknown sink material %s", $12) ;

            FREE_POINTER (free, $12) ;
            FREE_POINTER (free, $22) ;

            STKERROR (error_message) ;

            YYABORT ;
        }

        stkd->HeatSink->SpreaderMaterial =

            find_material_in_list (stkd->MaterialsList, $22) ;

        if (stkd->HeatSink->SpreaderMaterial == NULL)
        {
            sprintf (error_message, "Unknown spreader material %s", $22) ;

            FREE_POINTER (free, $12) ;
            FREE_POINTER (free, $22) ;

            STKERROR (error_message) ;

            YYABORT ;
        }

        stkd->HeatSink->SinkMaterial->Used++ ;
        stkd->HeatSink->SpreaderMaterial->Used++ ;

        FREE_POINTER (free, $12) ;
        FREE_POINTER (free, $22) ;
    }

  | CONNECTION TO AMBIENT ':'
        HEAT TRANSFER COEFFICIENT DVALUE ';'  // $8
        AMBIENT TEMPERATURE       DVALUE ';'  // $12
    {
        stkd->HeatSink = alloc_and_init_heat_sink () ;

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
        stkd->Channel = calloc_channel () ;

        if (stkd->Channel == NULL)
        {
            FREE_POINTER (free, $19) ;

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
        stkd->Channel->Coolant.FlowRate  = FLOW_RATE_FROM_MLMIN_TO_UM3SEC ($24) ;
        stkd->Channel->Coolant.HTCSide   = $26.HTCSide ;
        stkd->Channel->Coolant.HTCTop    = $26.HTCTop ;
        stkd->Channel->Coolant.HTCBottom = $26.HTCBottom ;
        stkd->Channel->Coolant.VHC       = $31 ;
        stkd->Channel->Coolant.TIn       = $36 ;
        stkd->Channel->WallMaterial      = find_material_in_list (stkd->MaterialsList, $19) ;

        if (stkd->Channel->WallMaterial == NULL)
        {
            sprintf (error_message, "Unknown material %s", $19) ;

            FREE_POINTER (free, $19) ;

            STKERROR (error_message) ;

            YYABORT ;
        }

        stkd->Channel->WallMaterial->Used++ ;

        FREE_POINTER (free, $19) ;
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
        stkd->Channel = calloc_channel () ;

        if (stkd->Channel == NULL)
        {
            FREE_POINTER (free, $17) ;

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
        stkd->Channel->Coolant.FlowRate  = FLOW_RATE_FROM_MLMIN_TO_UM3SEC ($22) ;
        stkd->Channel->Coolant.HTCSide   = $24.HTCSide ;
        stkd->Channel->Coolant.HTCTop    = $24.HTCTop ;
        stkd->Channel->Coolant.HTCBottom = $24.HTCBottom ;
        stkd->Channel->Coolant.VHC       = $29 ;
        stkd->Channel->Coolant.TIn       = $34 ;
        stkd->Channel->WallMaterial      = find_material_in_list (stkd->MaterialsList, $17) ;

        if (stkd->Channel->WallMaterial == NULL)
        {
            sprintf (error_message, "Unknown material %s", $17) ;

            FREE_POINTER (free, $17) ;

            STKERROR (error_message) ;

            YYABORT ;
        }

        stkd->Channel->WallMaterial->Used++ ;

        FREE_POINTER (free, $17) ;
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
        stkd->Channel = calloc_channel () ;

        if (stkd->Channel == NULL)
        {
            FREE_POINTER (free, $20) ;

            STKERROR ("Malloc channel failed") ;

            YYABORT ;
        }

        stkd->Channel->Height                = $4 ;
        stkd->Channel->Porosity              = POROSITY ($8, $12) ;
        stkd->Channel->Pitch                 = $12 ;
        stkd->Channel->ChannelModel          = $16 ;
        stkd->Channel->NLayers               = NUM_LAYERS_CHANNEL_2RM ;
        stkd->Channel->SourceLayerOffset     = SOURCE_OFFSET_CHANNEL_2RM ;
        stkd->Channel->Coolant.DarcyVelocity = $24 ;
        stkd->Channel->Coolant.HTCSide       = 0.0 ;
        stkd->Channel->Coolant.HTCTop        = 0.0 ;
        stkd->Channel->Coolant.HTCBottom     = 0.0 ;
        stkd->Channel->Coolant.VHC           = $30 ;
        stkd->Channel->Coolant.TIn           = $35 ;
        stkd->Channel->WallMaterial          = find_material_in_list (stkd->MaterialsList, $20) ;

        if (stkd->Channel->WallMaterial == NULL)
        {
            sprintf (error_message, "Unknown material %s", $20) ;

            FREE_POINTER (free, $20) ;

            STKERROR (error_message) ;

            YYABORT ;
        }

        stkd->Channel->WallMaterial->Used++ ;

        FREE_POINTER (free, $20) ;
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

  :                    // Declaring a list of layers is not mandatory
    {
        stkd->LayersList = NULL ;
    }
  | layers_list

  ;

layers_list

  : layer
    {
        stkd->LayersList = $1 ;

        $$ = $1 ;      // $1 will be the new layer in the list
    }
  | layers_list layer  // $1 : pointer to the last layer in the list
                       // $2 : pointer to the layer to add in the list
    {

        // checks if layer id already exist

        if (find_layer_in_list (stkd->LayersList, $2->Id) != NULL)
        {
            sprintf (error_message, "Layer %s already declared", $2->Id) ;

            FREE_POINTER (free_layer, $2) ;

            STKERROR (error_message) ;

            YYABORT ;
        }

        JOIN_ELEMENTS($1, $2) ;

        $$ = $2 ;               // $2 will be the new last element in the list
    }
  ;

layer

  : LAYER IDENTIFIER ':'       // $2
       HEIGHT DVALUE ';'       // $5
       MATERIAL IDENTIFIER ';' // $8
  {
        Layer_t *layer = $$ = calloc_layer () ;

        if (layer == NULL)
        {
            FREE_POINTER (free, $2) ;
            FREE_POINTER (free, $8) ;

            STKERROR ("Malloc layer failed") ;

            YYABORT ;
        }

        layer->Height   = $5 ;
        layer->Id       = $2 ;
        layer->Material = find_material_in_list (stkd->MaterialsList, $8) ;

        if (layer->Material == NULL)
        {
            sprintf (error_message, "Unknown material %s", $8) ;

            FREE_POINTER (free,       $8) ;
            FREE_POINTER (free_layer, layer) ;

            STKERROR (error_message) ;

            YYABORT ;
        }

        layer->Material->Used++ ;

        FREE_POINTER (free, $8) ;
  }

/******************************************************************************/
/******************************* Dies *****************************************/
/******************************************************************************/

die_layers_list

  :                    // Declaring a "non active" layer in a die is not mandatory
    {
        $$ = NULL ;    // The first layer in the list will be null
    }
  | die_layers_list die_layer  // $1 : pointer to the last layer in the list
                               // $2 : pointer to the layer to add in the list
    {
        if ($1 != NULL)
            JOIN_ELEMENTS ($2, $1) ; // this reverse the order !
        $$ = $2 ;                    // $2 will be the new reference to the list
    }
  ;

die_layer         : LAYER  die_layer_content { $$ = $2 ; } ;

die_source_layer  : SOURCE die_layer_content { $$ = $2 ; } ;

die_layer_content :

    DVALUE IDENTIFIER ';' // $1 and $2

    {
        Layer_t *layer = $$ = calloc_layer () ;

        if (layer == NULL)
        {
            FREE_POINTER (free, $2) ;

            STKERROR ("Malloc layer failed") ;

            YYABORT ;
        }

        layer->Height   = $1 ;
        layer->Id       = NULL ;
        layer->Material = find_material_in_list (stkd->MaterialsList, $2) ;

        if (layer->Material == NULL)
        {
            sprintf (error_message, "Unknown material %s", $2) ;

            FREE_POINTER (free,       $2) ;
            FREE_POINTER (free_layer, layer) ;

            STKERROR (error_message) ;

            YYABORT ;
        }

        layer->Material->Used++ ;

        FREE_POINTER (free, $2) ;
    }
  ;

dies_list

  : die                   // $1 : pointer to the first die found
    {
        stkd->DiesList = $1 ;

        $$ = $1 ;         // $1 will be the new die in the list
    }
  | dies_list die         // $1 : pointer to the last die in the list
                          // $2 : pointer to the die to add in the list
    {
        if (find_die_in_list (stkd->DiesList, $2->Id) != NULL)
        {
            sprintf (error_message, "Die %s already declared", $2->Id) ;

            FREE_POINTER (free_die, $2) ;

            STKERROR (error_message) ;

            YYABORT ;
        }

        $1->Next = $2 ;   // insert $2 at the end of the list
        $$ = $2 ;         // $2 will be the new last element in the list
    }
  ;

die

  : DIE IDENTIFIER ':'         // $2 Die identifier
        die_layers_list        // $4 list of layers (above source -> top)
        die_source_layer       // $5 source layer
        die_layers_list        // $6 list of layers (bottom -> below source)
    {
        Die_t *die = $$ = calloc_die () ;

        if (die == NULL)
        {
            FREE_POINTER (free,             $2) ;
            FREE_POINTER (free_layers_list, $4) ;
            FREE_POINTER (free_layer,       $5) ;
            FREE_POINTER (free_layers_list, $6);

            STKERROR ("Malloc die failed") ;

            YYABORT ;
        }

        die->Id = $2 ;
        die->SourceLayer = $5 ;

        // The layers within a die are declared in the stack file from top
        // to bottom but here we revert the order: the first layer in the list
        // LayersList will be the bottom-most layer in the die

        if ($6 != NULL)
        {
            // if there are layers below the source,
            // then the list of layers begins with $6

            die->BottomLayer = $6 ;

            die->NLayers++ ;
            die->SourceLayerOffset++ ;

            // $6 moved until the end ..

            while ($6->Next != NULL)
            {
                $6 = $6->Next ;

                die->NLayers++ ;
                die->SourceLayerOffset++ ;
            }

            // the list $6 continues with the source layer $5

            JOIN_ELEMENTS ($6, $5) ;
        }
        else
        {
            // if there aren't layers below the source, the list of layers
            // begins directly with the source layer $5

            die->BottomLayer = $5 ;
        }

        die->NLayers++ ;

        if ($4 != NULL)
        {
            // if there are layers above the source
            // $5 is connected to the list $4

            JOIN_ELEMENTS ($5, $4) ;

            die->NLayers++ ;

            // $4 moved until the end ..

            while ($4->Next != NULL)
            {
                $4 = $4->Next ;

                die->NLayers++ ;
            }

            // the list finishes with the last layer in $4

            die->TopLayer = $4 ;
        }
        else
        {
            // if there aren't layers below the source,
            // The list finishes with the source layer $5

            die->TopLayer = $5 ;
        }
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
        stkd->Dimensions = calloc_dimensions () ;

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

        if (stkd->BottomStackElement->Type == TDICE_STACK_ELEMENT_CHANNEL)
        {
            STKERROR ("Error: cannot declare a channel as bottom-most stack element") ;

            YYABORT ;
        }

        if (stkd->HeatSink == NULL && stkd->Channel == NULL)

            fprintf (stderr, "Warning: neither heat sink nor channel has been declared\n") ;


        FOR_EVERY_ELEMENT_IN_LIST_NEXT (Material_t, material, stkd->MaterialsList)

            if (material->Used == 0)

                fprintf (stderr, "Warning: material %s declared but not used\n", material->Id) ;

        FOR_EVERY_ELEMENT_IN_LIST_NEXT (Layer_t, layer, stkd->LayersList)

            if (layer->Used == 0)

                fprintf (stderr, "Warning: layer %s declared but not used\n", layer->Id) ;

        FOR_EVERY_ELEMENT_IN_LIST_NEXT (Die_t, die, stkd->DiesList)

            if (die->Used == 0)

                fprintf (stderr, "Warning: die %s declared but not used\n", die->Id) ;

        if (stkd->HeatSink != NULL)
        {
            if (stkd->HeatSink->SinkModel == TDICE_HEATSINK_MODEL_CONNECTION_TO_AMBIENT)
            {
                if (stkd->TopStackElement->Type == TDICE_STACK_ELEMENT_LAYER)
                {
                    stkd->HeatSink->SinkMaterial =

                        stkd->TopStackElement->Pointer.Layer->Material ;
                }
                else
                {
                    stkd->HeatSink->SinkMaterial =

                        stkd->TopStackElement->Pointer.Die->TopLayer->Material ;
                }
            }

            // Creates an extra stack element to be add in the 3d stack

            StackElement_t *stack_element = alloc_and_init_stack_element () ;

            if (stack_element == NULL)
            {
                STKERROR ("Malloc stack element failed") ;

                YYABORT ;
            }

            stack_element->Id               = strdup ("Ambient") ;
            stack_element->Type             = TDICE_STACK_ELEMENT_HEATSINK ;
            stack_element->Pointer.HeatSink = stkd->HeatSink ;
            stack_element->NLayers          = stkd->HeatSink->NLayers ;

            // Add the stack element to the queue

            JOIN_ELEMENTS (stkd->TopStackElement, stack_element) ;

            stkd->TopStackElement = stack_element ;
        }

        // Counts the number of layers and fix the layer offset starting from
        // the bottom most element in the stack. This operation can be done only
        // here since the parser processes elements in the stack from the top most.

        CellIndex_t layer_index = 0u ;

        FOR_EVERY_ELEMENT_IN_LIST_NEXT (StackElement_t, stk_el, stkd->BottomStackElement)
        {
            stk_el->Offset = layer_index ;
            layer_index   += stk_el->NLayers ;
        }

        if (   stkd->TopStackElement->Type == TDICE_STACK_ELEMENT_HEATSINK
            && stkd->HeatSink->SinkModel == TDICE_HEATSINK_MODEL_CONNECTION_TO_AMBIENT)

            stkd->TopStackElement->Offset-- ;

        stkd->Dimensions->Grid.NLayers = layer_index ;

        // Stores the vertical profile of cell heights

        stkd->Dimensions->Cell.NHeights = layer_index ;
        stkd->Dimensions->Cell.Heights  = malloc (layer_index * sizeof (CellDimension_t)) ;

        if (stkd->Dimensions->Cell.Heights == NULL)
        {
            STKERROR ("Malloc heights failed") ;

            YYABORT ;
        }

        layer_index  = 0u ;

        FOR_EVERY_ELEMENT_IN_LIST_NEXT (StackElement_t, stk_el_, stkd->BottomStackElement)
        {
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
                FOR_EVERY_ELEMENT_IN_LIST_NEXT (Layer_t, layer, stk_el_->Pointer.Die->BottomLayer)
                {
                    stkd->Dimensions->Cell.Heights [ layer_index++ ] = layer->Height ;
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
                "%llu are too many cells ... (SuperLU uses 'int')",
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

  : stack_element                 // $1 : pointer to the first stack element found
    {
        if (   stkd->TopStackElement == NULL
            && $1->Type == TDICE_STACK_ELEMENT_CHANNEL)
        {
            STKERROR ("Error: cannot declare a channel as top-most stack element") ;

            YYABORT ;
        }

        stkd->TopStackElement    = $1 ;
        stkd->BottomStackElement = $1 ;
        $$ = $1 ;
    }
  | stack_elements stack_element  // $1 : pointer to the last stack element in the list
                                  // $2 : pointer to the stack element to add in the list
    {
        if (find_stack_element_in_list ($1, $2->Id) != NULL)
        {
            sprintf (error_message, "Stack element %s already declared", $2->Id) ;

            FREE_POINTER (free_stack_element, $2) ;

            STKERROR (error_message) ;

            YYABORT ;
        }

        if (   $1->Type == TDICE_STACK_ELEMENT_CHANNEL
            && $2->Type == TDICE_STACK_ELEMENT_CHANNEL)
        {
            STKERROR ("Error: cannot declare two consecutive channels") ;

            YYABORT ;
        }

        JOIN_ELEMENTS ($2, $1) ;

        stkd->BottomStackElement = $2 ;
        $$ = $2 ;                 // $2 will be the last stack element in the list
    }
  ;

stack_element

  : LAYER IDENTIFIER IDENTIFIER ';'    // $2 Identifier for the stack element
                                       // $3 Identifier of the layer
    {
        StackElement_t *stack_element = $$ = alloc_and_init_stack_element () ;

        if (stack_element == NULL)
        {
            FREE_POINTER (free, $2) ;
            FREE_POINTER (free, $3) ;

            STKERROR ("Malloc stack element failed") ;

            YYABORT ;
        }

        Layer_t *layer = find_layer_in_list (stkd->LayersList, $3) ;

        if (layer == NULL)
        {
            sprintf (error_message, "Unknown layer %s", $3) ;

            FREE_POINTER (free,               $2) ;
            FREE_POINTER (free,               $3) ;
            FREE_POINTER (free_stack_element, stack_element) ;

            STKERROR (error_message) ;

            YYABORT ;
        }

        layer->Used++ ;

        stack_element->Type          = TDICE_STACK_ELEMENT_LAYER ;
        stack_element->Pointer.Layer = layer ;
        stack_element->Id            = $2 ;
        stack_element->NLayers       = 1 ;

        FREE_POINTER (free, $3) ;
    }

  | CHANNEL IDENTIFIER ';'  // $2 Identifier for the stack element
    {
        num_channels++ ;

        if (stkd->Channel == NULL)
        {
            STKERROR ("Error: channel used in stack but not declared") ;

            YYABORT ;
        }

        StackElement_t *stack_element = $$ = alloc_and_init_stack_element () ;

        if (stack_element == NULL)
        {
            FREE_POINTER (free, $2) ;

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

        StackElement_t *stack_element = $$ = alloc_and_init_stack_element () ;

        if (stack_element == NULL)
        {
            FREE_POINTER (free, $2) ;
            FREE_POINTER (free, $3) ;
            FREE_POINTER (free, $5) ;

            STKERROR ("Malloc stack element failed") ;

            YYABORT ;
        }

        stack_element->Type        = TDICE_STACK_ELEMENT_DIE ;
        stack_element->Id          = $2 ;
        stack_element->Pointer.Die = find_die_in_list (stkd->DiesList, $3) ;

        if (stack_element->Pointer.Die == NULL)
        {
            sprintf (error_message, "Unknown die %s", $3) ;

            FREE_POINTER (free,               $3) ;
            FREE_POINTER (free,               $5) ;
            FREE_POINTER (free_stack_element, stack_element) ;

            STKERROR (error_message) ;

            YYABORT ;
        }

        stack_element->Pointer.Die->Used++ ;

        stack_element->NLayers = stack_element->Pointer.Die->NLayers ;
        stack_element->Floorplan = alloc_and_init_floorplan () ;

        if (stack_element->Floorplan == NULL)
        {
            FREE_POINTER (free,               $3) ;
            FREE_POINTER (free,               $5) ;
            FREE_POINTER (free_stack_element, stack_element) ;

            STKERROR ("Malloc floorplan failed") ;

            YYABORT ;
        }

        if (   fill_floorplan (stack_element->Floorplan, stkd->Dimensions, $5)
            == TDICE_FAILURE)
        {
            FREE_POINTER (free,                   $3) ;
            FREE_POINTER (free,                   $5) ;
            FREE_POINTER (free_stack_description, stkd) ;

            YYABORT ; // CHECKME error messages printed in this case ....
        }

        FREE_POINTER (free, $5) ;  // FIXME check memory leak
        FREE_POINTER (free, $3) ;
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

  :  inspection_point // $1 : pointer to the first inspection point found
     {
        add_inspection_point (output, $1) ;
     }

  |  inspection_points_list inspection_point
                      // $1 : not used
                      // $2 : pointer to the inspection point to add in the list
     {
        add_inspection_point (output, $2) ;
     }
  ;

inspection_point

  :  TCELL '(' IDENTIFIER ',' DVALUE ',' DVALUE ',' PATH when ')' ';'

     // $3       Identifier of the stack element (layer, channel or die)
     // ($5, $7) Coordinates of the cell to monitor
     // $9       Path of the output file
     // $10      when to generate output for this observation

     {
        StackElement_t *stack_element =

            find_stack_element_in_list (stkd->BottomStackElement, $3) ;

        if (stack_element == NULL)
        {
            sprintf (error_message, "Unknown stack element %s", $3) ;

            STKERROR (error_message) ;

            FREE_POINTER (free, $3) ;
            FREE_POINTER (free, $9) ;

            YYABORT ;
        }

        Tcell_t *tcell = alloc_and_init_tcell () ;

        if (tcell == NULL)
        {
            FREE_POINTER (free, $3) ;
            FREE_POINTER (free, $9) ;

            STKERROR ("Malloc tcell failed") ;

            YYABORT ;
        }

        align_tcell (tcell, $5, $7, stkd->Dimensions) ;

        InspectionPoint_t *inspection_point = $$ = alloc_and_init_inspection_point () ;

        if (inspection_point == NULL)
        {
            FREE_POINTER (free, $3) ;
            FREE_POINTER (free, $9) ;

            FREE_POINTER (free_tcell, tcell) ;

            STKERROR ("Malloc inspection point failed") ;

            YYABORT ;
        }

        inspection_point->Type          = TDICE_OUTPUT_TYPE_TCELL ;
        inspection_point->Instant       = $10 ;
        inspection_point->FileName      = $9 ;
        inspection_point->Pointer.Tcell = tcell ;
        inspection_point->StackElement  = stack_element ;

        FREE_POINTER (free, $3) ;
     }

  |  TFLP  '(' IDENTIFIER ',' PATH ',' maxminavg when ')'  ';'

     // $3 Identifier of the stack element (must be a die)
     // $5 Path of the output file
     // $7 temperature type
     // $8 when to generate output for this observation

     {
        StackElement_t *stack_element =

            find_stack_element_in_list (stkd->BottomStackElement, $3) ;

        if (stack_element == NULL)
        {
            sprintf (error_message, "Unknown stack element %s", $3) ;

            STKERROR (error_message) ;

            FREE_POINTER (free, $3) ;
            FREE_POINTER (free, $5) ;

            YYABORT ;
        }

        if (stack_element->Type != TDICE_STACK_ELEMENT_DIE)
        {
            sprintf (error_message, "The stack element %s must be a die", $3) ;

            STKERROR (error_message) ;

            FREE_POINTER (free, $3) ;
            FREE_POINTER (free, $5) ;

            YYABORT ;
        }

        Tflp_t *tflp = alloc_and_init_tflp () ;

        if (tflp == NULL)
        {
            FREE_POINTER (free, $3) ;
            FREE_POINTER (free, $5) ;

            STKERROR ("Malloc tflp failed") ;

            YYABORT ;
        }

        tflp->Quantity = $7 ;

        InspectionPoint_t *inspection_point = $$ = alloc_and_init_inspection_point () ;

        if (inspection_point == NULL)
        {
            FREE_POINTER (free, $3) ;
            FREE_POINTER (free, $5) ;

            FREE_POINTER (free_tflp, tflp) ;

            STKERROR ("Malloc inspection point failed") ;

            YYABORT ;
        }

        inspection_point->Type         = TDICE_OUTPUT_TYPE_TFLP ;
        inspection_point->Instant      = $8 ;
        inspection_point->FileName     = $5 ;
        inspection_point->Pointer.Tflp = tflp ;
        inspection_point->StackElement = stack_element ;

        FREE_POINTER (free, $3) ;
     }

  |  TFLPEL '(' IDENTIFIER '.' IDENTIFIER ',' PATH ',' maxminavg when ')' ';'

     // $3  Identifier of the stack element (must be a die)
     // $5  Identifier of the floorplan element
     // $7  Path of the output file
     // $9  temperature type
     // $10 when to generate output for this observation

     {
        StackElement_t *stack_element =

            find_stack_element_in_list (stkd->BottomStackElement, $3) ;

        if (stack_element == NULL)
        {
            sprintf (error_message, "Unknown stack element %s", $3) ;

            STKERROR (error_message) ;

            FREE_POINTER (free, $3) ;
            FREE_POINTER (free, $5) ;
            FREE_POINTER (free, $7) ;

            YYABORT ;
        }

        if (stack_element->Type != TDICE_STACK_ELEMENT_DIE)
        {
            sprintf (error_message, "The stack element %s must be a die", $3) ;

            STKERROR (error_message) ;

            FREE_POINTER (free, $3) ;
            FREE_POINTER (free, $5) ;
            FREE_POINTER (free, $7) ;

            YYABORT ;
        }

        FloorplanElement_t *floorplan_element = find_floorplan_element_in_list

            (stack_element->Floorplan->ElementsList, $5) ;

        if (floorplan_element == NULL)
        {
            sprintf (error_message, "Unknown floorplan element %s", $5) ;

            STKERROR (error_message) ;

            FREE_POINTER (free, $3) ;
            FREE_POINTER (free, $5) ;
            FREE_POINTER (free, $7) ;

            YYABORT ;
        }

        Tflpel_t *tflpel = alloc_and_init_tflpel () ;

        if (tflpel == NULL)
        {
            FREE_POINTER (free, $3) ;
            FREE_POINTER (free, $5) ;
            FREE_POINTER (free, $7) ;

            STKERROR ("Malloc Tflpel failed") ;

            YYABORT ;
        }

        tflpel->FloorplanElement = floorplan_element ;
        tflpel->Quantity         = $9 ;

        InspectionPoint_t *inspection_point = $$ = alloc_and_init_inspection_point () ;

        if (inspection_point == NULL)
        {
            FREE_POINTER (free, $3) ;
            FREE_POINTER (free, $5) ;
            FREE_POINTER (free, $7) ;

            FREE_POINTER (free_tflpel, tflpel) ;

            STKERROR ("Malloc inspection point failed") ;

            YYABORT ;
        }

        inspection_point->Type           = TDICE_OUTPUT_TYPE_TFLPEL ;
        inspection_point->Instant        = $10 ;
        inspection_point->FileName       = $7 ;
        inspection_point->Pointer.Tflpel = tflpel ;
        inspection_point->StackElement   = stack_element ;

        FREE_POINTER (free, $3) ;
        FREE_POINTER (free, $5) ;
     }

  |  TMAP '(' IDENTIFIER ',' PATH when ')' ';'

     // $3 Identifier of the stack element (layer, channel or die)
     // $5 Path of the output file
     // $6 when to generate output for this observation

     {
        StackElement_t *stack_element =

            find_stack_element_in_list (stkd->BottomStackElement, $3) ;

        if (stack_element == NULL)
        {
            sprintf (error_message, "Unknown stack element %s", $3) ;

            STKERROR (error_message) ;

            FREE_POINTER (free, $3) ;
            FREE_POINTER (free, $5) ;

            YYABORT ;
        }

        InspectionPoint_t *inspection_point = $$ = alloc_and_init_inspection_point () ;

        if (inspection_point == NULL)
        {
            FREE_POINTER (free, $3) ;
            FREE_POINTER (free, $5) ;

            STKERROR ("Malloc inspection point failed") ;

            YYABORT ;
        }

        inspection_point->Type         = TDICE_OUTPUT_TYPE_TMAP ;
        inspection_point->Instant      = $6 ;
        inspection_point->FileName     = $5 ;
        inspection_point->StackElement = stack_element ;

        FREE_POINTER (free, $3) ;
     }

  |  PMAP '(' IDENTIFIER ',' PATH when ')' ';'

     // $3 Identifier of the stack element (must be a die)
     // $5 Path of the output file
     // $6 when to generate output for this observation

    {
        StackElement_t *stack_element =

            find_stack_element_in_list (stkd->BottomStackElement, $3) ;

        if (stack_element == NULL)
        {
            sprintf (error_message, "Unknown stack element %s", $3) ;

            STKERROR (error_message) ;

            FREE_POINTER (free, $3) ;
            FREE_POINTER (free, $5) ;

            YYABORT ;
        }

        if (stack_element->Type != TDICE_STACK_ELEMENT_DIE)
        {
            sprintf (error_message, "The stack element %s must be a die", $3) ;

            STKERROR (error_message) ;

            FREE_POINTER (free, $3) ;
            FREE_POINTER (free, $5) ;

            YYABORT ;
        }

        InspectionPoint_t *inspection_point = $$ = alloc_and_init_inspection_point () ;

        if (inspection_point == NULL)
        {
            FREE_POINTER (free, $3) ;
            FREE_POINTER (free, $5) ;

            STKERROR ("Malloc inspection point failed") ;

            YYABORT ;
        }

        inspection_point->Type         = TDICE_OUTPUT_TYPE_PMAP ;
        inspection_point->Instant      = $6 ;
        inspection_point->FileName     = $5 ;
        inspection_point->StackElement = stack_element ;

        FREE_POINTER (free, $3) ;
    }

  |  TCOOLANT '(' IDENTIFIER ',' PATH ',' maxminavg when ')' ';'

     // $3 Identifier of the stack element (must be a channel)
     // $5 Path of the output file
     // $7 temperature type
     // $8 when to generate output for this observation

     {
        StackElement_t *stack_element =

            find_stack_element_in_list (stkd->BottomStackElement, $3) ;

        if (stack_element == NULL)
        {
            sprintf (error_message, "Unknown stack element %s", $3) ;

            STKERROR (error_message) ;

            FREE_POINTER (free, $3) ;
            FREE_POINTER (free, $5) ;

            YYABORT ;
        }

        if (stack_element->Type != TDICE_STACK_ELEMENT_CHANNEL)
        {
            sprintf (error_message, "The stack element %s must be a channel", $3) ;

            STKERROR (error_message) ;

            FREE_POINTER (free, $3) ;
            FREE_POINTER (free, $5) ;

            YYABORT ;
        }

        InspectionPoint_t *inspection_point = $$ = alloc_and_init_inspection_point () ;

        if (inspection_point == NULL)
        {
            FREE_POINTER (free, $3) ;
            FREE_POINTER (free, $5) ;

            STKERROR ("Malloc inspection point failed") ;

            YYABORT ;
        }

        Tcoolant_t *tcoolant = alloc_and_init_tcoolant () ;

        if (tcoolant == NULL)
        {
            FREE_POINTER (free, $3) ;
            FREE_POINTER (free, $5) ;

            STKERROR ("Malloc tcoolant failed") ;

            YYABORT ;
        }

        tcoolant->Quantity = $7 ;

        inspection_point->Type             = TDICE_OUTPUT_TYPE_TCOOLANT ;
        inspection_point->Instant          = $8 ;
        inspection_point->FileName         = $5 ;
        inspection_point->Pointer.Tcoolant = tcoolant ;
        inspection_point->StackElement     = stack_element ;

        FREE_POINTER (free, $3) ;
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

    free_stack_description (stkd) ;
    init_stack_description (stkd) ;

    init_analysis (analysis) ;

    free_output (output) ;
    init_output (output) ;
}

/******************************************************************************/

Error_t parse_stack_description_file
(
    String_t            filename,
    StackDescription_t *stkd,
    Analysis_t         *analysis,
    Output_t           *output
)
{
    FILE*    input ;
    int      result ;
    yyscan_t scanner ;

    input = fopen (filename, "r") ;
    if (input == NULL)
    {
        fprintf (stderr, "Unable to open stack file %s\n", filename) ;

        return TDICE_FAILURE ;
    }

    stkd->FileName = strdup (filename) ;  // FIXME memory leak

    stack_description_lex_init (&scanner) ;
    stack_description_set_in (input, scanner) ;

    result = stack_description_parse (stkd, analysis, output, scanner) ;

    stack_description_lex_destroy (scanner) ;
    fclose (input) ;

//  From Bison manual:
//  The value returned by yyparse is 0 if parsing was successful (return is
//  due to end-of-input). The value is 1 if parsing failed (return is due to
//  a syntax error).

    if (result == 0)

        return TDICE_SUCCESS ;

    else

        return TDICE_FAILURE ;
}

/******************************************************************************/

Error_t generate_stack_description_file
(
    String_t            filename,
    StackDescription_t *stkd,
    Analysis_t         *analysis,
    Output_t           *output
)
{
    FILE *out = fopen (filename, "w") ;

    if (out == NULL)
    {
        fprintf (stderr, "Unable to open stack file %s\n", filename) ;

        return TDICE_FAILURE ;
    }

    print_formatted_stack_description (stkd, out, "") ;

    fprintf (out, "\n") ;

    print_formatted_analysis (analysis, out, "") ;

    fprintf (out, "\n") ;

    print_formatted_output (output, out, "") ;

    fclose (out) ;

    return TDICE_SUCCESS ;
}
