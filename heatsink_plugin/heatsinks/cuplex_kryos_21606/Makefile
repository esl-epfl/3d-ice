
SRC = ../../common/libraries/ThermalBlocks.mo ../../common/HeatsinkBlocks.mo Cuplex.mo
OBJ = Cuplex.Cuplex21606_ConstantFlowRate_Interface3DICE \
      Cuplex.Cuplex21606_VariableFlowRate_Interface3DICE
DUMMY = .dummy

# OpenModelica produces temporary files with a name where . is replaced with _
DOT = .
UNDERSCORE = _
OBJ_UNDERSCORE = $(subst $(DOT),$(UNDERSCORE),$(OBJ))

all: $(DUMMY)

# Making an FMI from OpenModelica requires to use its own
# build system scripting language
# This rule makes all OBJs in one go, so we cant't say $(OBJ) : $(SRC)
# or make will repeat the rule multiple times. That's why we use a dummy
$(DUMMY): $(SRC)
	omc buildfmi.mos
	rm -rf $(OBJ)
	$(foreach O,$(OBJ),unzip -q $(O).fmu -d $(O);)
	$(foreach O,$(OBJ),rm -f $(O).fmu;)
	$(foreach O,$(OBJ_UNDERSCORE),rm -f $(O)_FMU.* $(O).log;)
	touch $(DUMMY)

clean:
	rm -rf $(OBJ) $(DUMMY)
