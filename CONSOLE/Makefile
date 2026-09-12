CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17 -Iinclude
TARGET = cards_proto
SRC = $(wildcard src/*.cpp)
OBJ = $(patsubst src/%.cpp, obj/%.o, $(SRC))

all: obj $(TARGET)

obj:
	mkdir -p obj

$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJ)

obj/%.o: src/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf obj/ $(TARGET)

dist:
	tar -cvzf cards_proto.tar.gz src/ include/ Makefile