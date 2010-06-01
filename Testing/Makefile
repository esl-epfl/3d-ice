
CPP                = g++
CPPFLAGS           = -Wall -Wextra -pedantic

TL_DIR             = /home/vincenzi/ThermalLibrary
TL_INCLUDE         = -I$(TL_DIR)/Include
TL_LIB_DIR         = -L$(TL_DIR)/Lib
TL_LIB             = -lTermalLibrary

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

MV_INCLUDE         = -I$(SL_DIR)/mv/include

SLU_EXECS          = slu_small_step slu_big_step

BICG_EXECS         = bicg_diag_pre_small_step bicg_diag_pre_big_step \
                     bicg_ilu_pre_small_step bicg_ilu_pre_big_step

BICGSTAB_EXECS     = bicgstab_diag_pre_small_step bicgstab_diag_pre_big_step \
                     bicgstab_ilu_pre_small_step bicgstab_ilu_pre_big_step

CGS_EXECS          = cgs_diag_pre_small_step cgs_diag_pre_big_step \
                     cgs_ilu_pre_small_step cgs_ilu_pre_big_step

GMRES_EXECS        = gmres_diag_pre_small_step gmres_diag_pre_big_step \
                     gmres_ilu_pre_small_step gmres_ilu_pre_big_step

IR_EXECS           = ir_ilu_pre_small_step ir_ilu_pre_big_step

QMR_EXECS          = qmr_diag_pre_small_step qmr_diag_pre_big_step \
                     qmr_ilu_pre_small_step qmr_ilu_pre_big_step

EXECS              = $(SLU_EXECS) $(BICG_EXECS) $(BICGSTAB_EXECS) \
                     $(CGS_EXECS) $(GMRES_EXECS) $(IR_EXECS) $(QMR_EXECS)

all : $(EXECS)

################################################################################
## Super LU ####################################################################
################################################################################

slu_small_step: slu_small_step.o
	$(CPP) $(CPPFLAGS) $< $(TL_LIB_DIR) $(SLU_LIB_DIR) $(TL_LIB) $(SLU_LIBS) -o $@ $(PROFILE)

slu_big_step: slu_big_step.o
	$(CPP) $(CPPFLAGS) $< $(TL_LIB_DIR) $(SLU_LIB_DIR) $(TL_LIB) $(SLU_LIBS) -o $@ $(PROFILE)

slu_small_step.o: iterative_test.cc
	$(CPP) $(CPPFLAGS) -c $< -DSLU -DDETAILS $(TL_INCLUDE) $(SLU_INCLUDE) -o $@ $(PROFILE)

slu_big_step.o: iterative_test.cc
	$(CPP) $(CPPFLAGS) -c $< -DSLU $(TL_INCLUDE) $(SLU_INCLUDE) -o $@ $(PROFILE)

################################################################################
# Biconjugate Gradient #########################################################
################################################################################

bicg_diag_pre_small_step: bicg_diag_pre_small_step.o
	$(CPP) $(CPPFLAGS) $< $(TL_LIB_DIR) $(SL_LIB_DIR) $(TL_LIB) $(SL_LIBS) -o $@ $(PROFILE)

bicg_diag_pre_big_step: bicg_diag_pre_big_step.o
	$(CPP) $(CPPFLAGS) $< $(TL_LIB_DIR) $(SL_LIB_DIR) $(TL_LIB) $(SL_LIBS) -o $@ $(PROFILE)

bicg_ilu_pre_small_step: bicg_ilu_pre_small_step.o
	$(CPP) $(CPPFLAGS) $< $(TL_LIB_DIR) $(SL_LIB_DIR) $(TL_LIB) $(SL_LIBS) -o $@ $(PROFILE)

bicg_ilu_pre_big_step: bicg_ilu_pre_big_step.o
	$(CPP) $(CPPFLAGS) $< $(TL_LIB_DIR) $(SL_LIB_DIR) $(TL_LIB) $(SL_LIBS) -o $@ $(PROFILE)

bicg_diag_pre_small_step.o: iterative_test.cc
	$(CPP) $(CPPFLAGS) -c $< -DBICG -DDIAGONAL -DDETAILS $(TL_INCLUDE) $(IML_INCLUDE) -o $@ $(PROFILE)

bicg_diag_pre_big_step.o: iterative_test.cc
	$(CPP) $(CPPFLAGS) -c $< -DBICG -DDIAGONAL $(TL_INCLUDE) $(IML_INCLUDE) -o $@ $(PROFILE)

bicg_ilu_pre_small_step.o: iterative_test.cc
	$(CPP) $(CPPFLAGS) -c $< -DBICG -DILU -DDETAILS $(TL_INCLUDE) $(IML_INCLUDE) -o $@ $(PROFILE)

bicg_ilu_pre_big_step.o: iterative_test.cc
	$(CPP) $(CPPFLAGS) -c $< -DBICG -DILU $(TL_INCLUDE) $(IML_INCLUDE) -o $@ $(PROFILE)

