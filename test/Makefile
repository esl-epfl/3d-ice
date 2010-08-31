 ##############################################################################
 # Makefile "3D-ICe/test/Makefile"                                            #
 #                                                                            #
 # This file is part of 3D-ICE 1.0 (beta).                                    #
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
 # Station 11                                          threed-ice@esl.epfl.ch #
 # 1015 Lausanne, Switzerland                       http://esl.epfl.ch/3D-ICE #
 ##############################################################################

TDICE_MAIN = ..

include $(TDICE_MAIN)/makefile.def

all: FillThermalData Emulate3DICe

CINCLUDES := $(CINCLUDES) -I $(SLU_INCLUDE) 
CLIBS    = $(TDICE_ARCHIVE) $(SLU_LIBS)

FillThermalData: $(TDICE_LIB_ARCHIVE)
	$(CC) $(CFLAGS) $@.c $(CINCLUDES) $(CLIBS) -o $@

Emulate3DICe: $(TDICE_LIB_ARCHIVE)
	$(CC) $(CFLAGS) $@.c $(CINCLUDES) $(CLIBS) -o $@


clean:
	@$(RM) $(RMFLAGS) FillThermalData
	@$(RM) $(RMFLAGS) Emulate3DICe
