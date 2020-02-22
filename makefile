CXX = g++
DOX = doxygen

EXE := ./bin/hw1
SRC_DIR := ./src
OBJ_DIR := ./obj
INC_DIR := ./inc
DOX_CONF := ./documentation-config.doxygen
DOX_DIR := ./doc/hw1

SRC := $(wildcard $(SRC_DIR)/*.cpp)
OBJ := $(SRC:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

CXXFLAGS   := -Wall -std=c++11 -lgtest -lpthread
LDFLAGS  := -Llib
LDLIBS   := -lm -lgtest -pthread

.PHONY: all clean

all: $(EXE)

$(EXE): $(OBJ)
	$(CXX) $(LDFLAGS) $^ $(LDLIBS) -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -I $(INC_DIR) -c $< -o $@

$(OBJ_DIR):
	mkdir $@

clean:
	$(RM) $(OBJ) $(EXE) -r $(DOX_DIR)

docs:
	$(DOX) $(DOX_CONF)

