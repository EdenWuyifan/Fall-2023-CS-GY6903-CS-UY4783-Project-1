.PHONY: all build run

CPP_FLAGS = -std=c++17 -g

all: build run

build:
	c++ $(CPP_FLAGS) -o build/main main.cpp

run: build
	./build/main $(ARGS)