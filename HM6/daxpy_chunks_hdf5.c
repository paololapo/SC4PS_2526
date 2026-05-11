/*
 * HomeWork - Chunked DAXPY with HDF5 output
 *
 * Operation:
 *
 *     d[i] = a * x[i] + y[i]
 *
 * This program compares:
 *
 *   1. The original direct implementation with one loop.
 *   2. A chunked implementation with an outer loop over chunks.
 *
 * It also computes:
 *
 *   - the sum of every chunk;
 *   - the total sum from the partial chunk sums;
 *   - a check against the sum of the original direct result.
 *
 * Input:
 *
 *   The input is read from a simple text configuration file:
 *
 *       Variable = Value
 *
 * Example:
 *
 *       N = 100
 *       chunk_size = 8
 *       a = 2.0
 *       x_val = 3.0
 *       y_val = 4.0
 *       output_file = daxpy_chunks.h5
 *
 * Compile:
 *
 *   h5cc -std=c11 -Wall -Wextra -O2 daxpy_chunks_hdf5.c -o daxpy_chunks_hdf5 -lm
 *
 * Run:
 *
 *   ./daxpy_chunks_hdf5 input.txt
 *
 * Output:
 *
 *   The program creates an HDF5 file containing:
 *
 *       /config
 *       /full_vectors/x
 *       /full_vectors/y
 *       /full_vectors/d_reference
 *       /full_vectors/d_chunked
 *       /partial_chunk_sum
 *       /chunks/chunk_0000/x
 *       /chunks/chunk_0000/y
 *       /chunks/chunk_0000/d
 *       /chunks/chunk_0000/partial_sum
 *       ...
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "hdf5.h"

#define MAX_LINE 512
#define MAX_FILENAME 256

/*
 * Structure containing all input parameters.
 */
typedef struct {
    long N;
    long chunk_size;
    double a;
    double x_val;
    double y_val;
    char output_file[MAX_FILENAME];
} Config;

/*
 * Remove spaces, tabs and newlines from the beginning and end of a string.
 */
char *trim(char *s)
{
    char *end;

    while (*s == ' ' || *s == '\t' || *s == '\n' || *s == '\r') {
        s++;
    }

    if (*s == '\0') {
        return s;
    }

    end = s + strlen(s) - 1;

    while (end > s &&
           (*end == ' ' || *end == '\t' || *end == '\n' || *end == '\r')) {
        *end = '\0';
        end--;
    }

    return s;
}

/*
 * Read a simple configuration file.
 *
 * The expected format is:
 *
 *     Variable = Value
 *
 * Lines starting with # are ignored.
 * Text after # is also ignored.
 */
int read_config(const char *filename, Config *cfg)
{
    FILE *fp;
    char line[MAX_LINE];

    /*
     * Initialize values to invalid defaults.
     * This helps us check whether the user forgot some input.
     */
    cfg->N = -1;
    cfg->chunk_size = -1;
    cfg->a = 0.0;
    cfg->x_val = 0.0;
    cfg->y_val = 0.0;
    cfg->output_file[0] = '\0';

    fp = fopen(filename, "r");

    if (fp == NULL) {
        fprintf(stderr, "Error: could not open config file '%s'\n", filename);
        return 0;
    }

    while (fgets(line, sizeof(line), fp) != NULL) {
        char *comment;
        char *equal;
        char *key;
        char *value;

        /*
         * Remove comments.
         */
        comment = strchr(line, '#');
        if (comment != NULL) {
            *comment = '\0';
        }

        /*
         * Search for '='.
         * If there is no '=', skip the line.
         */
        equal = strchr(line, '=');
        if (equal == NULL) {
            continue;
        }

        *equal = '\0';

        key = trim(line);
        value = trim(equal + 1);

        if (strcmp(key, "N") == 0) {
            cfg->N = atol(value);
        } else if (strcmp(key, "chunk_size") == 0) {
            cfg->chunk_size = atol(value);
        } else if (strcmp(key, "a") == 0) {
            cfg->a = atof(value);
        } else if (strcmp(key, "x_val") == 0) {
            cfg->x_val = atof(value);
        } else if (strcmp(key, "y_val") == 0) {
            cfg->y_val = atof(value);
        } else if (strcmp(key, "output_file") == 0) {
            strncpy(cfg->output_file, value, MAX_FILENAME - 1);
            cfg->output_file[MAX_FILENAME - 1] = '\0';
        }
    }

    fclose(fp);

    /*
     * Check that all important quantities were provided.
     */
    if (cfg->N <= 0) {
        fprintf(stderr, "Error: N must be positive.\n");
        return 0;
    }

    if (cfg->chunk_size <= 0) {
        fprintf(stderr, "Error: chunk_size must be positive.\n");
        return 0;
    }

    if (cfg->output_file[0] == '\0') {
        fprintf(stderr, "Error: output_file was not specified.\n");
        return 0;
    }

    return 1;
}

