/*
 * Exercise 3 - Change of variables: Y = U^2
 *
 * Goal:
 *   Generate random numbers U uniformly distributed in [0, 1),
 *   then transform them using:
 *
 *       Y = U^2
 *
 *   The goal is to compare the histogram of Y with the analytic
 *   probability density function.
 *
 * Theory:
 *   If U is uniform in [0, 1], then:
 *
 *       Y = U^2
 *
 *   Since U = sqrt(Y), the change of variables formula gives:
 *
 *       f_Y(y) = f_U(sqrt(y)) * |d/dy sqrt(y)|
 *
 *   Since f_U(u) = 1 on [0, 1],
 *
 *       f_Y(y) = 1 / (2 sqrt(y))
 *
 *   for:
 *
 *       0 < y <= 1
 *
 * Important:
 *   The density diverges near y = 0, but it is still integrable.
 *
 * Output:
 *   The program writes a text file with one column:
 *
 *       y
 *
 *   where each value is one generated sample of Y = U^2.
 *
 * Compile:
 *   gcc -std=c11 -Wall -Wextra -O2 change_variables_y_u2.c -o change_variables_y_u2
 *
 * Run:
 *   ./change_variables_y_u2
 *
 * Optional:
 *   You can choose the output filename:
 *
 *   ./change_variables_y_u2 my_output.txt
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

/*
 * Number of samples.
 * A large number gives a smoother histogram.
 */
#define N_SAMPLES 200000

/*
 * Linear Congruential Generator parameters.
 *
 * Recurrence:
 *
 *     state_{n+1} = a * state_n + c
 *
 * Since state is uint32_t, overflow is automatic modulo 2^32.
 */
static uint32_t lcg32_next(uint32_t *state)
{
    const uint32_t a = 1664525u;
    const uint32_t c = 1013904223u;

    *state = a * (*state) + c;

    return *state;
}

/*
 * Convert the 32-bit integer produced by the LCG into a double
 * in the interval [0, 1).
 */
static double lcg32_next_double(uint32_t *state)
{
    return (double)lcg32_next(state) / 4294967296.0;
}

int main(int argc, char *argv[])
{
    /*
     * Default output file.
     * It can be changed from the command line.
     */
    const char *output_filename = "change_variables_y_u2.txt";

    /*
     * Fixed seed for reproducibility.
     * If the seed is the same, the generated sequence is the same.
     */
    uint32_t state = 13579u;

    FILE *fp;

    /*
     * Optional command-line argument:
     *
     *     ./change_variables_y_u2 output.txt
     */
    if (argc == 2) {
        output_filename = argv[1];
    } else if (argc > 2) {
        printf("Usage: %s [output_file.txt]\n", argv[0]);
        return EXIT_FAILURE;
    }

    /*
     * Open output file.
     */
    fp = fopen(output_filename, "w");

    if (fp == NULL) {
        fprintf(stderr, "Error: could not open output file '%s'\n",
                output_filename);
        return EXIT_FAILURE;
    }

    /*
     * Header.
     * Lines beginning with # are ignored by numpy.loadtxt in Python.
     */
    fprintf(fp, "# Change of variables: Y = U^2\n");
    fprintf(fp, "# U is generated from Uniform(0, 1) using an LCG\n");
    fprintf(fp, "# Column:\n");
    fprintf(fp, "# y\n");

    /*
     * Main sampling loop.
     *
     * For each random U:
     *
     *     Y = U^2
     *
     * We save Y to the text file.
     */
    for (int i = 0; i < N_SAMPLES; i++) {
        double u = lcg32_next_double(&state);
        double y = u * u;

        fprintf(fp, "%.12f\n", y);
    }

    fclose(fp);

    printf("Generated %d samples of Y = U^2.\n", N_SAMPLES);
    printf("Data saved to: %s\n", output_filename);

    return EXIT_SUCCESS;
}