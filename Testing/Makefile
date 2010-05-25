
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
	$(CPP) $(CPPFLAGS) $< $(TL_LIB_DIR) $(SLU_LIB_DIR) $(TL_LIB) $(SLU_LIBS) -o $@

slu_big_step: slu_big_step.o
	$(CPP) $(CPPFLAGS) $< $(TL_LIB_DIR) $(SLU_LIB_DIR) $(TL_LIB) $(SLU_LIBS) -o $@

slu_small_step.o: test_slu.cc
	$(CPP) $(CPPFLAGS) -c $< -DDETAILS $(TL_INCLUDE) $(SLU_INCLUDE) -o $@

slu_big_step.o: test_slu.cc
	$(CPP) $(CPPFLAGS) -c $< $(TL_INCLUDE) $(SLU_INCLUDE) -o $@

################################################################################
# Biconjugate Gradient #########################################################
################################################################################

bicg_diag_pre_small_step: bicg_diag_pre_small_step.o
	$(CPP) $(CPPFLAGS) $< $(TL_LIB_DIR) $(SL_LIB_DIR) $(TL_LIB) $(SL_LIBS) -o $@ 

bicg_diag_pre_big_step: bicg_diag_pre_big_step.o
	$(CPP) $(CPPFLAGS) $< $(TL_LIB_DIR) $(SL_LIB_DIR) $(TL_LIB) $(SL_LIBS) -o $@ 

bicg_ilu_pre_small_step: bicg_ilu_pre_small_step.o
	$(CPP) $(CPPFLAGS) $< $(TL_LIB_DIR) $(SL_LIB_DIR) $(TL_LIB) $(SL_LIBS) -o $@ 

bicg_ilu_pre_big_step: bicg_ilu_pre_big_step.o
	$(CPP) $(CPPFLAGS) $< $(TL_LIB_DIR) $(SL_LIB_DIR) $(TL_LIB) $(SL_LIBS) -o $@ 

bicg_diag_pre_small_step.o: iterative_test.cc
	$(CPP) $(CPPFLAGS) -c $< -DTD_BICG -DTL_DIAGONAL_PRECONDITIONER -DDETAILS $(TL_INCLUDE) $(IML_INCLUDE) -o $@

bicg_diag_pre_big_step.o: iterative_test.cc
	$(CPP) $(CPPFLAGS) -c $< -DTD_BICG -DTL_DIAGONAL_PRECONDITIONER $(TL_INCLUDE) $(IML_INCLUDE) -o $@

bicg_ilu_pre_small_step.o: iterative_test.cc
	$(CPP) $(CPPFLAGS) -c $< -DTD_BICG -DTL_ILU_PRECONDITIONER -DDETAILS $(TL_INCLUDE) $(IML_INCLUDE) -o $@

bicg_ilu_pre_big_step.o: iterative_test.cc
	$(CPP) $(CPPFLAGS) -c $< -DTD_BICG -DTL_ILU_PRECONDITIONER $(TL_INCLUDE) $(IML_INCLUDE) -o $@

################################################################################
# Biconjugate Gradient stabilized ##############################################
################################################################################

bicgstab_diag_pre_small_step: bicgstab_diag_pre_small_step.o
	$(CPP) $(CPPFLAGS) $< $(TL_LIB_DIR) $(SL_LIB_DIR) $(TL_LIB) $(SL_LIBS) -o $@ 

bicgstab_diag_pre_big_step: bicgstab_diag_pre_big_step.o
	$(CPP) $(CPPFLAGS) $< $(TL_LIB_DIR) $(SL_LIB_DIR) $(TL_LIB) $(SL_LIBS) -o $@ 

bicgstab_ilu_pre_small_step: bicgstab_ilu_pre_small_step.o
	$(CPP) $(CPPFLAGS) $< $(TL_LIB_DIR) $(SL_LIB_DIR) $(TL_LIB) $(SL_LIBS) -o $@ 

bicgstab_ilu_pre_big_step: bicgstab_ilu_pre_big_step.o
	$(CPP) $(CPPFLAGS) $< $(TL_LIB_DIR) $(SL_LIB_DIR) $(TL_LIB) $(SL_LIBS) -o $@ 

bicgstab_diag_pre_small_step.o: iterative_test.cc
	$(CPP) $(CPPFLAGS) -c $< -DTD_BICGSTAB -DTL_DIAGONAL_PRECONDITIONER -DDETAILS $(TL_INCLUDE) $(IML_INCLUDE) -o $@

