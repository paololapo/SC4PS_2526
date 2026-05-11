/*
 * Exercise 1 - Coin tosses and the Law of Large Numbers
 *
 * Goal:
 *   Generate 100,000 fair coin tosses using a Linear Congruential Generator
 *   (LCG) and study the running fraction of heads.
 *
 * Mathematical idea:
 *   For a fair coin, the probability of heads is 0.5.
 *   The Law of Large Numbers says that, when the number of tosses becomes
 *   large, the running fraction of heads should approach 0.5.
 *
 * Output:
 *   The program writes a text file with two columns:
 *
 *       toss_index    running_fraction_of_heads
 *
 *   This file can then be read by a Python script to make a plot.
 *
 * Compile:
 *   gcc -std=c11 -Wall -Wextra -O2 coin_toss_lln.c -o coin_toss_lln
 *
 * Run:
 *   ./coin_toss_lln
 *
 * Optional:
 *   You may also choose the output filename:
 *
 *   ./coin_toss_lln my_output.txt
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

/*
 * Total number of coin tosses requested by the exercise.
 */
#define N_TOSSES 100000

/*
 * Linear Congruential Generator parameters.
 *
 * The recurrence is:
 *
 *     state_{n+1} = a * state_n + c
 *
 * Since state is an unsigned 32-bit integer, overflow is automatic
 * modulo 2^32. This is exactly what we want for this simple LCG.
 */
static uint32_t lcg32_next(uint32_t *state)
{
    const uint32_t a = 1664525u;
    const uint32_t c = 1013904223u;

    *state = a * (*state) + c;

    return *state;
}

/*
 * Convert the random unsigned integer into a floating-point number
 * in the interval [0, 1).
 *
 * 4294967296.0 = 2^32.
 */
static double lcg32_next_double(uint32_t *state)
{
    return (double)lcg32_next(state) / 4294967296.0;
}

int main(int argc, char *argv[])
{
    /*
     * Default output filename.
     * The user can overwrite this by passing a filename on the command line.
     */
    const char *output_filename = "coin_toss_lln.txt";

    /*
     * Initial seed of the random number generator.
     * Using a fixed seed makes the result reproducible.
     */
    uint32_t state = 123456789u;

    /*
     * Counter for the number of heads observed so far.
     */
    int heads = 0;

    FILE *fp;

    /*
     * Optional command-line argument:
     *
     *     ./coin_toss_lln output.txt
     */
    if (argc == 2) {
        output_filename = argv[1];
    } else if (argc > 2) {
        printf("Usage: %s [output_file.txt]\n", argv[0]);
        return EXIT_FAILURE;
    }

    /*
     * Open the output text file.
     */
    fp = fopen(output_filename, "w");

    if (fp == NULL) {
        fprintf(stderr, "Error: could not open output file '%s'\n",
                output_filename);
        return EXIT_FAILURE;
    }

    /*
     * Write a small header.
     * Lines starting with # are ignored by numpy.loadtxt in Python.
     */
    fprintf(fp, "# Coin tosses and Law of Large Numbers\n");
    fprintf(fp, "# Columns:\n");
    fprintf(fp, "# toss_index running_fraction_of_heads\n");

    /*
     * Main simulation loop.
     *
     * At every step:
     *   1. Generate a random number u in [0, 1).
     *   2. Interpret u < 0.5 as heads.
     *   3. Update the number of heads.
     *   4. Save the running fraction of heads.
     */
    for (int i = 1; i <= N_TOSSES; i++) {
        double u = lcg32_next_double(&state);

        /*
         * Fair coin:
         *   heads if u < 0.5
         *   tails otherwise
         */
        int is_head = (u < 0.5) ? 1 : 0;

        heads += is_head;

        /*
         * Running estimate of P(heads).
         */
        double running_fraction = (double)heads / (double)i;

        fprintf(fp, "%d %.12f\n", i, running_fraction);
    }

    fclose(fp);

    /*
     * Print only a short summary to the terminal.
     * The full data are stored in the text file.
     */
    printf("Generated %d fair coin tosses.\n", N_TOSSES);
    printf("Final number of heads = %d\n", heads);
    printf("Final fraction of heads = %.12f\n",
           (double)heads / (double)N_TOSSES);
    printf("Data saved to: %s\n", output_filename);

    return EXIT_SUCCESS;
}