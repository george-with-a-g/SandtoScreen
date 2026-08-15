CXX ?= g++
CXXFLAGS ?= -O2 -Wall -Wextra -std=c++17

TARGET = toy_emulator

all: $(TARGET)

$(TARGET): toy_emulator.cpp
	$(CXX) $(CXXFLAGS) -o $(TARGET) toy_emulator.cpp

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET)

.PHONY: all run clean
