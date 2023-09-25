CC=c++
PYTHON=python
ARGS=exampleestringexamplestring
CPP_FLAGS = -std=c++17 -g -Wall -Wextra -fsanitize=address -fsanitize=undefined

OBJ=build/main

.PHONY: all build run enc clean

all: build
	@cat resources/cipher_1 | ./build/main 1
	@cat resources/cipher_2 | ./build/main 1
	@cat resources/cipher_3 | ./build/main 1
	@cat resources/cipher_4 | ./build/main 1
	@cat resources/cipher_5 | ./build/main 1

build: $(OBJ)

run: $(OBJ)
	./build/main

clean:
	rm build/main

enc: enc.py
	$(PYTHON) enc.py $(ARGS) "1 2 3 4"

$(OBJ): main.cpp main.h Makefile
	@$(CC) -o build/main main.cpp $(CPP_FLAGS)