bicgstab_diag_pre_big_step.o: iterative_test.cc
	$(CPP) $(CPPFLAGS) -c $< -DTD_BICGSTAB -DTL_DIAGONAL_PRECONDITIONER $(TL_INCLUDE) $(IML_INCLUDE) -o $@

bicgstab_ilu_pre_small_step.o: iterative_test.cc
	$(CPP) $(CPPFLAGS) -c $< -DTD_BICGSTAB -DTL_ILU_PRECONDITIONER -DDETAILS $(TL_INCLUDE) $(IML_INCLUDE) -o $@

bicgstab_ilu_pre_big_step.o: iterative_test.cc
	$(CPP) $(CPPFLAGS) -c $< -DTD_BICGSTAB -DTL_ILU_PRECONDITIONER $(TL_INCLUDE) $(IML_INCLUDE) -o $@

################################################################################
# Conjugate Gradient Squared Method ############################################
################################################################################

cgs_diag_pre_small_step: cgs_diag_pre_small_step.o
	$(CPP) $(CPPFLAGS) $< $(TL_LIB_DIR) $(SL_LIB_DIR) $(TL_LIB) $(SL_LIBS) -o $@ 

cgs_diag_pre_big_step: cgs_diag_pre_big_step.o
	$(CPP) $(CPPFLAGS) $< $(TL_LIB_DIR) $(SL_LIB_DIR) $(TL_LIB) $(SL_LIBS) -o $@ 

cgs_ilu_pre_small_step: cgs_ilu_pre_small_step.o
	$(CPP) $(CPPFLAGS) $< $(TL_LIB_DIR) $(SL_LIB_DIR) $(TL_LIB) $(SL_LIBS) -o $@ 

cgs_ilu_pre_big_step: cgs_ilu_pre_big_step.o
	$(CPP) $(CPPFLAGS) $< $(TL_LIB_DIR) $(SL_LIB_DIR) $(TL_LIB) $(SL_LIBS) -o $@ 

cgs_diag_pre_small_step.o: iterative_test.cc
	$(CPP) $(CPPFLAGS) -c $< -DTD_CGS -DTL_DIAGONAL_PRECONDITIONER -DDETAILS $(TL_INCLUDE) $(IML_INCLUDE) -o $@

cgs_diag_pre_big_step.o: iterative_test.cc
	$(CPP) $(CPPFLAGS) -c $< -DTD_CGS -DTL_DIAGONAL_PRECONDITIONER $(TL_INCLUDE) $(IML_INCLUDE) -o $@

cgs_ilu_pre_small_step.o: iterative_test.cc
	$(CPP) $(CPPFLAGS) -c $< -DTD_CGS -DTL_ILU_PRECONDITIONER -DDETAILS $(TL_INCLUDE) $(IML_INCLUDE) -o $@

cgs_ilu_pre_big_step.o: iterative_test.cc
	$(CPP) $(CPPFLAGS) -c $< -DTD_CGS -DTL_ILU_PRECONDITIONER $(TL_INCLUDE) $(IML_INCLUDE) -o $@

################################################################################
# Generalized minimum Residual Method ##########################################
################################################################################

gmres_diag_pre_small_step: gmres_diag_pre_small_step.o
	$(CPP) $(CPPFLAGS) $< $(TL_LIB_DIR) $(SL_LIB_DIR) $(TL_LIB) $(SL_LIBS) -o $@ 

gmres_diag_pre_big_step: gmres_diag_pre_big_step.o
	$(CPP) $(CPPFLAGS) $< $(TL_LIB_DIR) $(SL_LIB_DIR) $(TL_LIB) $(SL_LIBS) -o $@ 

gmres_ilu_pre_small_step: gmres_ilu_pre_small_step.o
	$(CPP) $(CPPFLAGS) $< $(TL_LIB_DIR) $(SL_LIB_DIR) $(TL_LIB) $(SL_LIBS) -o $@ 

gmres_ilu_pre_big_step: gmres_ilu_pre_big_step.o
	$(CPP) $(CPPFLAGS) $< $(TL_LIB_DIR) $(SL_LIB_DIR) $(TL_LIB) $(SL_LIBS) -o $@ 

