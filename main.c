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
    x = floor(k/Ps);
    y = (k%Ps)*(k%Ps);
    
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (
                (n/Ps)*x <= i && i <= (n/Ps)*(x+1)-1
                &&
                (n/Ps)*y <= j && j <= (n/Ps)*(y+1)-1
            )
            {
                for (int r = 0; r < n-1; r++) {
                    C[i][j] += A[i][r] * B[r][j];
                }
            }
            else continue;
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
    P = n*n;
    Ps = n;
    pthread_t threads[P];

    for (int r = 0; r < P; r++) {
        if (pthread_create(&threads[r], NULL, thr_fn, (void*) &r) != 0) {
            fprintf(stderr, "Cannot create thread %d!\n", r);
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