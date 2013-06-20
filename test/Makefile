 ##############################################################################
 # This file is part of 3D-ICE, version 2.2.4 .                               #
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
	@echo ""
	@echo "Comparison of system matrices ...."
	@echo "----------------------------------"
	@echo -n "solid transient top hs    : "
	@./GenerateSystemMatrix solid/transient/topsink.stk tr_topsink.txt
	@./CompareSystemMatrix  tr_topsink.txt solid/transient/system_matrix_topsink.txt
	@echo -n "solid steady top hs       : "
	@./GenerateSystemMatrix solid/steady/topsink.stk st_topsink.txt
	@./CompareSystemMatrix  st_topsink.txt solid/steady/system_matrix_topsink.txt
	@echo -n "solid transient bottom hs : "
	@./GenerateSystemMatrix solid/transient/bottomsink.stk tr_bottomsink.txt
	@./CompareSystemMatrix  tr_bottomsink.txt solid/transient/system_matrix_bottomsink.txt
	@echo -n "solid steady bottom hs    : "
	@./GenerateSystemMatrix solid/steady/bottomsink.stk st_bottomsink.txt
	@./CompareSystemMatrix  st_bottomsink.txt solid/steady/system_matrix_bottomsink.txt
	@echo -n "solid transient both hs   : "
	@./GenerateSystemMatrix solid/transient/bothsink.stk tr_bothsink.txt
	@./CompareSystemMatrix  tr_bothsink.txt solid/transient/system_matrix_bothsink.txt
	@echo -n "solid steady both hs      : "
	@./GenerateSystemMatrix solid/steady/bothsink.stk st_bothsink.txt
	@./CompareSystemMatrix  st_bothsink.txt solid/steady/system_matrix_bothsink.txt
	@echo -n "mc4rm transient           : "
	@./GenerateSystemMatrix mc4rm/transient/2dies_four_elements.stk tr_4rm.txt
	@./CompareSystemMatrix  tr_4rm.txt mc4rm/transient/system_matrix.txt
	@echo -n "mc4rm steady              : "
	@./GenerateSystemMatrix mc4rm/steady/2dies_four_elements.stk st_4rm.txt
	@./CompareSystemMatrix  st_4rm.txt mc4rm/steady/system_matrix.txt
	@echo -n "mc2rm transient           : "
	@./GenerateSystemMatrix mc2rm/transient/2dies_four_elements.stk tr_2rm.txt
	@./CompareSystemMatrix  tr_2rm.txt mc2rm/transient/system_matrix.txt
	@echo -n "mc2rm steady              : "
	@./GenerateSystemMatrix mc2rm/steady/2dies_four_elements.stk st_2rm.txt
	@./CompareSystemMatrix  st_2rm.txt mc2rm/steady/system_matrix.txt
	@echo -n "pf2rm transient           : "
	@./GenerateSystemMatrix pf2rm/transient/2dies_four_elements.stk tr_pf.txt
	@./CompareSystemMatrix  tr_pf.txt pf2rm/transient/system_matrix.txt
	@echo -n "pf2rm steady              : "
	@./GenerateSystemMatrix pf2rm/steady/2dies_four_elements.stk st_pf.txt
	@./CompareSystemMatrix  st_pf.txt pf2rm/steady/system_matrix.txt
	@echo ""
	@echo "Comparison of transient results ...."
	@echo "------------------------------------"
	@echo -n "solid top    : "
	@../bin/3D-ICE-Emulator solid/transient/topsink.stk > /dev/null
	@./CompareTemperatures  solid/transient/node1_top.txt solid/transient/node2_top.txt solid/transient/output_top.txt
	@echo -n "solid bottom : "
	@../bin/3D-ICE-Emulator solid/transient/bottomsink.stk > /dev/null
	@./CompareTemperatures  solid/transient/node1_bottom.txt solid/transient/node2_bottom.txt solid/transient/output_bottom.txt
	@echo -n "solid both   : "
	@../bin/3D-ICE-Emulator solid/transient/bothsink.stk > /dev/null
	@./CompareTemperatures  solid/transient/node1_both.txt solid/transient/node2_both.txt solid/transient/output_both.txt
	@echo -n "mc4rm 4e     : "
	@../bin/3D-ICE-Emulator mc4rm/transient/2dies_four_elements.stk > /dev/null
	@./CompareTemperatures  mc4rm/transient/four_elements_node1.txt mc4rm/transient/four_elements_node2.txt  mc4rm/transient/output_four_elements.txt
	@echo -n "mc4rm bg     : "
	@../bin/3D-ICE-Emulator mc4rm/transient/2dies_background.stk > /dev/null
	@./CompareTemperatures  mc4rm/transient/background_node1.txt    mc4rm/transient/background_node2.txt     mc4rm/transient/output_background.txt
	@echo -n "mc2rm 4e     : "
	@../bin/3D-ICE-Emulator mc2rm/transient/2dies_four_elements.stk > /dev/null
	@./CompareTemperatures  mc2rm/transient/four_elements_node1.txt mc2rm/transient/four_elements_node2.txt  mc2rm/transient/output_four_elements.txt
	@echo -n "mc2rm bg     : "
	@../bin/3D-ICE-Emulator mc2rm/transient/2dies_background.stk > /dev/null
	@./CompareTemperatures  mc2rm/transient/background_node1.txt    mc2rm/transient/background_node2.txt     mc2rm/transient/output_background.txt
	@echo -n "pf2rm 4e     : "
	@../bin/3D-ICE-Emulator pf2rm/transient/2dies_four_elements.stk > /dev/null
	@./CompareTemperatures  pf2rm/transient/four_elements_node1.txt pf2rm/transient/four_elements_node2.txt  pf2rm/transient/output_four_elements.txt
	@echo -n "pf2rm bg     : "
	@../bin/3D-ICE-Emulator pf2rm/transient/2dies_background.stk > /dev/null
	@./CompareTemperatures  pf2rm/transient/background_node1.txt    pf2rm/transient/background_node2.txt     pf2rm/transient/output_background.txt
	@echo ""
	@echo "Comparison of steady state results ...."
	@echo "---------------------------------------"
	@echo -n "solid top    : "
	@../bin/3D-ICE-Emulator solid/steady/topsink.stk > /dev/null
	@./CompareTemperatures  solid/steady/node1_top.txt solid/steady/node2_top.txt solid/steady/output_top.txt
	@echo -n "solid bottom : "
	@../bin/3D-ICE-Emulator solid/steady/bottomsink.stk > /dev/null
	@./CompareTemperatures  solid/steady/node1_bottom.txt solid/steady/node2_bottom.txt solid/steady/output_bottom.txt
	@echo -n "solid both   : "
	@../bin/3D-ICE-Emulator solid/steady/bothsink.stk > /dev/null
	@./CompareTemperatures  solid/steady/node1_both.txt solid/steady/node2_both.txt solid/steady/output_both.txt
	@echo -n "mc4rm 4e     : "
	@../bin/3D-ICE-Emulator mc4rm/steady/2dies_four_elements.stk > /dev/null
	@./CompareTemperatures mc4rm/steady/four_elements_node1.txt mc4rm/steady/four_elements_node2.txt mc4rm/steady/output_four_elements.txt
	@echo -n "mc4rm bg     : "
	@../bin/3D-ICE-Emulator mc4rm/steady/2dies_background.stk > /dev/null
	@./CompareTemperatures mc4rm/steady/background_node1.txt    mc4rm/steady/background_node2.txt    mc4rm/steady/output_background.txt
	@echo -n "mc2rm 4e     : "
	@../bin/3D-ICE-Emulator mc2rm/steady/2dies_four_elements.stk > /dev/null
	@./CompareTemperatures mc2rm/steady/four_elements_node1.txt mc2rm/steady/four_elements_node2.txt mc2rm/steady/output_four_elements.txt
	@echo -n "mc2rm bg     : "
	@../bin/3D-ICE-Emulator mc2rm/steady/2dies_background.stk > /dev/null
	@./CompareTemperatures mc2rm/steady/background_node1.txt    mc2rm/steady/background_node2.txt    mc2rm/steady/output_background.txt
	@echo -n "pf2rm 4e     : "
	@../bin/3D-ICE-Emulator pf2rm/steady/2dies_four_elements.stk > /dev/null
	@./CompareTemperatures pf2rm/steady/four_elements_node1.txt pf2rm/steady/four_elements_node2.txt pf2rm/steady/output_four_elements.txt
	@echo -n "pf2rm bg     : "
	@../bin/3D-ICE-Emulator pf2rm/steady/2dies_background.stk > /dev/null
	@./CompareTemperatures pf2rm/steady/background_node1.txt    pf2rm/steady/background_node2.txt    pf2rm/steady/output_background.txt

