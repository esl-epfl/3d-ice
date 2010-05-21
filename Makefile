#
# Makefile
#
#
# "make DEBUG=-g"
# "make DEBUG=-DDEBUG_FILL_CONDUCTANCES"
# "make DEBUG=-DDEBUG_FILL_CAPACITIES"
# "make DEBUG=-DDEBUG_FILL_SOURCES"
# "make DEBUG=-DDEBUG_FILL_SYSTEM_MATRIX"
#

CC              = gcc
CFLAGS          = -Werror -Wall -Wextra
INCLUDE         = -IInclude
LIB             = Lib/libTermalLibrary.a
SUPERLU_INCLUDE = ../SuperLU_4.0/SRC/


OBJECTS = Sources/dimensions.o                  \
          Sources/material.o                    \
          Sources/layer.o                       \
          Sources/floorplan_element.o           \
          Sources/die.o                         \
          Sources/floorplan_scanner.o           \
          Sources/floorplan_parser.o            \
          Sources/floorplan.o                   \
          Sources/channel.o                     \
          Sources/stack_element.o               \
          Sources/stack_description_scanner.o   \
          Sources/stack_description_parser.o    \
          Sources/stack_description.o           \
          Sources/system_vector.o               \
          Sources/system_matrix.o               \
          Sources/conductances.o                \
          Sources/slu_thermal_data.o

all: $(LIB)

################################################################################

$(LIB):	$(OBJECTS)
	rm -f $@
	ar cr $@ $(OBJECTS)

################################################################################

Sources/dimensions.o: Sources/dimensions.c Include/dimensions.h
	$(CC) $(CFLAGS) $(DEBUG) -o $@ $(INCLUDE) -c $<

################################################################################

Sources/material.o: Sources/material.c Include/material.h
	$(CC) $(CFLAGS) $(DEBUG) -o $@ $(INCLUDE) -c $<

################################################################################

Sources/layer.o: Sources/layer.c Include/layer.h
	$(CC) $(CFLAGS) $(DEBUG) -o $@ $(INCLUDE) -c $<

################################################################################

Sources/floorplan_element.o: Sources/floorplan_element.c \
                             Include/floorplan_element.h
	$(CC) $(CFLAGS) $(DEBUG) -o $@ $(INCLUDE) -c $<

################################################################################

Sources/die.o: Sources/die.c Include/die.h
	$(CC) $(CFLAGS) $(DEBUG) -o $@ $(INCLUDE) -c $<

###############################################################################

Sources/floorplan_parser.o: Sources/floorplan_parser.c
	$(CC) $(CFLAGS) $(DEBUG) -o $@ $(INCLUDE) -c $<

Sources/floorplan_parser.c : Bison/floorplan_parser.y
	bison -d Bison/floorplan_parser.y
	mv Sources/floorplan_parser.h Include/

################################################################################

Sources/floorplan_scanner.o: Sources/floorplan_scanner.c \
                             Sources/floorplan_parser.c
	$(CC) $(DEBUG) -o $@ $(INCLUDE) -c $<

Sources/floorplan_scanner.c : Flex/floorplan_scanner.l
		flex Flex/floorplan_scanner.l

################################################################################

Sources/floorplan.o: Sources/floorplan.c Include/floorplan.h
	$(CC) $(CFLAGS) $(DEBUG) -o $@ $(INCLUDE) -c $<

################################################################################

Sources/channel.o: Sources/channel.c Include/channel.h
	$(CC) $(CFLAGS) $(DEBUG) -o $@ $(INCLUDE) -c $<

################################################################################

Sources/stack_element.o: Sources/stack_element.c Include/stack_element.h
	$(CC) $(CFLAGS) $(DEBUG) -o $@ $(INCLUDE) -c $<

################################################################################

Sources/stack_description_parser.o: Sources/stack_description_parser.c
	$(CC) $(CFLAGS) $(DEBUG) -o $@ $(INCLUDE) -c $<

Sources/stack_description_parser.c : Bison/stack_description_parser.y
	bison -d Bison/stack_description_parser.y
	mv Sources/stack_description_parser.h Include/

################################################################################

Sources/stack_description_scanner.o: Sources/stack_description_scanner.c \
                                     Sources/stack_description_parser.c
	$(CC) $(DEBUG) -o $@ $(INCLUDE) -c $<

Sources/stack_description_scanner.c : Flex/stack_description_scanner.l
	flex Flex/stack_description_scanner.l

################################################################################

Sources/stack_description.o: Sources/stack_description.c \
                             Include/stack_description.h
	$(CC) $(CFLAGS) $(DEBUG) -o $@ $(INCLUDE) -c $<

################################################################################

Sources/system_vector.o: Sources/system_vector.c Include/system_vector.h
	$(CC) $(CFLAGS) $(DEBUG) -o $@ $(INCLUDE) -c $<

################################################################################

Sources/system_matrix.o: Sources/system_matrix.c Include/system_matrix.h
	$(CC) $(CFLAGS) $(DEBUG) -o $@ $(INCLUDE) -c $<

################################################################################

Sources/conductances.o: Sources/conductances.c Include/conductances.h
	$(CC) $(CFLAGS) $(DEBUG) -o $@ $(INCLUDE) -c $<

################################################################################

Sources/slu_thermal_data.o: Sources/slu_thermal_data.c Include/slu_thermal_data.h
	$(CC) $(CFLAGS) $(DEBUG) -I$(SUPERLU_INCLUDE) -o $@ $(INCLUDE) -c $<

################################################################################

clean:
	rm -f $(OBJECTS)
	rm -f $(LIB)
	rm -f Include/floorplan_scanner.h
	rm -f Include/floorplan_parser.h
	rm -f Include/stack_description_scanner.h
	rm -f Include/stack_description_parser.h
	rm -f Sources/floorplan_scanner.c
	rm -f Sources/floorplan_parser.c
	rm -f Sources/stack_description_scanner.c
	rm -f Sources/stack_description_parser.c

