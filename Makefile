CXX      = g++
CXXFLAGS = -std=c++17 -O3 -Wall -Wextra $(shell sdl2-config --cflags)
LDFLAGS  = $(shell sdl2-config --libs)

TARGET  = rasterizerV3
SRCS    = Renderer.cpp
OBJS    = $(SRCS:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $@ $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $@ $<

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean
