/* Author: Dang Nguyen
 * A test program that creates and verifies test cases for main.c
 * Usage: Make sure to compile matrixgen.c and main.c successfully
 *        first. Run this program with ./test (Can improve later
 *        if see fit).
 * 
 * HOW IT WORKS!?: The program will generate upto tnum test cases, 
 *                  each test case will generate matrix size n and
 *                  the bound randomly in the range of (0, N_lim/bound_lim].
 *                  It will then execute ./matrixgen -s n -b bound and
 *                  ./main then compare the data in data_output produced
 *                  by main.c to the result computed by using sequential
 *                  matrix multiplication.
 */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "timer.h"
#include "lab1_IO.h"

int main() {
    srand(time(NULL));

    int N, bound;
    const int N_lim = 150; // Bound for number n (Recommend max 150 else you might blow up something)
    const int bound_lim = 100000; // Bound for matrix bound (Recommend max 100,000 to avoid overflow)
    const int tnum = 100; // Number of test cases (Recommend max 100 else you will be wasting time and might blow up something)

    int **A, **B;
    int n;

    char mgen[50];
    FILE *op;

    int e; // Hold each C[i][j] perform by sequential matrix multiplication
    int pass;

    printf("Running %d test cases...\n\n", tnum);
    sleep(3);

    for (int t = 0; t < tnum; t++) {
        N = rand() % N_lim + 1; // Generate random n in range (0, N_lim]
        bound = rand() % bound_lim + 1; // Generate random bound in range (0, bound_lim]
        printf("Test Case %d starts: n = %d, bound = %d\n", t, N, bound);

        // Execute matrixgen and main.c
        sprintf(&mgen, "./matrixgen -s %d -b %d", N, bound);
        if (system(&mgen) == -1) {
            fprintf(stderr, "Cannot execute %s!\n", &mgen);
            exit(EXIT_FAILURE);
        }
        if (system("./main") == -1) {
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
        for (int i = 0; i < 50; i++) mgen[i] = '\0';
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