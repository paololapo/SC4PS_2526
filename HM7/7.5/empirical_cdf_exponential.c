/*
 * Exercise 5 - Empirical CDF for an exponential sample
 *
 * Goal:
 *   Generate an exponential sample using inverse transform sampling,
 *   compute the empirical cumulative distribution function (CDF),
 *   and save the data to a text file.
 *
 * Distribution:
 *   The exponential probability density function is:
 *
 *       f(y) = lambda * exp(-lambda y)
 *
 *   for y >= 0.
 *
 *   The exact cumulative distribution function is:
 *
 *       F(y) = 1 - exp(-lambda y)
 *
 * Method:
 *   1. Generate U ~ Uniform(0, 1) using the LCG.
 *   2. Transform it into an exponential variable:
 *
 *          Y = -log(1 - U) / lambda
 *
 *   3. Sort the generated sample.
 *   4. For the sorted sample y_i, define the empirical CDF as:
 *
 *          F_n(y_i) = (i + 1) / n
 *
 *      where i starts from 0.
 *
 * Output:
 *   The program writes a text file with two columns:
 *
 *       y_sorted    empirical_cdf
 *
 * Compile:
 *   gcc -std=c11 -Wall -Wextra -O2 empirical_cdf_exponential.c \
 *       -o empirical_cdf_exponential -lm
 *
 * Run:
 *   ./empirical_cdf_exponential
 *
 * Optional:
 *   You can choose the output filename:
 *
 *   ./empirical_cdf_exponential my_output.txt
 */

#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

/*
 * Number of generated samples.
 *
 * A larger value gives a smoother empirical CDF.
 */
#define N_SAMPLES 50000

/*
 * Parameter of the exponential distribution.
 */
#define LAMBDA 1.5

/*
 * Linear Congruential Generator.
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

/*
 * Generate one exponential random variable using inverse transform sampling.
 *
 * If U is uniform in [0, 1), then:
 *
 *     Y = -log(1 - U) / lambda
 *
 * follows an exponential distribution with parameter lambda.
 */
static double sample_exponential(uint32_t *state)
{
    double u = lcg32_next_double(state);

    return -log(1.0 - u) / LAMBDA;
}

/*
 * Comparison function used by qsort.
 *
 * qsort needs a function returning:
 *
 *   - a negative value if a < b
 *   - zero if a == b
 *   - a positive value if a > b
 */
static int compare_double(const void *pa, const void *pb)
{
    const double a = *(const double *)pa;
    const double b = *(const double *)pb;

    if (a < b) {
        return -1;
    }

    if (a > b) {
        return 1;
    }

    return 0;
}

int main(int argc, char *argv[])
{
    /*
     * Default output file.
     * It can be changed from the command line.
     */
    const char *output_filename = "empirical_cdf_exponential.txt";

    /*
     * Fixed seed for reproducibility.
     */
    uint32_t state = 246813579u;

    double *sample = NULL;
    FILE *fp;

    /*
     * Optional command-line argument:
     *
     *     ./empirical_cdf_exponential output.txt
     */
    if (argc == 2) {
        output_filename = argv[1];
    } else if (argc > 2) {
        printf("Usage: %s [output_file.txt]\n", argv[0]);
        return EXIT_FAILURE;
    }

    /*
     * Allocate memory for the sample.
     *
     * We need to store all values because the empirical CDF requires sorting.
     */
    sample = malloc((size_t)N_SAMPLES * sizeof(*sample));

    if (sample == NULL) {
        fprintf(stderr, "Error: allocation failed.\n");
        return EXIT_FAILURE;
    }

    /*
     * Generate the exponential sample.
     */
    for (int i = 0; i < N_SAMPLES; i++) {
        sample[i] = sample_exponential(&state);
    }

    /*
     * Sort the sample in increasing order.
     *
     * After sorting:
     *
     *     sample[0] <= sample[1] <= ... <= sample[n-1]
     */
    qsort(sample, (size_t)N_SAMPLES, sizeof(*sample), compare_double);

    /*
     * Open output file.
     */
    fp = fopen(output_filename, "w");

    if (fp == NULL) {
        fprintf(stderr, "Error: could not open output file '%s'\n",
                output_filename);
        free(sample);
        return EXIT_FAILURE;
    }

    /*
     * Header.
     * Lines beginning with # are ignored by numpy.loadtxt in Python.
     */
    fprintf(fp, "# Empirical CDF for an exponential sample\n");
    fprintf(fp, "# lambda = %.12f\n", LAMBDA);
    fprintf(fp, "# Columns:\n");
    fprintf(fp, "# y_sorted empirical_cdf\n");

    /*
     * Write empirical CDF values.
     *
     * For the i-th sorted sample point, with i starting at 0:
     *
     *     F_n(sample[i]) = (i + 1) / N
     */
    for (int i = 0; i < N_SAMPLES; i++) {
        double empirical_cdf = (double)(i + 1) / (double)N_SAMPLES;

        fprintf(fp, "%.12f %.12f\n", sample[i], empirical_cdf);
    }

    fclose(fp);

    printf("Generated %d exponential samples.\n", N_SAMPLES);
    printf("lambda = %.6f\n", LAMBDA);
    printf("Empirical CDF data saved to: %s\n", output_filename);
    printf("Minimum sample value = %.12f\n", sample[0]);
    printf("Maximum sample value = %.12f\n", sample[N_SAMPLES - 1]);

    free(sample);

    return EXIT_SUCCESS;
}