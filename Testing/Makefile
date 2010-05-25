
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

EXECS              = slu_small_step  slu_big_step          \
                     bicg_small_step bicg_big_step         \
                     bicgstab_small_step bicgstab_big_step \
                     cgs_small_step cgs_big_step           \
                     gmres_small_step gmres_big_step       \
                     ir_small_step ir_big_step


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

bicg_small_step: bicg_small_step.o
	$(CPP) $(CPPFLAGS) $< $(TL_LIB_DIR) $(SL_LIB_DIR) $(TL_LIB) $(SL_LIBS) -o $@ 

bicg_big_step: bicg_big_step.o
	$(CPP) $(CPPFLAGS) $< $(TL_LIB_DIR) $(SL_LIB_DIR) $(TL_LIB) $(SL_LIBS) -o $@ 

bicg_small_step.o: test_bicg.cc
	$(CPP) $(CPPFLAGS) -c $< -DDETAILS $(TL_INCLUDE) $(IML_INCLUDE) -o $@

bicg_big_step.o: test_bicg.cc
	$(CPP) $(CPPFLAGS) -c $< $(TL_INCLUDE) $(IML_INCLUDE) -o $@

################################################################################
# Biconjugate Gradient stabilized ##############################################
################################################################################

bicgstab_small_step: bicgstab_small_step.o
	$(CPP) $(CPPFLAGS) $< $(TL_LIB_DIR) $(SL_LIB_DIR) $(TL_LIB) $(SL_LIBS) -o $@ 

bicgstab_big_step: bicgstab_big_step.o
	$(CPP) $(CPPFLAGS) $< $(TL_LIB_DIR) $(SL_LIB_DIR) $(TL_LIB) $(SL_LIBS) -o $@ 

bicgstab_small_step.o: test_bicgstab.cc
	$(CPP) $(CPPFLAGS) -c $< -DDETAILS $(TL_INCLUDE) $(IML_INCLUDE) -o $@

bicgstab_big_step.o: test_bicgstab.cc
	$(CPP) $(CPPFLAGS) -c $< $(TL_INCLUDE) $(IML_INCLUDE) -o $@

################################################################################
# Conjugate Gradient Squared Method ############################################
################################################################################

cgs_small_step: cgs_small_step.o
	$(CPP) $(CPPFLAGS) $< $(TL_LIB_DIR) $(SL_LIB_DIR) $(TL_LIB) $(SL_LIBS) -o $@ 

cgs_big_step: cgs_big_step.o
	$(CPP) $(CPPFLAGS) $< $(TL_LIB_DIR) $(SL_LIB_DIR) $(TL_LIB) $(SL_LIBS) -o $@ 

cgs_small_step.o: test_cgs.cc
	$(CPP) $(CPPFLAGS) -c $< -DDETAILS $(TL_INCLUDE) $(IML_INCLUDE) -o $@

cgs_big_step.o: test_cgs.cc
	$(CPP) $(CPPFLAGS) -c $< $(TL_INCLUDE) $(IML_INCLUDE) -o $@

################################################################################
# Generalized minimum Residual Method ##########################################
################################################################################

gmres_small_step: gmres_small_step.o
	$(CPP) $(CPPFLAGS) $< $(TL_LIB_DIR) $(SL_LIB_DIR) $(TL_LIB) $(SL_LIBS) -o $@ 

gmres_big_step: gmres_big_step.o
	$(CPP) $(CPPFLAGS) $< $(TL_LIB_DIR) $(SL_LIB_DIR) $(TL_LIB) $(SL_LIBS) -o $@ 

gmres_small_step.o: test_gmres.cc
	$(CPP) $(CPPFLAGS) -c $< -DDETAILS $(TL_INCLUDE) $(IML_INCLUDE) -o $@

gmres_big_step.o: test_gmres.cc
	$(CPP) $(CPPFLAGS) -c $< $(TL_INCLUDE) $(IML_INCLUDE) -o $@

################################################################################
# Iterative Refinement #########################################################
################################################################################

ir_small_step: ir_small_step.o
	$(CPP) $(CPPFLAGS) $< $(TL_LIB_DIR) $(SL_LIB_DIR) $(TL_LIB) $(SL_LIBS) -o $@ 

ir_big_step: ir_big_step.o
	$(CPP) $(CPPFLAGS) $< $(TL_LIB_DIR) $(SL_LIB_DIR) $(TL_LIB) $(SL_LIBS) -o $@ 

ir_small_step.o: test_ir.cc
	$(CPP) $(CPPFLAGS) -c $< -DDETAILS $(TL_INCLUDE) $(IML_INCLUDE) -o $@

ir_big_step.o: test_ir.cc
	$(CPP) $(CPPFLAGS) -c $< $(TL_INCLUDE) $(IML_INCLUDE) -o $@

################################################################################

clean:
	rm -fr *.o
	rm -fr $(EXECS)
