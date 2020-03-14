.PHONY: all clean
.DEFAULT_GOAL := all

CC ?= gcc
BUILD ?= RELEASE
BASE_CFLAGS ?= -Wall
BUILD_DIR ?= out
RELEASE_CFLAGS ?= $(BASE_CFLAGS) -O3
DEBUG_CFLAGS ?= $(BASE_CFLAGS) -O0 -g -ggdb

COMMON_SRC = $(wildcard common/*.c)
COMMON_OBJ = $(patsubst %.c, %.o, $(COMMON_SRC))

# ------------------------------------------------------------------------------
# Debug or release
ifeq ($(BUILD),DEBUG)
CFLAGS ?= $(DEBUG_CFLAGS)
else
CFLAGS ?= $(RELEASE_CFLAGS)
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

$(WAL_INFO_OBJ): %o : %c
	$(CC) $(CFLAGS) -c $< -o $@

$(WAL_INFO_EXE): $(WAL_INFO_OBJ) $(COMMON_OBJ)
	$(CC) $(CFLAGS) $(WAL_INFO_OBJ) $(COMMON_OBJ) -o $@

# ------------------------------------------------------------------------------
all: $(WAL_INFO_EXE)
clean:
	-rm $(COMMON_OBJ)
	-rm $(WAL_INFO_OBJ)
	-rm $(WAL_INFO_EXE)

$(shell mkdir -p $(BUILD_DIR))