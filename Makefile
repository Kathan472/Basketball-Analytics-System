# Compiler and Flags
CXX = g++
CXXFLAGS = -std=c++17 -Wall

# Target executable
TARGET = game

# Default target
all: $(TARGET)

# Compile the program
$(TARGET):
	$(CXX) $(CXXFLAGS) -o $(TARGET) *.cpp

# Run the program
run: $(TARGET)
	./$(TARGET)

# Clean compiled files
clean:
	rm -f $(TARGET)