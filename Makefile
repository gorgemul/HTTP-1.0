CC = gcc
CFLAGS = -std=c11 -pedantic -Wall -Wextra -Wpointer-arith -Wwrite-strings -Wswitch-default -Wunreachable-code -Winit-self -Wmissing-field-initializers -Wno-unknown-pragmas -Wstrict-prototypes -Wundef -Wold-style-definition

SRC_DIR = src
BUILD_DIR = build
EXE_DIR = bin
TEST_DIR = tests

TEST_LIB = lib/unity/unity.c

EXE = $(EXE_DIR)/httpclient

SRCS = $(wildcard $(SRC_DIR)/*.c)

OBJS = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRCS))

TESTS = $(wildcard $(TEST_DIR)/*.c)
TEST_EXE = $(patsubst $(TEST_DIR)/test_%.c,$(EXE_DIR)/test_%, $(TESTS))

# Color definitions
COLOR_GREEN = \033[0;32m
COLOR_YELLOW = \033[1;33m
COLOR_RESET = \033[0m

.PHONY: test

run: client
	@echo "$(COLOR_YELLOW)****** RUNNING EXE ******$(COLOR_RESET)"
	@$(EXE)

client: $(EXE)

clean:
	@rm -rf $(BUILD_DIR) $(EXE_DIR)
	@echo "$(COLOR_GREEN)****** CLEAN SUCCESS ******$(COLOR_RESET)"

test: $(TEST_EXE)
	@echo "$(COLOR_YELLOW)****** RUNNING ALL TESTS ******$(COLOR_RESET)"
	@for exe in $(TEST_EXE); do \
		echo "======> Running $$exe..."; \
		$$exe; \
	done
	@echo "$(COLOR_YELLOW)****** ALL TESTS FINISHED ******$(COLOR_RESET)"

$(EXE): $(OBJS) | $(EXE_DIR)
	@echo "$(COLOR_YELLOW)****** GENERATING EXE ******$(COLOR_RESET)"
	@$(CC) -o $@ $^
	@echo "$(COLOR_GREEN)****** BUILD SUCESS ******$(COLOR_RESET)"

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	@echo "$(COLOR_YELLOW)****** COMPILING $< ******$(COLOR_RESET)"
	@$(CC) $(CFLAGS) -c $< -o $@

$(EXE_DIR)/test_%: $(SRC_DIR)/%.c $(TEST_DIR)/test_%.c | $(EXE_DIR)
	@$(CC) $(CFLAGS) $^ $(TEST_LIB) -o $@

$(BUILD_DIR):
	@mkdir -p $@

$(EXE_DIR):
	@mkdir -p $@
