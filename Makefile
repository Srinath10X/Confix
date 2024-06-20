CXX = g++
CXXFLAGS = -g -Wall -Werror

MAIN_FILE = src/main.cpp
LIBS = -ljsoncpp
BIN_FILE_NAME = confix

all: main

main: $(MAIN_FILE)
	$(CXX) $(CXXFLAGS) $(MAIN_FILE) $(LIBS) -o $(BIN_FILE_NAME)

install:
	$(CXX) $(CXXFLAGS) $(MAIN_FILE) $(LIBS) -o $(BIN_FILE_NAME)
	mv $(BIN_FILE_NAME) $(shell echo "$$HOME")/.local/bin/

clean:
	rm -f $(BIN_FILE_NAME)
