###############################################################################
#                                                                             #
# Sources/Makefile                                                            #
#                                                                             #
# EPFL-STI-IEL-ESL                                                            #
# Bâtiment ELG, ELG 130                                                       #
# Station 11                                                                  #
# 1015 Lausanne, Switzerland                    alessandro.vincenzi@epfl.ch   #
###############################################################################

TL_MAIN = ..

include $(TL_MAIN)/makefile.def

all: $(TL_SOURCES)/floorplan_parser.c         \
     $(TL_SOURCES)/stack_description_parser.c

$(TL_SOURCES)/floorplan_parser.c: floorplan_parser.y
	$(YACC) $(YFLAGS) $<
	@$(MV) $(MVFLAGS) $(TL_SOURCES)/floorplan_parser.h $(TL_INCLUDE)

$(TL_SOURCES)/stack_description_parser.c: stack_description_parser.y
	$(YACC) $(YFLAGS) $<
	@$(MV) $(MVFLAGS) $(TL_SOURCES)/stack_description_parser.h $(TL_INCLUDE)

clean:
	@$(RM) $(RMFLAGS) $(TL_INCLUDE)/floorplan_parser.h
	@$(RM) $(RMFLAGS) $(TL_INCLUDE)/stack_description_parser.h
	@$(RM) $(RMFLAGS) $(TL_SOURCES)/floorplan_parser.c
	@$(RM) $(RMFLAGS) $(TL_SOURCES)/stack_description_parser.c
