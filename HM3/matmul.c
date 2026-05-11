#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

/*
 * Matrix multiplication benchmark
 *
 * This program computes:
 *
 *     C[i][j] = sum_k A[i][k] * B[k][j]
 *
 * where:
 *
 *     A is an N x N matrix with all elements equal to a
 *     B is an N x N matrix with all elements equal to b
 *
 * Since every multiplication is a * b, and the sum has N terms:
 *
 *     C[i][j] = N * a * b
 *
 * for every element of C.
 *
 * Command line inputs:
 *
 *     ./program <a> <b> <N> <fileout>
 *
 * Example:
 *
 *     ./program 2.0 3.0 4 output.txt
 *
 * In this case every element of C should be:
 *
 *     4 * 2.0 * 3.0 = 24.0
 *
 * The program:
 *     1. Allocates matrices A, B, and C
 *     2. Initializes A and B
 *     3. Benchmarks different loop orderings
 *     4. Checks the result
 *     5. Saves matrix C to a file
 */

#define IDX(i, j, N) ((i) * (N) + (j))
#define REPEATS 3

/*
 * Return the current CPU time in seconds.
 *
 * clock() is simple and useful for basic benchmarks.
 * We call it before and after the matrix multiplication.
 */
double get_time(void) {
    return (double)clock() / CLOCKS_PER_SEC;
}

/*
 * Fill all elements of a matrix with the same value.
 */
void fill_matrix(double *M, int N, double value) {
    for (int i = 0; i < N * N; i++) {
        M[i] = value;
    }
}

/*
 * Set all elements of matrix C to zero.
 *
 * This is necessary because some loop orderings use:
 *
 *     C[i][j] += ...
 *
 * so C must start from zero.
 */
void zero_matrix(double *C, int N) {
    for (int i = 0; i < N * N; i++) {
        C[i] = 0.0;
    }
}

/*
 * Compare two floating-point numbers.
 *
 * Floating-point values should not usually be compared with ==,
 * because small numerical errors may appear.
 */
int almost_equal(double x, double y) {
    double epsilon = 1e-9;

    if (fabs(x - y) < epsilon) {
        return 1;
    }

    return 0;
}

/*
 * Full check of the result.
 *
 * This checks every element of C.
 *
 * Cost:
 *     O(N^2)
 *
 * This is the safest check, but it can take time for large matrices.
 */
int full_check(double *C, int N, double expected) {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            if (!almost_equal(C[IDX(i, j, N)], expected)) {
                return 0;
            }
        }
    }

    return 1;
}

/*
 * Quick check of the result.
 *
 * This checks only a few elements:
 *     - top-left
 *     - top-right
 *     - bottom-left
 *     - bottom-right
 *     - center
 *
 * Cost:
 *     O(1)
 *
 * This is much faster than checking all N^2 elements.
 *
 * Important:
 *     This is not as safe as full_check().
 *     It is only a quick test.
 */
int quick_check(double *C, int N, double expected) {
    if (!almost_equal(C[IDX(0, 0, N)], expected)) {
        return 0;
    }

    if (!almost_equal(C[IDX(0, N - 1, N)], expected)) {
        return 0;
    }

    if (!almost_equal(C[IDX(N - 1, 0, N)], expected)) {
        return 0;
    }

    if (!almost_equal(C[IDX(N - 1, N - 1, N)], expected)) {
        return 0;
    }

    if (!almost_equal(C[IDX(N / 2, N / 2, N)], expected)) {
        return 0;
    }

    return 1;
}

/*
 * Loop ordering: i-j-k
 *
 * This is the most direct translation of:
 *
 *     C[i][j] = sum_k A[i][k] * B[k][j]
 */
void matmul_ijk(double *A, double *B, double *C, int N) {
    zero_matrix(C, N);

    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            for (int k = 0; k < N; k++) {
                C[IDX(i, j, N)] += A[IDX(i, k, N)] * B[IDX(k, j, N)];
            }
        }
    }
}

/*
 * Loop ordering: i-k-j
 *
 * This is often faster in C because the inner loop uses j.
 *
 * In C, matrices stored as 1D arrays are row-major:
 * elements with consecutive j indices are close in memory.
 */
void matmul_ikj(double *A, double *B, double *C, int N) {
    zero_matrix(C, N);

    for (int i = 0; i < N; i++) {
        for (int k = 0; k < N; k++) {
            for (int j = 0; j < N; j++) {
                C[IDX(i, j, N)] += A[IDX(i, k, N)] * B[IDX(k, j, N)];
            }
        }
    }
}

/*
 * Loop ordering: j-i-k
 */
void matmul_jik(double *A, double *B, double *C, int N) {
    zero_matrix(C, N);

    for (int j = 0; j < N; j++) {
        for (int i = 0; i < N; i++) {
            for (int k = 0; k < N; k++) {
                C[IDX(i, j, N)] += A[IDX(i, k, N)] * B[IDX(k, j, N)];
            }
        }
    }
}

/*
 * Loop ordering: j-k-i
 */
void matmul_jki(double *A, double *B, double *C, int N) {
    zero_matrix(C, N);

    for (int j = 0; j < N; j++) {
        for (int k = 0; k < N; k++) {
            for (int i = 0; i < N; i++) {
                C[IDX(i, j, N)] += A[IDX(i, k, N)] * B[IDX(k, j, N)];
            }
        }
    }
}

/*
 * Loop ordering: k-i-j
 *
 * This can also be fast because the inner loop uses j.
 */
