###############################################################################
#                                                                             #
# sources/Makefile                                                            #
#                                                                             #
# EPFL-STI-IEL-ESL                                                            #
# Bâtiment ELG, ELG 130                                                       #
# Station 11                                                                  #
# 1015 Lausanne, Switzerland                    alessandro.vincenzi@epfl.ch   #
###############################################################################

TL_MAIN = ..

include $(TL_MAIN)/makefile.def

CINCLUDES := $(CINCLUDES) -I $(SLU_INCLUDE)

CFLAGS := $(CFLAGS) -Wall -Wextra -Werror
#CFLAGS := $(CFLAGS) -DENABLE_ASSERT
#CFLAGS := $(CFLAGS) -DPRINT_CONDUCTANCES
#CFLAGS := $(CFLAGS) -DPRINT_CAPACITIES
#CFLAGS := $(CFLAGS) -DPRINT_SOURCES
#CFLAGS := $(CFLAGS) -DPRINT_SYSTEM_MATRIX

all: $(TL_SOURCE_OBJECTS)

-include $(TL_SOURCE_OBJECTS:.o=.d)

clean:
	@$(RM) $(RMFLAGS) $(TL_SOURCE_OBJECTS)
	@$(RM) $(RMFLAGS) $(TL_SOURCE_OBJECTS:.o=.d)

