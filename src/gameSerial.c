/*
 * gameSerial.c
   * Conway's Game of Life - Serial Implementation
   *
   * CSC 510 - Operating Systems
 * Description: Serial (single-process) simulation of Conway's Game of Life.
   *              Reads an NxN grid from a CSV file, simulates N iterations,
 *              and prints the board after each generation.
 *
 * Compile:  gcc -o gameSerial gameSerial.c
 * Run:      ./gameSerial
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LEN 1024

char FILENAME[100];
int N;
int **grid;
int **gridTemp;

int readFile() {
      int x, y;
    char line[MAX_LEN];
    char *token;
    FILE *file = fopen(FILENAME, "r");
    if (!file) { printf("Could not open input file.\n"); return 1; }
    N = 0;
    while (fgets(line, MAX_LEN, file) != NULL) N++;
    if (N == 0) { printf("Empty input file.\n"); return 1; }
    rewind(file);
    grid     = (int **)malloc(N * sizeof(int *));
    gridTemp = (int **)malloc(N * sizeof(int *));
    for (y = 0; y < N; y++) {
        grid[y]     = (int *)malloc(N * sizeof(int));
        gridTemp[y] = (int *)malloc(N * sizeof(int));
    }
    for (y = 0; y < N; y++) {
        if (fgets(line, MAX_LEN, file) == NULL) return 1;
        token = strtok(line, ",");
        for (x = 0; x < N; x++) {
            if (token == NULL) return 1;
            grid[y][x] = atoi(token);
            token = strtok(NULL, ",");
        }
    }
    fclose(file);
    return 0;
}

void printGrid() {
      for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) printf(grid[i][j] == 1 ? "*" : " ");
        printf("\n");
      }
}

int countNeighbourLiveCell(int r, int c) {
      int i, j, count = 0;
    for (i = r - 1; i <= r + 1; i++)
              for (j = c - 1; j <= c + 1; j++) {
            if ((i == r && j == c) || i < 0 || j < 0 || i >= N || j >= N) continue;
            if (grid[i][j] == 1) count++;
              }
    return count;
}

int updateGrid() {
      int flag = 0, i, j, nb;
    for (i = 0; i < N; i++)
              for (j = 0; j < N; j++) {
            nb = countNeighbourLiveCell(i, j);
            gridTemp[i][j] = (grid[i][j]==1 && (nb==2||nb==3)) ? 1 : (grid[i][j]==0 && nb==3) ? 1 : 0;
            if (grid[i][j] != gridTemp[i][j]) flag = 1;
              }
    for (i = 0; i < N; i++)
              for (j = 0; j < N; j++) grid[i][j] = gridTemp[i][j];
    return flag;
}

int main() {
      int iterations, flag = 1, i;
    printf("Enter the input filename with extension: ");
    scanf("%s", FILENAME);
    printf("Enter the number of iterations: ");
    scanf("%d", &iterations);
    if (readFile() != 0) return 1;
    for (i = 0; i < iterations && flag; i++) {
        printf("Generation %d:\n", i + 1);
        printGrid();
        flag = updateGrid();
    }
    for (i = 0; i < N; i++) { free(grid[i]); free(gridTemp[i]); }
    free(grid); free(gridTemp);
    return 0;
}
