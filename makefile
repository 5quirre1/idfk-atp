CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2

LDFLAGS = -lraylib -lgdi32 -lwinmm -lshell32 -lopengl32

SRC = $(wildcard src/*.cpp)
OBJ = $(SRC:.cpp=.o)
EXEC = greg.exe

all: $(EXEC)

$(EXEC): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(EXEC)
.PHONY: all clean
