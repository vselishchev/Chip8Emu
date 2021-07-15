CXX := g++
CXX_FLAGS := -std=c++17 -O0 -ggdb

BIN := bin
SRC := src
INCLUDE := /usr/include/SDL2

LIBRARIES := SDL2
EXECUTABLE := Chip8Emu

all: $(BIN)/$(EXECUTABLE)

run: clean all
	clear
	./$(BIN)/$(EXECUTABLE)

$(BIN)/$(EXECUTABLE): $(SRC)/*.cpp
	$(CXX) $(CXX_FLAGS) -I$(INCLUDE) $^ -o $@ -l$(LIBRARIES)

clean:
	-rm $(BIN)/*