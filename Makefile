CC=c++
PYTHON=python
ARGS=exampleestringexamplestring
CPP_FLAGS = -std=c++17 -g -Wall -Wextra -fsanitize=address -fsanitize=undefined
KEY_LEN=4
EXPAND=2

SRC_DIR = src
INC_DIR = include
BUILD_DIR = build

SRCS = $(wildcard $(SRC_DIR)/*.cpp)
OBJS = $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(SRCS))

INC_DIRS = -I$(INC_DIR)
OUTPUT = $(BUILD_DIR)/main

.PHONY: all build enc clean

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CPP_FLAGS) $(INC_DIRS) -c $< -o $@

$(OUTPUT): $(OBJS)
	$(CC) $(CPP_FLAGS) $(OBJS) -o $(OUTPUT)

all: build
	@cat resources/key_$(KEY_LEN)/cipher_1 | ./build/main 1 $(EXPAND) > results/$(KEY_LEN)_$(EXPAND)_1.out 2> results/$(KEY_LEN)_$(EXPAND)_1.err
	@cat resources/key_$(KEY_LEN)/cipher_2 | ./build/main 1 $(EXPAND) > results/$(KEY_LEN)_$(EXPAND)_2.out 2> results/$(KEY_LEN)_$(EXPAND)_2.err
	@cat resources/key_$(KEY_LEN)/cipher_3 | ./build/main 1 $(EXPAND) > results/$(KEY_LEN)_$(EXPAND)_3.out 2> results/$(KEY_LEN)_$(EXPAND)_3.err
	@cat resources/key_$(KEY_LEN)/cipher_4 | ./build/main 1 $(EXPAND) > results/$(KEY_LEN)_$(EXPAND)_4.out 2> results/$(KEY_LEN)_$(EXPAND)_4.err
	@cat resources/key_$(KEY_LEN)/cipher_5 | ./build/main 1 $(EXPAND) > results/$(KEY_LEN)_$(EXPAND)_5.out 2> results/$(KEY_LEN)_$(EXPAND)_5.err

build: $(OUTPUT)

clean:
	rm -rf $(BUILD_DIR)

enc: enc.py
	$(PYTHON) enc.py $(ARGS) "1 2 3 4"
