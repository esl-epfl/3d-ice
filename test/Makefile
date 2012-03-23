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

all: GenerateSystemMatrix CompareSystemMatrix CompareTemperatures runtest

CINCLUDES := $(CINCLUDES) -I$(SLU_INCLUDE)
CLIBS = $(3DICE_LIB_A) $(SLU_LIBS) -lm

-include GenerateSystemMatrix.d

GenerateSystemMatrix: GenerateSystemMatrix.o
	$(CC) $< $(CLIBS) -o $@

-include CompareSystemMatrix.d

CompareSystemMatrix: CompareSystemMatrix.o
	$(CC) $< $(CLIBS) -o $@

-include CompareTemperatures.d

CompareTemperatures: CompareTemperatures.o
	$(CC) $< $(CLIBS) -o $@

runtest: GenerateSystemMatrix CompareSystemMatrix CompareTemperatures ../bin/3D-ICE-Emulator
	@echo "Comparison of system matrices ...."
	@echo -n "mc4rm transient : "
	@./GenerateSystemMatrix mc4rm/transient/2dies_four_elements.stk tr_4rm.txt
	@./CompareSystemMatrix  tr_4rm.txt mc4rm/transient/system_matrix.txt
	@echo -n "mc4rm steady    : "
	@./GenerateSystemMatrix mc4rm/steady/2dies_four_elements.stk st_4rm.txt
	@./CompareSystemMatrix  st_4rm.txt mc4rm/steady/system_matrix.txt
	@echo -n "mc2rm transient : "
	@./GenerateSystemMatrix mc2rm/transient/2dies_four_elements.stk tr_2rm.txt
	@./CompareSystemMatrix  tr_2rm.txt mc2rm/transient/system_matrix.txt
	@echo -n "mc2rm steady    : "
	@./GenerateSystemMatrix mc2rm/steady/2dies_four_elements.stk st_2rm.txt
	@./CompareSystemMatrix  st_2rm.txt mc2rm/steady/system_matrix.txt
	@echo -n "pf2rm transient : "
	@./GenerateSystemMatrix pf2rm/transient/2dies_four_elements.stk tr_pf.txt
	@./CompareSystemMatrix  tr_pf.txt pf2rm/transient/system_matrix.txt
	@echo -n "pf2rm steady    : "
	@./GenerateSystemMatrix pf2rm/steady/2dies_four_elements.stk st_pf.txt
	@./CompareSystemMatrix  st_pf.txt pf2rm/steady/system_matrix.txt
	@echo ""
	@echo "Comparison of transient results ...."
	@echo "------------------------------------"
	@echo "mc4rm 4e"
	@echo "--------"
	@../bin/3D-ICE-Emulator mc4rm/transient/2dies_four_elements.stk
	@./CompareTemperatures mc4rm/transient/four_elements_node1.txt \
                               mc4rm/transient/four_elements_node2.txt \
                               mc4rm/transient/output_four_elements.txt
	@echo "--------"
	@echo "mc4rm bg"
	@echo "--------"
	@../bin/3D-ICE-Emulator mc4rm/transient/2dies_background.stk
	@./CompareTemperatures mc4rm/transient/background_node1.txt  \
                               mc4rm/transient/background_node2.txt  \
                               mc4rm/transient/output_background.txt
	@echo "--------"
	@echo "mc2rm 4e"
	@echo "--------"
	@../bin/3D-ICE-Emulator mc2rm/transient/2dies_four_elements.stk
	@./CompareTemperatures mc2rm/transient/four_elements_node1.txt \
                               mc2rm/transient/four_elements_node2.txt \
                               mc2rm/transient/output_four_elements.txt
	@echo "--------"
	@echo "mc2rm bg"
	@echo "--------"
	@../bin/3D-ICE-Emulator mc2rm/transient/2dies_background.stk
	@./CompareTemperatures mc2rm/transient/background_node1.txt  \
                               mc2rm/transient/background_node2.txt  \
                               mc2rm/transient/output_background.txt
	@echo "--------"
	@echo "pf2rm 4e"
	@echo "--------"
	@../bin/3D-ICE-Emulator pf2rm/transient/2dies_four_elements.stk
	@./CompareTemperatures pf2rm/transient/four_elements_node1.txt  \
                               pf2rm/transient/four_elements_node2.txt  \
                               pf2rm/transient/output_four_elements.txt
	@echo "--------"
	@echo "pf2rm bg"
	@echo "--------"
	@../bin/3D-ICE-Emulator pf2rm/transient/2dies_background.stk
	@./CompareTemperatures pf2rm/transient/background_node1.txt  \
                               pf2rm/transient/background_node2.txt  \
                               pf2rm/transient/output_background.txt
	@echo ""
	@echo "Comparison of steady state results ...."
	@echo "---------------------------------------"
	@echo "mc4rm 4e"
	@echo "--------"
	@../bin/3D-ICE-Emulator mc4rm/steady/2dies_four_elements.stk
	@./CompareTemperatures mc4rm/steady/four_elements_node1.txt \
                               mc4rm/steady/four_elements_node2.txt \
                               mc4rm/steady/output_four_elements.txt
	@echo "--------"
	@echo "mc4rm bg"
	@echo "--------"
	@../bin/3D-ICE-Emulator mc4rm/steady/2dies_background.stk
	@./CompareTemperatures mc4rm/steady/background_node1.txt  \
                               mc4rm/steady/background_node2.txt  \
                               mc4rm/steady/output_background.txt
	@echo "--------"
	@echo "mc2rm 4e"
	@echo "--------"
	@../bin/3D-ICE-Emulator mc2rm/steady/2dies_four_elements.stk
	@./CompareTemperatures mc2rm/steady/four_elements_node1.txt \
                               mc2rm/steady/four_elements_node2.txt \
                               mc2rm/steady/output_four_elements.txt
	@echo "--------"
	@echo "mc2rm bg"
	@echo "--------"
	@../bin/3D-ICE-Emulator mc2rm/steady/2dies_background.stk
	@./CompareTemperatures mc2rm/steady/background_node1.txt  \
                               mc2rm/steady/background_node2.txt  \
                               mc2rm/steady/output_background.txt
	@echo "--------"
	@echo "pf2rm 4e"
	@echo "--------"
	@../bin/3D-ICE-Emulator pf2rm/steady/2dies_four_elements.stk
	@./CompareTemperatures pf2rm/steady/four_elements_node1.txt \
                               pf2rm/steady/four_elements_node2.txt \
                               pf2rm/steady/output_four_elements.txt
	@echo "--------"
	@echo "pf2rm bg"
	@echo "--------"
	@../bin/3D-ICE-Emulator pf2rm/steady/2dies_background.stk
	@./CompareTemperatures pf2rm/steady/background_node1.txt  \
                               pf2rm/steady/background_node2.txt  \
                               pf2rm/steady/output_background.txt

