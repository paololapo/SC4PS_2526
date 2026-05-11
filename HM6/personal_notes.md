# Key Take-Home Messages

## 1. Floating-Point Numbers

### Real numbers vs floating-point numbers

- Real numbers are continuous and unbounded.
- Computers cannot represent all real numbers.
- Floating-point numbers form a finite and discrete subset of real numbers.

Main idea:

> Floating-point numbers are approximations.

---

### Floating-point representation

A floating-point number has the form:

\[
\varphi = (-1)^{\text{sign}} S b^m
\]

where:

- `sign` determines positive or negative;
- \(S = 1 + f\) is the significand or mantissa;
- \(b\) is the base, usually \(2\) in computers;
- \(m\) is the exponent.

In binary:

\[
x = (-1)^{\text{sign}}(1+f)2^n
\]

The leading `1` is often implicit.

---

### Finite precision

In theory, numbers may need infinitely many digits.

In practice, computers store only a finite number of digits:

\[
\sum_{i=0}^{\infty} \rightarrow \sum_{i=0}^{d}
\]

Consequences:

- not all real numbers are representable;
- arithmetic operations may produce non-representable results;
- rounding is unavoidable.

---

### Spacing between floating-point numbers

For binary numbers in the interval:

\[
[2^n, 2^{n+1})
\]

there are \(2^d\) equally spaced floating-point numbers.

The spacing is:

\[
2^{n-d}
\]

So:

- numbers are denser near zero;
- numbers are more spaced out at large magnitudes.

---

### Machine precision

Machine precision measures the gap between `1` and the next representable number larger than `1`.

\[
\epsilon_{\text{mach}} = 2^{-d}
\]

For IEEE 754:

| Type | Mantissa bits | Machine epsilon |
|---|---:|---:|
| 32-bit float | 23 | \(\approx 1.19 \times 10^{-7}\) |
| 64-bit double | 52 | \(\approx 2.22 \times 10^{-16}\) |
| 128-bit quad | 112 | \(\approx 1.93 \times 10^{-34}\) |

Important:

> A 64-bit `double` does not have 64 mantissa bits.  
> It has 1 sign bit, 11 exponent bits, and 52 stored mantissa bits.

---

### Rounding

The floating-point representation of a real number is written as:

\[
fl(x)
\]

It is the nearest representable floating-point number.

If \(x \in [2^m, 2^{m+1})\), the spacing is:

\[
DFL = 2^{m-d}
\]

The rounding error satisfies:

\[
|fl(x)-x| \leq \frac{1}{2}DFL
\]

or:

\[
|fl(x)-x| \leq 2^{m-d-1}
\]

Main idea:

> Every floating-point operation can introduce a small rounding error.

---

### Floating-point summation exercise

For the vector:

```text
[1.0, 1.0e16, -1.0e16, -0.5]
```

different summation methods may give different results.

Reason:

- floating-point addition is not associative;
- adding a very small number to a very large number can lose the small contribution;
- changing the order of summation can change the final result;
- Kahan summation reduces this loss by keeping track of the lost compensation.

Main lesson:

> In floating-point arithmetic, `(a + b) + c` may not equal `a + (b + c)`.

---

## 2. Git

### What Git is

Git is a version control system.

It tracks changes in code and documents over time.

It avoids messy filenames such as:

```text
analysis_final.py
analysis_final2.py
analysis_really_final.py
```

Main idea:

> Git is like a laboratory notebook for code.

---

### Why Git is useful in research

Git helps with:

- reproducibility;
- organization;
- collaboration;
- backups;
- experimenting with alternative approaches.

For physics projects, Git makes it easier to know exactly which version of the code produced a result.

---

### Core concepts

| Concept | Meaning |
|---|---|
| Repository | Project folder tracked by Git |
| Commit | Snapshot of the project |
| Branch | Independent line of development |
| Merge | Combine changes from branches |
| Remote | Online copy of the repository |
| HEAD | Current position in the commit history |

---

### Basic workflow

```bash
git status
git add file.c
git commit -m "Describe the change"
```

Typical cycle:

1. modify files;
2. check status;
3. stage changes;
4. commit changes.

---

### Branching

Branches are useful for experiments.

