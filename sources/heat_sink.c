/******************************************************************************
 * This file is part of 3D-ICE, version 2.2.7 .                               *
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

#include <math.h>   // Fo the math function sqrt
#include <stdlib.h> // For the memory functions malloc/free
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <dlfcn.h>

#include "heat_sink.h"
#include "macros.h"

/******************************************************************************/

void heat_sink_init (HeatSink_t *hsink)
{
    hsink->SinkModel          = TDICE_HEATSINK_NONE ;
    hsink->AmbientHTC         = (AmbientHTC_t) 0.0 ;
    hsink->AmbientTemperature = (Temperature_t) 0.0 ;
    
    hsink->SpreaderLength     = 0.0;
    hsink->SpreaderWidth      = 0.0;
    hsink->SpreaderHeight     = 0.0;
    
    material_init(&hsink->SpreaderMaterial);
    string_init(&hsink->Plugin);
    string_init(&hsink->Args);
    
    hsink->CellLength         = 0.0;
    hsink->CellWidth          = 0.0;
    hsink->NRows              = 0;
    hsink->NColumns           = 0;
    hsink->NumRowsBorder      = 0;
    hsink->NumColumnsBorder   = 0;
    
    hsink->CurrentSinkHeatFlows     = NULL;
    hsink->PluggableHeatsink        = NULL;
}

/******************************************************************************/

void *array_alloc_copy(const void *src, size_t size)
{
    void *result=malloc(size);
    // TODO: It would be better to report the error in a better way, but
    // heat_sink_copy returns void and not an error code
    assert(result != NULL);
    memcpy(result,src,size);
    return result;
}

/******************************************************************************/

void heat_sink_copy (HeatSink_t *dst, HeatSink_t *src)
{
    heat_sink_destroy (dst) ;

    dst->SinkModel          = src->SinkModel ;
    dst->AmbientHTC         = src->AmbientHTC ;
    dst->AmbientTemperature = src->AmbientTemperature ;
    
    dst->SpreaderLength     = src->SpreaderLength;
    dst->SpreaderWidth      = src->SpreaderWidth;
    dst->SpreaderHeight     = src->SpreaderHeight;
    
    material_copy(&dst->SpreaderMaterial,&src->SpreaderMaterial);
    string_copy(&dst->Plugin,&src->Plugin);
    string_copy(&dst->Args,  &src->Args);
    
    dst->CellLength         = src->CellLength;
    dst->CellWidth          = src->CellWidth;
    dst->NRows              = src->NRows;
    dst->NColumns           = src->NColumns;
    dst->NumRowsBorder      = src->NumRowsBorder;
    dst->NumColumnsBorder   = src->NumColumnsBorder;
    
    size_t size = src->NColumns * src->NRows * sizeof(double);
    dst->CurrentSinkHeatFlows  = (double *) array_alloc_copy(src->CurrentSinkHeatFlows,  size);
    dst->PluggableHeatsink  = src->PluggableHeatsink;
}

/******************************************************************************/

void heat_sink_destroy (HeatSink_t *hsink)
{    
    material_destroy (&hsink->SpreaderMaterial);
    string_destroy (&hsink->Plugin);
    string_destroy (&hsink->Args);
    
    free(hsink->CurrentSinkHeatFlows);
    
    heat_sink_init (hsink) ;
}

/******************************************************************************/

HeatSink_t *heat_sink_calloc (void)
{
    HeatSink_t *hsink = (HeatSink_t *) malloc (sizeof(HeatSink_t)) ;

    if (hsink != NULL)

        heat_sink_init (hsink) ;

    return hsink ;
}

/******************************************************************************/

HeatSink_t *heat_sink_clone (HeatSink_t *hsink)
{
    if (hsink == NULL)

        return NULL ;

    HeatSink_t *newh = heat_sink_calloc ( ) ;

    if (newh != NULL)

        heat_sink_copy (newh, hsink) ;

    return newh ;
}

/******************************************************************************/

void heat_sink_free (HeatSink_t *hsink)
{
    if (hsink == NULL)

        return ;

    heat_sink_destroy (hsink) ;

    free (hsink) ;
}

/******************************************************************************/

Conductance_t heat_sink_conductance
(
    HeatSink_t    *hsink,
    Dimensions_t  *dimensions,
    CellIndex_t    row_index,
    CellIndex_t    column_index
)
{
    assert(hsink->SinkModel != TDICE_HEATSINK_TOP_PLUGGABLE);
    
    return    hsink->AmbientHTC
            * get_cell_length (dimensions, column_index)
            * get_cell_width  (dimensions, row_index) ;
}

