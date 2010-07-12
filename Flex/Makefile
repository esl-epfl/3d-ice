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

all: $(TL_SOURCES)/floorplan_scanner.c         \
     $(TL_SOURCES)/stack_description_scanner.c

$(TL_SOURCES)/floorplan_scanner.c: floorplan_scanner.l
	$(LEX) $(LFLAGS) $^
	@$(MV) $(MVFLAGS) floorplan_scanner.h $(TL_INCLUDE)
	@$(MV) $(MVFLAGS) floorplan_scanner.c $(TL_SOURCES)

$(TL_SOURCES)/stack_description_scanner.c: stack_description_scanner.l
	$(LEX) $(LFLAGS) $^
	@$(MV) $(MVFLAGS) stack_description_scanner.h $(TL_INCLUDE)
	@$(MV) $(MVFLAGS) stack_description_scanner.c $(TL_SOURCES)

clean:
	@$(RM) $(RMFLAGS) $(TL_INCLUDE)/floorplan_scanner.h
	@$(RM) $(RMFLAGS) $(TL_INCLUDE)/stack_description_scanner.h
	@$(RM) $(RMFLAGS) $(TL_SOURCES)/floorplan_scanner.c
	@$(RM) $(RMFLAGS) $(TL_SOURCES)/stack_description_scanner.c

