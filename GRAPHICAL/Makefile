
CXX      := g++
CXXFLAGS := -std=c++23 -Wall -Wextra -Wno-unused-parameter -Ideps/TLOT/deps -Ideps/TLOT/deps/glad/include -Ideps/TLOT/include -Iinclude  -Ideps/TLOT/deps/imgui -Ideps/TLOT/deps/imgui/backends -Ideps/TLOT/deps/freeType/include
CXXFLAGS += -O3

LDFLAGS := 

BIN_WINDOWS := bin/scopalatro.exe
BIN_LINUX := bin/scopalatro
LIBS_WINDOWS:= -ltetoEngine -lglfw3 -lgdi32 -lopengl32 -lstdc++exp -lfreetype
LIBS_LINUX:= -ltetoEngine -lglfw3 -lstdc++exp -lfreetype


SRC := $(wildcard src/*.cpp)

ifeq ($(OS), Windows_NT)
LIBDIRS	:= -Ldeps/TLOT/libs/windows/glfw3 -Ldeps/TLOT/libs/windows/freeType -Ldeps/TLOT/bin/lib
LIBS	:= $(LIBS_WINDOWS)
BIN	    := $(BIN_WINDOWS)
else
LIBDIRS	:= -Ldeps/TLOT/libs/linux/glfw3 -Ldeps/TLOT/bin/lib
LIBS	:= $(LIBS_LINUX) #TODO
BIN 	:= $(BIN_LINUX)
endif

OBJ := $(SRC:.cpp=.o)

all : $(BIN)

$(BIN) : $(OBJ) deps/TLOT/bin/lib/libTetoEngine.a
	$(CXX) $(LDFLAGS) $(OBJ) $(LIBDIRS) $(LIBS) -o $(BIN)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

%.o: %.c
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
ifeq ($(OS), Windows_NT)
	del /q /s src\*.o scopalatro.exe
else
	rm -rf $(OBJ) $(BIN)
endif