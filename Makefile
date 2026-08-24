CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -pedantic -pthread
TARGET = portscanner
SRC = main.cpp

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET)

clean:
	rm -f $(TARGET)