```bash
git switch -c new-analysis
```

Work on the branch, then merge:

```bash
git switch main
git merge new-analysis
```

Main idea:

> Use branches to try new ideas without breaking the main code.

---

### Remotes

A remote repository, such as GitHub or GitLab, is useful for backup and collaboration.

Common commands:

```bash
git clone <url>
git pull
git push
```

Main idea:

> `pull` gets changes from the remote, `push` sends your changes to the remote.

---

### Daily commands

| Command | Purpose |
|---|---|
| `git status` | Check current state |
| `git add` | Stage changes |
| `git commit` | Save a snapshot |
| `git log` | See history |
| `git switch` | Change branch |
| `git merge` | Merge branches |
| `git pull` | Get remote changes |
| `git push` | Send remote changes |
| `git diff` | Inspect changes |

---

## 3. Unit Testing

### Why testing matters

Unit testing is useful in scientific computing because it improves:

- correctness;
- debugging;
- reproducibility;
- confidence when modifying code;
- collaboration.

Main idea:

> Tests help ensure that your scientific results are not caused by hidden bugs.

---

### What a unit test is

A unit test checks one small part of the code, usually one function.

Example:

- test an energy function;
- test a force calculation;
- test a vector normalization routine;
- test an ODE update step.

A good unit test is:

- small;
- fast;
- isolated;
- repeatable;
- self-checking.

---

### What to test in physics code

Test:

- physical formulas;
- numerical algorithms;
- edge cases;
- invalid inputs;
- utility functions;
- conservation laws when possible.

Examples:

- zero velocity;
- negative mass;
- division by zero;
- known analytical solutions.

---

### Floating-point comparisons in tests

Avoid direct equality for computed floating-point values.

Bad:

```python
assert result == expected
```

Better:

```python
assert result == pytest.approx(expected)
```

or in `unittest`:

```python
self.assertAlmostEqual(result, expected)
```

Main idea:

> Floating-point tests should use tolerances.

---

### Best practices

- Write tests for new code early.
- Run tests often.
- Use meaningful test names.
- Test both valid and invalid inputs.
- Do not test external libraries; test your own code.
- Use analytical solutions when available.

---

## 4. Modern Workflow: CI/CD

### Version control as the foundation

Modern workflows rely on Git or another version control system.

Version control provides:

- a shared source of truth;
- history;
- branches;
- triggers for automatic testing.

---

### Automated testing

Automated tests run without manual intervention.

Testing levels:

| Level | Description |
|---|---|
| Unit tests | Small, fast, many |
| Integration tests | Check interaction between components |
| End-to-end tests | Test the full system |

Main idea:

> Unit tests are the base of the testing pyramid.

---

### Continuous Integration

Continuous Integration, or CI, means:

> Every code change is automatically built and tested.

Typical CI workflow:

1. developer pushes code;
2. CI server detects the change;
3. project is built;
4. tests are run;
5. result is reported.

Benefits:

- catches bugs early;
- avoids integration problems;
- keeps the main branch healthy;
- gives fast feedback.

---

### Continuous Delivery and Deployment

Continuous Delivery means the code is automatically prepared for release, but final deployment is manual.

Continuous Deployment means every successful change is automatically deployed.

Main difference:

| Practice | Final deployment |
|---|---|
| Continuous Delivery | Manual approval |
| Continuous Deployment | Automatic |

---

### Practical starting point

For a research project:

1. use Git;
2. write unit tests;
3. set up automatic testing on push;
4. add more tests over time;
5. automate builds and releases when useful.

Main idea:

> Start small: Git + tests + simple CI already gives large benefits.

---

## 5. CMake

### What CMake is

CMake is a cross-platform build system generator.

It does not usually build the code directly.  
Instead, it generates build files such as Makefiles.

Basic workflow:

```bash
mkdir build
cd build
cmake ..
make
```

---

### Why CMake is useful

CMake helps when projects have:

- multiple source files;
- multiple languages;
- external libraries;
- different operating systems;
- different compilers;
- HPC environments.

Main idea:

> CMake makes building complex scientific code more reproducible and portable.

---

### Core files and concepts