/*
 * Floating-point comparison using a relative tolerance.
 *
 * We do not use == for doubles because roundoff errors may appear.
 */
int almost_equal(double x, double y)
{
    double eps = 1.0e-12;
    double scale = fmax(1.0, fmax(fabs(x), fabs(y)));

    return fabs(x - y) <= eps * scale;
}

/*
 * Original DAXPY implementation.
 *
 * This is the reference implementation with one single loop.
 */
void daxpy_original(long N, double a, const double *x, const double *y, double *d)
{
    for (long i = 0; i < N; i++) {
        d[i] = a * x[i] + y[i];
    }
}

/*
 * Chunked DAXPY implementation.
 *
 * The vector is divided into chunks.
 *
 * For every chunk:
 *
 *   - compute d[i] = a*x[i] + y[i];
 *   - compute the sum of d[i] inside the chunk;
 *   - save that sum into partial_chunk_sum[chunk].
 */
void daxpy_chunked(long N,
                   long chunk_size,
                   double a,
                   const double *x,
                   const double *y,
                   double *d,
                   double *partial_chunk_sum,
                   long *chunk_start,
                   long *chunk_end,
                   long *chunk_length)
{
    long number_of_chunks = (N + chunk_size - 1) / chunk_size;

    for (long chunk = 0; chunk < number_of_chunks; chunk++) {
        long start = chunk * chunk_size;
        long end = start + chunk_size;
        double local_sum = 0.0;

        /*
         * Last chunk may be smaller.
         */
        if (end > N) {
            end = N;
        }

        chunk_start[chunk] = start;
        chunk_end[chunk] = end - 1;
        chunk_length[chunk] = end - start;

        /*
         * Process only the elements of the current chunk.
         */
        for (long i = start; i < end; i++) {
            d[i] = a * x[i] + y[i];
            local_sum += d[i];
        }

        partial_chunk_sum[chunk] = local_sum;
    }
}

/*
 * Sum all elements of a vector.
 */
double sum_vector(const double *v, long N)
{
    double sum = 0.0;

    for (long i = 0; i < N; i++) {
        sum += v[i];
    }

    return sum;
}

/*
 * Sum all elements of the partial chunk sum array.
 */
double sum_partial_chunks(const double *partial_chunk_sum, long number_of_chunks)
{
    double sum = 0.0;

    for (long c = 0; c < number_of_chunks; c++) {
        sum += partial_chunk_sum[c];
    }

    return sum;
}

/*
 * Check that two vectors are equal within floating-point tolerance.
 */
int check_vectors(const double *a, const double *b, long N)
{
    for (long i = 0; i < N; i++) {
        if (!almost_equal(a[i], b[i])) {
            return 0;
        }
    }

    return 1;
}

/*
 * Small helper for checking HDF5 errors.
 */
void check_hdf5_status(herr_t status, const char *message)
{
    if (status < 0) {
        fprintf(stderr, "HDF5 error: %s\n", message);
        exit(EXIT_FAILURE);
    }
}

/*
 * Write a one-dimensional double array to HDF5.
 */
void h5_write_double_vector(hid_t parent, const char *name, const double *data, long n)
{
    hsize_t dims[1];
    hid_t dataspace_id;
    hid_t dataset_id;
    herr_t status;

    dims[0] = (hsize_t)n;

    dataspace_id = H5Screate_simple(1, dims, NULL);
    if (dataspace_id < 0) {
        fprintf(stderr, "HDF5 error: could not create dataspace for %s\n", name);
        exit(EXIT_FAILURE);
    }

    dataset_id = H5Dcreate(parent,
                           name,
                           H5T_NATIVE_DOUBLE,
                           dataspace_id,
                           H5P_DEFAULT,
                           H5P_DEFAULT,
                           H5P_DEFAULT);

    if (dataset_id < 0) {
        fprintf(stderr, "HDF5 error: could not create dataset %s\n", name);
        H5Sclose(dataspace_id);
        exit(EXIT_FAILURE);
    }

    status = H5Dwrite(dataset_id,
                      H5T_NATIVE_DOUBLE,
                      H5S_ALL,
                      H5S_ALL,
                      H5P_DEFAULT,
                      data);

    check_hdf5_status(status, "could not write double vector");

    H5Dclose(dataset_id);
    H5Sclose(dataspace_id);
}

