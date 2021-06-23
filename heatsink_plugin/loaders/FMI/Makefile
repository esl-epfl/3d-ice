3DICE_MAIN = ../../..

include $(3DICE_MAIN)/makefile.def

BIN = fmi_loader.so
OBJ = entrypoint.o fmiwrapper.o \
      libraries/fmi-interface/fmi_interface.o \
      libraries/gridmapper/gridmapper.o

CXXFLAGS += -fPIC
LDFLAGS  = -fPIC -shared -Wl,-soname,$(BIN)
LDLIBS   = -lpugixml

all: $(BIN)

clean:
	rm -f $(BIN) $(OBJ)

$(BIN): $(OBJ)
	$(CXX) $(LDFLAGS) -o $(BIN) $(OBJ) $(LDLIBS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@
