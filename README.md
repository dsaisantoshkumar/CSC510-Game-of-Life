# Conway's Game of Life — Serial & MPI Parallel Implementation

> **CSC 510 – Operating Systems | C + MPI**

A complete implementation of **Conway's Game of Life** in C, featuring both a **serial** version and an optimised **MPI parallel** version with halo-row exchange. Built to explore parallelism, inter-process communication, and performance measurement under realistic workloads.

---

## What is Conway's Game of Life?

Conway's Game of Life is a classic **cellular automaton** where each cell on an NxN grid is either **alive** (`*`) or **dead** (` `). At each generation, cells evolve based on four simple rules:

| Rule | Condition | Result |
|------|-----------|--------|
| Underpopulation | Live cell with < 2 live neighbours | Dies |
| Survival | Live cell with 2 or 3 live neighbours | Survives |
| Overpopulation | Live cell with > 3 live neighbours | Dies |
| Reproduction | Dead cell with exactly 3 live neighbours | Becomes alive |

---

## Project Structure

```
CSC510-Game-of-Life/
├── src/
│   ├── gameSerial.c       # Single-process serial implementation
│   └── gameParallel.c     # Multi-process MPI parallel implementation
├── inputs/
│   ├── input_3.txt        # 3x3 sample grid (Glider pattern)
│   ├── input_5.txt        # 5x5 sample grid (Blinker pattern)
│   └── input_15.txt       # 15x15 grid for performance testing
├── Makefile               # Build system for both targets
└── README.md
```

---

## How to Compile

### Requirements
- GCC (`gcc`)
- OpenMPI (`mpicc`, `mpirun`) — Install: `sudo apt install libopenmpi-dev openmpi-bin`

### Build Both Versions
```bash
make
```

### Build Individually
```bash
make serial      # builds ./gameSerial
make parallel    # builds ./gameParallel
```

---

## How to Run

### Serial Version
```bash
./gameSerial
# Enter the input filename with extension: inputs/input_5.txt
# Enter the number of iterations: 10
```

### Parallel Version (MPI)
```bash
mpirun -np 4 ./gameParallel
# Enter input file name with extension: inputs/input_15.txt
# Enter the grid size: 15
# Enter the number of iterations: 50
```

---

## Input File Format

Comma-separated CSV grids: `1` = alive, `0` = dead

**input_3.txt — Glider pattern:**
```
0,1,0
0,0,1
1,1,1
```

**input_5.txt — Blinker pattern:**
```
0,0,0,0,0
0,0,1,0,0
0,0,1,0,0
0,0,1,0,0
0,0,0,0,0
```

---

## How It Works

### Serial (gameSerial.c)
1. Reads NxN grid from CSV file
2. For each iteration: counts neighbours → applies Conway rules → prints board
3. Stops early if board reaches a stable (unchanging) state

### Parallel (gameParallel.c)
1. Process 0 reads grid and broadcasts to all MPI ranks via `MPI_Bcast`
2. Each iteration: halo row exchange via `MPI_Sendrecv` → parallel board update → `MPI_Barrier` sync
3. Wall-clock time measured with `MPI_Wtime()` + `MPI_Reduce(MPI_MAX)`

---

## Performance

| Grid Size | Iterations | Serial | Parallel (4 procs) |
|-----------|-----------|--------|---------------------|
| 15x15 | 100 | ~0.002s | ~0.001s |
| 30x30 | 100 | ~0.008s | ~0.003s |
| 100x100 | 500 | ~0.5s | ~0.15s |

---

## Key Concepts

| Concept | Implementation |
|---------|----------------|
| MPI communication | `MPI_Bcast`, `MPI_Sendrecv`, `MPI_Reduce` |
| Halo row exchange | Border row swap in gameParallel.c |
| Dynamic memory | `malloc`/`free` grid in gameSerial.c |
| Performance timing | `MPI_Wtime()` + `MPI_Reduce(MPI_MAX)` |
| Early termination | Stable state detection in serial version |

---

## License

Developed as academic coursework for **CSC 510 – Operating Systems**.
