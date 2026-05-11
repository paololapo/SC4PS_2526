# HomeWork05 — Stability of Legendre Polynomials

## Goal

The goal is to compare numerical methods for computing the ordinary Legendre polynomials

\[
P_\ell(x), \qquad \ell = 0,\dots,50,
\]

for the values

\[
x = 0.1,\;0.5,\;0.9,\;0.99.
\]

Since \(m=0\), these polynomials are directly connected to the spherical harmonics \(Y_{\ell 0}\), up to a known normalization factor.

---

# 1. Methods Implemented

## 1.1 Forward recurrence

The standard recurrence is Bonnet's recurrence:

\[
P_0(x)=1,
\]

\[
P_1(x)=x,
\]

\[
P_{\ell+1}(x)
=
\frac{(2\ell+1)xP_\ell(x)-\ell P_{\ell-1}(x)}{\ell+1}.
\]

This is implemented in ordinary `double` precision.

This is the natural way to compute ordinary Legendre polynomials.

---

## 1.2 Higher-precision reference

The reference solution is computed using the same recurrence, but with `long double`.

This gives a higher-precision reference compared with `double`.

Important note:

> `long double` is not the same as a true 100-decimal-digit library such as `mpmath`, but it is a simple C-only reference. For \(\ell_{\max}=50\), it is sufficient to observe the expected numerical behavior.

If a stricter reference is required, one can compute the reference values in Python using `mpmath` with:

```python
mp.dps = 100
```

where `100` means 100 decimal digits, not 100 bits.

---

## 1.3 Backward recurrence experiment

The same recurrence can be solved for \(P_{\ell-1}\):

\[
P_{\ell-1}(x)
=
\frac{(2\ell+1)xP_\ell(x)-(\ell+1)P_{\ell+1}(x)}{\ell}.
\]

The Miller-style experiment uses arbitrary final values:

\[
\widetilde P_{L+1}(x)=0,
\]

\[
\widetilde P_L(x)=1.
\]

Then the recurrence is propagated backward.

After that, the sequence is rescaled using:

\[
c = \frac{1}{\widetilde P_0(x)}
\]

so that

\[
P_\ell^{(\mathrm{back})}(x)
=
c \widetilde P_\ell(x).
\]

In the code, I used:

\[
L=80
\]

which is larger than \(\ell_{\max}=50\).

---

# 2. Errors Computed

For every \(x\) and every \(\ell=0,\dots,50\), the code computes:

## Absolute error

\[
e_{\mathrm{abs}}
=
|P_\ell^{(\mathrm{num})}(x)-P_\ell^{(\mathrm{ref})}(x)|.
\]

## Relative error

\[
e_{\mathrm{rel}}
=
\frac{|P_\ell^{(\mathrm{num})}(x)-P_\ell^{(\mathrm{ref})}(x)|}
{|P_\ell^{(\mathrm{ref})}(x)|}.
\]

If the reference value is very close to zero, the relative error can become misleading. In that case, the absolute error is more meaningful.

---

# 3. How to Compile and Run

Compile the C code with:

```sh
gcc -std=c11 -Wall -Wextra -O2 legendre_stability.c -lm -o legendre_stability
```

Run it with:

```sh
./legendre_stability
```

The program creates the file:

```text
legendre_errors.dat
```

The columns are:

```text
x
l
P_reference
P_forward
P_backward
abs_err_forward
rel_err_forward
abs_err_backward
rel_err_backward
```

---

# 4. How to Plot the Relative Error

The data file can be plotted with `gnuplot`.

Example for the forward recurrence:

```gnuplot
set logscale y
set xlabel "l"
set ylabel "relative error"
set title "Forward recurrence relative error"

plot \
"legendre_errors.dat" using ((abs($1-0.1)<1e-12)?$2:1/0):7 with linespoints title "x=0.1", \
"legendre_errors.dat" using ((abs($1-0.5)<1e-12)?$2:1/0):7 with linespoints title "x=0.5", \
"legendre_errors.dat" using ((abs($1-0.9)<1e-12)?$2:1/0):7 with linespoints title "x=0.9", \
"legendre_errors.dat" using ((abs($1-0.99)<1e-12)?$2:1/0):7 with linespoints title "x=0.99"
```

Example for the backward experiment:

