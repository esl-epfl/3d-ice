###############################################################################
#                                                                             #
# sources/Makefile                                                            #
#                                                                             #
# EPFL-STI-IEL-ESL                                                            #
# Bâtiment ELG, ELG 130                                                       #
# Station 11                                                                  #
# 1015 Lausanne, Switzerland                    alessandro.vincenzi@epfl.ch   #
###############################################################################

HS2TDICE_MAIN = ..

include $(HS2TDICE_MAIN)/makefile.def

CFLAGS    := $(CFLAGS)   -Wall -Wextra -Werror
CINCLUDES := $(CINCLUDES) -I $(TDICE_INCLUDE)

all: $(HS2TDICE_SOURCES_OBJECTS)

-include $(HS2TDICE_SOURCE_OBJECTS:.o=.d)

clean:
	@$(RM) $(RMFLAGS) $(HS2TDICE_SOURCES_OBJECTS)
	@$(RM) $(RMFLAGS) $(HS2TDICE_SOURCES_OBJECTS:.o=.d)

