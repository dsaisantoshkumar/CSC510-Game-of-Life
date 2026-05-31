# Makefile - CSC510 Conway's Game of Life
# Builds the serial and MPI parallel versions.
#
# Usage:
#   make            - build both
#   make serial     - serial only
#   make parallel   - MPI parallel only
#   make clean      - remove binaries

CC     = gcc
MPICC  := $(shell command -v mpicc 2>/dev/null || command -v mpicc.openmpi 2>/dev/null)
CFLAGS = -Wall -O2

.PHONY: all serial parallel clean

all: serial parallel

serial: src/gameSerial.c
	$(CC) $(CFLAGS) -o gameSerial src/gameSerial.c
	@echo "gameSerial built"

parallel: src/gameParallel.c
	$(MPICC) $(CFLAGS) -o gameParallel src/gameParallel.c
	@echo "gameParallel built"

clean:
	rm -f gameSerial gameParallel
