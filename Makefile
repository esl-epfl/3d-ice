#
# Makefile
#

CC             = gcc
CFLAGS         = -Werror -Wall -Wextra
INCLUDE        = -IInclude
LIB            = Lib/libTermalLibrary.a

OBJECTS = Sources/dimensions.o          \
          Sources/material.o            \
          Sources/layer.o               \
          Sources/floorplan_element.o   \
          Sources/die.o                 \
          Sources/floorplan_scanner.o   \
          Sources/floorplan_parser.o    \
          Sources/floorplan.o           \
          Sources/channel.o             \
          Sources/stack_element.o

all: $(LIB)

################################################################################

$(LIB):	$(OBJECTS)
	rm -f $@
	ar cr $@ $(OBJECTS)

################################################################################

Sources/dimensions.o: Sources/dimensions.c Include/dimensions.h
	$(CC) $(CFLAGS) -o $@ $(INCLUDE) -c $<

################################################################################

Sources/material.o: Sources/material.c Include/material.h
	$(CC) $(CFLAGS) -o $@ $(INCLUDE) -c $<

################################################################################

Sources/layer.o: Sources/layer.c Include/layer.h
	$(CC) $(CFLAGS) -o $@ $(INCLUDE) -c $<

################################################################################

Sources/floorplan_element.o: Sources/floorplan_element.c \
                             Include/floorplan_element.h
	$(CC) $(CFLAGS) -o $@ $(INCLUDE) -c $<

################################################################################

Sources/die.o: Sources/die.c Include/die.h
	$(CC) $(CFLAGS) -o $@ $(INCLUDE) -c $<

###############################################################################

Sources/floorplan_parser.o: Sources/floorplan_parser.c
	$(CC) $(PARSER_CFLAGS) -o $@ $(INCLUDE) -c $<

Sources/floorplan_parser.c : Bison/floorplan_parser.y
	bison -d Bison/floorplan_parser.y
	mv Sources/floorplan_parser.h Include/

################################################################################

Sources/floorplan_scanner.o: Sources/floorplan_scanner.c \
                             Sources/floorplan_parser.c
	$(CC) $(SCANNER_CFLAGS) -o $@ $(INCLUDE) -c $<

Sources/floorplan_scanner.c : Flex/floorplan_scanner.l
		flex Flex/floorplan_scanner.l

################################################################################

Sources/floorplan.o: Sources/floorplan.c Include/floorplan.h
	$(CC) $(CFLAGS) -o $@ $(INCLUDE) -c $<

################################################################################

Sources/channel.o: Sources/channel.c Include/channel.h
	$(CC) $(CFLAGS) -o $@ $(INCLUDE) -c $<

################################################################################

Sources/stack_element.o: Sources/stack_element.c Include/stack_element.h
	$(CC) $(CFLAGS) -o $@ $(INCLUDE) -c $<

################################################################################

clean:
	rm -f $(OBJECTS)
	rm -f $(LIB)
	rm -f Sources/floorplan_scanner.c
	rm -f Include/floorplan_scanner.h
	rm -f Sources/floorplan_parser.c
	rm -f Include/floorplan_parser.h