/*
 * Write a one-dimensional long array to HDF5.
 */
void h5_write_long_vector(hid_t parent, const char *name, const long *data, long n)
{
    hsize_t dims[1];
    hid_t dataspace_id;
    hid_t dataset_id;
    herr_t status;

    dims[0] = (hsize_t)n;

    dataspace_id = H5Screate_simple(1, dims, NULL);
    if (dataspace_id < 0) {
        fprintf(stderr, "HDF5 error: could not create dataspace for %s\n", name);
        exit(EXIT_FAILURE);
    }

    dataset_id = H5Dcreate(parent,
                           name,
                           H5T_NATIVE_LONG,
                           dataspace_id,
                           H5P_DEFAULT,
                           H5P_DEFAULT,
                           H5P_DEFAULT);

    if (dataset_id < 0) {
        fprintf(stderr, "HDF5 error: could not create dataset %s\n", name);
        H5Sclose(dataspace_id);
        exit(EXIT_FAILURE);
    }

    status = H5Dwrite(dataset_id,
                      H5T_NATIVE_LONG,
                      H5S_ALL,
                      H5S_ALL,
                      H5P_DEFAULT,
                      data);

    check_hdf5_status(status, "could not write long vector");

    H5Dclose(dataset_id);
    H5Sclose(dataspace_id);
}

/*
 * Write a scalar double as a one-element dataset.
 */
void h5_write_double_scalar(hid_t parent, const char *name, double value)
{
    h5_write_double_vector(parent, name, &value, 1);
}

/*
 * Write a scalar long as a one-element dataset.
 */
void h5_write_long_scalar(hid_t parent, const char *name, long value)
{
    h5_write_long_vector(parent, name, &value, 1);
}

/*
 * Save all useful results to an HDF5 file.
 */
