#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int main(int argc, char *argv[]) {
    if (argc != 5) {
        printf("Usage: %s <N> <a> <x_val> <y_val>\n", argv[0]);
        return 1;
    }

    // Parse command line arguments
    long N = atol(argv[1]);
    double a = atof(argv[2]);
    double x_val = atof(argv[3]);
    double y_val = atof(argv[4]);

    // Allocate memory on the heap (stack memory might not be enough)
    double *x = (double *)malloc(N * sizeof(double));
    double *y = (double *)malloc(N * sizeof(double));
    double *d = (double *)malloc(N * sizeof(double));

    if (x == NULL || y == NULL || d == NULL) {
        printf("Memory allocation failed!\n");
        return 1;
    }

    // Initialize and Compute
    for (long i = 0; i < N; i++) {
        x[i] = x_val;
        y[i] = y_val;
        d[i] = a * x[i] + y[i];
    }

    // Validation
    double expected = a * x_val + y_val;
    int success = 1;
    double epsilon = 1e-12; // Tolerance for floating point comparison

    for (long i = 0; i < N; i++) {
        if (fabs(d[i] - expected) > epsilon) {
            success = 0;
            break;
        }
    }

    // Output Results
    printf("N: %ld | a: %.2f, x: %.2f, y: %.2f\n", N, a, x_val, y_val);
    printf("Expected result: %.15f\n", expected);
    printf("Actual result (d[0]): %.15f\n", d[0]);
    
    if (success) {
        printf("Verification: SUCCESS (All elements match expected value)\n");
    } else {
        printf("Verification: FAILED\n");
    }

    // Clean up
    free(x);
    free(y);
    free(d);

    return 0;
}