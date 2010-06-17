
CPP                = g++
CPPFLAGS           = -Wall -Wextra -pedantic -O3 

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
SLU_LIB            = -lsuperlu_4.0 -lblas -lm

#
# Iterative Methods Library
#

IML_DIR            = /home/vincenzi/IterativeMethodsLibrary
IML_INCLUDE        = -I$(IML_DIR)/include

#
# Sparse Library
#

SL_DIR             = /home/vincenzi/SparseLibrary
SL_INCLUDE         = -I$(SL_DIR)/include -I$(SL_DIR)/mv/include
SL_LIB_DIR         = -L$(SL_DIR)/lib
SL_LIB             = -lmv -lsparse -lspblas -lm

#
# Cublas Library
#

CUBLAS_DIR         = /usr/local/cuda
CUBLAS_INCLUDE     = -I$(CUBLAS_DIR)/include
CUBLAS_LIB_DIR     = -L$(CUBLAS_DIR)/lib64
CUBLAS_LIB         = -lcublasemu

#
# Direct compiling parameters
#

DIRECT_INCLUDE     = $(TL_INCLUDE) $(SLU_INCLUDE) $(SL_INCLUDE)
DIRECT_LIB_DIR     = $(TL_LIB_DIR) $(SLU_LIB_DIR) $(SL_LIB_DIR)
DIRECT_LIB         = $(TL_LIB) $(SLU_LIB) $(SL_LIB)
DIRECT_EXECS       = direct direct_print_temps

#
# Iterative compiling parameters
#

ITERATIVE_INCLUDE  = $(TL_INCLUDE) $(SL_INCLUDE)
ITERATIVE_LIB_DIR  = $(TL_LIB_DIR) $(SL_LIB_DIR)# $(CUBLAS_LIB_DIR)
ITERATIVE_LIB      = $(TL_LIB) $(SL_LIB)# $(CUBLAS_LIB)
ITERATIVE_EXECS    = iterative iterative_print_temps


EXECS = $(DIRECT_EXECS) $(ITERATIVE_EXECS)

all : $(EXECS)

################################################################################
## Direct ######################################################################
################################################################################

direct: direct.o
	$(CPP) $(CPPFLAGS) $< $(DIRECT_LIB_DIR) $(DIRECT_LIB) -o $@

direct_print_temps: direct_print_temps.o
	$(CPP) $(CPPFLAGS) $< $(DIRECT_LIB_DIR) $(DIRECT_LIB) -o $@

################################################################################

direct.o: direct_test.cc
	$(CPP) $(CPPFLAGS) -c $< $(DIRECT_INCLUDE) -o $@

direct_print_temps.o: direct_test.cc
	$(CPP) $(CPPFLAGS) -c $< -DPRINT_TEMPS $(DIRECT_INCLUDE) -o $@

################################################################################
# Iterative  ###################################################################
################################################################################

iterative: iterative.o
	$(CPP) $(CPPFLAGS) $< $(ITERATIVE_LIB_DIR) $(ITERATIVE_LIB) -o $@

iterative_print_temps: iterative_print_temps.o
	$(CPP) $(CPPFLAGS) $< $(ITERATIVE_LIB_DIR) $(ITERATIVE_LIB) -o $@

################################################################################

iterative.o: iterative_test.cc
	$(CPP) $(CPPFLAGS) -c $< $(ITERATIVE_SOLVER) $(ITERATIVE_INCLUDE) -o $@

iterative_print_temps.o: iterative_test.cc
	$(CPP) $(CPPFLAGS) -c $< $(ITERATIVE_SOLVER) -DPRINT_TEMPS $(ITERATIVE_INCLUDE) -o $@

###############################################################################

clean:
	@rm -vf *.o
	@rm -vf $(EXECS)