################################################################################
# Biconjugate Gradient stabilized ##############################################
################################################################################

bicgstab_diag_pre_small_step: bicgstab_diag_pre_small_step.o
	$(CPP) $(CPPFLAGS) $< $(TL_LIB_DIR) $(SL_LIB_DIR) $(TL_LIB) $(SL_LIBS) -o $@ $(PROFILE) 

bicgstab_diag_pre_big_step: bicgstab_diag_pre_big_step.o
	$(CPP) $(CPPFLAGS) $< $(TL_LIB_DIR) $(SL_LIB_DIR) $(TL_LIB) $(SL_LIBS) -o $@ $(PROFILE)

bicgstab_ilu_pre_small_step: bicgstab_ilu_pre_small_step.o
	$(CPP) $(CPPFLAGS) $< $(TL_LIB_DIR) $(SL_LIB_DIR) $(TL_LIB) $(SL_LIBS) -o $@ $(PROFILE)

bicgstab_ilu_pre_big_step: bicgstab_ilu_pre_big_step.o
	$(CPP) $(CPPFLAGS) $< $(TL_LIB_DIR) $(SL_LIB_DIR) $(TL_LIB) $(SL_LIBS) -o $@ $(PROFILE)

bicgstab_diag_pre_small_step.o: iterative_test.cc
	$(CPP) $(CPPFLAGS) -c $< -DBICGSTAB -DDIAGONAL -DDETAILS $(TL_INCLUDE) $(IML_INCLUDE) -o $@ $(PROFILE)

bicgstab_diag_pre_big_step.o: iterative_test.cc
	$(CPP) $(CPPFLAGS) -c $< -DBICGSTAB -DDIAGONAL $(TL_INCLUDE) $(IML_INCLUDE) -o $@ $(PROFILE)

bicgstab_ilu_pre_small_step.o: iterative_test.cc
	$(CPP) $(CPPFLAGS) -c $< -DBICGSTAB -DILU -DDETAILS $(TL_INCLUDE) $(IML_INCLUDE) -o $@ $(PROFILE)

bicgstab_ilu_pre_big_step.o: iterative_test.cc
	$(CPP) $(CPPFLAGS) -c $< -DBICGSTAB -DILU $(TL_INCLUDE) $(IML_INCLUDE) -o $@ $(PROFILE)

################################################################################
# Conjugate Gradient Squared Method ############################################
################################################################################

cgs_diag_pre_small_step: cgs_diag_pre_small_step.o
	$(CPP) $(CPPFLAGS) $< $(TL_LIB_DIR) $(SL_LIB_DIR) $(TL_LIB) $(SL_LIBS) -o $@ $(PROFILE)

cgs_diag_pre_big_step: cgs_diag_pre_big_step.o
	$(CPP) $(CPPFLAGS) $< $(TL_LIB_DIR) $(SL_LIB_DIR) $(TL_LIB) $(SL_LIBS) -o $@ $(PROFILE)

cgs_ilu_pre_small_step: cgs_ilu_pre_small_step.o
	$(CPP) $(CPPFLAGS) $< $(TL_LIB_DIR) $(SL_LIB_DIR) $(TL_LIB) $(SL_LIBS) -o $@ $(PROFILE)

cgs_ilu_pre_big_step: cgs_ilu_pre_big_step.o
	$(CPP) $(CPPFLAGS) $< $(TL_LIB_DIR) $(SL_LIB_DIR) $(TL_LIB) $(SL_LIBS) -o $@ $(PROFILE)

cgs_diag_pre_small_step.o: iterative_test.cc
	$(CPP) $(CPPFLAGS) -c $< -DCGS -DDIAGONAL -DDETAILS $(TL_INCLUDE) $(IML_INCLUDE) -o $@ $(PROFILE)

cgs_diag_pre_big_step.o: iterative_test.cc
	$(CPP) $(CPPFLAGS) -c $< -DCGS -DDIAGONAL $(TL_INCLUDE) $(IML_INCLUDE) -o $@ $(PROFILE)

cgs_ilu_pre_small_step.o: iterative_test.cc
	$(CPP) $(CPPFLAGS) -c $< -DCGS -DILU -DDETAILS $(TL_INCLUDE) $(IML_INCLUDE) -o $@ $(PROFILE)

cgs_ilu_pre_big_step.o: iterative_test.cc
	$(CPP) $(CPPFLAGS) -c $< -DCGS -DILU $(TL_INCLUDE) $(IML_INCLUDE) -o $@ $(PROFILE)

################################################################################
# Generalized minimum Residual Method ##########################################
################################################################################

gmres_diag_pre_small_step: gmres_diag_pre_small_step.o
	$(CPP) $(CPPFLAGS) $< $(TL_LIB_DIR) $(SL_LIB_DIR) $(TL_LIB) $(SL_LIBS) -o $@ $(PROFILE)

gmres_diag_pre_big_step: gmres_diag_pre_big_step.o
	$(CPP) $(CPPFLAGS) $< $(TL_LIB_DIR) $(SL_LIB_DIR) $(TL_LIB) $(SL_LIBS) -o $@ $(PROFILE)

