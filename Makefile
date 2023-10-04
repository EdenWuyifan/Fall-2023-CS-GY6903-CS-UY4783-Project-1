CC=c++
PYTHON=python
ARGS=exampleestringexamplestring
CPP_FLAGS = -std=c++17 -g -Wall -Wextra -fsanitize=address -fsanitize=undefined
KEY_LEN=4
SEARCH_SPACE=120

KEY_LENS = 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24

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
	@echo "Running with key length $(KEY_LEN) and search space $(SEARCH_SPACE)"
	@mkdir -p results/$(SEARCH_SPACE)
	- @cat resources/key_$(KEY_LEN)/cipher_1 | ./build/main 1 $(SEARCH_SPACE) > results/$(SEARCH_SPACE)/$(KEY_LEN)_1.out 2> results/$(SEARCH_SPACE)/$(KEY_LEN)_1.err
	- @cat resources/key_$(KEY_LEN)/cipher_2 | ./build/main 1 $(SEARCH_SPACE) > results/$(SEARCH_SPACE)/$(KEY_LEN)_2.out 2> results/$(SEARCH_SPACE)/$(KEY_LEN)_2.err
	- @cat resources/key_$(KEY_LEN)/cipher_3 | ./build/main 1 $(SEARCH_SPACE) > results/$(SEARCH_SPACE)/$(KEY_LEN)_3.out 2> results/$(SEARCH_SPACE)/$(KEY_LEN)_3.err
	- @cat resources/key_$(KEY_LEN)/cipher_4 | ./build/main 1 $(SEARCH_SPACE) > results/$(SEARCH_SPACE)/$(KEY_LEN)_4.out 2> results/$(SEARCH_SPACE)/$(KEY_LEN)_4.err
	- @cat resources/key_$(KEY_LEN)/cipher_5 | ./build/main 1 $(SEARCH_SPACE) > results/$(SEARCH_SPACE)/$(KEY_LEN)_5.out 2> results/$(SEARCH_SPACE)/$(KEY_LEN)_5.err

test: build
	@$(foreach key_len,$(KEY_LENS),$(MAKE) SEARCH_SPACE=$(SEARCH_SPACE) KEY_LEN=$(key_len) all;)
	@python evaluate.py $(SEARCH_SPACE)

build: $(OUTPUT)

clean:
	rm -rf $(BUILD_DIR)

enc: enc.py
	$(PYTHON) enc.py $(ARGS) "1 2 3 4"