/******************************************************************************/

void heat_sink_print (HeatSink_t *hsink, FILE *stream, String_t prefix)
{
    int format=0;
    
    if (hsink->SinkModel == TDICE_HEATSINK_TOP)

        fprintf (stream, "%stop heat sink :\n", prefix) ;

    else if (hsink->SinkModel == TDICE_HEATSINK_BOTTOM)

        fprintf (stream, "%sbottom heat sink :\n", prefix) ;
    
    else if (hsink->SinkModel == TDICE_HEATSINK_TOP_PLUGGABLE)
    {
        fprintf (stream, "%stop pluggable heat sink :\n", prefix) ;
        format=1;
    }
    else
    {
        fprintf (stream, "wrong heat sink model\n") ;

        return ;
    }

    if(format==0)
    {
        fprintf (stream,
            "%s   heat transfer coefficient %.4e ;\n",
            prefix, hsink->AmbientHTC) ;

        fprintf (stream,
            "%s   temperature               %.2f ;\n",
            prefix, hsink->AmbientTemperature) ;
    }
    else
    {
        fprintf (stream,
            "%s   spreader length          %.0f ;\n",
            prefix, hsink->SpreaderLength) ;

        fprintf (stream,
            "%s   spreader width           %.0f ;\n",
            prefix, hsink->SpreaderWidth) ;
        
        fprintf (stream,
            "%s   spreader height          %.0f ;\n",
            prefix, hsink->SpreaderHeight) ;
        
        material_print(&hsink->SpreaderMaterial, stream, prefix);
        
        fprintf (stream,
            "%s   plugin                  %s ;\n",
            prefix, hsink->Plugin) ;
        
        fprintf (stream,
            "%s   plugin args             %s ;\n",
            prefix, hsink->Args) ;
        
        fprintf (stream,
            "%s   cell     length          %.0f ;\n",
            prefix, hsink->CellLength) ;

        fprintf (stream,
            "%s   cell     width           %.0f ;\n",
            prefix, hsink->CellWidth) ;
        
        fprintf (stream,
            "%s   num rows                 %d ;\n",
            prefix, hsink->NRows) ;
        
        fprintf (stream,
            "%s   num columns              %d ;\n",
            prefix, hsink->NColumns) ;
        
        fprintf (stream,
            "%s   num rows border          %d ;\n",
            prefix, hsink->NumRowsBorder) ;
        
        fprintf (stream,
            "%s   num columns border       %d ;\n",
            prefix, hsink->NumColumnsBorder) ;
    }
}

// Need a static variable because atexit doesn't allow parameters
static void *so = NULL;

static void close_shared_object()
{
    if(so) dlclose(so);
}

