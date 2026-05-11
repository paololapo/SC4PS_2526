# Pointers, Memory Layout, and Floating-Point Numbers in C

## 1. Pointers in C

### Core idea

A **pointer** is a variable that stores the **memory address** of another variable.

> Think of a pointer as the “GPS location” of a value in memory.

```c
int x = 42;
int *p = &x;
```

Meaning:

- `x` stores the value `42`
- `&x` is the address of `x`
- `p` stores the address of `x`
- `*p` accesses the value stored at that address

### Key operators

| Operator | Meaning |
|---|---|
| `&x` | address of `x` |
| `*p` | value pointed to by `p` |

Example:

```c
int x = 42;
int *p = &x;

*p = 100;   // modifies x through the pointer
```

After this, `x == 100`.

---

## 2. Why Pointers Matter in Scientific Computing

Pointers are essential when you want to:

- pass large arrays to functions without copying them;
- allocate arrays or matrices whose size is known only at runtime;
- allow functions to modify variables directly;
- interface with numerical libraries such as BLAS, LAPACK, or Fortran codes.

Main idea:

> Use pointers when a function must access or modify existing data instead of working on a local copy.

---

## 3. Pointers and Arrays

In C, arrays and pointers are closely related.

When an array is passed to a function, it **decays** into a pointer to its first element.

```c
double v[3] = {1.0, 2.0, 3.0};

scale_array(v, 3, 2.0);
```

Inside the function:

```c
void scale_array(double *a, int n, double alpha)
```

`a` is a pointer to the first element of `v`.

### Important equivalence

```c
a[i] == *(a + i)
```

So array indexing is actually pointer arithmetic.

### Key message

> Passing an array to a function does not copy the full array.  
> The function receives a pointer to the original data.

---

## 4. Passing by Reference

C normally uses **pass by value**.

This means a function receives a copy of the variable.

```c
void swap_wrong(int a, int b)
```

This does **not** modify the original variables.

To modify variables inside a function, pass their addresses:

```c
void swap(int *a, int *b) {
    int tmp = *a;
    *a = *b;
    *b = tmp;
}
```

Call it with:

```c
swap(&x, &y);
```

### Key message

> To let a function modify a variable, pass a pointer to that variable.

---

## 5. Pointer Arithmetic

When a pointer is incremented, it moves by **one element**, not one byte.

```c
double a[4] = {0.1, 0.2, 0.3, 0.4};
double *p = a;

p++;   // now points to a[1]
```

If `p` points to a `double`, then `p + 1` moves by `sizeof(double)` bytes.

### Common numerical uses

```c
for (double *p = a; p < a + n; p++) {
    // process each element
}
```

You can also pass slices:

```c
kernel(a + offset, n - offset);
```

### Warning

Pointer arithmetic is valid only:

- inside the same array;
- or one position past the last element.

Going outside this range gives **undefined behaviour**.

---

## 6. Dynamic Memory Allocation

Use dynamic memory when the size is known only at runtime.

```c
double *x = malloc(n * sizeof(double));
```

or, more idiomatically:

```c
double *x = malloc(n * sizeof *x);
```

### Basic rules

| Rule | Reason |
|---|---|
| Check if `malloc` returns `NULL` | allocation may fail |
| Initialize memory before using it | uninitialized values are garbage |
| Call `free` when done | prevents memory leaks |
| Free each allocation exactly once | prevents undefined behaviour |
| Set pointer to `NULL` after `free` | avoids dangling pointer mistakes |

Example:

```c
double *x = malloc(n * sizeof *x);

if (x == NULL) {
    return 1;
}

/* use x */

free(x);
x = NULL;
```

### Key message

> Every `malloc` should have a corresponding `free`.

---

## 7. Common Pointer Pitfalls

### 1. Uninitialized pointers

```c
int *p;
*p = 10;   // wrong
```

`p` contains a random address.

Correct approach:

```c
int *p = NULL;
```

or:

```c
int x;
int *p = &x;
```

---

### 2. Dangling pointers

```c
int *p = malloc(sizeof *p);
free(p);
*p = 5;   // wrong
```

After `free`, the pointer no longer points to valid memory.

Better:

```c
free(p);
p = NULL;
```

---

### 3. Returning address of a local variable

```c
int *foo(void) {
    int x = 42;
    return &x;   // wrong
}
```

`x` disappears when the function returns.

Correct alternatives:

- allocate memory with `malloc`;
- or let the caller provide the storage.

---

### 4. Mismatched pointer types

```c
int x;
double *p = (double *)&x;   // usually wrong
```

Pointer type should match the type of the object it points to.

---

## 8. Array of Structures vs Structure of Arrays

For 3D points, there are two common layouts.

---

## Array of Structures: AoS

```c
struct Point {
    float x;
    float y;
    float z;
};

struct Point p[N];
```

Memory layout:

```text
x0 y0 z0 | x1 y1 z1 | x2 y2 z2 | ...
```

### Best for

AoS is good when you often access all coordinates of one point together:

```c
p[i].x;
p[i].y;
p[i].z;
```

Example use:

- particle-by-particle operations;
- random access to full points.

---

## Structure of Arrays: SoA

```c
struct Points {
    float x[N];
    float y[N];
    float z[N];
};
```

Memory layout:

```text
x0 x1 x2 ... | y0 y1 y2 ... | z0 z1 z2 ...
```

### Best for

SoA is good when you process one component at a time:

```c
p.x[i];
```

Example use:

- compute average of all `x`;
- vectorized operations;
- cache-friendly bulk operations.

---

## AoS vs SoA Summary

