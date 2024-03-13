CXX=g++
CXXFLAGS=-std=c++11 -Iinclude
SRC=main.cpp
OBJ=$(SRC:.cpp=.o)
EXEC=gcal-cli

all: $(EXEC)

$(EXEC): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^

clean:
	rm -f $(OBJ) $(EXEC)
