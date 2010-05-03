#
# Makefile
#

CC             = gcc
CFLAGS         = -Werror -Wall -Wextra
INCLUDE        = -IInclude
LIB            = Lib/libTermalLibrary.a

OBJECTS = Sources/material.o

all: $(LIB)

################################################################################

$(LIB):	$(OBJECTS)
	rm -f $@
	ar cr $@ $(OBJECTS)

################################################################################

Sources/material.o: Sources/material.c Include/material.h
	$(CC) $(CFLAGS) -o $@ $(INCLUDE) -c $<

################################################################################

clean:
	rm -f $(OBJECTS)
	rm -f $(LIB)
