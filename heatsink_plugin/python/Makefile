
BIN = heatsink_plugin_python.so
OBJ = entrypoint.o pythonwrapper.o

CXX      = g++
CXXFLAGS = -fPIC -O2 -Wall -std=c++11 -I/usr/include/python3.6m -I/usr/include/numpy
LDFLAGS  = -shared -Wl,-soname,$(BIN)
LDLIBS   = -lpython3.6m

all: $(BIN)

clean:
	rm -rf $(BIN) $(OBJ) heatsink.pyc __pycache__

$(BIN): $(OBJ)
	$(CXX) $(LDFLAGS) -o $(BIN) $(OBJ) $(LDLIBS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@
