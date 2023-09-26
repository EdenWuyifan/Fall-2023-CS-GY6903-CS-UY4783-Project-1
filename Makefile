CC=c++
PYTHON=python
ARGS=exampleestringexamplestring
CPP_FLAGS = -std=c++17 -g -Wall -Wextra -fsanitize=address -fsanitize=undefined

SRC_DIR = src
INC_DIR = include
BUILD_DIR = build

SRCS = $(wildcard $(SRC_DIR)/*.cpp)
OBJS = $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(SRCS))

INC_DIRS = -I$(INC_DIR)
OUTPUT = $(BUILD_DIR)/main

.PHONY: all build enc clean

$(OBJ): main.cpp main.h Makefile
	@$(CC) -o build/main main.cpp $(CPP_FLAGS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CPP_FLAGS) $(INC_DIRS) -c $< -o $@

$(OUTPUT): $(OBJS)
	$(CC) $(CPP_FLAGS) $(OBJS) -o $(OUTPUT)

all: build
	@cat resources/cipher_1 | ./build/main 1
	@cat resources/cipher_2 | ./build/main 1
	@cat resources/cipher_3 | ./build/main 1
	@cat resources/cipher_4 | ./build/main 1
	@cat resources/cipher_5 | ./build/main 1

build: $(OUTPUT)

clean:
	rm -rf $(BUILD_DIR)

enc: enc.py
	$(PYTHON) enc.py $(ARGS) "1 2 3 4"
