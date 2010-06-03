
CPP                = g++
CPPFLAGS           = -Wall -Wextra -pedantic

TL_DIR             = /home/vincenzi/ThermalLibrary
TL_INCLUDE         = -I$(TL_DIR)/Include
TL_LIB_DIR         = -L$(TL_DIR)/Lib
TL_LIB             = -lThermalLibrary

SLU_DIR            = /home/vincenzi/SuperLU_4.0
SLU_INCLUDE        = -I$(SLU_DIR)/SRC
SLU_LIB_DIR        = -L$(SLU_DIR)/lib -L/usr/lib64
SLU_LIBS           = -lsuperlu_4.0 -lblas -lm

IML_DIR            = /home/vincenzi/IterativeMethodsLibrary
IML_INCLUDE        = -I$(IML_DIR)/include

SL_DIR             = /home/vincenzi/SparseLibrary
SL_INCLUDE         = -I$(SL_DIR)/include
SL_LIB_DIR         = -L$(SL_DIR)/lib
SL_LIBS            = -lmv -lsparse -lspblas -lm

CULA_DIR           = /home/vincenzi/cula
CULA_INCLUDE       = -I$(CULA_DIR)/include
CULA_LIB_DIR       = -L$(CULA_DIR)/lib64
CULA_LIBS          = -lcula -lcublas -lcudart


MV_INCLUDE         = -I$(SL_DIR)/mv/include

SLU_EXECS          = slu_small_step slu_big_step

BICG_EXECS         = bicg_small_step bicg_big_step

BICGSTAB_EXECS     = bicgstab_small_step bicgstab_big_step

CGS_EXECS          = cgs_small_step cgs_big_step 

GMRES_EXECS        = gmres_small_step gmres_big_step

IR_EXECS           = ir_small_step ir_big_step

QMR_EXECS          = qmr_small_step qmr_big_step

CULA_EXECS         = cula_small_step cula_big_step

EXECS              = $(SLU_EXECS) $(BICG_EXECS) $(BICGSTAB_EXECS) \
                     $(CGS_EXECS) $(GMRES_EXECS) $(IR_EXECS) $(QMR_EXECS) \
                     $(CULA_EXECS)

all : $(EXECS)

################################################################################
## Super LU ####################################################################
################################################################################

slu_small_step: slu_small_step.o
	$(CPP) $(CPPFLAGS) $(OPT) $< $(TL_LIB_DIR) $(SLU_LIB_DIR) $(TL_LIB) $(SLU_LIBS) -o $@ $(PROFILE)

slu_big_step: slu_big_step.o
	$(CPP) $(CPPFLAGS) $(OPT) $< $(TL_LIB_DIR) $(SLU_LIB_DIR) $(TL_LIB) $(SLU_LIBS) -o $@ $(PROFILE)

slu_small_step.o: iterative_test.cc
	$(CPP) $(CPPFLAGS) $(OPT) -c $< $(PRINT_TEMPS) -DSLU -DDETAILS $(TL_INCLUDE) $(SLU_INCLUDE) -o $@ $(PROFILE)

slu_big_step.o: iterative_test.cc
	$(CPP) $(CPPFLAGS) $(OPT) -c $< $(PRINT_TEMPS) -DSLU $(TL_INCLUDE) $(SLU_INCLUDE) -o $@ $(PROFILE)

################################################################################
# Biconjugate Gradient #########################################################
################################################################################

bicg_small_step: bicg_small_step.o
	$(CPP) $(CPPFLAGS) $(OPT) $< $(TL_LIB_DIR) $(SL_LIB_DIR) $(TL_LIB) $(SL_LIBS) -o $@ $(PROFILE)

bicg_big_step: bicg_big_step.o
	$(CPP) $(CPPFLAGS) $(OPT) $< $(TL_LIB_DIR) $(SL_LIB_DIR) $(TL_LIB) $(SL_LIBS) -o $@ $(PROFILE)

bicg_small_step.o: iterative_test.cc
	$(CPP) $(CPPFLAGS) $(OPT) -c $< $(PRINT_TEMPS) -DBICG -DDETAILS $(TL_INCLUDE) $(IML_INCLUDE) -o $@ $(PROFILE)

bicg_big_step.o: iterative_test.cc
	$(CPP) $(CPPFLAGS) $(OPT) -c $< $(PRINT_TEMPS) -DBICG $(TL_INCLUDE) $(IML_INCLUDE) -o $@ $(PROFILE)

################################################################################
# Biconjugate Gradient stabilized ##############################################
################################################################################

bicgstab_small_step: bicgstab_small_step.o
	$(CPP) $(CPPFLAGS) $(OPT) $< $(TL_LIB_DIR) $(SL_LIB_DIR) $(TL_LIB) $(SL_LIBS) -o $@ $(PROFILE) 

bicgstab_big_step: bicgstab_big_step.o
	$(CPP) $(CPPFLAGS) $(OPT) $< $(TL_LIB_DIR) $(SL_LIB_DIR) $(TL_LIB) $(SL_LIBS) -o $@ $(PROFILE)

bicgstab_small_step.o: iterative_test.cc
	$(CPP) $(CPPFLAGS) $(OPT) -c $< $(PRINT_TEMPS) -DBICGSTAB -DDETAILS $(TL_INCLUDE) $(IML_INCLUDE) -o $@ $(PROFILE)

bicgstab_big_step.o: iterative_test.cc
	$(CPP) $(CPPFLAGS) $(OPT) -c $< $(PRINT_TEMPS) -DBICGSTAB $(TL_INCLUDE) $(IML_INCLUDE) -o $@ $(PROFILE)

################################################################################
# Conjugate Gradient Squared Method ############################################
################################################################################

