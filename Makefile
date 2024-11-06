CXX = g++
CXXFLAGS = -g -Wall -Werror

LIBS = -ljsoncpp
SRC_FILE = src/main.cpp
BIN_FILE_NAME = confix
NIX_OUTPUT_BIN_DIR = result

all: main

main: $(SRC_FILE)
	$(CXX) $(CXXFLAGS) $(SRC_FILE) $(LIBS) -o $(BIN_FILE_NAME)

install: $(SRC_FILE)
	$(CXX) $(CXXFLAGS) $(SRC_FILE) $(LIBS) -o $(BIN_FILE_NAME)
	mkdir -p $(HOME)/.local/bin/
	mv $(BIN_FILE_NAME) $(HOME)/.local/bin/

clean:
	rm -f $(BIN_FILE_NAME) $(NIX_OUTPUT_BIN_DIR)
