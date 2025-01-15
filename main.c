#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>
#include <string.h>
#include "timer.h"
#include "lab1_IO.h"

int P, Ps; // Number of thread and its square root
int **A, **B, **C; // Matrix A, B, and C
int n; // Size of the matrices
double start, end; // Start and end timing of process

/* The thread function
 *      arg: The assigned rank for the thread
 */
void* thr_fn(void *arg) {
    int k = *((int*) arg); // Current rank of this thread
    int x, y; // Current row, col of the process block

    // from page 2 of manual
    x = floor(k / Ps);
    y = (k%Ps);
    
    // calculate the range of rows and columns (from page 2 of manual)
    int row_start = x * (n / Ps);
    int row_end = (x + 1) * (n / Ps);
    int col_start = y * (n / Ps);
    int col_end = (y + 1) * (n / Ps);

    // Perform matrix multiplication for this block
    for (int i = row_start; i < row_end; i++) { // iterate over all the rows in matrix A
        for (int j = col_start; j < col_end; j++) {  // iterate over all the blocks in matrix B
            for (int r = 0; r < n; r++) { // Full row-column multiplication
                C[i][j] += A[i][r] * B[r][j]; 
            }
        }
    }

    pthread_exit(NULL);
    return NULL; // To make very damn sure that the thread terminate
}

int main(int argv, char *argc[]) {

    // used the helper function from lab1_IO.h to load input
    Lab1_loadinput(&A, &B, &n);

    // initialization of product matrix
    C = malloc(n * sizeof(int *));
    for (int i = 0; i < n; i++) {
        C[i] = malloc(n * sizeof(int));
        for (int j = 0; j < n; j++) { // Initialize all C[i][j] to 0
            C[i][j] = 0;
        }
    }

    GET_TIME(start);

    // implementation of the matrix multiplication here
    if (argv == 1) P = n*n; // By default
    else {
        if (argv != 2) {
            fprintf(stderr, "Expect either 0 or 1 argument!\n");
            exit(EXIT_FAILURE);
        }
        
        char *endptr;
        if ((P = (int) strtol(argc[1], &endptr, 10)) == 0 || *endptr != '\0') {
            fprintf(stderr, "\'%s\' is not a valid argument. Must be an int that is > 0\n", argc[1]);
            exit(EXIT_FAILURE);
        }
    }
    Ps = sqrt(P); // sqrt(P) = n

    // Check if P perfectly divides n^2 and is perfect square
    if (P < 0) {
        fprintf(stderr, "P must be positive!\n");
        exit(EXIT_FAILURE);
    }
    if ((n*n) % P != 0) {
        fprintf(stderr, "P must perfectly divides n^2!\n");
        exit(EXIT_FAILURE);
    }
    if (P != Ps*Ps) {
        fprintf(stderr, "P must be a perfect square number!\n");
        exit(EXIT_FAILURE);
    }

    pthread_t threads[P];
    int thread_ranks[P];
    for (int r = 0; r < P; r++) {
        thread_ranks[r] = r;
        if (pthread_create(&threads[r], NULL, thr_fn, &thread_ranks[r]) != 0) {
            fprintf(stderr, "Cannot create thread %d.\n", r);
            exit(EXIT_FAILURE);
        }
    }

    // Wait for all thread to terminate
    for (int r = 0; r < P; r++) {
        if (pthread_join(threads[r], NULL) != 0) {
            fprintf(stderr, "Thread %d failed to terminate!\n", r);
        }
    }

    GET_TIME(end);

    // used the helper function from lab1_IO.h to save output
    Lab1_saveoutput(C, &n, end - start);

    // free memory allocation for all the matrices
    for (int i = 0; i < n; i++) {
        free(A[i]);
        free(B[i]);
        free(C[i]);
    }

    free(A);
    free(B);
    free(C);
}