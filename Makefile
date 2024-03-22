CXX=g++
CXXFLAGS=-std=c++17 -I./include
SRC_DIR=./src
SRC=$(SRC_DIR)/main.cpp $(SRC_DIR)/GoogleOauth.cpp $(SRC_DIR)/ConfigManager.cpp $(SRC_DIR)/TokenManager.cpp 
OBJ=$(SRC:.cpp=.o)
EXEC=gcal-cli

all: $(EXEC)

$(EXEC): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^

clean:
	rm -f $(SRC_DIR)/*.o $(EXEC)
