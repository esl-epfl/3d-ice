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

CC              = g++
CFLAGS          = -Werror -Wall -Wextra -g -O3

TL_INCLUDE      = -IInclude
TL_LIB          = Lib/libThermalLibrary.a

SLU_INCLUDE     = -I../SuperLU_4.0/SRC/

IML_INCLUDE     = -I../IterativeMethodsLibrary/include/ 

SL_INCLUDE      = -I../SparseLibrary/include/ -I../SparseLibrary/mv/include/

CUBLAS_INCLUDE  = -I/usr/local/cuda/include

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
          Sources/system_matrix.o               \
          Sources/conductances.o                \
          Sources/thermal_data_direct.o         \
          Sources/thermal_data_iterative.o

#
# These rules are not well written! FIX THEM
#

$(TL_LIB): $(OBJECTS)
	@ar rv $@ $?

bicg:
	@rm -f $(TL_LIB)
	@make ITERATIVE_SOLVER="-DTL_BICG_ITERATIVE_SOLVER"
	cd Testing; make ITERATIVE_SOLVER="-DTL_BICG_ITERATIVE_SOLVER"

bicgstab:
	@rm -f $(TL_LIB)
	@make ITERATIVE_SOLVER="-DTL_BICGSTAB_ITERATIVE_SOLVER"
	cd Testing; make ITERATIVE_SOLVER="-DTL_BICGSTAB_ITERATIVE_SOLVER"

cgs:
	@rm -f $(TL_LIB)
	@make ITERATIVE_SOLVER="-DTL_CGS_ITERATIVE_SOLVER"
	cd Testing; make ITERATIVE_SOLVER="-DTL_CGS_ITERATIVE_SOLVER"

gmres:
	@rm -f $(TL_LIB)
	@make ITERATIVE_SOLVER="-DTL_GMRES_ITERATIVE_SOLVER"
	cd Testing; make ITERATIVE_SOLVER="-DTL_GMRES_ITERATIVE_SOLVER"

ir:
	@rm -f $(TL_LIB)
	@make ITERATIVE_SOLVER="-DTL_IR_ITERATIVE_SOLVER"
	cd Testing; make ITERATIVE_SOLVER="-DTL_IR_ITERATIVE_SOLVER"

qmr:
	@rm -f $(TL_LIB)
	@make ITERATIVE_SOLVER="-DTL_QMR_ITERATIVE_SOLVER"
	cd Testing; make ITERATIVE_SOLVER="-DTL_QMR_ITERATIVE_SOLVER"

################################################################################

Sources/dimensions.o: Sources/dimensions.c Include/dimensions.h
	$(CC) $(CFLAGS) $(DEBUG) -o $@ $(TL_INCLUDE) -c $<

################################################################################

Sources/material.o: Sources/material.c Include/material.h
	$(CC) $(CFLAGS) $(DEBUG) -o $@ $(TL_INCLUDE) -c $<

################################################################################

Sources/layer.o: Sources/layer.c Include/layer.h
	$(CC) $(CFLAGS) $(DEBUG) -o $@ $(TL_INCLUDE) -c $<

################################################################################

Sources/floorplan_element.o: Sources/floorplan_element.c \
                             Include/floorplan_element.h
	$(CC) $(CFLAGS) $(DEBUG) -o $@ $(TL_INCLUDE) -c $<

################################################################################

Sources/die.o: Sources/die.c Include/die.h
	$(CC) $(CFLAGS) $(DEBUG) -o $@ $(TL_INCLUDE) -c $<

###############################################################################

Sources/floorplan_parser.o: Sources/floorplan_parser.c
	$(CC) $(CFLAGS) $(DEBUG) -o $@ $(TL_INCLUDE) -c $<

Sources/floorplan_parser.c : Bison/floorplan_parser.y
	bison -d Bison/floorplan_parser.y
	mv Sources/floorplan_parser.h Include/

################################################################################

Sources/floorplan_scanner.o: Sources/floorplan_scanner.c \
                             Sources/floorplan_parser.c
	$(CC) $(DEBUG) -o $@ $(TL_INCLUDE) -c $<

Sources/floorplan_scanner.c : Flex/floorplan_scanner.l
		flex Flex/floorplan_scanner.l

################################################################################

Sources/floorplan.o: Sources/floorplan.c Include/floorplan.h
	$(CC) $(CFLAGS) $(DEBUG) -o $@ $(TL_INCLUDE) -c $<

################################################################################

Sources/channel.o: Sources/channel.c Include/channel.h
	$(CC) $(CFLAGS) $(DEBUG) -o $@ $(TL_INCLUDE) -c $<

################################################################################

Sources/stack_element.o: Sources/stack_element.c Include/stack_element.h
	$(CC) $(CFLAGS) $(DEBUG) -o $@ $(TL_INCLUDE) -c $<

################################################################################

Sources/stack_description_parser.o: Sources/stack_description_parser.c
	$(CC) $(CFLAGS) $(DEBUG) -o $@ $(TL_INCLUDE) -c $<

Sources/stack_description_parser.c : Bison/stack_description_parser.y
	bison -d Bison/stack_description_parser.y
	mv Sources/stack_description_parser.h Include/

################################################################################

Sources/stack_description_scanner.o: Sources/stack_description_scanner.c \
                                     Sources/stack_description_parser.c
	$(CC) $(DEBUG) -o $@ $(TL_INCLUDE) -c $<

Sources/stack_description_scanner.c : Flex/stack_description_scanner.l
	flex Flex/stack_description_scanner.l

################################################################################

Sources/stack_description.o: Sources/stack_description.c \
                             Include/stack_description.h
	$(CC) $(CFLAGS) $(DEBUG) -o $@ $(TL_INCLUDE) -c $<

################################################################################

Sources/system_matrix.o: Sources/system_matrix.c Include/system_matrix.h
	$(CC) $(CFLAGS) $(DEBUG) -o $@ $(TL_INCLUDE) -c $<

################################################################################

Sources/conductances.o: Sources/conductances.c Include/conductances.h
	$(CC) $(CFLAGS) $(DEBUG) -o $@ $(TL_INCLUDE) -c $<

################################################################################

Sources/thermal_data_direct.o: Sources/thermal_data_direct.c Include/thermal_data_direct.h
	$(CC) $(CFLAGS) $(DEBUG) -o $@ $(TL_INCLUDE) $(SLU_INCLUDE) $(SL_INCLUDE) -c $<

################################################################################

Sources/thermal_data_iterative.o: Sources/thermal_data_iterative.c Include/thermal_data_iterative.h
	$(CC) $(CFLAGS) $(DEBUG) $(ITERATIVE_SOLVER) -DSUPPORT_CUBLAS -o $@ $(TL_INCLUDE) $(IML_INCLUDE) $(SL_INCLUDE) $(CUBLAS_INCLUDE) -c $<

################################################################################

clean:
	@rm -vf $(OBJECTS)
	@rm -vf $(TL_LIB)
	@rm -vf Include/floorplan_scanner.h
	@rm -vf Include/floorplan_parser.h
	@rm -vf Include/stack_description_scanner.h
	@rm -vf Include/stack_description_parser.h
	@rm -vf Sources/floorplan_scanner.c
	@rm -vf Sources/floorplan_parser.c
	@rm -vf Sources/stack_description_scanner.c
	@rm -vf Sources/stack_description_parser.c
	cd Testing; make clean;
