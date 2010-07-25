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

TL_SOURCES = channel.c                   \
             conductances.c              \
             die.c                       \
             dimensions.c                \
             floorplan.c                 \
             floorplan_element.c         \
             layer.c                     \
             material.c                  \
             stack_description.c         \
             heatsink.c                  \
             stack_element.c             \
             system_matrix.c             \
             system_vector.c             \
             slu_thermal_data.c          \
             stack_description_scanner.c \
             stack_description_parser.c  \
             floorplan_scanner.c         \
             floorplan_parser.c

TL_INCLUDES = -I $(TL_INCLUDE) -I $(SLU_INCLUDE)

all: $(TL_ARCHIVE)

$(TL_ARCHIVE): $(TL_SOURCES:.c=.o)
	@$(AR) $(ARFLAGS) $@ $?

%.o : %.c
	$(CC) $(CFLAGS) $(TL_INCLUDES) -c $< -o $@
	@cp $*.d $*.dd
	@sed -e 's/#.*//' -e 's/^[^:]*: *//' -e 's/ *\\$$//' \
	     -e '/^$$/ d' -e 's/$$/ :/' < $*.dd >> $*.d
	@rm -f $*.dd

-include $(TL_SOURCES:.c=.d)

clean:
	@$(RM) $(RMFLAGS) $(TL_SOURCES:.c=.o)
	@$(RM) $(RMFLAGS) $(TL_SOURCES:.c=.d)
	@$(RM) $(RMFLAGS) $(TL_ARCHIVE)
