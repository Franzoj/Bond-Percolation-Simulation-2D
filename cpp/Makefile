# Variables
CXX = g++
CXXFLAGS = -std=c++11
TARGET = main
SRC = main.cpp
PKG_CONFIG = `pkg-config --cflags --libs opencv4`

# Default target
all: $(TARGET)

# Compiling the program
$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRC) $(PKG_CONFIG)

# Clean up
clean:
	rm -f $(TARGET)

# PHONY target
.PHONY: all clean