void save_to_hdf5(const Config *cfg,
                  long number_of_chunks,
                  const double *x,
                  const double *y,
                  const double *d_reference,
                  const double *d_chunked,
                  const double *partial_chunk_sum,
                  const long *chunk_start,
                  const long *chunk_end,
                  const long *chunk_length)
{
    hid_t file_id;
    hid_t config_group;
    hid_t vectors_group;
    hid_t chunks_group;
    herr_t status;

    /*
     * Create a new HDF5 file.
     * H5F_ACC_TRUNC means overwrite the file if it already exists.
     */
    file_id = H5Fcreate(cfg->output_file,
                        H5F_ACC_TRUNC,
                        H5P_DEFAULT,
                        H5P_DEFAULT);

    if (file_id < 0) {
        fprintf(stderr, "HDF5 error: could not create file %s\n", cfg->output_file);
        exit(EXIT_FAILURE);
    }

    /*
     * Save input parameters in /config.
     */
    config_group = H5Gcreate(file_id,
                             "/config",
                             H5P_DEFAULT,
                             H5P_DEFAULT,
                             H5P_DEFAULT);

    if (config_group < 0) {
        fprintf(stderr, "HDF5 error: could not create /config group\n");
        H5Fclose(file_id);
        exit(EXIT_FAILURE);
    }

    h5_write_long_scalar(config_group, "N", cfg->N);
    h5_write_long_scalar(config_group, "chunk_size", cfg->chunk_size);
    h5_write_long_scalar(config_group, "number_of_chunks", number_of_chunks);
    h5_write_double_scalar(config_group, "a", cfg->a);
    h5_write_double_scalar(config_group, "x_val", cfg->x_val);
    h5_write_double_scalar(config_group, "y_val", cfg->y_val);

    status = H5Gclose(config_group);
    check_hdf5_status(status, "could not close /config group");

    /*
     * Save complete vectors in /full_vectors.
     */
    vectors_group = H5Gcreate(file_id,
                              "/full_vectors",
                              H5P_DEFAULT,
                              H5P_DEFAULT,
                              H5P_DEFAULT);

    if (vectors_group < 0) {
        fprintf(stderr, "HDF5 error: could not create /full_vectors group\n");
        H5Fclose(file_id);
        exit(EXIT_FAILURE);
    }

    h5_write_double_vector(vectors_group, "x", x, cfg->N);
    h5_write_double_vector(vectors_group, "y", y, cfg->N);
    h5_write_double_vector(vectors_group, "d_reference", d_reference, cfg->N);
    h5_write_double_vector(vectors_group, "d_chunked", d_chunked, cfg->N);

    status = H5Gclose(vectors_group);
    check_hdf5_status(status, "could not close /full_vectors group");

    /*
     * Save chunk metadata and partial sums.
     */
    chunks_group = H5Gcreate(file_id,
                             "/chunks",
                             H5P_DEFAULT,
                             H5P_DEFAULT,
                             H5P_DEFAULT);

    if (chunks_group < 0) {
        fprintf(stderr, "HDF5 error: could not create /chunks group\n");
        H5Fclose(file_id);
        exit(EXIT_FAILURE);
    }

    h5_write_long_vector(chunks_group, "start_index", chunk_start, number_of_chunks);
    h5_write_long_vector(chunks_group, "end_index", chunk_end, number_of_chunks);
    h5_write_long_vector(chunks_group, "length", chunk_length, number_of_chunks);
    h5_write_double_vector(chunks_group,
                           "partial_chunk_sum",
                           partial_chunk_sum,
                           number_of_chunks);

    /*
     * Save every chunk separately.
     *
     * Each chunk has its own group:
     *
     *     /chunks/chunk_0000
     *     /chunks/chunk_0001
     *     ...
     *
     * Inside every chunk group we save x, y, d, and the partial sum.
     */
    for (long c = 0; c < number_of_chunks; c++) {
        char group_name[64];
        hid_t chunk_group;
        long start = chunk_start[c];
        long length = chunk_length[c];

        snprintf(group_name, sizeof(group_name), "chunk_%04ld", c);

        chunk_group = H5Gcreate(chunks_group,
                                group_name,
                                H5P_DEFAULT,
                                H5P_DEFAULT,
                                H5P_DEFAULT);

        if (chunk_group < 0) {
            fprintf(stderr, "HDF5 error: could not create group %s\n", group_name);
            H5Gclose(chunks_group);
            H5Fclose(file_id);
            exit(EXIT_FAILURE);
        }

        h5_write_long_scalar(chunk_group, "start_index", chunk_start[c]);
        h5_write_long_scalar(chunk_group, "end_index", chunk_end[c]);
        h5_write_long_scalar(chunk_group, "length", chunk_length[c]);
        h5_write_double_scalar(chunk_group,
                               "partial_sum",
                               partial_chunk_sum[c]);

        h5_write_double_vector(chunk_group, "x", x + start, length);
        h5_write_double_vector(chunk_group, "y", y + start, length);
        h5_write_double_vector(chunk_group, "d", d_chunked + start, length);

        status = H5Gclose(chunk_group);
        check_hdf5_status(status, "could not close chunk group");
    }

    status = H5Gclose(chunks_group);
    check_hdf5_status(status, "could not close /chunks group");

    status = H5Fclose(file_id);
    check_hdf5_status(status, "could not close HDF5 file");
}