| Aspect | AoS | SoA |
|---|---|---|
| Access style | `p[i].x` | `p.x[i]` |
| Memory grouping | coordinates of same point together | same coordinate component together |
| Good for | per-particle operations | bulk component-wise operations |
| Cache behavior | good if using `x`, `y`, and `z` together | good if using only one component |

### Key message

> Choose the memory layout according to the access pattern.

If you only need all `x` values, SoA is usually more cache-efficient.  
If you need all coordinates of each point, AoS is often more convenient.

---

# Floating-Point Numbers

## 9. Real Numbers vs Floating-Point Numbers

The real numbers are:

- continuous;
- infinite;
- unbounded.

Computers cannot represent all real numbers.

Instead, computers use a finite set of numbers called **floating-point numbers**.

### Key message

> Floating-point numbers are approximations of real numbers.

---

## 10. Scientific Notation and Floating-Point Representation

Scientific notation separates:

- sign;
- significand or mantissa;
- exponent.

Example:

```text
299792458 = 2.99792458 × 10^8
```

Floating-point numbers use the same idea.

Generic form:

```text
x = (-1)^sign × significand × base^exponent
```

For binary computers:

```text
x = (-1)^sign × (1 + f) × 2^n
```

where:

- `sign` is 0 for positive numbers and 1 for negative numbers;
- `1 + f` is the significand;
- `n` is the exponent.

---

## 11. Finite Precision

A real number may require infinitely many digits.

A computer stores only a finite number of digits.

Therefore:

- many real numbers cannot be represented exactly;
- operations can produce results that must be rounded;
- floating-point arithmetic is not exact arithmetic.

### Key message

> Floating-point numbers are discrete, not continuous.

---

## 12. Spacing Between Floating-Point Numbers

For binary floating-point numbers of the form:

```text
x = (1 + f) × 2^n
```

there are a fixed number of representable values in each interval:

```text
[2^n, 2^(n+1))
```

Within this interval, the numbers are evenly spaced.

The spacing depends on the exponent:

```text
spacing = 2^(n-d)
```

where `d` is the number of fractional bits in the significand.

### Consequence

The spacing becomes:

- smaller near small numbers;
- larger near large numbers.

### Key message

> Floating-point numbers are denser near zero and more widely spaced at large magnitudes.

---

## 13. Machine Precision

Machine precision measures the distance between `1` and the next representable number larger than `1`.

For a binary system:

```text
epsilon_machine = 2^(-d)
```

where `d` is the number of fractional bits.

### Interpretation

Machine precision tells you the smallest relative change that can be represented near 1.

---

## 14. IEEE 754 Standard

Modern computers usually follow the IEEE 754 floating-point standard.

### Double precision: 64-bit

| Field | Bits |
|---|---|
| sign | 1 |
| exponent | 11 |
| mantissa | 52 |

The leading `1` of the significand is implicit, so the effective precision is 53 bits.

Machine epsilon:

```text
eps ≈ 2^(-52) ≈ 2.22 × 10^(-16)
```

---

### Single precision: 32-bit

| Field | Bits |
|---|---|
| sign | 1 |
| exponent | 8 |
| mantissa | 23 |

Machine epsilon:

```text
eps ≈ 2^(-23) ≈ 1.19 × 10^(-7)
```

---

### Quadruple precision: 128-bit

| Field | Bits |
|---|---|
| sign | 1 |
| exponent | 15 |
| mantissa | 112 |

Machine epsilon:

```text
eps ≈ 2^(-112) ≈ 1.93 × 10^(-34)
```

---

## 15. Why Not Always Use 128-bit?

Higher precision is not always better in practice.

Main reasons:

### 1. Memory addressing

More bits allow more addressable memory, but 64-bit addressing is already enormous for most practical uses.

Approximate address spaces:

| Architecture | Addressable memory |
|---|---|
| 32-bit | about 4 GB |
| 64-bit | about 16 exabytes |
| 128-bit | astronomically large |

### 2. Energy and performance

Higher precision usually means:

- more memory traffic;
- more energy consumption;
- slower arithmetic;
- larger data structures.

### Key message

> 64-bit floating point is often the best compromise between precision, speed, memory usage, and energy cost.

---

## 16. Rounding

A real number `x` is mapped to the nearest floating-point number:

```text
fl(x)
```

This is called rounding.

If `x` lies in:

```text
[2^m, 2^(m+1))
```

then the distance between neighboring floating-point numbers is:

```text
DFL = 2^(m-d)
```

The maximum rounding error is half the spacing:

```text
|fl(x) - x| ≤ 1/2 × DFL
```

therefore:

```text
|fl(x) - x| ≤ 2^(m-d-1)
```

### Key message

> Every floating-point operation may introduce a small rounding error.

---

# Final Summary

## Pointers

- A pointer stores an address, not a value.
- `&x` gives the address of `x`.
- `*p` accesses the value pointed to by `p`.
- Arrays passed to functions become pointers to their first element.
- Use pointers to modify variables inside functions.
- Use `malloc` for runtime-sized arrays.
- Always pair `malloc` with `free`.
- Avoid uninitialized, dangling, and type-mismatched pointers.

## Memory Layout

- AoS stores data grouped by object.
- SoA stores data grouped by component.
- AoS is convenient for per-object operations.
- SoA is often faster for component-wise loops.
- Cache efficiency depends strongly on memory access patterns.

## Floating-Point Numbers

- Floating-point numbers approximate real numbers.
- They are finite and discrete.
- Not every real number can be represented exactly.
- The spacing between numbers grows with magnitude.
- Machine precision measures the relative spacing near 1.
- In IEEE 754 double precision, machine epsilon is about `2.22e-16`.
- Rounding error is unavoidable in floating-point arithmetic.

## Main Practical Lesson

> In numerical computing, performance and accuracy depend not only on the algorithm, but also on how data is represented, stored, accessed, and rounded.
