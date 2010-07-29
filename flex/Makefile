###############################################################################
#                                                                             #
# flex/Makefile                                                               #
#                                                                             #
# EPFL-STI-IEL-ESL                                                            #
# Bâtiment ELG, ELG 130                                                       #
# Station 11                                                                  #
# 1015 Lausanne, Switzerland                    alessandro.vincenzi@epfl.ch   #
###############################################################################

TL_MAIN = ..

include $(TL_MAIN)/makefile.def

all: $(TL_FLEX_OBJECTS:.o=.c)

-include $(TL_FLEX_OBJECTS:.o=.d)

clean:
	@$(RM) $(RMFLAGS) $(TL_FLEX_OBJECTS)
	@$(RM) $(RMFLAGS) $(TL_FLEX_OBJECTS:.o=.c)
	@$(RM) $(RMFLAGS) $(TL_FLEX_OBJECTS:.o=.h)
	@$(RM) $(RMFLAGS) $(TL_FLEX_OBJECTS:.o=.d)

