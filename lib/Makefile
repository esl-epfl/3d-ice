###############################################################################
#                                                                             #
# lib/Makefile                                                                #
#                                                                             #
# EPFL-STI-IEL-ESL                                                            #
# Bâtiment ELG, ELG 130                                                       #
# Station 11                                                                  #
# 1015 Lausanne, Switzerland                    alessandro.vincenzi@epfl.ch   #
###############################################################################

TL_MAIN = ..

include $(TL_MAIN)/makefile.def

all: $(TL_ARCHIVE)

$(TL_ARCHIVE): $(TL_LIB_OBJECTS)
	@$(AR) $(ARFLAGS) $@ $?

-include $(TL_LIB_OBJECTS:.o=.d)

clean:
	@$(RM) $(RMFLAGS) $(TL_ARCHIVE)