cgs_small_step: cgs_small_step.o
	$(CPP) $(CPPFLAGS) $(OPT) $< $(TL_LIB_DIR) $(SL_LIB_DIR) $(TL_LIB) $(SL_LIBS) -o $@ $(PROFILE)

cgs_big_step: cgs_big_step.o
	$(CPP) $(CPPFLAGS) $(OPT) $< $(TL_LIB_DIR) $(SL_LIB_DIR) $(TL_LIB) $(SL_LIBS) -o $@ $(PROFILE)

cgs_small_step.o: iterative_test.cc
	$(CPP) $(CPPFLAGS) $(OPT) -c $< $(PRINT_TEMPS) -DCGS -DDETAILS $(TL_INCLUDE) $(IML_INCLUDE) -o $@ $(PROFILE)

cgs_big_step.o: iterative_test.cc
	$(CPP) $(CPPFLAGS) $(OPT) -c $< $(PRINT_TEMPS) -DCGS $(TL_INCLUDE) $(IML_INCLUDE) -o $@ $(PROFILE)

################################################################################
# Generalized minimum Residual Method ##########################################
################################################################################

gmres_small_step: gmres_small_step.o
	$(CPP) $(CPPFLAGS) $(OPT) $< $(TL_LIB_DIR) $(SL_LIB_DIR) $(TL_LIB) $(SL_LIBS) -o $@ $(PROFILE)

gmres_big_step: gmres_big_step.o
	$(CPP) $(CPPFLAGS) $(OPT) $< $(TL_LIB_DIR) $(SL_LIB_DIR) $(TL_LIB) $(SL_LIBS) -o $@ $(PROFILE)

gmres_small_step.o: iterative_test.cc
	$(CPP) $(CPPFLAGS) $(OPT) -c $< $(PRINT_TEMPS) -DGMRES -DDETAILS $(TL_INCLUDE) $(IML_INCLUDE) -o $@ $(PROFILE)

gmres_big_step.o: iterative_test.cc
	$(CPP) $(CPPFLAGS) $(OPT) -c $< $(PRINT_TEMPS) -DGMRES $(TL_INCLUDE) $(IML_INCLUDE) -o $@ $(PROFILE)

################################################################################
# Iterative Refinement #########################################################
################################################################################

ir_small_step: ir_small_step.o
	$(CPP) $(CPPFLAGS) $(OPT) $< $(TL_LIB_DIR) $(SL_LIB_DIR) $(TL_LIB) $(SL_LIBS) -o $@ $(PROFILE) 

ir_big_step: ir_big_step.o
	$(CPP) $(CPPFLAGS) $(OPT) $< $(TL_LIB_DIR) $(SL_LIB_DIR) $(TL_LIB) $(SL_LIBS) -o $@ $(PROFILE)

ir_small_step.o: iterative_test.cc
	$(CPP) $(CPPFLAGS) $(OPT) -c $< $(PRINT_TEMPS) -DIR -DDETAILS $(TL_INCLUDE) $(IML_INCLUDE) -o $@ $(PROFILE)

ir_big_step.o: iterative_test.cc
	$(CPP) $(CPPFLAGS) $(OPT) -c $< $(PRINT_TEMPS) -DIR $(TL_INCLUDE) $(IML_INCLUDE) -o $@ $(PROFILE)

################################################################################
# Quasi Minimal Residual #######################################################
################################################################################

qmr_small_step: qmr_small_step.o
	$(CPP) $(CPPFLAGS) $(OPT) $< $(TL_LIB_DIR) $(SL_LIB_DIR) $(TL_LIB) $(SL_LIBS) -o $@ $(PROFILE)

qmr_big_step: qmr_big_step.o
	$(CPP) $(CPPFLAGS) $(OPT) $< $(TL_LIB_DIR) $(SL_LIB_DIR) $(TL_LIB) $(SL_LIBS) -o $@ $(PROFILE)

qmr_small_step.o: iterative_test.cc
	$(CPP) $(CPPFLAGS) $(OPT) -c $< $(PRINT_TEMPS) -DQMR -DDETAILS $(TL_INCLUDE) $(IML_INCLUDE) -o $@ $(PROFILE)

qmr_big_step.o: iterative_test.cc
	$(CPP) $(CPPFLAGS) $(OPT) -c $< $(PRINT_TEMPS) -DQMR $(TL_INCLUDE) $(IML_INCLUDE) -o $@ $(PROFILE)

################################################################################
# CULA Lapack ##################################################################
################################################################################

cula_small_step: cula_small_step.o
	$(CPP) $(CPPFLAGS) $(OPT) $< $(TL_LIB_DIR) $(SL_LIB_DIR) $(CULA_LIB_DIR) $(TL_LIB) $(SL_LIBS) $(CULA_LIBS) -o $@ $(PROFILE)

cula_big_step: cula_big_step.o
	$(CPP) $(CPPFLAGS) $(OPT) $< $(TL_LIB_DIR) $(SL_LIB_DIR) $(CULA_LIB_DIR) $(TL_LIB) $(SL_LIBS) $(CULA_LIBS) -o $@ $(PROFILE)

cula_small_step.o: iterative_test.cc
	$(CPP) $(CPPFLAGS) $(OPT) -c $< $(PRINT_TEMPS) -DCULA -DDETAILS $(TL_INCLUDE) $(IML_INCLUDE) $(CULA_INCLUDE) -o $@ $(PROFILE)

cula_big_step.o: iterative_test.cc
	$(CPP) $(CPPFLAGS) $(OPT) -c $< $(PRINT_TEMPS) -DCULA $(TL_INCLUDE) $(IML_INCLUDE) $(CULA_INCLUDE) -o $@ $(PROFILE)

################################################################################

clean:
	rm -fr *.o
	rm -fr $(EXECS)
