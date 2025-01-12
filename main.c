#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>
#include "timer.h"
#include "lab1_IO.h"

int P, Ps; // Number of thread and its square root
int **A, **B, **C; // Matrix A, B, and C
int n; // Size of the matrics
double start, end; // Start and end timing of process

/* The thread function
 *      arg: The assigned rank for the thread
 */
void* thr_fn(void *arg) {
    int k = *((int*) arg); // Current rank of this thread
    int x, y; // Current row, col of the process block

    // from page 2 of manual
    int x = floor(k / Ps);
    y = (k%Ps)*(k%Ps); // Shammir: I am still confuse about this one.
    
    // calculate the range of rows and columns (from page 2 of manual)
    int row_start = x * (n / Ps);
    int row_end = (x + 1) * (n / Ps);
    int col_start = y * (n / Ps);
    int col_end = (y + 1) * (n / Ps);

    // Perform matrix multiplication for this block
    for (int i = row_start; i < row_end; i++) {
        for (int j = col_start; j < col_end; j++) {
            for (int r = 0; r < n; r++) { // Full row-column multiplication
                C[i][j] += A[i][r] * B[r][j];
            }
        }
    }

    pthread_exit(NULL);
    return NULL; // To make very damn sure that the thread terminate
}

int main(int argc, char *argv[]) {

    // used the helper function from lab1_IO.h to load input
    Lab1_loadinput(&A, &B, &n);

    // initialization of product matrix
    C = malloc(n * sizeof(int *));
    for (int i = 0; i < n; i++) {
        C[i] = malloc(n * sizeof(int));
    }

    GET_TIME(start);

    // implementation of the matrix multiplication here
    P = P = atoi(argv[1]);
    Ps = sqrt(P);

    pthread_t threads[P];
    int *thread_ranks = malloc(P * sizeof(int));
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
    free(thread_ranks);
}