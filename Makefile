CC=c++
PYTHON=python
ARGS=exampleestringexamplestring
CPP_FLAGS = -std=c++17 -g -Wall -Wextra -fsanitize=address -fsanitize=undefined

OBJ=build/main

.PHONY: all build run enc clean

all: build

build: $(OBJ)

run: $(OBJ)
	./build/main $(ARGS)

clean:
	rm build/main

enc: enc.py
	$(PYTHON) enc.py $(ARGS) "1 2 3 4"

$(OBJ): main.cpp main.h Makefile
	$(CC) -o build/main main.cpp $(CPP_FLAGS)
