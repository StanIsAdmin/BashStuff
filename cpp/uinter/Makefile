#Compiler options
FLAGS = -std=c++11 -Wall -Wextra

all: uinter

uinter: uinter.cpp
	mkdir -p bin
	g++ $(FLAGS) uinter.cpp -o bin/uinter
	
clean:
	rm bin/uinter