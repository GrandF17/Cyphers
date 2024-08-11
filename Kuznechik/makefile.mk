CXX = g++
CXXFLAGS = -pthread
SRCDIR = libs/src
SRCS = $(wildcard $(SRCDIR)/*.cpp)
MAIN = main.cpp
TARGET = main

all: $(TARGET)

$(TARGET): $(MAIN) $(SRCS)
	$(CXX) $(CXXFLAGS) $(MAIN) $(SRCS) -o $(TARGET)

clean:
	rm -f $(TARGET)