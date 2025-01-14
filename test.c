/* Author: Dang Nguyen
 * A test program that creates and verifies test cases for main.c
 * Usage: Make sure to compile matrixgen.c and main.c successfully
 *        first. Run this program with ./test (Can improve later
 *        if see fit).
 * 
 * HOW IT WORKS!?: The program will generate upto tnum test cases, 
 *                  each test case will generate matrix size n and
 *                  the bound randomly in the range of (0, threads/bound_lim].
 *                  It will then execute ./matrixgen -s n -b bound and
 *                  ./main then compare the data in data_output produced
 *                  by main.c to the result computed by using sequential
 *                  matrix multiplication.
 * 
 * NOTE: Test cases where P != n^2 are highlighted in yellow
 */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <unistd.h>
#include "timer.h"
#include "lab1_IO.h"

int main() {
    srand(time(NULL));

    unsigned int p, P, Nsq, N, bound;
    unsigned int mul;

    const int bound_lim = 100000; // Bound for matrix bound (Recommend max 100,000 to avoid overflow)
    const int tnum = 100; // Number of test cases (Recommend max 100 else you will be wasting time and might blow up something)
    const int threads = 15180; // Recommend max number of thread to be created (atleast on my machine)
    const int th_sqrt = 123; // Square root of max num of thread

    int **A, **B;
    int n;

    char mgen[50], mexe[50];
    FILE *op;

    int e; // Hold each C[i][j] perform by sequential matrix multiplication
    int pass;

    printf("Running %d test cases...\n\n", tnum);
    sleep(3);

    for (int t = 0; t < tnum; t++) {
        p = rand() % th_sqrt + 1; // Generate a random base multiplier in range (0, th_sqrt]
        P = p*p; // Make sure P is perfect square
        bound = rand() % bound_lim + 1; // Generate random bound in range (0, bound_lim]
        mul = rand() % 1000 + 1;

        // Generate N^2
        Nsq = 1;
        for (int i = 0; i < mul; i++) { // Make sure P perfectly divides n^2
            Nsq *= P;
            if (Nsq > threads) {
                Nsq /= P;
                break;
            }
        }
        N = sqrt(Nsq);
        
        /* Safe guard code to protect my laptop from nuking itself */
        if (N > 150) {
            printf("Sonething went wrong: Nsq = %d, N = %d, P = %d\n", Nsq, N, P);
            exit(EXIT_FAILURE);
        }

        if (Nsq != P) printf("\033[0;33m"); // Switch to yellow text
        printf("Test Case %d starts: n = %d, bound = %d, P = %d\n", t, N, bound, P);
        printf("\033[0;37m"); // Reset to white text

        // Execute matrixgen and main.c
        sprintf(mgen, "./matrixgen -s %d -b %d", N, bound);
        if (system(mgen) == -1) {
            fprintf(stderr, "Cannot execute %s!\n", mgen);
            exit(EXIT_FAILURE);
        }
        sprintf(mexe, "./main %d", P);
        if (system(mexe) == -1) {
            fprintf(stderr, "Cannot execute main!\n");
            exit(EXIT_FAILURE);
        }

        // Load A, B and n
        if (Lab1_loadinput(&A, &B, &n) != 0) {
            fprintf(stderr, "Failed to load inputs! <In test.c>\n");
            exit(EXIT_FAILURE);
        }

        if (n != N) {
            fprintf(stderr, "Expect n = %d, instead got n = %d!\n", N, n);
            exit(EXIT_FAILURE);
        }

        if ((op = fopen("data_output", "r")) == NULL) {
            fprintf(stderr, "Cannot open file data_output for reading! <In test.c>\n");
            exit(EXIT_FAILURE);
        }

        // Size n and individual matrix element obtained from data_output
        int n_o, e_o;

        if (fscanf(op, "%d\n", &n_o) == EOF) {
            fprintf(stderr, "Failed to read size n from file data_output! <In test.c>\n");
            exit(EXIT_FAILURE);
        }

        if (n_o != N) {
            fprintf(stderr, "Expect output matrix of size %d, instead got %d!\n", N, n_o);
            exit(EXIT_FAILURE);
        }

        /* Perform sequential matrix multiplication and comapre it to the result
         * obtained by main.c
         */
        pass = 0;
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                e = 0;
                for (int r = 0; r < N; r++) { // Perform the sequential matrix multiplication
                    e += A[i][r] * B[r][j];
                }
                if (fscanf(op, "%d\t", &e_o) == EOF) {
                    fprintf(stderr, "Cannot read element C[%d][%d] from data output!\n", i, j);
                    exit(EXIT_FAILURE);
                }
                if (e != e_o) {
                    fprintf(stderr, "Incorrect output C[%d][%d]: Expected %d but got %d!\n", i, j, e, e_o);
                    pass += 1;
                }
            }
        }

        if (pass != 0) {
            printf("Test Case %d Failed!\n\n", t);
            exit(EXIT_SUCCESS);
        }
        else {
            printf("Test Case %d: PASSED!\n\n", t);
        }

        if (fclose(op) != 0) {
            fprintf(stderr, "Cannot close file data_output!\n");
            exit(EXIT_FAILURE);
        }

        // Reset the stack and free any allocated memory
        for (int i = 0; i < 50; i++) {
            mgen[i] = '\0';
            mexe[i] = '\0';
        }
        for (int i = 0; i < N; i++) {
            free(A[i]);
            free(B[i]);
        }

        free(A);
        free(B);
    }

    printf("ALL %d TEST CASES PASSED SUCCESSFULLY!\n", tnum);

    return 0;
}