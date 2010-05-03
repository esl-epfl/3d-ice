#
# Makefile
#

CC             = gcc
CFLAGS         = -Werror -Wall -Wextra
INCLUDE        = -IInclude
LIB            = Lib/libTermalLibrary.a

OBJECTS = Sources/dimensions.o          \
          Sources/material.o            \
          Sources/layer.o

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

clean:
	rm -f $(OBJECTS)
	rm -f $(LIB)
