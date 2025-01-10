#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "timer.h"
#include "lab1_IO.h"

int main(int argc, char *argv[]) {
    int **A, **B, **C; // matrix A, B, and C
    int n; // size of the matrics
    double start, end; // start and end timing of process

    // used the helper function from lab1_IO.h to load input
    Lab1_loadinput(&A, &B, &n);

    // initialization of product matrix
    C = malloc(n * sizeof(int *));
    for (int i = 0; i < n; i++) {
        C[i] = malloc(n * sizeof(int));
    }

    GET_TIME(start);

    // implementation of the matrix multiplication here

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