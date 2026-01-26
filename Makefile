# Makefile for PA01: Processes and Threads
# Roll No: MT25045
#
# Usage:
#   make          - Build all programs
#   make clean    - Remove all compiled files

CC = gcc
CFLAGS = -Wall -Wextra -O2
LDFLAGS = -lpthread -lm

ROLL = MT25045

PROG_A = program_a
PROG_B = program_b

.PHONY: all clean help

all: $(PROG_A) $(PROG_B)

$(PROG_A): $(ROLL)_Part_A_Program_A.c $(ROLL)_Part_B_workers.c $(ROLL)_Part_B_workers.h
	$(CC) $(CFLAGS) -o $@ $(ROLL)_Part_A_Program_A.c $(ROLL)_Part_B_workers.c $(LDFLAGS)

$(PROG_B): $(ROLL)_Part_A_Program_B.c $(ROLL)_Part_B_workers.c $(ROLL)_Part_B_workers.h
	$(CC) $(CFLAGS) -o $@ $(ROLL)_Part_A_Program_B.c $(ROLL)_Part_B_workers.c $(LDFLAGS)

clean:
	rm -f $(PROG_A) $(PROG_B)
	rm -f /tmp/io_worker_*.dat

help:
	@echo "Usage:"
	@echo "  make       - Build program_a and program_b"
	@echo "  make clean - Remove compiled files"
