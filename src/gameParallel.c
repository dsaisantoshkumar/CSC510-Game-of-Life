/*
 * gameParallel.c
 * Conway's Game of Life - MPI Parallel Implementation
 *
 * CSC 510 - Operating Systems
 * Compile:  mpicc -Wall -O2 -o gameParallel gameParallel.c
 * Run:      mpirun -np <N> ./gameParallel
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>

#define ALIVE 1
#define DEAD  0
#define MAX_GRID 512

static int board[MAX_GRID][MAX_GRID];
static int new_board[MAX_GRID][MAX_GRID];
static char FILENAME[256];
static int N = 0;

static int read_Input_File(void) {
    char line[8192];
    char *token;
    int  y, x;
    FILE *fp = fopen(FILENAME, "r");
    if (!fp) { printf("Cannot open %s\n", FILENAME); return 1; }
    N = 0;
    while (fgets(line, sizeof line, fp)) N++;
    if (N == 0) { fclose(fp); return 1; }
    rewind(fp);
    for (y = 0; y < N; y++) {
        if (!fgets(line, sizeof line, fp)) { fclose(fp); return 1; }
        token = strtok(line, ",");
        for (x = 0; x < N; x++) {
            if (!token) { fclose(fp); return 1; }
            board[y][x] = atoi(token);
            token = strtok(NULL, ",");
        }
    }
    fclose(fp);
    return 0;
}

static void print_Board(void) {
    int i, j;
    for (i = 0; i < N; i++) {
        for (j = 0; j < N; j++) putchar(board[i][j] == ALIVE ? '*' : ' ');
        putchar('\n');
    }
}

static int count_Neighbors(int y, int x) {
    int count = 0, dy, dx, ny, nx;
    for (dy = -1; dy <= 1; dy++)
        for (dx = -1; dx <= 1; dx++) {
            if (dy == 0 && dx == 0) continue;
            ny = y + dy; nx = x + dx;
            if (ny >= 0 && ny < N && nx >= 0 && nx < N) count += board[ny][nx];
        }
    return count;
}

static void update_Board(void) {
    int i, j, nb;
    for (i = 0; i < N; i++)
        for (j = 0; j < N; j++) {
            nb = count_Neighbors(i, j);
            new_board[i][j] = (board[i][j]==ALIVE) ? ((nb==2||nb==3)?ALIVE:DEAD) : ((nb==3)?ALIVE:DEAD);
        }
    memcpy(board, new_board, sizeof(int)*N*N);
}

int main(int argc, char **argv) {
    int rank, num_procs, num_iterations, iter, top, bottom;
    double t_start, t_end, local_t, elapsed;
    MPI_Status status;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);
    if (rank == 0) {
        printf("Enter input file name (with extension): "); fflush(stdout);
        if (scanf("%255s", FILENAME) != 1) { MPI_Abort(MPI_COMM_WORLD, 1); }
        if (read_Input_File()) { MPI_Abort(MPI_COMM_WORLD, 1); }
        printf("Grid size: %d x %d\n", N, N);
        printf("Enter number of iterations: "); fflush(stdout);
        if (scanf("%d", &num_iterations) != 1) { MPI_Abort(MPI_COMM_WORLD, 1); }
    }
    MPI_Bcast(&N, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&num_iterations, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(board, N*N, MPI_INT, 0, MPI_COMM_WORLD);
    t_start = MPI_Wtime();
    for (iter = 0; iter < num_iterations; iter++) {
        if (rank == 0) { printf("\n--- Iteration %d ---\n", iter+1); print_Board(); }
        MPI_Barrier(MPI_COMM_WORLD);
        top = (rank-1+num_procs)%num_procs; bottom = (rank+1)%num_procs;
        MPI_Sendrecv(board[0], N, MPI_INT, top, 0, board[N-1], N, MPI_INT, bottom, 0, MPI_COMM_WORLD, &status);
        MPI_Sendrecv(board[N-1], N, MPI_INT, bottom, 1, board[0], N, MPI_INT, top, 1, MPI_COMM_WORLD, &status);
        update_Board();
        MPI_Bcast(board, N*N, MPI_INT, 0, MPI_COMM_WORLD);
    }
    t_end = MPI_Wtime(); local_t = t_end - t_start;
    MPI_Reduce(&local_t, &elapsed, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);
    if (rank == 0) printf("\nElapsed time = %.6f seconds (%d procs, %d iters)\n", elapsed, num_procs, num_iterations);
    MPI_Finalize();
    return 0;
}
