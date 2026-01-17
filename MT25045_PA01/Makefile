# Compiler and flags
CC      = gcc
CFLAGS  = -O2 -Wall -Wextra
LDFLAGS = -pthread

# Directories
SRC_DIR = src

# Targets
PROG_A = progA
PROG_B = progB

# Source files
WORKERS_SRC = $(SRC_DIR)/workers.c
PROGA_SRC   = $(SRC_DIR)/progA.c
PROGB_SRC   = $(SRC_DIR)/progB.c

# Default target
all: $(PROG_A) $(PROG_B)

# Build Program A (processes)
$(PROG_A): $(PROGA_SRC) $(WORKERS_SRC)
	$(CC) $(CFLAGS) -o $@ $^

# Build Program B (threads)
$(PROG_B): $(PROGB_SRC) $(WORKERS_SRC)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# Clean build artifacts
clean:
	rm -f $(PROG_A) $(PROG_B) $(SRC_DIR)/*.o

.PHONY: all clean


