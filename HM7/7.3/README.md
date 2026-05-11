# Exercise 3 — Change of Variables: \(Y = U^2\)

## Goal

Generate random values

\[
U \sim \mathrm{Uniform}(0,1)
\]

using the LCG, then transform them as

\[
Y = U^2.
\]

Compare the histogram of \(Y\) with the analytic density.

---

## Theory

Since

\[
Y = U^2,
\]

we can write

\[
U = \sqrt{Y}.
\]

Using the change of variables formula:

\[
f_Y(y) = f_U(\sqrt{y}) \left| \frac{d}{dy}\sqrt{y} \right|.
\]

Since \(U\) is uniform on \([0,1]\),

\[
f_U(u)=1.
\]

Therefore:

\[
f_Y(y) = \frac{1}{2\sqrt{y}},
\qquad 0 < y \le 1.
\]

The density diverges near \(y=0\), but it is still integrable.

---

## Files

- `change_variables_y_u2.c`: C program
- `plot.py`: Python plotting script
- `change_variables_y_u2.txt`: generated data
- `change_variables_y_u2_histogram.png`: generated plot

---

## Compile

```bash
gcc -std=c11 -Wall -Wextra -O2 change_variables_y_u2.c -o change_variables_y_u2
```

---

## Run the C program

```bash
./change_variables_y_u2
```

This creates:

```text
change_variables_y_u2.txt
```

The file contains one column:

```text
y
```

where:

\[
y = u^2.
\]

---

## Make the plot

```bash
python3 plot.py
```

This creates:

```text
change_variables_y_u2_histogram.png
```

---

## Expected behavior

The histogram should be larger near \(y=0\), because many values of \(U\) close to zero are mapped to even smaller values of \(Y\).

The simulated histogram should follow the analytic curve:

\[
f_Y(y) = \frac{1}{2\sqrt{y}}.
\]

---

## Main take-home message

A nonlinear transformation changes the probability density.

Even if \(U\) is uniform, \(Y=U^2\) is not uniform.
