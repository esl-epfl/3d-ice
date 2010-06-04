
CPP                = g++
CPPFLAGS           = -Wall -Wextra -pedantic -g -O3

#
# Thermal Library
#

TL_DIR             = /home/vincenzi/ThermalLibrary
TL_INCLUDE         = -I$(TL_DIR)/Include
TL_LIB_DIR         = -L$(TL_DIR)/Lib
TL_LIB             = -lThermalLibrary

#
# SuperLu Library
#

SLU_DIR            = /home/vincenzi/SuperLU_4.0
SLU_INCLUDE        = -I$(SLU_DIR)/SRC
SLU_LIB_DIR        = -L$(SLU_DIR)/lib -L/usr/lib64
SLU_LIBS           = -lsuperlu_4.0 -lblas -lm

#
# Iterative Methods Library
#

IML_DIR            = /home/vincenzi/IterativeMethodsLibrary
IML_INCLUDE        = -I$(IML_DIR)/include

#
# Sparse Library
#

SL_DIR             = /home/vincenzi/SparseLibrary
SL_INCLUDE         = -I$(SL_DIR)/include
SL_LIB_DIR         = -L$(SL_DIR)/lib
SL_LIBS            = -lmv -lsparse -lspblas -lm

#
# Direct compiling parameters
#

DIRECT_INCLUDE     = $(TL_INCLUDE) $(SLU_INCLUDE)
DIRECT_LIB_DIRS    = $(TL_LIB_DIR) $(SLU_LIB_DIR)
DIRECT_LIBS        = $(TL_LIB)     $(SLU_LIBS) 
DIRECT_EXECS       = direct_small_step direct_big_step \
                     direct_small_step_print_temps direct_big_step_print_temps

#
# Iterative compiling parameters
#

ITERATIVE_INCLUDE  = $(TL_INCLUDE) $(IML_INCLUDE)
ITERATIVE_LIB_DIRS = $(TL_LIB_DIR) $(SL_LIB_DIR)
ITERATIVE_LIBS     = $(TL_LIB)     $(SL_LIBS)
ITERATIVE_EXECS    = iterative_small_step iterative_big_step \
                     iterative_small_step_print_temps iterative_big_step_print_temps


EXECS = $(DIRECT_EXECS) $(ITERATIVE_EXECS)

all : $(EXECS)

################################################################################
## Direct ######################################################################
################################################################################

direct_small_step: direct_small_step.o
	$(CPP) $(CPPFLAGS) $< $(DIRECT_LIB_DIRS) $(DIRECT_LIBS) -o $@

direct_big_step: direct_big_step.o
	$(CPP) $(CPPFLAGS) $< $(DIRECT_LIB_DIRS) $(DIRECT_LIBS) -o $@

direct_small_step_print_temps: direct_small_step_print_temps.o
	$(CPP) $(CPPFLAGS) $< $(DIRECT_LIB_DIRS) $(DIRECT_LIBS) -o $@

direct_big_step_print_temps: direct_big_step_print_temps.o
	$(CPP) $(CPPFLAGS) $< $(DIRECT_LIB_DIRS) $(DIRECT_LIBS) -o $@

################################################################################

direct_small_step.o: direct_test.cc
	$(CPP) $(CPPFLAGS) -c $< -DSMALL $(DIRECT_INCLUDE) -o $@

direct_big_step.o: direct_test.cc
	$(CPP) $(CPPFLAGS) -c $< $(DIRECT_INCLUDE) -o $@

direct_small_step_print_temps.o: direct_test.cc
	$(CPP) $(CPPFLAGS) -c $< -DSMALL -DPRINT_TEMPS $(DIRECT_INCLUDE) -o $@

direct_big_step_print_temps.o: direct_test.cc
	$(CPP) $(CPPFLAGS) -c $< -DPRINT_TEMPS $(DIRECT_INCLUDE) -o $@

################################################################################
# Iterative  ###################################################################
################################################################################

iterative_small_step: iterative_small_step.o
	$(CPP) $(CPPFLAGS) $< $(ITERATIVE_LIB_DIRS) $(TL_LIB) $(SL_LIBS) -o $@ $(PROFILE)

iterative_big_step: iterative_big_step.o
	$(CPP) $(CPPFLAGS) $< $(ITERATIVE_LIB_DIRS) $(TL_LIB) $(SL_LIBS) -o $@ $(PROFILE)

iterative_small_step_print_temps: iterative_small_step_print_temps.o
	$(CPP) $(CPPFLAGS) $< $(ITERATIVE_LIB_DIRS) $(TL_LIB) $(SL_LIBS) -o $@ $(PROFILE)

iterative_big_step_print_temps: iterative_big_step_print_temps.o
	$(CPP) $(CPPFLAGS) $< $(ITERATIVE_LIB_DIRS) $(TL_LIB) $(SL_LIBS) -o $@ $(PROFILE)

################################################################################

iterative_small_step.o: iterative_test.cc
	$(CPP) $(CPPFLAGS) -c $< $(ITERATIVE_SOLVER) -DSMALL $(ITERATIVE_INCLUDE) -o $@

iterative_big_step.o: iterative_test.cc
	$(CPP) $(CPPFLAGS) -c $< $(ITERATIVE_SOLVER) $(ITERATIVE_INCLUDE) -o $@

iterative_small_step_print_temps.o: iterative_test.cc
	$(CPP) $(CPPFLAGS) -c $< $(ITERATIVE_SOLVER) -DSMALL -DPRINT_TEMPS $(ITERATIVE_INCLUDE) -o $@

iterative_big_step_print_temps.o: iterative_test.cc
	$(CPP) $(CPPFLAGS) -c $< $(ITERATIVE_SOLVER) -DPRINT_TEMPS $(ITERATIVE_INCLUDE) -o $@

###############################################################################

clean:
	@rm -vf *.o
	@rm -vf $(EXECS)
