/*
 * HomeWork05 - Stability of Legendre Polynomials
 *
 * Goal:
 *   Compare different numerical ways of computing the ordinary Legendre
 *   polynomials P_l(x), for m = 0 and l = 0,...,50.
 *
 * Methods:
 *   1. Forward recurrence in double precision.
 *   2. Higher-precision reference using long double.
 *   3. Backward Miller-style experiment with arbitrary final values.
 *
 * Important conclusion:
 *   For ordinary Legendre polynomials P_l(x), with |x| < 1, the standard
 *   forward recurrence is stable for this test. The arbitrary backward
 *   recurrence does not generally recover P_l(x).
 *
 * Output:
 *   The program writes a data file called:
 *
 *       legendre_errors.dat
 *
 *   with columns:
 *
 *       x  l  P_reference  P_forward  P_backward
 *       abs_err_forward  rel_err_forward
 *       abs_err_backward rel_err_backward
 *
 * Compile:
 *   gcc -std=c11 -Wall -Wextra -O2 legendre_stability.c -lm -o legendre_stability
 *
 * Run:
 *   ./legendre_stability
 */

#include <float.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define LMAX 50
#define LBWD 80

/*
 * Compute P_l(x) using the standard forward three-term recurrence.
 *
 * P_0(x) = 1
 * P_1(x) = x
 *
 * P_{l+1}(x) = ((2l + 1) x P_l(x) - l P_{l-1}(x)) / (l + 1)
 *
 * This function uses double precision.
 */
void legendre_forward_double(double x, int lmax, double *P)
{
    P[0] = 1.0;

    if (lmax == 0) {
        return;
    }

    P[1] = x;

    for (int l = 1; l < lmax; l++) {
        P[l + 1] = ((2.0 * l + 1.0) * x * P[l]
                    - (double)l * P[l - 1]) / (double)(l + 1);
    }
}

/*
 * Compute reference values using the same recurrence,
 * but in long double precision.
 *
 * This is a simple C-only higher-precision reference.
 *
 * Note:
 *   long double is not the same as a true 100-decimal-digit library like
 *   mpmath, but it is more precise than double on most common systems.
 *   For this homework range, l <= 50, it is enough to show the behavior.
 */
void legendre_reference_long_double(double x, int lmax, long double *Pref)
{
    long double xl = (long double)x;

    Pref[0] = 1.0L;

    if (lmax == 0) {
        return;
    }

    Pref[1] = xl;

    for (int l = 1; l < lmax; l++) {
        Pref[l + 1] =
            (((long double)(2 * l + 1)) * xl * Pref[l]
             - (long double)l * Pref[l - 1]) / (long double)(l + 1);
    }
}

/*
 * Backward recurrence experiment.
 *
 * The recurrence can be solved for P_{l-1}:
 *
 * P_{l-1}(x) = ((2l + 1) x P_l(x) - (l + 1) P_{l+1}(x)) / l
 *
 * Miller-style experiment:
 *
 *   Q[L+1] = 0
 *   Q[L]   = 1
 *
 * Then we propagate backward from L to 1.
 *
 * Finally we rescale the sequence so that:
 *
 *   Q[0] = 1
 *
 * This is only an experiment. It does not generally recover P_l(x) for the
 * ordinary Legendre polynomials on |x| < 1.
 */
void legendre_backward_experiment(double x, int lmax, int L, double *Pback)
{
    if (L <= lmax) {
        fprintf(stderr, "Error: need L > lmax in backward experiment.\n");
        exit(EXIT_FAILURE);
    }

    /*
     * We need entries from Q[0] to Q[L+1].
     * calloc initializes all entries to zero.
     */
    double *Q = calloc((size_t)(L + 2), sizeof(*Q));

    if (Q == NULL) {
        fprintf(stderr, "Error: allocation failed in backward experiment.\n");
        exit(EXIT_FAILURE);
    }

    /*
     * Arbitrary final values.
     * These do not contain information about the true Legendre sequence.
     */
    Q[L + 1] = 0.0;
    Q[L] = 1.0;

    /*
     * Propagate backward.
     */
    for (int l = L; l >= 1; l--) {
        Q[l - 1] = ((2.0 * l + 1.0) * x * Q[l]
                    - (double)(l + 1) * Q[l + 1]) / (double)l;
    }

    /*
     * Rescale so that P_0 = 1.
     */
    if (fabs(Q[0]) < DBL_MIN) {
        fprintf(stderr, "Error: Q[0] is too small for safe rescaling.\n");
        free(Q);
        exit(EXIT_FAILURE);
    }

    double scale = 1.0 / Q[0];

    for (int l = 0; l <= lmax; l++) {
        Pback[l] = scale * Q[l];
    }

    free(Q);
}

