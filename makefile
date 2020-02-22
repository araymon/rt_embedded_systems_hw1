CXX = g++
DOX = doxygen

EXE := ./bin/hw1
BIN_DIR := ./bin
SRC_DIR := ./src
OBJ_DIR := ./obj
INC_DIR := ./inc
DOX_CONF := ./documentation-config.doxygen
DOX_DIR := ./doc

SRC := $(wildcard $(SRC_DIR)/*.cpp)
OBJ := $(SRC:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

CXXFLAGS   := -Wall -std=c++11 -lgtest -lpthread
LDFLAGS  := -Llib
LDLIBS   := -lm -lgtest -pthread

.PHONY: all clean

all: $(EXE)

$(EXE): $(OBJ) | $(BIN_DIR)
	$(CXX) $(LDFLAGS) $^ $(LDLIBS) -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -I $(INC_DIR) -c $< -o $@

$(OBJ_DIR):
	mkdir $@

$(BIN_DIR):
	mkdir $@

$(DOX_DIR):
	mkdir -p $@

clean:
	$(RM) -r $(OBJ_DIR) $(BIN_DIR) $(DOX_DIR)

docs: $(DOX_DIR)
	$(DOX) $(DOX_CONF)


