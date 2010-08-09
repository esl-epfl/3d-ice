###############################################################################
#                                                                             #
# bin/Makefile                                                                #
#                                                                             #
# EPFL-STI-IEL-ESL                                                            #
# Bâtiment ELG, ELG 130                                                       #
# Station 11                                                                  #
# 1015 Lausanne, Switzerland                    alessandro.vincenzi@epfl.ch   #
###############################################################################

HS2TDICE_MAIN = ..

include $(HS2TDICE_MAIN)/makefile.def

CINCLUDES := $(CINCLUDES) -I $(TDICE_INCLUDE)
CLIBS     := $(CLIBS)     $(TDICE_LIB_ARCHIVE)
	
all: $(HS2TDICE_EXE)

$(HS2TDICE_EXE): $(HS2TDICE_EXE_OBJECTS) $(TDICE_LIB_ARCHIVE)
	$(CC) $(CFLAGS) $(CINCLUDES) $^ $(CLIBS) -o $@

-include $(HS2TDICE_EXE_OBJECTS:.o=.d)

clean:
	@$(RM) $(RMFLAGS) $(HS2TDICE_EXE)

