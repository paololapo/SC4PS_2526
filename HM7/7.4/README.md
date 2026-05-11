# Exercise 4 — Inverse Transform Sampling: Exponential Distribution

## Goal

Generate an exponential random variable using the LCG output.

We start from:

\[
U \sim \mathrm{Uniform}(0,1)
\]

and use:

\[
Y = -\frac{\ln(1-U)}{\lambda}
\]

with:

\[
\lambda = 1.5.
\]

---

## Theory

The exponential probability density function is:

\[
f_Y(y) = \lambda e^{-\lambda y},
\qquad y \ge 0.
\]

The cumulative distribution function is:

\[
F_Y(y) = 1 - e^{-\lambda y}.
\]

The inverse transform method sets:

\[
U = F_Y(Y).
\]

So:

\[
U = 1 - e^{-\lambda Y}.
\]

Solving for \(Y\):

\[
e^{-\lambda Y} = 1-U
\]

\[
-\lambda Y = \ln(1-U)
\]

\[
Y = -\frac{\ln(1-U)}{\lambda}.
\]

---

## Files

- `inverse_transform_exponential.c`: C program
- `plot.py`: Python plotting script
- `inverse_transform_exponential.txt`: generated data
- `inverse_transform_exponential_histogram.png`: generated plot

---

## Compile

```bash
gcc -std=c11 -Wall -Wextra -O2 inverse_transform_exponential.c -o inverse_transform_exponential -lm
```

The `-lm` is needed because the C code uses `log()` from the math library.

---

## Run the C program

```bash
./inverse_transform_exponential
```

This creates:

```text
inverse_transform_exponential.txt
```

The file contains one column:

```text
y
```

where:

\[
y = -\frac{\ln(1-u)}{\lambda}.
\]

---

## Make the plot

```bash
python3 plot.py
```

This creates:

```text
inverse_transform_exponential_histogram.png
```

---

## Expected behavior

The histogram should follow the exponential PDF:

\[
f_Y(y) = \lambda e^{-\lambda y}.
\]

For \(\lambda = 1.5\), the distribution is largest near \(y=0\) and then decays exponentially.

---

## Main take-home message

The inverse transform method lets us generate non-uniform random variables from uniform random numbers by applying the inverse cumulative distribution function.