| Concept | Meaning |
|---|---|
| `CMakeLists.txt` | Main build configuration file |
| Generator | Creates Makefiles or other build systems |
| Build directory | Where compilation happens |
| Target | Executable or library to build |

---

### Basic commands

```cmake
cmake_minimum_required(VERSION 3.10)
project(MyProject LANGUAGES CXX)

add_executable(my_program main.cpp)
```

Useful commands:

| Command | Purpose |
|---|---|
| `project()` | Define project |
| `add_executable()` | Build executable |
| `add_library()` | Build library |
| `target_link_libraries()` | Link libraries |
| `target_include_directories()` | Add include paths |
| `find_package()` | Find dependencies |

---

### Dependencies

CMake can find and link libraries such as:

- GSL;
- FFTW;
- HDF5;
- MPI;
- OpenMP;
- CUDA.

Example:

```cmake
find_package(GSL REQUIRED)
target_link_libraries(my_code PRIVATE GSL::gsl GSL::gslcblas)
```

---

### Testing with CMake

CMake can register tests:

```cmake
enable_testing()
add_test(NAME MyTest COMMAND my_test_executable)
```

Main idea:

> CMake can connect compilation, dependencies, and testing in one workflow.

---

## 6. HDF5

### What HDF5 is

HDF5 is a file format and library for storing large and structured scientific data.

It is useful for:

- large arrays;
- simulation outputs;
- experimental data;
- metadata;
- multi-dimensional datasets.

Main idea:

> HDF5 stores both data and information about the data.

---

### Why not just text or binary files?

| Format | Problem |
|---|---|
| Text files | Slow and large |
| Raw binary | Fast but not self-describing |
| Many small files | Hard to organize |

HDF5 solves these problems by being:

- efficient;
- structured;
- self-describing;
- suitable for large datasets.

---

### HDF5 structure

An HDF5 file is similar to a file system.

| HDF5 object | Analogy |
|---|---|
| File | Disk/file system |
| Group | Folder |
| Dataset | File containing array data |
| Attribute | Metadata note |

Example structure:

```text
/
├── Simulation
│   ├── Parameters
│   ├── Time_0001
│   │   ├── Temperature
│   │   ├── Velocity
│   │   └── Density
│   └── Time_0002
```

---

### Important C API concepts

| Type | Meaning |
|---|---|
| `hid_t` | Handle to an HDF5 object |
| `herr_t` | Error/status return type |

Important rule:

> Every opened or created HDF5 handle must be closed.

Examples:

```c
H5Fclose(file_id);
H5Gclose(group_id);
H5Dclose(dataset_id);
H5Sclose(dataspace_id);
H5Aclose(attribute_id);
```

---

### Typical HDF5 workflow in C

1. create or open a file;
2. create groups;
3. define dataspace dimensions;
4. create datasets;
5. write data;
6. add attributes;
7. close all handles.

---

### Attributes

Attributes store metadata such as:

- units;
- simulation parameters;
- code version;
- time step;
- grid size;
- author;
- date.

Main idea:

> Use attributes to make files self-describing.

---

### Hyperslabs

Hyperslabs allow reading or writing only part of a large dataset.

This is important when the full dataset is too large to fit in memory.

Main idea:

> Do not load huge arrays all at once if you only need a slice.

---

### HDF5 best practices

- Plan the file structure before coding.
- Use clear group and dataset names.
- Store units and parameters as attributes.
- Check errors.
- Close all handles.
- Use hyperslabs for large data.
- Do not usually track large `.h5` files in Git.

---

# Final Compact Summary

## Numerical computing

- Floating-point numbers are finite approximations of real numbers.
- Rounding error is unavoidable.
- Floating-point addition is not associative.
- Use stable algorithms and tolerance-based comparisons.

## Research software workflow

- Use Git to track code history.
- Use branches for experiments.
- Use remotes for backup and collaboration.
- Write unit tests to protect correctness.
- Use CI to run tests automatically.

## Building and data management

- Use CMake for portable and reproducible builds.
- Use HDF5 for large, structured, self-describing scientific data.
- Store metadata with attributes.
- Use hyperslabs for efficient partial I/O.

Main lesson:

> Reliable scientific computing is not only about equations: it also depends on numerical accuracy, version control, testing, build systems, and good data management.
