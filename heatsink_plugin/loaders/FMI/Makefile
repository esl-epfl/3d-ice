
BIN = fmi_loader.so
OBJ = entrypoint.o fmiwrapper.o

CXX      = g++
CXXFLAGS = -fPIC -O2 -Wall -std=c++11
LDFLAGS  = -shared -Wl,-soname,$(BIN)
LDLIBS   = -lpugixml

all: $(BIN)

clean:
	rm -f $(BIN) $(OBJ)

$(BIN): $(OBJ)
	$(CXX) $(LDFLAGS) -o $(BIN) $(OBJ) $(LDLIBS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@