/*
 * Compute a relative error.
 *
 * If the reference value is very close to zero, the relative error can become
 * misleadingly huge. In that case we return the absolute error instead.
 */
long double relative_error(long double value, long double reference)
{
    long double abs_err = fabsl(value - reference);

    if (fabsl(reference) > LDBL_MIN) {
        return abs_err / fabsl(reference);
    }

    return abs_err;
}

/*
 * Helper function:
 * compute and write results for a single x value.
 */
void run_one_x(FILE *fp, double x)
{
    double Pf[LMAX + 1];
    double Pb[LMAX + 1];
    long double Pref[LMAX + 1];

    long double max_rel_f = 0.0L;
    long double max_rel_b = 0.0L;

    /*
     * Compute the three sequences.
     */
    legendre_reference_long_double(x, LMAX, Pref);
    legendre_forward_double(x, LMAX, Pf);
    legendre_backward_experiment(x, LMAX, LBWD, Pb);

    /*
     * Write one row for each degree l.
     */
    for (int l = 0; l <= LMAX; l++) {
        long double pf = (long double)Pf[l];
        long double pb = (long double)Pb[l];

        long double abs_err_f = fabsl(pf - Pref[l]);
        long double abs_err_b = fabsl(pb - Pref[l]);

        long double rel_err_f = relative_error(pf, Pref[l]);
        long double rel_err_b = relative_error(pb, Pref[l]);

        if (rel_err_f > max_rel_f) {
            max_rel_f = rel_err_f;
        }

        if (rel_err_b > max_rel_b) {
            max_rel_b = rel_err_b;
        }

        fprintf(fp,
                "%.17g %d %.21Le %.21Le %.21Le %.21Le %.21Le %.21Le %.21Le\n",
                x,
                l,
                Pref[l],
                pf,
                pb,
                abs_err_f,
                rel_err_f,
                abs_err_b,
                rel_err_b);
    }

    /*
     * Blank line separates data blocks for different x values.
     * This is useful for plotting with gnuplot.
     */
    fprintf(fp, "\n");

    /*
     * Print a short summary on the terminal.
     */
    printf("x = %.2f | max relative error forward = %.3Le | "
           "max relative error backward = %.3Le\n",
           x, max_rel_f, max_rel_b);
}

int main(void)
{
    /*
     * Values requested by the homework.
     */
    double xs[] = {0.1, 0.5, 0.9, 0.99};
    int nx = (int)(sizeof(xs) / sizeof(xs[0]));

    /*
     * Open output file.
     */
    FILE *fp = fopen("legendre_errors.dat", "w");

    if (fp == NULL) {
        fprintf(stderr, "Error: could not open output file.\n");
        return EXIT_FAILURE;
    }

    /*
     * Header explaining the columns.
     */
    fprintf(fp, "# HomeWork05 - Legendre polynomial stability\n");
    fprintf(fp, "# Columns:\n");
    fprintf(fp, "# x l P_reference P_forward P_backward ");
    fprintf(fp, "abs_err_forward rel_err_forward ");
    fprintf(fp, "abs_err_backward rel_err_backward\n");

    printf("HomeWork05 - Stability of Legendre Polynomials\n");
    printf("lmax = %d, backward start L = %d\n\n", LMAX, LBWD);

    /*
     * Run the experiment for each requested x value.
     */
    for (int i = 0; i < nx; i++) {
        run_one_x(fp, xs[i]);
    }

    fclose(fp);

    printf("\nData written to legendre_errors.dat\n");

    return EXIT_SUCCESS;
}