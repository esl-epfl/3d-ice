 ##############################################################################
 # Makefile "3D-ICE/Makefile"                                                 #
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

3DICE_MAIN = .

include $(3DICE_MAIN)/makefile.def

.PHONY: all lib examples clean archive

all: lib examples

lib:
	cd $(3DICE_FLEX)    ; make ;
	cd $(3DICE_BISON)   ; make ;
	cd $(3DICE_SOURCES) ; make ;
	cd $(3DICE_LIB)     ; make ;

examples: lib
	cd $(3DICE_EXAMPLES) ; make ;

archive:
	@cd .. ; tar cpzfv $(3DICE_LIBNAME)-$(3DICE_VERSION).tar.gz    \
	$(subst ./,$(3DICE_LIBNAME)-$(3DICE_VERSION)/,$(3DICE_BISON))/Makefile \
	$(subst ./,$(3DICE_LIBNAME)-$(3DICE_VERSION)/,$(3DICE_BISON_Y))        \
	$(3DICE_LIBNAME)-$(3DICE_VERSION)/CHANGELOG                            \
	$(3DICE_LIBNAME)-$(3DICE_VERSION)/COPYING                              \
	$(3DICE_LIBNAME)-$(3DICE_VERSION)/doc/3D-ICE-User_Guide.pdf            \
	$(3DICE_LIBNAME)-$(3DICE_VERSION)/doc/3D-ICE-ICCAD2010.pdf             \
	$(3DICE_LIBNAME)-$(3DICE_VERSION)/examples/2dies1channel.stk           \
	$(3DICE_LIBNAME)-$(3DICE_VERSION)/examples/Emulate3DICe.c              \
	$(3DICE_LIBNAME)-$(3DICE_VERSION)/examples/FillThermalData.c           \
	$(3DICE_LIBNAME)-$(3DICE_VERSION)/examples/four_elements.flp           \
	$(3DICE_LIBNAME)-$(3DICE_VERSION)/examples/Makefile                    \
	$(subst ./,$(3DICE_LIBNAME)-$(3DICE_VERSION)/,$(3DICE_FLEX))/Makefile  \
	$(subst ./,$(3DICE_LIBNAME)-$(3DICE_VERSION)/,$(3DICE_FLEX_L))         \
	$(subst ./,$(3DICE_LIBNAME)-$(3DICE_VERSION)/,$(3DICE_INCLUDE_H))      \
	$(subst ./,$(3DICE_LIBNAME)-$(3DICE_VERSION)/,$(3DICE_LIB))/Makefile   \
	$(3DICE_LIBNAME)-$(3DICE_VERSION)/Makefile                             \
	$(3DICE_LIBNAME)-$(3DICE_VERSION)/makefile.def                         \
	$(3DICE_LIBNAME)-$(3DICE_VERSION)/README                               \
	$(subst ./,$(3DICE_LIBNAME)-$(3DICE_VERSION)/,$(3DICE_SOURCES))/Makefile  \
	$(subst ./,$(3DICE_LIBNAME)-$(3DICE_VERSION)/,$(3DICE_SOURCES_C))	

clean:
	cd $(3DICE_FLEX)       ; make clean ;
	cd $(3DICE_BISON)      ; make clean ;
	cd $(3DICE_SOURCES)    ; make clean ;
	cd $(3DICE_LIB)        ; make clean ;
	cd $(3DICE_EXAMPLES)   ; make clean ;

