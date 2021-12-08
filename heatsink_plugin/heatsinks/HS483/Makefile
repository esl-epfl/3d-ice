 ##############################################################################
 # This file is part of 3D-ICE, version 3.1.0 .                               #
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
 #                             Copyright (C) 2021                             #
 #   Embedded Systems Laboratory - Ecole Polytechnique Federale de Lausanne   #
 #                            All Rights Reserved.                            #
 #                                                                            #
 # Authors: Arvind Sridhar              Alessandro Vincenzi                   #
 #          Giseong Bak                 Martino Ruggiero                      #
 #          Thomas Brunschwiler         Eder Zulian                           #
 #          Federico Terraneo           Darong Huang                          #
 #          Luis Costero                Marina Zapater                        #
 #          David Atienza                                                     #
 #                                                                            #
 # For any comment, suggestion or request  about 3D-ICE, please  register and #
 # write to the mailing list (see http://listes.epfl.ch/doc.cgi?liste=3d-ice) #
 # Any usage  of 3D-ICE  for research,  commercial or other  purposes must be #
 # properly acknowledged in the resulting products or publications.           #
 #                                                                            #
 # EPFL-STI-IEL-ESL                     Mail : 3d-ice@listes.epfl.ch          #
 # Batiment ELG, ELG 130                       (SUBSCRIPTION IS NECESSARY)    #
 # Station 11                                                                 #
 # 1015 Lausanne, Switzerland           Url  : http://esl.epfl.ch/3d-ice      #
 ##############################################################################

SRC = ../../common/libraries/ThermalBlocks.mo ../../common/HeatsinkBlocks.mo HS483.mo
OBJ = HS483.HS483_P14752_ConstantFanSpeed_Interface3DICE \
      HS483.HS483_P14752_VariableFanSpeed_Interface3DICE
DUMMY = .dummy

# OpenModelica produces temporary files with a name where . is replaced with _
DOT = .
UNDERSCORE = _
OBJ_UNDERSCORE = $(subst $(DOT),$(UNDERSCORE),$(OBJ))

all: $(DUMMY)

# Making an FMI from OpenModelica requires to use its own
# build system scripting language
# This rule makes all OBJs in one go, so we cant't say $(OBJ) : $(SRC)
# or make will repeat the rule multiple times. That's why we use a dummy
$(DUMMY): $(SRC)
	omc buildfmi.mos
	rm -rf $(OBJ)
	$(foreach O,$(OBJ),unzip -q $(O).fmu -d $(O);)
	$(foreach O,$(OBJ),rm -f $(O).fmu;)
	$(foreach O,$(OBJ_UNDERSCORE),rm -f $(O)_FMU.* $(O).log;)
	touch $(DUMMY)

clean:
	rm -rf $(OBJ) $(DUMMY)