clean:
	@$(RM) $(RMFLAGS) GenerateSystemMatrix GenerateSystemMatrix.o GenerateSystemMatrix.d
	@$(RM) $(RMFLAGS) CompareSystemMatrix  CompareSystemMatrix.o  CompareSystemMatrix.d
	@$(RM) $(RMFLAGS) CompareTemperatures  CompareTemperatures.o  CompareTemperatures.d
	@$(RM) $(RMFLAGS) tr_topsink.txt tr_bottomsink.txt tr_bothsink.txt
	@$(RM) $(RMFLAGS) st_topsink.txt st_bottomsink.txt st_bothsink.txt
	@$(RM) $(RMFLAGS) tr_solid.txt tr_4rm.txt tr_pf.txt tr_2rm.txt
	@$(RM) $(RMFLAGS) st_solid.txt st_4rm.txt st_pf.txt st_2rm.txt
	@$(RM) $(RMFLAGS) solid/transient/node1_top.txt           solid/transient/node2_top.txt
	@$(RM) $(RMFLAGS) solid/transient/node1_bottom.txt        solid/transient/node2_bottom.txt
	@$(RM) $(RMFLAGS) solid/transient/node1_both.txt          solid/transient/node2_both.txt
	@$(RM) $(RMFLAGS) mc2rm/transient/background_node1.txt    mc2rm/transient/four_elements_node1.txt
	@$(RM) $(RMFLAGS) mc2rm/transient/background_node2.txt    mc2rm/transient/four_elements_node2.txt
	@$(RM) $(RMFLAGS) mc4rm/transient/background_node1.txt    mc4rm/transient/four_elements_node1.txt   
	@$(RM) $(RMFLAGS) mc4rm/transient/background_node2.txt    mc4rm/transient/four_elements_node2.txt
	@$(RM) $(RMFLAGS) pf2rm/transient/background_node1.txt    pf2rm/transient/four_elements_node1.txt
	@$(RM) $(RMFLAGS) pf2rm/transient/background_node2.txt    pf2rm/transient/four_elements_node2.txt
	@$(RM) $(RMFLAGS) solid/steady/node1_top.txt              solid/steady/node2_top.txt
	@$(RM) $(RMFLAGS) solid/steady/node1_bottom.txt           solid/steady/node2_bottom.txt
	@$(RM) $(RMFLAGS) solid/steady/node1_both.txt             solid/steady/node2_both.txt
	@$(RM) $(RMFLAGS) mc4rm/steady/background_node1.txt       mc4rm/steady/four_elements_node1.txt
	@$(RM) $(RMFLAGS) mc4rm/steady/background_node2.txt       mc4rm/steady/four_elements_node2.txt
	@$(RM) $(RMFLAGS) mc2rm/steady/background_node1.txt       mc2rm/steady/four_elements_node1.txt
	@$(RM) $(RMFLAGS) mc2rm/steady/background_node2.txt       mc2rm/steady/four_elements_node2.txt
	@$(RM) $(RMFLAGS) pf2rm/steady/background_node1.txt       pf2rm/steady/four_elements_node1.txt
	@$(RM) $(RMFLAGS) pf2rm/steady/background_node2.txt       pf2rm/steady/four_elements_node2.txt


