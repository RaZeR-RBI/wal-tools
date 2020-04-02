.PHONY: all clean tests
.DEFAULT_GOAL := all

C_VERSION ?= c89
CC ?= gcc
BUILD ?= DEBUG
BASE_CFLAGS ?= -Wall -fPIC
COVERAGE_FLAGS ?= -ftest-coverage -fprofile-arcs
BUILD_DIR ?= out
BASE_LDFLAGS ?=
RELEASE_CFLAGS ?= $(BASE_CFLAGS) -O3
DEBUG_CFLAGS ?= $(BASE_CFLAGS) -O0 -g -ggdb $(COVERAGE_FLAGS)
RELEASE_LDFLAGS ?= $(BASE_LDFLAGS)
DEBUG_LDFLAGS ?= $(BASE_LDFLAGS) -lgcov --coverage

COMMON_SRC = $(wildcard common/*.c)
COMMON_OBJ = $(patsubst %.c, %.o, $(COMMON_SRC))

# ------------------------------------------------------------------------------
# Debug or release
ifeq ($(BUILD),DEBUG)
CFLAGS ?= $(DEBUG_CFLAGS)
LDFLAGS ?= $(DEBUG_LDFLAGS)
else
CFLAGS ?= $(RELEASE_CFLAGS)
LDFLAGS ?= $(RELEASE_LDFLAGS)
endif

# ------------------------------------------------------------------------------
# Windows-specific
EXEEXT ?=
ifeq ($(OS),Windows_NT)
	EXEEXT := .exe
endif

# ------------------------------------------------------------------------------
# Common objects
$(COMMON_OBJ): %o : %c
	$(CC) $(CFLAGS) -c $< -o $@

# ------------------------------------------------------------------------------
EXE_SRC = $(wildcard src/*.c)
EXE_OBJ = $(patsubst %.c, %.o, $(EXE_SRC))
EXE_LIST = $(addsuffix $(EXEEXT), $(notdir $(basename $(EXE_SRC))))
EXE_OUT = $(patsubst %, $(BUILD_DIR)/%$(EXEEXT), $(EXE_LIST))

$(EXE_OBJ): %.o : %.c
	$(CC) $(CFLAGS) -std=$(C_VERSION) -c $< -o $@

$(EXE_OUT): $(COMMON_OBJ) $(EXE_OBJ)
	$(eval OBJ_NAME=src/$(notdir $(basename $@)).o)
	$(CC) $(CFLAGS) $(OBJ_NAME) -std=$(C_VERSION) $(COMMON_OBJ) -o $@

# ------------------------------------------------------------------------------
# Tests
TESTS_SRC = $(wildcard tests/*.c)
TESTS_OBJ = $(patsubst %.c, %.o, $(TESTS_SRC))
TESTS_EXE = $(patsubst %.o, %.out$(EXEEXT), $(TESTS_OBJ))
$(TESTS_OBJ): %.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(TESTS_EXE): %.out : %.o
	$(CC) $(LDFLAGS) -lcmocka $< $(COMMON_OBJ) -o $@

# ------------------------------------------------------------------------------
all: $(EXE_OUT)
tests: $(COMMON_OBJ) $(TESTS_EXE)
clean:
	-rm -f $(COMMON_OBJ)
	-rm -f $(EXE_OBJ)
	-rm -f $(EXE_OUT)
	-rm -f $(TESTS_OBJ)
	-rm -f $(TESTS_EXE)
	-find . -type f -name '*.gcov' -delete
	-find . -type f -name '*.gcno' -delete
	-find . -type f -name '*.gcda' -delete
	-find . -type f -name '*.gcda.info' -delete
	-rm -rf docs/coverage/*

$(shell mkdir -p $(BUILD_DIR))