gmres_diag_pre_small_step.o: iterative_test.cc
	$(CPP) $(CPPFLAGS) -c $< -DTD_GMRES -DTL_DIAGONAL_PRECONDITIONER -DDETAILS $(TL_INCLUDE) $(IML_INCLUDE) -o $@

gmres_diag_pre_big_step.o: iterative_test.cc
	$(CPP) $(CPPFLAGS) -c $< -DTD_GMRES -DTL_DIAGONAL_PRECONDITIONER $(TL_INCLUDE) $(IML_INCLUDE) -o $@

gmres_ilu_pre_small_step.o: iterative_test.cc
	$(CPP) $(CPPFLAGS) -c $< -DTD_GMRES -DTL_ILU_PRECONDITIONER -DDETAILS $(TL_INCLUDE) $(IML_INCLUDE) -o $@

gmres_ilu_pre_big_step.o: iterative_test.cc
	$(CPP) $(CPPFLAGS) -c $< -DTD_GMRES -DTL_ILU_PRECONDITIONER $(TL_INCLUDE) $(IML_INCLUDE) -o $@

################################################################################
# Iterative Refinement #########################################################
################################################################################

ir_ilu_pre_small_step: ir_ilu_pre_small_step.o
	$(CPP) $(CPPFLAGS) $< $(TL_LIB_DIR) $(SL_LIB_DIR) $(TL_LIB) $(SL_LIBS) -o $@ 

ir_ilu_pre_big_step: ir_ilu_pre_big_step.o
	$(CPP) $(CPPFLAGS) $< $(TL_LIB_DIR) $(SL_LIB_DIR) $(TL_LIB) $(SL_LIBS) -o $@ 

ir_ilu_pre_small_step.o: iterative_test.cc
	$(CPP) $(CPPFLAGS) -c $< -DTD_IR -DDETAILS $(TL_INCLUDE) $(IML_INCLUDE) -o $@

ir_ilu_pre_big_step.o: iterative_test.cc
	$(CPP) $(CPPFLAGS) -c $< -DTD_IR $(TL_INCLUDE) $(IML_INCLUDE) -o $@

################################################################################
# Quasi Minimal Residual #######################################################
################################################################################

qmr_diag_pre_small_step: qmr_diag_pre_small_step.o
	$(CPP) $(CPPFLAGS) $< $(TL_LIB_DIR) $(SL_LIB_DIR) $(TL_LIB) $(SL_LIBS) -o $@ 

qmr_diag_pre_big_step: qmr_diag_pre_big_step.o
	$(CPP) $(CPPFLAGS) $< $(TL_LIB_DIR) $(SL_LIB_DIR) $(TL_LIB) $(SL_LIBS) -o $@ 

qmr_ilu_pre_small_step: qmr_ilu_pre_small_step.o
	$(CPP) $(CPPFLAGS) $< $(TL_LIB_DIR) $(SL_LIB_DIR) $(TL_LIB) $(SL_LIBS) -o $@ 

qmr_ilu_pre_big_step: qmr_ilu_pre_big_step.o
	$(CPP) $(CPPFLAGS) $< $(TL_LIB_DIR) $(SL_LIB_DIR) $(TL_LIB) $(SL_LIBS) -o $@ 

qmr_diag_pre_small_step.o: iterative_test.cc
	$(CPP) $(CPPFLAGS) -c $< -DTD_QMR -DTL_DIAGONAL_PRECONDITIONER -DDETAILS $(TL_INCLUDE) $(IML_INCLUDE) -o $@

qmr_diag_pre_big_step.o: iterative_test.cc
	$(CPP) $(CPPFLAGS) -c $< -DTD_QMR -DTL_DIAGONAL_PRECONDITIONER $(TL_INCLUDE) $(IML_INCLUDE) -o $@

qmr_ilu_pre_small_step.o: iterative_test.cc
	$(CPP) $(CPPFLAGS) -c $< -DTD_QMR -DTL_ILU_PRECONDITIONER -DDETAILS $(TL_INCLUDE) $(IML_INCLUDE) -o $@

qmr_ilu_pre_big_step.o: iterative_test.cc
	$(CPP) $(CPPFLAGS) -c $< -DTD_QMR -DTL_ILU_PRECONDITIONER $(TL_INCLUDE) $(IML_INCLUDE) -o $@

################################################################################

clean:
	rm -fr *.o
	rm -fr $(EXECS)
