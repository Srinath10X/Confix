CXX = g++
CXXFLAGS = -g -Wall -Werror

MAIN_FILE = main.cpp

all: main

main: $(MAIN_FILE)
	$(CXX) $(CXXFLAGS) $(MAIN_FILE) -o confix

install:
	mv confix-beta $(shell echo "$$HOME")/.local/bin/

clean:
	rm -f confix-beta confix-test
