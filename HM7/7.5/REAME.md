# Exercise 5 — Empirical CDF for the Exponential Sample

## Goal

Generate an exponential sample using the inverse transform method and compare its empirical CDF with the exact CDF.

The exponential parameter is:

\[
\lambda = 1.5.
\]

---

## Theory

The exponential probability density function is:

\[
f(y) = \lambda e^{-\lambda y},
\qquad y \ge 0.
\]

The exact cumulative distribution function is:

\[
F(y) = 1 - e^{-\lambda y}.
\]

To generate the sample, we start from:

\[
U \sim \mathrm{Uniform}(0,1)
\]

and use inverse transform sampling:

\[
Y = -\frac{\ln(1-U)}{\lambda}.
\]

---

## Empirical CDF

Given a sample:

\[
Y_1, Y_2, \dots, Y_n,
\]

first sort it:

\[
Y_{(1)} \le Y_{(2)} \le \dots \le Y_{(n)}.
\]

Then the empirical CDF at the sorted points is:

\[
F_n(Y_{(i)}) = \frac{i}{n}.
\]

In the C code, the index starts from zero, so:

\[
F_n(\text{sample}[i]) = \frac{i+1}{n}.
\]

---

## Files

- `empirical_cdf_exponential.c`: C program
- `plot.py`: Python plotting script
- `empirical_cdf_exponential.txt`: generated CDF data
- `empirical_cdf_exponential.png`: empirical CDF vs exact CDF
- `empirical_cdf_exponential_error.png`: absolute CDF error

---

## Compile

```bash
gcc -std=c11 -Wall -Wextra -O2 empirical_cdf_exponential.c -o empirical_cdf_exponential -lm
```

The `-lm` is needed because the C code uses `log()`.

---

## Run the C program

```bash
./empirical_cdf_exponential
```

This creates:

```text
empirical_cdf_exponential.txt
```

The file contains:

```text
y_sorted    empirical_cdf
```

---

## Make the plots

```bash
python3 plot.py
```

This creates:

```text
empirical_cdf_exponential.png
empirical_cdf_exponential_error.png
```

---

## Expected behavior

The empirical CDF should look like a staircase.

As the sample size increases, the empirical CDF should become closer to the exact CDF:

\[
F(y) = 1 - e^{-\lambda y}.
\]

---

## Main take-home message

The empirical CDF is a direct data-based approximation of the true cumulative distribution function.

For large samples, it converges to the exact CDF.
