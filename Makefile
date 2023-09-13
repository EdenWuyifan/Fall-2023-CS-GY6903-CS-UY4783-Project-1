.PHONY: all build run

all: build run

build:
	c++ -std=c++17 -o build/main main.cpp

run: build
	./build/main $(ARGS)