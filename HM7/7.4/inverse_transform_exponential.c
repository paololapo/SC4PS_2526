/*
 * Exercise 4 - Inverse transform sampling for an exponential distribution
 *
 * Goal:
 *   Generate random variables Y distributed according to an exponential
 *   distribution with parameter lambda = 1.5.
 *
 * Method:
 *   Start from:
 *
 *       U ~ Uniform(0, 1)
 *
 *   The cumulative distribution function of an exponential random variable is:
 *
 *       F_Y(y) = 1 - exp(-lambda y)
 *
 *   To sample from this distribution, set:
 *
 *       U = F_Y(Y)
 *
 *   Then solve for Y:
 *
 *       U = 1 - exp(-lambda Y)
 *       exp(-lambda Y) = 1 - U
 *       -lambda Y = log(1 - U)
 *
 *   Therefore:
 *
 *       Y = -log(1 - U) / lambda
 *
 * Output:
 *   The program writes a text file with one column:
 *
 *       y
 *
 *   where each value is one exponential random sample.
 *
 * Compile:
 *   gcc -std=c11 -Wall -Wextra -O2 inverse_transform_exponential.c \
 *       -o inverse_transform_exponential -lm
 *
 * Run:
 *   ./inverse_transform_exponential
 *
 * Optional:
 *   You can choose the output filename:
 *
 *   ./inverse_transform_exponential my_output.txt
 */

#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

/*
 * Number of random samples.
 * A large value gives a smoother histogram.
 */
#define N_SAMPLES 200000

/*
 * Parameter of the exponential distribution.
 *
 * The probability density function is:
 *
 *     f(y) = lambda * exp(-lambda y)
 *
 * for y >= 0.
 */
#define LAMBDA 1.5

/*
 * Linear Congruential Generator.
 *
 * Recurrence:
 *
 *     state_{n+1} = a * state_n + c
 *
 * Since state is uint32_t, arithmetic overflow is automatic modulo 2^32.
 */
static uint32_t lcg32_next(uint32_t *state)
{
    const uint32_t a = 1664525u;
    const uint32_t c = 1013904223u;

    *state = a * (*state) + c;

    return *state;
}

/*
 * Convert the 32-bit random integer produced by the LCG into a double
 * in the interval [0, 1).
 */
static double lcg32_next_double(uint32_t *state)
{
    return (double)lcg32_next(state) / 4294967296.0;
}

/*
 * Generate one exponential random variable using inverse transform sampling.
 */
static double sample_exponential(uint32_t *state)
{
    /*
     * Generate U in [0, 1).
     */
    double u = lcg32_next_double(state);

    /*
     * If u is exactly 0, then y = 0.
     * If u is close to 1, y can be large.
     *
     * Since our LCG conversion gives u < 1, 1 - u is always positive,
     * so log(1 - u) is well-defined.
     */
    return -log(1.0 - u) / LAMBDA;
}

int main(int argc, char *argv[])
{
    /*
     * Default output file.
     * It can be changed from the command line.
     */
    const char *output_filename = "inverse_transform_exponential.txt";

    /*
     * Fixed seed for reproducibility.
     */
    uint32_t state = 246813579u;

    FILE *fp;

    /*
     * Optional command-line argument:
     *
     *     ./inverse_transform_exponential output.txt
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
    fprintf(fp, "# Inverse transform sampling: exponential distribution\n");
    fprintf(fp, "# lambda = %.12f\n", LAMBDA);
    fprintf(fp, "# Column:\n");
    fprintf(fp, "# y\n");

    /*
     * Generate samples and save them to the text file.
     */
    for (int i = 0; i < N_SAMPLES; i++) {
        double y = sample_exponential(&state);

        fprintf(fp, "%.12f\n", y);
    }

    fclose(fp);

    printf("Generated %d exponential samples.\n", N_SAMPLES);
    printf("lambda = %.6f\n", LAMBDA);
    printf("Data saved to: %s\n", output_filename);

    return EXIT_SUCCESS;
}