# Chunked DAXPY with HDF5 Output

## Exercise

Compute

\[
d[i] = a x[i] + y[i]
\]

in two ways:

1. original single-loop DAXPY;
2. chunked DAXPY.

Then:

- check that the chunked result is equal to the original result;
- compute one partial sum per chunk;
- sum the partial sums;
- check that this equals the sum of the original vector;
- save vectors, chunks, and partial sums to an HDF5 file.

---

## Files

- `daxpy_chunks_hdf5.c`: C source code
- `daxpy_input.txt`: example input file

---

## Example input file

```text
N = 100
chunk_size = 8
a = 2.0
x_val = 3.0
y_val = 4.0
output_file = daxpy_chunks.h5
```

The number of chunks is computed automatically as:

\[
\text{number_of_chunks}
=
\left\lceil \frac{N}{\text{chunk_size}} \right\rceil
\]

In integer C arithmetic:

```c
number_of_chunks = (N + chunk_size - 1) / chunk_size;
```

---

## Compile

Using the HDF5 compiler wrapper:

```bash
h5cc -std=c11 -Wall -Wextra -O2 daxpy_chunks_hdf5.c -o daxpy_chunks_hdf5 -lm
```

Alternative with `pkg-config`:

```bash
gcc -std=c11 -Wall -Wextra -O2 daxpy_chunks_hdf5.c -o daxpy_chunks_hdf5 \
    $(pkg-config --cflags --libs hdf5) -lm
```

---

## Run

```bash
./daxpy_chunks_hdf5 daxpy_input.txt
```

---

## Output

The program prints checks on screen and creates an HDF5 file, for example:

```text
daxpy_chunks.h5
```

The HDF5 structure contains:

```text
/config
/full_vectors/x
/full_vectors/y
/full_vectors/d_reference
/full_vectors/d_chunked
/chunks/start_index
/chunks/end_index
/chunks/length
/chunks/partial_chunk_sum
/chunks/chunk_0000/x
/chunks/chunk_0000/y
/chunks/chunk_0000/d
/chunks/chunk_0000/partial_sum
...
```

---

## Inspect the HDF5 file

If `h5dump` is installed:

```bash
h5dump -n daxpy_chunks.h5
```

To see the partial sums:

```bash
h5dump -d /chunks/partial_chunk_sum daxpy_chunks.h5
```

---

## Main take-home messages

- Chunking divides a long vector operation into smaller blocks.
- The last chunk may be smaller than the others.
- The chunked DAXPY should produce the same vector as the original DAXPY.
- Partial sums per chunk can be used to build a total sum.
- Floating-point sums should be compared with a tolerance, not with `==`.
- HDF5 is useful for storing vectors, chunks, metadata, and partial sums in one structured file.
