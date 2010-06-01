#
# Makefile
#
#
# "make DEBUG=-g"
# "make DEBUG=-DDEBUG_FILL_CONDUCTANCES"
# "make DEBUG=-DDEBUG_FILL_CAPACITIES"
# "make DEBUG=-DDEBUG_FILL_SOURCES"
# "make DEBUG=-DDEBUG_FILL_SYSTEM_MATRIX"
# "make DEBUG=-DDEBUG_FILL_SYSTEM_VECTOR"
#
# make PROFILE=-pg
#

CC              = g++
CFLAGS          = -Werror -Wall -Wextra
INCLUDE         = -IInclude
LIB             = Lib/libTermalLibrary.a
SLU_INCLUDE     = -I../SuperLU_4.0/SRC/
BICG_INCLUDE    = -I../IterativeMethodsLibrary/include/ 
BICG_OPT        = -DCOMPLEX='std::complex<double>'
SL_INCLUDE      = -I../SparseLibrary/include/ \
                  -I../SparseLibrary/mv/include/

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
          Sources/thermal_data_slu.o            \
          Sources/thermal_data_bicg.o           \
          Sources/thermal_data_bicgstab.o       \
          Sources/thermal_data_cgs.o            \
          Sources/thermal_data_gmres.o          \
          Sources/thermal_data_ir.o             \
          Sources/thermal_data_qmr.o

all: $(LIB)

################################################################################

$(LIB):	$(OBJECTS)
	rm -f $@
	ar cr $@ $(OBJECTS)

################################################################################

Sources/dimensions.o: Sources/dimensions.c Include/dimensions.h
	$(CC) $(CFLAGS) $(DEBUG) -o $@ $(INCLUDE) -c $< $(PROFILE)

################################################################################

Sources/material.o: Sources/material.c Include/material.h
	$(CC) $(CFLAGS) $(DEBUG) -o $@ $(INCLUDE) -c $< $(PROFILE)

################################################################################

Sources/layer.o: Sources/layer.c Include/layer.h
	$(CC) $(CFLAGS) $(DEBUG) -o $@ $(INCLUDE) -c $< $(PROFILE)

################################################################################

Sources/floorplan_element.o: Sources/floorplan_element.c \
                             Include/floorplan_element.h
	$(CC) $(CFLAGS) $(DEBUG) -o $@ $(INCLUDE) -c $< $(PROFILE)

################################################################################

Sources/die.o: Sources/die.c Include/die.h
	$(CC) $(CFLAGS) $(DEBUG) -o $@ $(INCLUDE) -c $< $(PROFILE)

###############################################################################

Sources/floorplan_parser.o: Sources/floorplan_parser.c
	$(CC) $(CFLAGS) $(DEBUG) -o $@ $(INCLUDE) -c $< $(PROFILE)

Sources/floorplan_parser.c : Bison/floorplan_parser.y
	bison -d Bison/floorplan_parser.y
	mv Sources/floorplan_parser.h Include/

################################################################################

Sources/floorplan_scanner.o: Sources/floorplan_scanner.c \
                             Sources/floorplan_parser.c
	$(CC) $(DEBUG) -o $@ $(INCLUDE) -c $< $(PROFILE)

Sources/floorplan_scanner.c : Flex/floorplan_scanner.l
		flex Flex/floorplan_scanner.l

################################################################################

Sources/floorplan.o: Sources/floorplan.c Include/floorplan.h
	$(CC) $(CFLAGS) $(DEBUG) -o $@ $(INCLUDE) -c $< $(PROFILE)

################################################################################

Sources/channel.o: Sources/channel.c Include/channel.h
	$(CC) $(CFLAGS) $(DEBUG) -o $@ $(INCLUDE) -c $< $(PROFILE)

################################################################################

Sources/stack_element.o: Sources/stack_element.c Include/stack_element.h
	$(CC) $(CFLAGS) $(DEBUG) -o $@ $(INCLUDE) -c $< $(PROFILE)

################################################################################

