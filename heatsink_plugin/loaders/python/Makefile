
BIN = py_loader.so
OBJ = entrypoint.o pythonwrapper.o

PYINCLUDE= `pkg-config --cflags python3-embed`
PYLIBS   = `pkg-config --libs python3-embed`

CXX      = g++
CXXFLAGS = -fPIC -O2 -Wall -std=c++11 $(PYINCLUDE) -I/usr/include/numpy
LDFLAGS  = -fPIC -shared -Wl,-soname,$(BIN)
LDLIBS   = $(PYLIBS)

all: $(BIN)

clean:
	rm -rf $(BIN) $(OBJ)

$(BIN): $(OBJ)
	$(CXX) $(LDFLAGS) -o $(BIN) $(OBJ) $(LDLIBS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@
