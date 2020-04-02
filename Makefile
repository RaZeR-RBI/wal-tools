.PHONY: all clean
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
# Executables
WAL_INFO_EXE = $(BUILD_DIR)/wal-info$(EXEEXT)
WAL_INFO_SRC = src/wal-info/wal-info.c
WAL_INFO_OBJ = $(patsubst %.c, %.o, $(WAL_INFO_SRC))

$(WAL_INFO_OBJ): %.o : %.c
	$(CC) $(CFLAGS) -std=$(C_VERSION) -c $< -o $@

$(WAL_INFO_EXE): $(WAL_INFO_OBJ) $(COMMON_OBJ)
	$(CC) $(CFLAGS) $(WAL_INFO_OBJ) $(COMMON_OBJ) -o $@



WAL_EXPORT_EXE = $(BUILD_DIR)/wal-export$(EXEEXT)
WAL_EXPORT_SRC = src/wal-export/wal-export.c
WAL_EXPORT_OBJ = $(patsubst %.c, %.o, $(WAL_EXPORT_SRC))

$(WAL_EXPORT_OBJ): %.o : %.c
	$(CC) $(CFLAGS) -std=$(C_VERSION) -c $< -o $@

$(WAL_EXPORT_EXE): $(WAL_EXPORT_OBJ) $(COMMON_OBJ)
	$(CC) $(CFLAGS) $(WAL_EXPORT_OBJ) $(COMMON_OBJ) -o $@



WAL_TERM_EXE = $(BUILD_DIR)/wal-term$(EXEEXT)
WAL_TERM_SRC = src/wal-term/wal-term.c
WAL_TERM_OBJ = $(patsubst %.c, %.o, $(WAL_TERM_SRC))

$(WAL_TERM_OBJ): %.o : %.c
	$(CC) $(CFLAGS) -std=$(C_VERSION) -c $< -o $@

$(WAL_TERM_EXE): $(WAL_TERM_OBJ) $(COMMON_OBJ)
	$(CC) $(CFLAGS) $(WAL_TERM_OBJ) $(COMMON_OBJ) -o $@

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
all: $(WAL_INFO_EXE) $(WAL_EXPORT_EXE) $(WAL_TERM_EXE)
tests: $(COMMON_OBJ) $(TESTS_EXE)
clean:
	-rm -f $(COMMON_OBJ)
	-rm -f $(WAL_INFO_OBJ)
	-rm -f $(WAL_INFO_EXE)
	-rm -f $(TESTS_OBJ)
	-rm -f $(TESTS_EXE)
	-find . -type f -name '*.gcov' -delete
	-find . -type f -name '*.gcno' -delete
	-find . -type f -name '*.gcda' -delete
	-find . -type f -name '*.gcda.info' -delete
	-rm -rf docs/coverage/*

$(shell mkdir -p $(BUILD_DIR))