Sources/stack_description_parser.o: Sources/stack_description_parser.c
	$(CC) $(CFLAGS) $(DEBUG) -o $@ $(INCLUDE) -c $< $(PROFILE)

Sources/stack_description_parser.c : Bison/stack_description_parser.y
	bison -d Bison/stack_description_parser.y
	mv Sources/stack_description_parser.h Include/

################################################################################

Sources/stack_description_scanner.o: Sources/stack_description_scanner.c \
                                     Sources/stack_description_parser.c
	$(CC) $(DEBUG) -o $@ $(INCLUDE) -c $< $(PROFILE)

Sources/stack_description_scanner.c : Flex/stack_description_scanner.l
	flex Flex/stack_description_scanner.l

################################################################################

Sources/stack_description.o: Sources/stack_description.c \
                             Include/stack_description.h
	$(CC) $(CFLAGS) $(DEBUG) -o $@ $(INCLUDE) -c $< $(PROFILE)

################################################################################

Sources/system_vector.o: Sources/system_vector.c Include/system_vector.h
	$(CC) $(CFLAGS) $(DEBUG) -o $@ $(INCLUDE) -c $< $(PROFILE)

################################################################################

Sources/system_matrix.o: Sources/system_matrix.c Include/system_matrix.h
	$(CC) $(CFLAGS) $(DEBUG) -o $@ $(INCLUDE) -c $< $(PROFILE)

################################################################################

Sources/conductances.o: Sources/conductances.c Include/conductances.h
	$(CC) $(CFLAGS) $(DEBUG) -o $@ $(INCLUDE) -c $< $(PROFILE)

################################################################################

Sources/thermal_data_slu.o: Sources/thermal_data_slu.c Include/thermal_data_slu.h
	$(CC) $(CFLAGS) $(DEBUG) -o $@ $(INCLUDE) $(SLU_INCLUDE) -c $< $(PROFILE)

################################################################################

Sources/thermal_data_bicg.o: Sources/thermal_data_bicg.c Include/thermal_data_bicg.h
	$(CC) $(CFLAGS) $(DEBUG) -o $@ $(INCLUDE) $(BICG_INCLUDE) $(BICG_OPT) $(SL_INCLUDE) -c $< $(PROFILE)

################################################################################

Sources/thermal_data_bicgstab.o: Sources/thermal_data_bicgstab.c Include/thermal_data_bicgstab.h
	$(CC) $(CFLAGS) $(DEBUG) -o $@ $(INCLUDE) $(BICG_INCLUDE) $(BICG_OPT) $(SL_INCLUDE) -c $< $(PROFILE)

################################################################################

Sources/thermal_data_cgs.o: Sources/thermal_data_cgs.c Include/thermal_data_cgs.h
	$(CC) $(CFLAGS) $(DEBUG) -o $@ $(INCLUDE) $(BICG_INCLUDE) $(BICG_OPT) $(SL_INCLUDE) -c $< $(PROFILE)

################################################################################

Sources/thermal_data_gmres.o: Sources/thermal_data_gmres.c Include/thermal_data_gmres.h
	$(CC) $(CFLAGS) $(DEBUG) -o $@ $(INCLUDE) $(BICG_INCLUDE) $(BICG_OPT) $(SL_INCLUDE) -c $< $(PROFILE)

################################################################################

Sources/thermal_data_ir.o: Sources/thermal_data_ir.c Include/thermal_data_ir.h
	$(CC) $(CFLAGS) $(DEBUG) -o $@ $(INCLUDE) $(BICG_INCLUDE) $(BICG_OPT) $(SL_INCLUDE) -c $< $(PROFILE)

################################################################################

Sources/thermal_data_qmr.o: Sources/thermal_data_qmr.c Include/thermal_data_qmr.h
	$(CC) $(CFLAGS) $(DEBUG) -o $@ $(INCLUDE) $(BICG_INCLUDE) $(BICG_OPT) $(SL_INCLUDE) -c $< $(PROFILE)

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