clean:
	@$(RM) $(RMFLAGS) GenerateSystemMatrix
	@$(RM) $(RMFLAGS) GenerateSystemMatrix.o
	@$(RM) $(RMFLAGS) GenerateSystemMatrix.d
	@$(RM) $(RMFLAGS) CompareSystemMatrix
	@$(RM) $(RMFLAGS) CompareSystemMatrix.o
	@$(RM) $(RMFLAGS) CompareSystemMatrix.d
	@$(RM) $(RMFLAGS) CompareTemperatures
	@$(RM) $(RMFLAGS) CompareTemperatures.o
	@$(RM) $(RMFLAGS) CompareTemperatures.d
	@$(RM) $(RMFLAGS) tr_4rm.txt tr_pf.txt tr_2rm.txt
	@$(RM) $(RMFLAGS) st_4rm.txt st_pf.txt st_2rm.txt
	@$(RM) $(RMFLAGS) mc2rm/transient/background_node1.txt
	@$(RM) $(RMFLAGS) mc2rm/transient/background_node2.txt
	@$(RM) $(RMFLAGS) mc2rm/transient/four_elements_node1.txt
	@$(RM) $(RMFLAGS) mc2rm/transient/four_elements_node2.txt
	@$(RM) $(RMFLAGS) mc4rm/transient/background_node1.txt
	@$(RM) $(RMFLAGS) mc4rm/transient/background_node2.txt
	@$(RM) $(RMFLAGS) mc4rm/transient/four_elements_node1.txt
	@$(RM) $(RMFLAGS) mc4rm/transient/four_elements_node2.txt
	@$(RM) $(RMFLAGS) pf2rm/transient/background_node1.txt
	@$(RM) $(RMFLAGS) pf2rm/transient/background_node2.txt
	@$(RM) $(RMFLAGS) pf2rm/transient/four_elements_node1.txt
	@$(RM) $(RMFLAGS) pf2rm/transient/four_elements_node2.txt
	@$(RM) $(RMFLAGS) mc4rm/steady/background_node1.txt
	@$(RM) $(RMFLAGS) mc4rm/steady/background_node2.txt
	@$(RM) $(RMFLAGS) mc4rm/steady/four_elements_node1.txt
	@$(RM) $(RMFLAGS) mc4rm/steady/four_elements_node2.txt
	@$(RM) $(RMFLAGS) mc2rm/steady/background_node1.txt
	@$(RM) $(RMFLAGS) mc2rm/steady/background_node2.txt
	@$(RM) $(RMFLAGS) mc2rm/steady/four_elements_node1.txt
	@$(RM) $(RMFLAGS) mc2rm/steady/four_elements_node2.txt
	@$(RM) $(RMFLAGS) pf2rm/steady/background_node1.txt
	@$(RM) $(RMFLAGS) pf2rm/steady/background_node2.txt
	@$(RM) $(RMFLAGS) pf2rm/steady/four_elements_node1.txt
	@$(RM) $(RMFLAGS) pf2rm/steady/four_elements_node2.txt
