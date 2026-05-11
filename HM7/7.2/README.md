# Exercise 2 — Monte Carlo Estimate of Pi

## Goal

Estimate \(\pi\) using random points generated with two LCG streams.

One LCG stream generates the \(x\) coordinates.  
The other LCG stream generates the \(y\) coordinates.

---

## Idea

Generate random points in the square:

\[
0 \le x < 1, \qquad 0 \le y < 1
\]

Then count how many points satisfy:

\[
x^2 + y^2 \le 1
\]

These points are inside the quarter unit circle.

The area of the square is:

\[
1
\]

The area of the quarter circle is:

\[
\frac{\pi}{4}
\]

Therefore:

\[
\frac{N_{\text{inside}}}{N} \approx \frac{\pi}{4}
\]

and:

\[
\pi \approx 4 \frac{N_{\text{inside}}}{N}
\]

---

## Files

- `monte_carlo_pi.c`: C program
- `plot.py`: Python plotting script
- `monte_carlo_pi.txt`: data file produced by the C program
- `monte_carlo_pi_estimate.png`: plot of the estimate
- `monte_carlo_pi_error.png`: plot of the absolute error
- `monte_carlo_pi_relative_error.png`: plot of the relative error

---

## Compile

```bash
gcc -std=c11 -Wall -Wextra -O2 monte_carlo_pi.c -o monte_carlo_pi -lm
```

---

## Run the C program

```bash
./monte_carlo_pi
```

This creates:

```text
monte_carlo_pi.txt
```

The columns are:

```text
N    pi_estimate    absolute_error    relative_error
```

---

## Make the plots

```bash
python3 plot.py
```

---

## Expected behavior

As \(N\) increases, the estimate of \(\pi\) should generally improve.

However, the Monte Carlo error is random, so it does not necessarily decrease perfectly at every single step.

The typical Monte Carlo convergence rate is approximately:

\[
\text{error} \sim \frac{1}{\sqrt{N}}
\]

So increasing \(N\) by a factor of 100 usually improves the error by about a factor of 10.

---

## Main take-home message

Monte Carlo methods are simple and flexible, but their convergence is slow compared with many deterministic numerical methods.