void matmul_kij(double *A, double *B, double *C, int N) {
    zero_matrix(C, N);

    for (int k = 0; k < N; k++) {
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                C[IDX(i, j, N)] += A[IDX(i, k, N)] * B[IDX(k, j, N)];
            }
        }
    }
}

/*
 * Loop ordering: k-j-i
 */
void matmul_kji(double *A, double *B, double *C, int N) {
    zero_matrix(C, N);

    for (int k = 0; k < N; k++) {
        for (int j = 0; j < N; j++) {
            for (int i = 0; i < N; i++) {
                C[IDX(i, j, N)] += A[IDX(i, k, N)] * B[IDX(k, j, N)];
            }
        }
    }
}

/*
 * Save matrix C to a text file.
 *
 * Each row of the matrix is written on a separate line.
 */
int save_matrix(const char *filename, double *C, int N) {
    FILE *fp = fopen(filename, "w");

    if (fp == NULL) {
        return 0;
    }

    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            fprintf(fp, "%.6f ", C[IDX(i, j, N)]);
        }

        fprintf(fp, "\n");
    }

    fclose(fp);
    return 1;
}

/*
 * Benchmark one matrix multiplication function.
 *
 * The same function is repeated REPEATS times.
 * We keep the best time, because one run can be slower due to noise.
 */
double benchmark(
    void (*matmul)(double *, double *, double *, int),
    double *A,
    double *B,
    double *C,
    int N
) {
    double best_time = -1.0;

    for (int r = 0; r < REPEATS; r++) {
        double start = get_time();

        matmul(A, B, C, N);

        double end = get_time();

        double elapsed = end - start;

        if (best_time < 0.0 || elapsed < best_time) {
            best_time = elapsed;
        }
    }

    return best_time;
}

int main(int argc, char *argv[]) {
    /*
     * Check the number of command line arguments.
     *
     * argv[0] is the program name.
     * argv[1] is a.
     * argv[2] is b.
     * argv[3] is N.
     * argv[4] is the output filename.
     */
    if (argc != 5) {
        printf("Usage: %s <a> <b> <N> <fileout>\n", argv[0]);
        return 1;
    }

    /*
     * Read input values from command line.
     */
    double a = atof(argv[1]);
    double b = atof(argv[2]);
    int N = atoi(argv[3]);
    const char *fileout = argv[4];

    if (N <= 0) {
        printf("Error: N must be positive.\n");
        return 1;
    }

    /*
     * Allocate memory for the matrices.
     *
     * Each matrix has N * N elements.
     * We use malloc because large matrices may not fit on the stack.
     */
    double *A = malloc(N * N * sizeof(double));
    double *B = malloc(N * N * sizeof(double));
    double *C = malloc(N * N * sizeof(double));

    if (A == NULL || B == NULL || C == NULL) {
        printf("Error: memory allocation failed.\n");

        free(A);
        free(B);
        free(C);

        return 1;
    }

    /*
     * Initialize A and B.
     *
     * Every element of A is equal to a.
     * Every element of B is equal to b.
     */
    fill_matrix(A, N, a);
    fill_matrix(B, N, b);

    /*
     * Expected value of every element of C.
     *
     * Since each C[i][j] is the sum of N terms,
     * and each term is a * b:
     *
     *     expected = N * a * b
     */
    double expected = N * a * b;

    printf("Matrix multiplication benchmark\n");
    printf("N = %d\n", N);
    printf("a = %.6f\n", a);
    printf("b = %.6f\n", b);
    printf("Expected value of each C[i][j] = %.6f\n\n", expected);

    /*
     * Benchmark all loop orderings.
     */
    double t_ijk = benchmark(matmul_ijk, A, B, C, N);
    printf("ijk time: %.6f seconds | quick check: %s\n",
           t_ijk,
           quick_check(C, N, expected) ? "OK" : "FAILED");

    double t_ikj = benchmark(matmul_ikj, A, B, C, N);
    printf("ikj time: %.6f seconds | quick check: %s\n",
           t_ikj,
           quick_check(C, N, expected) ? "OK" : "FAILED");

    double t_jik = benchmark(matmul_jik, A, B, C, N);
    printf("jik time: %.6f seconds | quick check: %s\n",
           t_jik,
           quick_check(C, N, expected) ? "OK" : "FAILED");

    double t_jki = benchmark(matmul_jki, A, B, C, N);
    printf("jki time: %.6f seconds | quick check: %s\n",
           t_jki,
           quick_check(C, N, expected) ? "OK" : "FAILED");

    double t_kij = benchmark(matmul_kij, A, B, C, N);
    printf("kij time: %.6f seconds | quick check: %s\n",
           t_kij,
           quick_check(C, N, expected) ? "OK" : "FAILED");

    double t_kji = benchmark(matmul_kji, A, B, C, N);
    printf("kji time: %.6f seconds | quick check: %s\n",
           t_kji,
           quick_check(C, N, expected) ? "OK" : "FAILED");

    /*
     * For simplicity, save the result from the last multiplication.
     * At this point C contains the result produced by matmul_kji().
     */
    printf("\nFull check before saving: ");

    if (full_check(C, N, expected)) {
        printf("SUCCESS\n");
    } else {
        printf("FAILED\n");
    }

    /*
     * Save matrix C to file.
     */
    if (!save_matrix(fileout, C, N)) {
        printf("Error: could not open output file.\n");

        free(A);
        free(B);
        free(C);

        return 1;
    }

    printf("Matrix C saved to file: %s\n", fileout);

    /*
     * Free allocated memory.
     */
    free(A);
    free(B);
    free(C);

    return 0;
}