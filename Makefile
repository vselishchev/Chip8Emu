CXX := g++
CXX_FLAGS := -std=c++17 -O0 -ggdb

BIN := bin
SRC := src

LIBRARIES := 
EXECUTABLE := Chip8Emu

all: $(BIN)/$(EXECUTABLE)

run: clean all
	clear
	./$(BIN)/$(EXECUTABLE)

$(BIN)/$(EXECUTABLE): $(SRC)/*.cpp
	$(CXX) $(CXX_FLAGS) -I$(SRC) $^ -o $@ $(LIBRARIES)

clean:
	-rm $(BIN)/*