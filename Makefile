CC=c++
ARGS=exampleestringexamplestring
CPP_FLAGS = -std=c++17 -g

.PHONY: all build run

all: build run

build: main.cpp main.h
	$(CC) $(CPP_FLAGS) -o build/main main.cpp

run: build/main
	./build/main $(ARGS)