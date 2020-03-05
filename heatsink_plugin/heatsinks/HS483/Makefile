
SRC = ../../common/libraries/ThermalBlocks.mo ../../common/HeatsinkBlocks.mo HS483.mo
OBJ = HS483.Specializations.HS483_NaturalConvection_Interface3DICE

# OpenModelica produces temporary files with a name where . is replaced with _
DOT = .
UNDERSCORE = _
OBJ_UNDERSCORE = $(subst $(DOT),$(UNDERSCORE),$(OBJ))

all: $(OBJ)

$(OBJ): $(SRC)
	# Making an FMI from OpenModelica requires to use its own
	# build system scripting language
	omc buildfmi.mos
	rm -rf $(OBJ)
	unzip -q $(OBJ).fmu -d $(OBJ)
	rm -f $(OBJ_UNDERSCORE)_FMU.* $(OBJ_UNDERSCORE).log $(OBJ).fmu

clean:
	rm -rf $(OBJ)