Error_t initialize_heat_spreader(HeatSink_t *hsink, Dimensions_t *chip)
{
    if(hsink->SinkModel != TDICE_HEATSINK_TOP_PLUGGABLE)
        return TDICE_SUCCESS;
        
    // If the spreader dimensions are not within 0.1 of the cell size print a warning
    const double maxSlack = 0.1;
    
    double borderLength = (hsink->SpreaderLength - chip->Chip.Length) / 2.0;
    hsink->CellLength = chip->Chip.Length / get_number_of_columns(chip);
    
    double numColumnsBorderDouble = borderLength / hsink->CellLength;
    if(numColumnsBorderDouble<0.0)
    {
        fprintf (stderr, "ERROR: spreader length is smaller than the chip\n") ;
        return TDICE_FAILURE;
    }
    hsink->NumColumnsBorder = numColumnsBorderDouble;
    if(numColumnsBorderDouble - hsink->NumColumnsBorder > maxSlack)
        fprintf (stderr, "WARNING: spreader length not a multiple of cell length\n") ;
    
    hsink->NColumns = 2 * hsink->NumColumnsBorder + get_number_of_columns(chip);
    
    
    double borderWidth  = (hsink->SpreaderWidth  - chip->Chip.Width)  / 2.0;
    hsink->CellWidth = chip->Chip.Width / get_number_of_rows(chip);
    
    double numRowsBorderDouble = borderWidth / hsink->CellWidth;
    if(numRowsBorderDouble<0.0)
    {
        fprintf (stderr, "ERROR: spreader width is smaller than the chip\n") ;
        return TDICE_FAILURE;
    }
    hsink->NumRowsBorder = numRowsBorderDouble;
    if(numRowsBorderDouble - hsink->NumRowsBorder > maxSlack)
        fprintf (stderr, "WARNING: spreader length not a multiple of cell length\n") ;
    
    hsink->NRows = 2 * hsink->NumRowsBorder + get_number_of_rows(chip);
    
    
    hsink->CurrentSinkHeatFlows  = (double*) calloc(hsink->NColumns * hsink->NRows, sizeof(double));
    if(hsink->CurrentSinkHeatFlows == NULL)
    {
        fprintf (stderr, "ERROR: could not allocate memory for pluggable heat sink\n") ;
        return TDICE_FAILURE;
    }
    
    char path[2048];
    memset(path,0,sizeof(path));
    
    if(strlen(hsink->Plugin)>0 && hsink->Plugin[0]!='/')
    {
        // Plugin has relative path, add <pwd>/ to path
        if(getcwd(path,sizeof(path)-1)==NULL)
        {
            fprintf (stderr, "ERROR: getcwd() failed\n") ;
            return TDICE_FAILURE;
        }
        strncat(path,"/",sizeof(path)-1);
    }
        
    strncat(path,hsink->Plugin,sizeof(path)-1);
    so = dlopen(path, RTLD_LAZY | RTLD_GLOBAL);
    if(so == NULL)
    {
        fprintf (stderr, "ERROR: could not load heatsink plugin %s\n", path) ;
        return TDICE_FAILURE;
    }
    atexit(close_shared_object);
    
    hsink->PluggableHeatsinkInit = 
    (int (*)(unsigned int, unsigned int, double, double, double, double, double, const char*))
           dlsym(so, "heatsink_init");
    if(hsink->PluggableHeatsinkInit == NULL)
    {
        fprintf (stderr, "ERROR: heatsink plugin reported %s\n", dlerror()) ;
        return TDICE_FAILURE;
    }
    
    hsink->PluggableHeatsink =
    (int (*)(const double*, double*))
           dlsym(so, "heatsink_simulate_step");
    if(hsink->PluggableHeatsink == NULL)
    {
        fprintf (stderr, "ERROR: heatsink plugin reported %s\n", dlerror()) ;
        return TDICE_FAILURE;
    }
    
    return TDICE_SUCCESS;
}

Error_t initialize_pluggable_heatsink(HeatSink_t *hsink, Analysis_t *analysis)
{
    double spreaderConductance = get_spreader_conductance_top_bottom(hsink);
    if(hsink->PluggableHeatsinkInit(
        hsink->NRows,     hsink->NColumns,
        hsink->CellWidth, hsink->CellLength,
        analysis->InitialTemperature,
        spreaderConductance,
        analysis->StepTime,
        hsink->Args) != 0)
        return TDICE_FAILURE;
    return TDICE_SUCCESS;
}

Capacity_t get_spreader_capacity(HeatSink_t *hsink)
{
    assert(hsink->SinkModel == TDICE_HEATSINK_TOP_PLUGGABLE);
    
    return   hsink->SpreaderMaterial.VolumetricHeatCapacity
            * hsink->CellLength
            * hsink->CellWidth
            * hsink->SpreaderHeight;
}

Conductance_t get_spreader_conductance_east_west(HeatSink_t *hsink)
{
    assert(hsink->SinkModel == TDICE_HEATSINK_TOP_PLUGGABLE);
    
    return (  hsink->SpreaderMaterial.ThermalConductivity
             * hsink->SpreaderHeight
             * hsink->CellWidth) / (hsink->CellLength / 2.0);
}

Conductance_t get_spreader_conductance_north_south(HeatSink_t *hsink)
{
    assert(hsink->SinkModel == TDICE_HEATSINK_TOP_PLUGGABLE);
    
    return (  hsink->SpreaderMaterial.ThermalConductivity
             * hsink->CellLength
             * hsink->SpreaderHeight) / (hsink->CellWidth / 2.0);
}

Conductance_t get_spreader_conductance_top_bottom(HeatSink_t *hsink)
{
    assert(hsink->SinkModel == TDICE_HEATSINK_TOP_PLUGGABLE);
    
    return (  hsink->SpreaderMaterial.ThermalConductivity
             * hsink->CellLength
             * hsink->CellWidth) / (hsink->SpreaderHeight / 2.0);
}

/******************************************************************************/
