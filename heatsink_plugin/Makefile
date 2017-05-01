
BIN = heatsink.so
OBJ = heatsink.o

CXX      = g++
CXXFLAGS = -fPIC -O2 -Wall -std=c++11
LDFLAGS  = -shared -Wl,-soname,$(BIN)

all: $(BIN)

clean:
	rm -f $(BIN) $(OBJ)

$(BIN): $(OBJ)
	$(CXX) $(LDFLAGS) -o $(BIN) $(OBJ)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@
