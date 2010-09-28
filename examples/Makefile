 ##############################################################################
 # Makefile "3D-ICe/test/Makefile"                                            #
 #                                                                            #
 # This file is part of 3D-ICE, version 1.0 .                                 #
 #                                                                            #
 # 3D-ICE is free software: you can  redistribute it and/or  modify it  under #
 # the terms of the  GNU General  Public  License as  published by  the  Free #
 # Software  Foundation, either  version  3  of  the License,  or  any  later #
 # version.                                                                   #
 #                                                                            #
 # 3D-ICE is  distributed  in the hope  that it will  be useful, but  WITHOUT #
 # ANY  WARRANTY; without  even the  implied warranty  of MERCHANTABILITY  or #
 # FITNESS  FOR A PARTICULAR  PURPOSE. See the GNU General Public License for #
 # more details.                                                              #
 #                                                                            #
 # You should have  received a copy of  the GNU General  Public License along #
 # with 3D-ICe. If not, see <http://www.gnu.org/licenses/>.                   #
 #                                                                            #
 #                             Copyright (C) 2010                             #
 #   Embedded Systems Laboratory - Ecole Polytechnique Federale de Lausanne   #
 #                            All Rights Reserved.                            #
 #                                                                            #
 # Authors: Arvind Sridhar                                                    #
 #          Alessandro Vincenzi                                               #
 #          Martino Ruggiero                                                  #
 #          Thomas Brunschwiler                                               #
 #          David Atienza                                                     #
 #                                                                            #
 # EPFL-STI-IEL-ESL                                                           #
 # Bstiment ELG, ELG 130                                                      #
 # Station 11                                           3d-ice@listes.epfl.ch #
 # 1015 Lausanne, Switzerland                  http://esl.epfl.ch/3d-ice.html #
 ##############################################################################

3DICE_MAIN = ..

include $(3DICE_MAIN)/makefile.def

all: FillThermalData Emulate3DICe

CINCLUDES := $(CINCLUDES) -I$(SLU_INCLUDE)
CLIBS = $(3DICE_LIB_A) $(SLU_LIBS)

-include FillThermalData.d

FillThermalData: FillThermalData.o $(3DICE_LIB_A)
	$(CC) $< $(CLIBS) -o $@

-include Emulate3DICe.d

Emulate3DICe: Emulate3DICe.o $(3DICE_LIB_A)
	$(CC) $< $(CLIBS) -o $@

clean:
	@$(RM) $(RMFLAGS) FillThermalData
	@$(RM) $(RMFLAGS) FillThermalData.o
	@$(RM) $(RMFLAGS) FillThermalData.d
	@$(RM) $(RMFLAGS) Emulate3DICe
	@$(RM) $(RMFLAGS) Emulate3DICe.o
	@$(RM) $(RMFLAGS) Emulate3DICe.d
