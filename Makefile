CXX=g++
CXXFLAGS=-std=c++11 -I./include
SRC_DIR=./src
SRC=$(SRC_DIR)/main.cpp $(SRC_DIR)/ConfigManager.cpp # Add new .cpp files here
OBJ=$(SRC:.cpp=.o)
EXEC=gcal-cli

all: $(EXEC)

$(EXEC): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^

clean:
	rm -f $(SRC_DIR)/*.o $(EXEC)