int main(int argc, char *argv[])
{
    Config cfg;
    long number_of_chunks;

    double *x = NULL;
    double *y = NULL;
    double *d_reference = NULL;
    double *d_chunked = NULL;
    double *partial_chunk_sum = NULL;

    long *chunk_start = NULL;
    long *chunk_end = NULL;
    long *chunk_length = NULL;

    double sum_reference;
    double sum_from_chunks;

    int vectors_ok;
    int sums_ok;

    /*
     * The only command-line argument is the name of the input file.
     */
    if (argc != 2) {
        printf("Usage: %s <input_config.txt>\n", argv[0]);
        return EXIT_FAILURE;
    }

    /*
     * Read input parameters from the config file.
     */
    if (!read_config(argv[1], &cfg)) {
        return EXIT_FAILURE;
    }

    /*
     * Compute number of chunks using integer ceiling:
     *
     *     ceil(N / chunk_size)
     *
     * for positive integers:
     *
     *     (N + chunk_size - 1) / chunk_size
     */
    number_of_chunks = (cfg.N + cfg.chunk_size - 1) / cfg.chunk_size;

    /*
     * Allocate vectors.
     */
    x = malloc((size_t)cfg.N * sizeof(*x));
    y = malloc((size_t)cfg.N * sizeof(*y));
    d_reference = malloc((size_t)cfg.N * sizeof(*d_reference));
    d_chunked = malloc((size_t)cfg.N * sizeof(*d_chunked));

    partial_chunk_sum =
        malloc((size_t)number_of_chunks * sizeof(*partial_chunk_sum));

    chunk_start = malloc((size_t)number_of_chunks * sizeof(*chunk_start));
    chunk_end = malloc((size_t)number_of_chunks * sizeof(*chunk_end));
    chunk_length = malloc((size_t)number_of_chunks * sizeof(*chunk_length));

    if (x == NULL || y == NULL || d_reference == NULL || d_chunked == NULL ||
        partial_chunk_sum == NULL ||
        chunk_start == NULL || chunk_end == NULL || chunk_length == NULL) {
        fprintf(stderr, "Error: memory allocation failed.\n");

        free(x);
        free(y);
        free(d_reference);
        free(d_chunked);
        free(partial_chunk_sum);
        free(chunk_start);
        free(chunk_end);
        free(chunk_length);

        return EXIT_FAILURE;
    }

    /*
     * Initialize x and y.
     *
     * As in the original exercise, all elements of x are x_val,
     * and all elements of y are y_val.
     */
    for (long i = 0; i < cfg.N; i++) {
        x[i] = cfg.x_val;
        y[i] = cfg.y_val;
    }

    /*
     * Compute the original direct version.
     */
    daxpy_original(cfg.N, cfg.a, x, y, d_reference);

    /*
     * Compute the chunked version.
     */
    daxpy_chunked(cfg.N,
                  cfg.chunk_size,
                  cfg.a,
                  x,
                  y,
                  d_chunked,
                  partial_chunk_sum,
                  chunk_start,
                  chunk_end,
                  chunk_length);

    /*
     * Check that the chunked result is the same as the original result.
     */
    vectors_ok = check_vectors(d_reference, d_chunked, cfg.N);

    /*
     * Check that the sum from partial chunks equals the sum of the
     * original result.
     */
    sum_reference = sum_vector(d_reference, cfg.N);
    sum_from_chunks = sum_partial_chunks(partial_chunk_sum, number_of_chunks);

    sums_ok = almost_equal(sum_reference, sum_from_chunks);

    /*
     * Print summary.
     */
    printf("Chunked DAXPY\n");
    printf("N                = %ld\n", cfg.N);
    printf("chunk_size       = %ld\n", cfg.chunk_size);
    printf("number_of_chunks = %ld\n", number_of_chunks);
    printf("a                = %.17g\n", cfg.a);
    printf("x_val            = %.17g\n", cfg.x_val);
    printf("y_val            = %.17g\n", cfg.y_val);
    printf("output_file      = %s\n", cfg.output_file);
    printf("\n");

    printf("Vector check: ");
    if (vectors_ok) {
        printf("SUCCESS, d_chunked is equal to d_reference\n");
    } else {
        printf("FAILED, d_chunked differs from d_reference\n");
    }

    printf("Sum from original d     = %.17g\n", sum_reference);
    printf("Sum from partial chunks = %.17g\n", sum_from_chunks);

    printf("Sum check: ");
    if (sums_ok) {
        printf("SUCCESS\n");
    } else {
        printf("FAILED\n");
    }

    /*
     * Save results to HDF5.
     */
    save_to_hdf5(&cfg,
                 number_of_chunks,
                 x,
                 y,
                 d_reference,
                 d_chunked,
                 partial_chunk_sum,
                 chunk_start,
                 chunk_end,
                 chunk_length);

    printf("\nSaved results to HDF5 file: %s\n", cfg.output_file);

    /*
     * Free memory.
     */
    free(x);
    free(y);
    free(d_reference);
    free(d_chunked);
    free(partial_chunk_sum);
    free(chunk_start);
    free(chunk_end);
    free(chunk_length);

    return EXIT_SUCCESS;
}