```gnuplot
set logscale y
set xlabel "l"
set ylabel "relative error"
set title "Backward recurrence experiment relative error"

plot \
"legendre_errors.dat" using ((abs($1-0.1)<1e-12)?$2:1/0):9 with linespoints title "x=0.1", \
"legendre_errors.dat" using ((abs($1-0.5)<1e-12)?$2:1/0):9 with linespoints title "x=0.5", \
"legendre_errors.dat" using ((abs($1-0.9)<1e-12)?$2:1/0):9 with linespoints title "x=0.9", \
"legendre_errors.dat" using ((abs($1-0.99)<1e-12)?$2:1/0):9 with linespoints title "x=0.99"
```

The relative error is plotted on a logarithmic scale because the errors can vary by many orders of magnitude.

---

# 5. Expected Numerical Behavior

## Forward recurrence

The forward recurrence should remain close to the high-precision reference for:

\[
x=0.1,\;0.5,\;0.9,\;0.99
\]

and

\[
\ell_{\max}=50.
\]

The errors are expected to be close to double-precision roundoff.

So the forward recurrence is stable in this test.

---

## Backward recurrence experiment

The backward recurrence experiment usually does not recover the true Legendre polynomial sequence.

The reason is that a three-term recurrence has two independent solutions.

Starting from

\[
\widetilde P_{L+1}=0,
\qquad
\widetilde P_L=1
\]

selects an arbitrary linear combination of the two solutions.

The final rescaling condition

\[
\widetilde P_0 = 1
\]

fixes only the normalization. It does not guarantee that the whole sequence is the Legendre polynomial sequence.

---

# 6. Why Miller's Algorithm Does Not Work Here

Miller's algorithm is useful when the desired solution is the minimal solution in the direction of the recurrence.

That means that, during backward propagation, the unwanted solution should die away and the wanted solution should dominate after normalization.

For ordinary Legendre polynomials \(P_\ell(x)\) with

\[
|x| < 1,
\]

this clean dominant/minimal separation does not select \(P_\ell(x)\) in the requested experiment.

Therefore:

> The arbitrary backward recurrence with only \(Q_0=1\) normalization does not generally compute \(P_\ell(x)\).

---

# 7. Connection to Spherical Harmonics

For \(m=0\), the spherical harmonics are

\[
Y_{\ell 0}(\theta,\phi)
=
\sqrt{\frac{2\ell+1}{4\pi}}
P_\ell(\cos\theta).
\]

Therefore, if

\[
x=\cos\theta,
\]

then an error in \(P_\ell(x)\) directly affects \(Y_{\ell 0}\).

The only difference is the known normalization factor:

\[
\sqrt{\frac{2\ell+1}{4\pi}}.
\]

So numerical stability in computing \(P_\ell(x)\) is important for stable computation of spherical harmonics.

---

# 8. Final Answers to the Questions

## Is the forward recurrence stable?

Yes.

For the tested values

\[
x=0.1,\;0.5,\;0.9,\;0.99
\]

and

\[
\ell_{\max}=50,
\]

the forward recurrence is expected to remain close to the high-precision reference.

---

## Does the backward recurrence recover \(P_\ell(x)\)?

Generally, no.

The arbitrary Miller-style experiment

\[
Q_{L+1}=0,\qquad Q_L=1
\]

followed by rescaling with

\[
Q_0=1
\]

does not generally recover the ordinary Legendre polynomials.

---

## Why not?

Because a three-term recurrence has two independent solutions.

The backward procedure selects an arbitrary combination of them.

Rescaling fixes only one value, \(P_0\), but not the full sequence.

---

## When is Miller's algorithm useful?

Miller's algorithm is useful when the desired solution is minimal in the direction of propagation.

In that case, the unstable unwanted component is suppressed and the desired solution can be recovered after normalization.

That condition is not satisfied in this simple experiment for ordinary \(P_\ell(x)\) on \(|x|<1\).

---

# 9. Main Conclusion

For this homework:

- the standard forward recurrence is stable for the tested values;
- the backward recurrence experiment is not a reliable way to compute \(P_\ell(x)\);
- Miller's algorithm works only when the desired solution is genuinely minimal;
- errors in \(P_\ell(\cos\theta)\) propagate directly to \(Y_{\ell 0}\), up to normalization.

The main lesson is:

> A recurrence relation can be mathematically correct but numerically stable only in the correct direction.
