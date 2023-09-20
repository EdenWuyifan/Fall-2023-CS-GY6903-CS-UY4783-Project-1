CC=c++
PYTHON=python
ARGS=exampleestringexamplestring
CPP_FLAGS = -std=c++17 -g

OBJ=build/main

.PHONY: all build run enc

all: build run

$(OBJ): main.cpp main.h
	$(CC) $(CPP_FLAGS) -o build/main main.cpp

build: $(OBJ)

run: $(OBJ)
	./build/main $(ARGS)

enc: enc.py
	$(PYTHON) enc.py $(ARGS)