gmres_ilu_pre_small_step: gmres_ilu_pre_small_step.o
	$(CPP) $(CPPFLAGS) $< $(TL_LIB_DIR) $(SL_LIB_DIR) $(TL_LIB) $(SL_LIBS) -o $@ $(PROFILE)

gmres_ilu_pre_big_step: gmres_ilu_pre_big_step.o
	$(CPP) $(CPPFLAGS) $< $(TL_LIB_DIR) $(SL_LIB_DIR) $(TL_LIB) $(SL_LIBS) -o $@ $(PROFILE)

gmres_diag_pre_small_step.o: iterative_test.cc
	$(CPP) $(CPPFLAGS) -c $< -DGMRES -DDIAGONAL -DDETAILS $(TL_INCLUDE) $(IML_INCLUDE) -o $@ $(PROFILE)

gmres_diag_pre_big_step.o: iterative_test.cc
	$(CPP) $(CPPFLAGS) -c $< -DGMRES -DDIAGONAL $(TL_INCLUDE) $(IML_INCLUDE) -o $@ $(PROFILE)

gmres_ilu_pre_small_step.o: iterative_test.cc
	$(CPP) $(CPPFLAGS) -c $< -DGMRES -DILU -DDETAILS $(TL_INCLUDE) $(IML_INCLUDE) -o $@ $(PROFILE)

gmres_ilu_pre_big_step.o: iterative_test.cc
	$(CPP) $(CPPFLAGS) -c $< -DGMRES -DILU $(TL_INCLUDE) $(IML_INCLUDE) -o $@ $(PROFILE)

################################################################################
# Iterative Refinement #########################################################
################################################################################

ir_ilu_pre_small_step: ir_ilu_pre_small_step.o
	$(CPP) $(CPPFLAGS) $< $(TL_LIB_DIR) $(SL_LIB_DIR) $(TL_LIB) $(SL_LIBS) -o $@ $(PROFILE) 

ir_ilu_pre_big_step: ir_ilu_pre_big_step.o
	$(CPP) $(CPPFLAGS) $< $(TL_LIB_DIR) $(SL_LIB_DIR) $(TL_LIB) $(SL_LIBS) -o $@ $(PROFILE)

ir_ilu_pre_small_step.o: iterative_test.cc
	$(CPP) $(CPPFLAGS) -c $< -DIR -DDETAILS $(TL_INCLUDE) $(IML_INCLUDE) -o $@ $(PROFILE)

ir_ilu_pre_big_step.o: iterative_test.cc
	$(CPP) $(CPPFLAGS) -c $< -DIR $(TL_INCLUDE) $(IML_INCLUDE) -o $@ $(PROFILE)

################################################################################
# Quasi Minimal Residual #######################################################
################################################################################

qmr_diag_pre_small_step: qmr_diag_pre_small_step.o
	$(CPP) $(CPPFLAGS) $< $(TL_LIB_DIR) $(SL_LIB_DIR) $(TL_LIB) $(SL_LIBS) -o $@ $(PROFILE)

qmr_diag_pre_big_step: qmr_diag_pre_big_step.o
	$(CPP) $(CPPFLAGS) $< $(TL_LIB_DIR) $(SL_LIB_DIR) $(TL_LIB) $(SL_LIBS) -o $@ $(PROFILE)

qmr_ilu_pre_small_step: qmr_ilu_pre_small_step.o
	$(CPP) $(CPPFLAGS) $< $(TL_LIB_DIR) $(SL_LIB_DIR) $(TL_LIB) $(SL_LIBS) -o $@ $(PROFILE)

qmr_ilu_pre_big_step: qmr_ilu_pre_big_step.o
	$(CPP) $(CPPFLAGS) $< $(TL_LIB_DIR) $(SL_LIB_DIR) $(TL_LIB) $(SL_LIBS) -o $@ $(PROFILE)

qmr_diag_pre_small_step.o: iterative_test.cc
	$(CPP) $(CPPFLAGS) -c $< -DQMR -DDIAGONAL -DDETAILS $(TL_INCLUDE) $(IML_INCLUDE) -o $@ $(PROFILE)

qmr_diag_pre_big_step.o: iterative_test.cc
	$(CPP) $(CPPFLAGS) -c $< -DQMR -DDIAGONAL $(TL_INCLUDE) $(IML_INCLUDE) -o $@ $(PROFILE)

qmr_ilu_pre_small_step.o: iterative_test.cc
	$(CPP) $(CPPFLAGS) -c $< -DQMR -DILU -DDETAILS $(TL_INCLUDE) $(IML_INCLUDE) -o $@ $(PROFILE)

qmr_ilu_pre_big_step.o: iterative_test.cc
	$(CPP) $(CPPFLAGS) -c $< -DQMR -DILU $(TL_INCLUDE) $(IML_INCLUDE) -o $@ $(PROFILE)

################################################################################

clean:
	rm -fr *.o
	rm -fr $(EXECS)
