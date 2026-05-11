/*
 * Exercise 2 - Monte Carlo estimate of pi
 *
 * Goal:
 *   Estimate pi using random points in the unit square.
 *
 * Method:
 *   Generate random points (x, y), with:
 *
 *       0 <= x < 1
 *       0 <= y < 1
 *
 *   Then count how many points fall inside the quarter circle:
 *
 *       x^2 + y^2 <= 1
 *
 *   The area of the unit square is 1.
 *   The area of the quarter circle is pi / 4.
 *
 *   Therefore:
 *
 *       fraction_inside ≈ pi / 4
 *
 *   and:
 *
 *       pi ≈ 4 * fraction_inside
 *
 * Random numbers:
 *   We use two independent LCG streams:
 *
 *       one stream for x
 *       one stream for y
 *
 * Output:
 *   The program writes a text file with columns:
 *
 *       N    pi_estimate    absolute_error    relative_error
 *
 * Compile:
 *   gcc -std=c11 -Wall -Wextra -O2 monte_carlo_pi.c -o monte_carlo_pi -lm
 *
 * Run:
 *   ./monte_carlo_pi
 *
 * Optional:
 *   You can also choose the output filename:
 *
 *   ./monte_carlo_pi my_output.txt
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

/*
 * A value of pi with enough digits for this exercise.
 * We use it only to compute the error of the Monte Carlo estimate.
 */
#define PI_TRUE 3.14159265358979323846

/*
 * Linear Congruential Generator parameters.
 *
 * The recurrence is:
 *
 *     state_{n+1} = a * state_n + c
 *
 * With uint32_t arithmetic, overflow happens automatically modulo 2^32.
 */
static uint32_t lcg32_next(uint32_t *state)
{
    const uint32_t a = 1664525u;
    const uint32_t c = 1013904223u;

    *state = a * (*state) + c;

    return *state;
}

/*
 * Convert the 32-bit random integer into a double in [0, 1).
 */
static double lcg32_next_double(uint32_t *state)
{
    return (double)lcg32_next(state) / 4294967296.0;
}

/*
 * Estimate pi using N random points.
 *
 * Inputs:
 *   N      number of Monte Carlo points
 *   seed_x seed for the x random stream
 *   seed_y seed for the y random stream
 *
 * Output:
 *   Monte Carlo estimate of pi.
 */
static double estimate_pi(int N, uint32_t seed_x, uint32_t seed_y)
{
    uint32_t state_x = seed_x;
    uint32_t state_y = seed_y;

    int inside = 0;

    for (int i = 0; i < N; i++) {
        /*
         * Generate one random point in the square [0,1) x [0,1).
         */
        double x = lcg32_next_double(&state_x);
        double y = lcg32_next_double(&state_y);

        /*
         * Check whether the point is inside the quarter unit circle.
         */
        if (x * x + y * y <= 1.0) {
            inside++;
        }
    }

    /*
     * The fraction inside estimates pi/4.
     */
    return 4.0 * (double)inside / (double)N;
}

int main(int argc, char *argv[])
{
    /*
     * Default output file.
     * It can be changed from the command line.
     */
    const char *output_filename = "monte_carlo_pi.txt";

    /*
     * Sample sizes used to study convergence.
     *
     * Larger N should usually give a better estimate, although Monte Carlo
     * convergence is statistical and therefore not perfectly monotonic.
     */
    const int sample_sizes[] = {
        100,
        1000,
        10000,
        100000,
        1000000
    };

    const int n_sizes = (int)(sizeof(sample_sizes) / sizeof(sample_sizes[0]));

    /*
     * Different seeds for the two random streams.
     */
    const uint32_t seed_x = 123u;
    const uint32_t seed_y = 987654321u;

    FILE *fp;

    /*
     * Optional command-line argument:
     *
     *     ./monte_carlo_pi output.txt
     */
    if (argc == 2) {
        output_filename = argv[1];
    } else if (argc > 2) {
        printf("Usage: %s [output_file.txt]\n", argv[0]);
        return EXIT_FAILURE;
    }

    fp = fopen(output_filename, "w");

    if (fp == NULL) {
        fprintf(stderr, "Error: could not open output file '%s'\n",
                output_filename);
        return EXIT_FAILURE;
    }

    /*
     * Header for the text file.
     * Python's numpy.loadtxt ignores lines starting with '#'.
     */
    fprintf(fp, "# Monte Carlo estimate of pi\n");
    fprintf(fp, "# Columns:\n");
    fprintf(fp, "# N pi_estimate absolute_error relative_error\n");

    printf("Monte Carlo estimate of pi\n\n");

    /*
     * Run the Monte Carlo estimate for each sample size.
     */
    for (int i = 0; i < n_sizes; i++) {
        int N = sample_sizes[i];

        double pi_estimate = estimate_pi(N, seed_x, seed_y);
        double absolute_error = fabs(pi_estimate - PI_TRUE);
        double relative_error = absolute_error / PI_TRUE;

        /*
         * Save data for plotting.
         */
        fprintf(fp,
                "%d %.12f %.12e %.12e\n",
                N,
                pi_estimate,
                absolute_error,
                relative_error);

        /*
         * Print a short summary on screen.
         */
        printf("N = %8d | pi estimate = %.12f | abs error = %.6e\n",
               N,
               pi_estimate,
               absolute_error);
    }

    fclose(fp);

    printf("\nData saved to: %s\n", output_filename);

    return EXIT_SUCCESS;
}