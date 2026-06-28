
CXX = g++
CXXFLAGS = -std=c++17 -Wall -fsanitize=address
LIBS = -lGLEW -lSDL2 -lGL
TARGET = eng
SRC = $(wildcard src/*.cpp)
OBJ = $(patsubst src/%.cpp, build/%.o, $(SRC))

$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET) $(LIBS)

build/%.o: src/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

.PHONY: clean run
run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(OBJS)