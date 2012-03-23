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
    Material             *material_p ;
    Coolant_t             coolant_v ;
    ChannelModel_t        channel_model_v ;
    Die                  *die_p ;
    Layer                *layer_p ;
    StackElement         *stack_element_p ;
    InspectionPoint      *inspection_point_p ;
    OutputInstant_t       output_instant_v ;
    OutputQuantity_t      output_quantity_v ;
}

%code
{
    #include "analysis.h"
    #include "channel.h"
    #include "conventional_heat_sink.h"
    #include "dimensions.h"
    #include "floorplan_element.h"
    #include "floorplan.h"
    #include "layer.h"
    #include "macros.h"
    #include "stack_description.h"

    #include "../flex/stack_description_scanner.h"

    void stack_description_error

        (StackDescription *stack, Analysis *analysis,
         yyscan_t scanner, String_t message) ;

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
%type <layer_p>            layer
%type <layer_p>            layers_list
%type <layer_p>            source_layer
%type <layer_p>            layer_content
%type <stack_element_p>    stack_element
%type <stack_element_p>    stack_elements
%type <inspection_point_p> inspection_point
%type <output_instant_v>   when
%type <output_quantity_v>  maxminavg

%token _2RM                  "keyword 2rm"
%token _4RM                  "keyword 4rm"
%token AMBIENT               "keyword ambient"
%token AVERAGE               "keyword average"
%token BOTTOM                "keyword bottom"
%token CAPACITY              "keyword capacity"
%token CELL                  "keyword cell"
%token CHANNEL               "keyword channel"
%token CHIP                  "keyword chip"
%token COEFFICIENT           "keyword coefficient"
%token CONDUCTIVITY          "keyword conductivity"
%token CONVENTIONAL          "keyword conventional"
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
%token RATE                  "keyword rate"
%token SIDE                  "keyword side"
%token SINK                  "keywork sink"
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

%destructor { FREE_POINTER (free,                     $$) ; } <string>
%destructor { FREE_POINTER (free_layers_list,         $$) ; } <layer_p>

%name-prefix "stack_description_"
%output      "stack_description_parser.c"

%pure-parser
%error-verbose

%parse-param { StackDescription *stkd     }
%parse-param { Analysis         *analysis }
%parse-param { yyscan_t          scanner }

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
    conventional_heat_sink
    microchannel
    dies_list
    dimensions
    stack
    solver
    initial_temperature
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

            stack_description_error (stkd, analysis, scanner, error_message) ;

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
        Material *material = $$ = alloc_and_init_material () ;

        if (material == NULL)
        {
            FREE_POINTER (free, $2) ;

            stack_description_error (stkd, analysis, scanner, "Malloc material failed") ;

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

conventional_heat_sink

  : // Declaring the heat sink section is not mandatory

  | CONVENTIONAL HEAT SINK ':'
        HEAT TRANSFER COEFFICIENT DVALUE ';'  // $8
        AMBIENT TEMPERATURE       DVALUE ';'  // $12
    {
        stkd->ConventionalHeatSink = alloc_and_init_conventional_heat_sink () ;

        if (stkd->ConventionalHeatSink == NULL)
        {
            stack_description_error (stkd, analysis, scanner, "Malloc conventional heat sink failed") ;

            YYABORT ;
        }

        stkd->ConventionalHeatSink->AmbientHTC         = (AmbientHTC_t) $8 ;
        stkd->ConventionalHeatSink->AmbientTemperature = $12 ;
    }
  ;

/******************************************************************************/
/******************************* MicroChannel *********************************/
/******************************************************************************/

microchannel

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
        stkd->Channel = alloc_and_init_channel () ;

        if (stkd->Channel == NULL)
        {
            FREE_POINTER (free, $19) ;

            stack_description_error (stkd, analysis, scanner, "Malloc channel failed") ;

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

            stack_description_error (stkd, analysis, scanner, error_message) ;

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
        stkd->Channel = alloc_and_init_channel () ;

        if (stkd->Channel == NULL)
        {
            FREE_POINTER (free, $17) ;

            stack_description_error (stkd, analysis, scanner, "Malloc channel failed") ;

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

            stack_description_error (stkd, analysis, scanner, error_message) ;

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
        stkd->Channel = alloc_and_init_channel () ;

        if (stkd->Channel == NULL)
        {
            FREE_POINTER (free, $20) ;

            stack_description_error (stkd, analysis, scanner, "Malloc channel failed") ;

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

            stack_description_error (stkd, analysis, scanner, error_message) ;

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

layers_list

  :                    // Declaring a "non active" layer in a die is not mandatory
    {
        $$ = NULL ;    // The first layer in the list will be null
    }
  | layers_list layer  // $1 : pointer to the last layer in the list
                       // $2 : pointer to the layer to add in the list
    {
        if ($1 != NULL)
            JOIN_ELEMENTS ($2, $1) ; // this reverse the order !
        $$ = $2 ;                    // $2 will be the new reference to the list
    }
  ;

layer         : LAYER  layer_content { $$ = $2 ; } ;

source_layer  : SOURCE layer_content { $$ = $2 ; } ;

layer_content :

    DVALUE IDENTIFIER ';' // $1 and $2

    {
        Layer *layer = $$ = alloc_and_init_layer () ;

        if (layer == NULL)
        {
            FREE_POINTER (free, $2) ;

            stack_description_error (stkd, analysis, scanner, "Malloc layer failed") ;

            YYABORT ;
        }

        layer->Height   = $1 ;
        layer->Material = find_material_in_list (stkd->MaterialsList, $2) ;

        if (layer->Material == NULL)
        {
            sprintf (error_message, "Unknown material %s", $2) ;

            FREE_POINTER (free,       $2) ;
            FREE_POINTER (free_layer, layer) ;

            stack_description_error (stkd, analysis, scanner, error_message) ;

            YYABORT ;
        }

        layer->Material->Used++ ;

        FREE_POINTER (free, $2) ;
    }
  ;

/******************************************************************************/
/******************************* Dies *****************************************/
/******************************************************************************/

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

            stack_description_error (stkd, analysis, scanner, error_message) ;

            YYABORT ;
        }

        $1->Next = $2 ;   // insert $2 at the end of the list
        $$ = $2 ;         // $2 will be the new last element in the list
    }
  ;

die

  : DIE IDENTIFIER ':'         // $2 Die identifier
        layers_list            // $4 list of layers (above source -> top)
        source_layer           // $5 source layer
        layers_list            // $6 list of layers (bottom -> below source)
    {
        Die *die = $$ = alloc_and_init_die () ;

        if (die == NULL)
        {
            FREE_POINTER (free,             $2) ;
            FREE_POINTER (free_layers_list, $4) ;
            FREE_POINTER (free_layer,       $5) ;
            FREE_POINTER (free_layers_list, $6);

            stack_description_error (stkd, analysis, scanner, "Malloc die failed") ;

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
        stkd->Dimensions = alloc_and_init_dimensions () ;

        if (stkd->Dimensions == NULL)
        {
            stack_description_error (stkd, analysis, scanner, "Malloc dimensions failed") ;
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
                    stack_description_error (stkd, analysis, scanner, "Error: cell dimensions does not fit the chip length correctly") ;

                    YYABORT ;
                }

                stkd->Dimensions->Grid.NColumns = 2 * ratio + 3 ;

                if ((stkd->Dimensions->Grid.NColumns & 1) == 0)
                {
                    stack_description_error (stkd, analysis, scanner, "Error: colum number must be odd when channel is declared") ;

                    YYABORT ;
                }

                // Check the number of columns

                if (stkd->Dimensions->Grid.NColumns < 3)
                {
                    stack_description_error (stkd, analysis, scanner, "Error: not enough columns") ;

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
            stack_description_error (stkd, analysis, scanner, "Error: stack must contain at least one die") ;

            YYABORT ;
        }

        if (stkd->BottomStackElement->Type == TDICE_STACK_ELEMENT_CHANNEL)
        {
            stack_description_error (stkd, analysis, scanner, "Error: cannot declare a channel as bottom-most stack element") ;

            YYABORT ;
        }

        if (stkd->ConventionalHeatSink == NULL && stkd->Channel == NULL)

            fprintf (stderr, "Warning: neither heat sink nor channel has been declared\n") ;


        FOR_EVERY_ELEMENT_IN_LIST_NEXT (Material, material, stkd->MaterialsList)

            if (material->Used == 0)

                fprintf (stderr, "Warning: material %s declared but not used\n", material->Id) ;


        FOR_EVERY_ELEMENT_IN_LIST_NEXT (Die, die, stkd->DiesList)

            if (die->Used == 0)

                fprintf (stderr, "Warning: die %s declared but not used\n", die->Id) ;

        if (stkd->ConventionalHeatSink != NULL)
        {
            if (stkd->TopStackElement->Type == TDICE_STACK_ELEMENT_LAYER)
            {
                stkd->ConventionalHeatSink->TopLayer = stkd->TopStackElement->Pointer.Layer ;
            }
            else
            {
                stkd->ConventionalHeatSink->TopLayer = stkd->TopStackElement->Pointer.Die->TopLayer ;
            }
        }

        // Counts the number of layers and fix the layer offset starting from
        // the bottom most element in the stack. This operation can be done only
        // here since the parser processes elements in the stack from the top most.

        CellIndex_t layer_index = 0u ;

        FOR_EVERY_ELEMENT_IN_LIST_NEXT (StackElement, stk_el, stkd->BottomStackElement)
        {
            stk_el->Offset = layer_index ;
            layer_index   += stk_el->NLayers ;
        }

        stkd->Dimensions->Grid.NLayers = layer_index ;

        // Evaluate the number of cells and nonzero elements

        stkd->Dimensions->Grid.NCells
            =   get_number_of_layers (stkd->Dimensions)
                * get_number_of_rows (stkd->Dimensions)
                * get_number_of_columns (stkd->Dimensions) ;

        if (stkd->Dimensions->Grid.NCells >  INT32_MAX)
        {
            sprintf (error_message, "%d are too many cells ... (SuperLU uses 'int')", stkd->Dimensions->Grid.NCells) ;

            stack_description_error (stkd, analysis, scanner, error_message) ;

            YYABORT ;
        }

        if (stkd->Channel == NULL)

            compute_number_of_connections

                (stkd->Dimensions, num_channels, TDICE_CHANNEL_MODEL_NONE) ;

        else

            compute_number_of_connections

                (stkd->Dimensions, num_channels, stkd->Channel->ChannelModel) ;

    }
  ;

stack_elements

  : stack_element                 // $1 : pointer to the first stack element found
    {
        if (   stkd->TopStackElement == NULL && $1->Type == TDICE_STACK_ELEMENT_CHANNEL)
        {
            stack_description_error (stkd, analysis, scanner, "Error: cannot declare a channel as top-most stack element") ;

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

            stack_description_error (stkd, analysis, scanner, error_message) ;

            YYABORT ;
        }

        if (   $1->Type == TDICE_STACK_ELEMENT_CHANNEL
            && $2->Type == TDICE_STACK_ELEMENT_CHANNEL)
        {
            stack_description_error (stkd, analysis, scanner, "Error: cannot declare two consecutive channels") ;

            YYABORT ;
        }

        JOIN_ELEMENTS ($2, $1) ;

        stkd->BottomStackElement = $2 ;
        $$ = $2 ;                 // $2 will be the last stack element in the list
    }
  ;

stack_element

  : LAYER IDENTIFIER DVALUE IDENTIFIER ';'    // $2 Identifier for the stack element
                                              // $3 Height of the layer
                                              // $4 Identifier of the material
    {
        StackElement *stack_element = $$ = alloc_and_init_stack_element () ;

        if (stack_element == NULL)
        {
            FREE_POINTER (free, $2) ;
            FREE_POINTER (free, $4) ;

            stack_description_error (stkd, analysis, scanner, "Malloc stack element failed") ;

            YYABORT ;
        }

        Layer *layer = alloc_and_init_layer () ;

        if (layer == NULL)
        {
            FREE_POINTER (free,               $2) ;
            FREE_POINTER (free,               $4) ;
            FREE_POINTER (free_stack_element, stack_element) ;

            stack_description_error (stkd, analysis, scanner, "Malloc layer failed") ;

            YYABORT ;
        }

        layer->Height   = $3 ;
        layer->Material = find_material_in_list (stkd->MaterialsList, $4) ;

        if (layer->Material == NULL)
        {
            sprintf (error_message, "Unknown material %s", $4) ;

            FREE_POINTER (free,               $2) ;
            FREE_POINTER (free,               $4) ;
            FREE_POINTER (free_stack_element, stack_element) ;
            FREE_POINTER (free_layer,         layer) ;

            stack_description_error (stkd, analysis, scanner, error_message) ;

            YYABORT ;
        }

        layer->Material->Used++ ;

        FREE_POINTER (free, $4) ;

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
            stack_description_error (stkd, analysis, scanner, "Error: channel used in stack but not declared") ;

            YYABORT ;
        }

        StackElement *stack_element = $$ = alloc_and_init_stack_element () ;

        if (stack_element == NULL)
        {
            FREE_POINTER (free, $2) ;

            stack_description_error (stkd, analysis, scanner, "Malloc stack element failed") ;

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

        StackElement *stack_element = $$ = alloc_and_init_stack_element () ;

        if (stack_element == NULL)
        {
            FREE_POINTER (free, $2) ;
            FREE_POINTER (free, $3) ;
            FREE_POINTER (free, $5) ;

            stack_description_error (stkd, analysis, scanner, "Malloc stack element failed") ;

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

            stack_description_error (stkd, analysis, scanner, error_message) ;

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

            stack_description_error (stkd, analysis, scanner, "Malloc floorplan failed") ;

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

  : SOLVER ':' STEADY ';'
    {
        // StepTime is set to 1 to avoid division by zero when computing
        // the capacitance of a thermal cell

        analysis->AnalysisType = TDICE_ANALYSIS_TYPE_STEADY ;
        analysis->StepTime     = (Time_t) 1.0 ;
        analysis->SlotTime     = (Time_t) 0.0 ;
        analysis->SlotLength   = 1u ; // CHECKME
    }

  | SOLVER ':' TRANSIENT STEP DVALUE ',' SLOT DVALUE ';'  // $5 StepTime
                                                          // $8 SlotTime
    {
        if ($8 < $5)
        {
            stack_description_error

                (stkd, analysis, scanner, "Slot time must be higher than StepTime") ;

            YYABORT ;
        }

        if ($5 <= 0.0)
        {
            stack_description_error

                (stkd, analysis, scanner, "StepTime must be a positive value") ;

            YYABORT ;
        }

        if ($8 <= 0.0)
        {
            stack_description_error

                (stkd, analysis, scanner, "SlotTime must be a positive value") ;

            YYABORT ;
        }

        analysis->AnalysisType = TDICE_ANALYSIS_TYPE_TRANSIENT ;
        analysis->StepTime     = (Time_t) $5 ;
        analysis->SlotTime     = (Time_t) $8 ;
        analysis->SlotLength   = (Quantity_t) ( $8 / $5 ) ;
    }
  ;

/******************************************************************************/
/*************************** Initial Temperature ******************************/
/******************************************************************************/

initial_temperature

  : INITIAL_ TEMPERATURE DVALUE ';'
    {
        analysis->InitialTemperature = (Temperature_t) $3;
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
        add_inspection_point_to_analysis (analysis, $1) ;
     }

  |  inspection_points_list inspection_point
                      // $1 : not used
                      // $2 : pointer to the inspection point to add in the list
     {
        add_inspection_point_to_analysis (analysis, $2) ;
     }
  ;

inspection_point

  :  TCELL '(' IDENTIFIER ',' DVALUE ',' DVALUE ',' PATH when ')' ';'

     // $3       Identifier of the stack element (layer, channel or die)
     // ($5, $7) Coordinates of the cell to monitor
     // $9       Path of the output file
     // $10      when to generate output for this observation

     {
        StackElement *stack_element =

            find_stack_element_in_list (stkd->BottomStackElement, $3) ;

        if (stack_element == NULL)
        {
            sprintf (error_message, "Unknown stack element %s", $3) ;

            stack_description_error (stkd, analysis, scanner, error_message) ;

            FREE_POINTER (free, $3) ;
            FREE_POINTER (free, $9) ;

            YYABORT ;
        }

        Tcell *tcell = alloc_and_init_tcell () ;

        if (tcell == NULL)
        {
            FREE_POINTER (free, $3) ;
            FREE_POINTER (free, $9) ;

            stack_description_error (stkd, analysis, scanner, "Malloc tcell failed") ;

            YYABORT ;
        }

        align_tcell (tcell, $5, $7, stkd->Dimensions) ;

        InspectionPoint *inspection_point = $$ = alloc_and_init_inspection_point () ;

        if (inspection_point == NULL)
        {
            FREE_POINTER (free, $3) ;
            FREE_POINTER (free, $9) ;

            FREE_POINTER (free_tcell, tcell) ;

            stack_description_error (stkd, analysis, scanner, "Malloc inspection point command failed") ;

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
        StackElement *stack_element =

            find_stack_element_in_list (stkd->BottomStackElement, $3) ;

        if (stack_element == NULL)
        {
            sprintf (error_message, "Unknown stack element %s", $3) ;

            stack_description_error (stkd, analysis, scanner, error_message) ;

            FREE_POINTER (free, $3) ;
            FREE_POINTER (free, $5) ;

            YYABORT ;
        }

        if (stack_element->Type != TDICE_STACK_ELEMENT_DIE)
        {
            sprintf (error_message, "The stack element %s must be a die", $3) ;

            stack_description_error (stkd, analysis, scanner, error_message) ;

            FREE_POINTER (free, $3) ;
            FREE_POINTER (free, $5) ;

            YYABORT ;
        }

        Tflp *tflp = alloc_and_init_tflp () ;

        if (tflp == NULL)
        {
            FREE_POINTER (free, $3) ;
            FREE_POINTER (free, $5) ;

            stack_description_error (stkd, analysis, scanner, "Malloc tflp failed") ;

            YYABORT ;
        }

        tflp->Quantity = $7 ;

        InspectionPoint *inspection_point = $$ = alloc_and_init_inspection_point () ;

        if (inspection_point == NULL)
        {
            FREE_POINTER (free, $3) ;
            FREE_POINTER (free, $5) ;

            FREE_POINTER (free_tflp, tflp) ;

            stack_description_error (stkd, analysis, scanner, "Malloc inspection point command failed") ;

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
        StackElement *stack_element =

            find_stack_element_in_list (stkd->BottomStackElement, $3) ;

        if (stack_element == NULL)
        {
            sprintf (error_message, "Unknown stack element %s", $3) ;

            stack_description_error (stkd, analysis, scanner, error_message) ;

            FREE_POINTER (free, $3) ;
            FREE_POINTER (free, $5) ;
            FREE_POINTER (free, $7) ;

            YYABORT ;
        }

        if (stack_element->Type != TDICE_STACK_ELEMENT_DIE)
        {
            sprintf (error_message, "The stack element %s must be a die", $3) ;

            stack_description_error (stkd, analysis, scanner, error_message) ;

            FREE_POINTER (free, $3) ;
            FREE_POINTER (free, $5) ;
            FREE_POINTER (free, $7) ;

            YYABORT ;
        }

        FloorplanElement *floorplan_element = find_floorplan_element_in_list

            (stack_element->Floorplan->ElementsList, $5) ;

        if (floorplan_element == NULL)
        {
            sprintf (error_message, "Unknown floorplan element %s", $5) ;

            stack_description_error (stkd, analysis, scanner, error_message) ;

            FREE_POINTER (free, $3) ;
            FREE_POINTER (free, $5) ;
            FREE_POINTER (free, $7) ;

            YYABORT ;
        }

        Tflpel *tflpel = alloc_and_init_tflpel () ;

        if (tflpel == NULL)
        {
            FREE_POINTER (free, $3) ;
            FREE_POINTER (free, $5) ;
            FREE_POINTER (free, $7) ;

            stack_description_error (stkd, analysis, scanner, "Malloc Tflpel failed") ;

            YYABORT ;
        }

        tflpel->FloorplanElement = floorplan_element ;
        tflpel->Quantity         = $9 ;

        InspectionPoint *inspection_point = $$ = alloc_and_init_inspection_point () ;

        if (inspection_point == NULL)
        {
            FREE_POINTER (free, $3) ;
            FREE_POINTER (free, $5) ;
            FREE_POINTER (free, $7) ;

            FREE_POINTER (free_tflpel, tflpel) ;

            stack_description_error (stkd, analysis, scanner, "Malloc inspection point command failed") ;

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
        StackElement *stack_element =

            find_stack_element_in_list (stkd->BottomStackElement, $3) ;

        if (stack_element == NULL)
        {
            sprintf (error_message, "Unknown stack element %s", $3) ;

            stack_description_error (stkd, analysis, scanner, error_message) ;

            FREE_POINTER (free, $3) ;
            FREE_POINTER (free, $5) ;

            YYABORT ;
        }

        InspectionPoint *inspection_point = $$ = alloc_and_init_inspection_point () ;

        if (inspection_point == NULL)
        {
            FREE_POINTER (free, $3) ;
            FREE_POINTER (free, $5) ;

            stack_description_error (stkd, analysis, scanner, "Malloc inspection point command failed") ;

            YYABORT ;
        }

        inspection_point->Type         = TDICE_OUTPUT_TYPE_TMAP ;
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
        StackElement *stack_element =

            find_stack_element_in_list (stkd->BottomStackElement, $3) ;

        if (stack_element == NULL)
        {
            sprintf (error_message, "Unknown stack element %s", $3) ;

            stack_description_error (stkd, analysis, scanner, error_message) ;

            FREE_POINTER (free, $3) ;
            FREE_POINTER (free, $5) ;

            YYABORT ;
        }

        if (stack_element->Type != TDICE_STACK_ELEMENT_CHANNEL)
        {
            sprintf (error_message, "The stack element %s must be a channel", $3) ;

            stack_description_error (stkd, analysis, scanner, error_message) ;

            FREE_POINTER (free, $3) ;
            FREE_POINTER (free, $5) ;

            YYABORT ;
        }

        InspectionPoint *inspection_point = $$ = alloc_and_init_inspection_point () ;

        if (inspection_point == NULL)
        {
            FREE_POINTER (free, $3) ;
            FREE_POINTER (free, $5) ;

            stack_description_error (stkd, analysis, scanner, "Malloc inspection point command failed") ;

            YYABORT ;
        }

        Tcoolant *tcoolant = alloc_and_init_tcoolant () ;

        if (tcoolant == NULL)
        {
            FREE_POINTER (free, $3) ;
            FREE_POINTER (free, $5) ;

            stack_description_error (stkd, analysis, scanner, "Malloc tcoolant failed") ;

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

void stack_description_error
(
    StackDescription *stkd,
    Analysis          __attribute__ ((unused)) *analysis,
    yyscan_t          scanner,
    String_t          message
)
{
    fprintf (stack_description_get_out (scanner),
             "%s:%d: %s\n",
            stkd->FileName, stack_description_get_lineno (scanner), message) ;

    FREE_POINTER (free_stack_description, stkd) ;
}
