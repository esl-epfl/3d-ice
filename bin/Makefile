 ##############################################################################
 # This file is part of 3D-ICE, version 2.1 .                                 #
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
 # with 3D-ICE. If not, see <http://www.gnu.org/licenses/>.                   #
 #                                                                            #
 #                             Copyright (C) 2010                             #
 #   Embedded Systems Laboratory - Ecole Polytechnique Federale de Lausanne   #
 #                            All Rights Reserved.                            #
 #                                                                            #
 # Authors: Arvind Sridhar                                                    #
 #          Alessandro Vincenzi                                               #
 #          Giseong Bak                                                       #
 #          Martino Ruggiero                                                  #
 #          Thomas Brunschwiler                                               #
 #          David Atienza                                                     #
 #                                                                            #
 # For any comment, suggestion or request  about 3D-ICE, please  register and #
 # write to the mailing list (see http://listes.epfl.ch/doc.cgi?liste=3d-ice) #
 # Any usage  of 3D-ICE  for research,  commercial or other  purposes must be #
 # properly acknowledged in the resulting products or publications.           #
 #                                                                            #
 # EPFL-STI-IEL-ESL                                                           #
 # Batiment ELG, ELG 130                Mail : 3d-ice@listes.epfl.ch          #
 # Station 11                                  (SUBSCRIPTION IS NECESSARY)    #
 # 1015 Lausanne, Switzerland           Url  : http://esl.epfl.ch/3d-ice.html #
 ##############################################################################

3DICE_MAIN = ..

include $(3DICE_MAIN)/makefile.def

all: 3D-ICE-Emulator 3D-ICE-Client 3D-ICE-Server

CFLAGS := $(CFLAGS) -Wall -Wextra -Werror

CINCLUDES := $(CINCLUDES) -I$(SLU_INCLUDE)
CLIBS = $(3DICE_LIB_A) $(SLU_LIBS) -lm

-include 3D-ICE-Emulator.d

3D-ICE-Emulator: 3D-ICE-Emulator.o $(3DICE_LIB_A)
	$(CC) $< $(CLIBS) -o $@

-include 3D-ICE-Client.d

3D-ICE-Client: 3D-ICE-Client.o $(3DICE_LIB_A)
	$(CC) $< $(CLIBS) -o $@

-include 3D-ICE-Server.d

3D-ICE-Server: 3D-ICE-Server.o $(3DICE_LIB_A)
	$(CC) $< $(CLIBS) -o $@

clean:
	@$(RM) $(RMFLAGS) 3D-ICE-Emulator
	@$(RM) $(RMFLAGS) 3D-ICE-Emulator.o
	@$(RM) $(RMFLAGS) 3D-ICE-Emulator.d
	@$(RM) $(RMFLAGS) 3D-ICE-Client
	@$(RM) $(RMFLAGS) 3D-ICE-Client.o
	@$(RM) $(RMFLAGS) 3D-ICE-Client.d
	@$(RM) $(RMFLAGS) 3D-ICE-Server
	@$(RM) $(RMFLAGS) 3D-ICE-Server.o
	@$(RM) $(RMFLAGS) 3D-ICE-Server.d

