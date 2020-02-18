###############################################################################
# This file is part of 3D-ICE, version 2.2.7 .                                #
#                                                                             #
# 3D-ICE is free software: you can  redistribute it and/or  modify it  under  #
# the terms of the  GNU General  Public  License as  published by  the  Free  #
# Software  Foundation, either  version  3  of  the License,  or  any  later  #
# version.                                                                    #
#                                                                             #
# 3D-ICE is  distributed  in the hope  that it will  be useful, but  WITHOUT  #
# ANY  WARRANTY; without  even the  implied warranty  of MERCHANTABILITY  or  #
# FITNESS  FOR A PARTICULAR  PURPOSE. See the GNU General Public License for  #
# more details.                                                               #
#                                                                             #
# You should have  received a copy of  the GNU General  Public License along  #
# with 3D-ICE. If not, see <http://www.gnu.org/licenses/>.                    #
#                                                                             #
#                             Copyright (C) 2017                              #
#   Embedded Systems Laboratory - Ecole Polytechnique Federale de Lausanne    #
#                            All Rights Reserved.                             #
#                                                                             #
# Authors: Federico Terraneo                                                  #
#          Arvind Sridhar                                                     #
#          Alessandro Vincenzi                                                #
#          Giseong Bak                                                        #
#          Martino Ruggiero                                                   #
#          Thomas Brunschwiler                                                #
#          David Atienza                                                      #
#                                                                             #
# For any comment, suggestion or request  about 3D-ICE, please  register and  #
# write to the mailing list (see http://listes.epfl.ch/doc.cgi?liste=3d-ice)  #
# Any usage  of 3D-ICE  for research,  commercial or other  purposes must be  #
# properly acknowledged in the resulting products or publications.            #
#                                                                             #
# EPFL-STI-IEL-ESL                                                            #
# Batiment ELG, ELG 130                Mail : 3d-ice@listes.epfl.ch           #
# Station 11                                  (SUBSCRIPTION IS NECESSARY)     #
# 1015 Lausanne, Switzerland           Url  : http://esl.epfl.ch/3d-ice.html  #
###############################################################################

# This is just a template, write your heatsink code here
# by implementing the heatsinkInit and heatsinkSimulateStep functions

ambientTemperature=0;
conductance=0;

def parallel(x,y):
    return x*y/(x+y)

def heatsinkInit(nRows,nCols,cellWidth,cellLength,initialTemperature,spreaderConductance,timeStep,args):
    print(nRows)
    print(nCols)
    print(cellWidth)
    print(cellLength)
    print(initialTemperature)
    print(spreaderConductance)
    print(timeStep)
    print(args)
    totalConductance=1
    ambientConductance=1/(nRows*nCols)
    global ambientTemperature
    global conductance
    conductance=parallel(spreaderConductance,ambientConductance)
    ambientTemperature=initialTemperature

def heatsinkSimulateStep(spreaderTemperatures):
    # See the C++ plugin comments for this
    global ambientTemperature
    global conductance
    heatFlow=[]
    for i in range(len(spreaderTemperatures)):
        heatFlow.append(conductance*(spreaderTemperatures[i]-ambientTemperature))
    return heatFlow
