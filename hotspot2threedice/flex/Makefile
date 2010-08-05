###############################################################################
#                                                                             #
# flex/Makefile                                                               #
#                                                                             #
# EPFL-STI-IEL-ESL                                                            #
# Bâtiment ELG, ELG 130                                                       #
# Station 11                                                                  #
# 1015 Lausanne, Switzerland                    alessandro.vincenzi@epfl.ch   #
###############################################################################

HS2TDICE_MAIN = ..

include $(HS2TDICE_MAIN)/makefile.def

all: $(HS2TDICE_FLEX_OBJECTS:.o=.c)

-include $(HS2TDICE_FLEX_OBJECTS:.o=.d)

clean:
	@$(RM) $(RMFLAGS) $(HS2TDICE_FLEX_OBJECTS)
	@$(RM) $(RMFLAGS) $(HS2TDICE_FLEX_OBJECTS:.o=.c)
	@$(RM) $(RMFLAGS) $(HS2TDICE_FLEX_OBJECTS:.o=.h)
	@$(RM) $(RMFLAGS) $(HS2TDICE_FLEX_OBJECTS:.o=.d)

