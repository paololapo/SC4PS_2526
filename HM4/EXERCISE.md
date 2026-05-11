# Guided Worksheet: Solutions

This is a completed worksheet with synthetic answers for the exercises in `LESSONS.md`, referencing the output of the provided programs.

## Part 1: Before running anything

1. **In your own words, what does an FFT tell you about a signal?**
   An FFT estimates how strongly different frequencies are present in a sampled signal.
2. **What is the difference between:**
   - **sampling rate**: Samples per second ($f_s$).
   - **Nyquist frequency**: Half the sampling rate ($f_N = f_s / 2$), the highest frequency that can be represented without aliasing.
   - **frequency resolution**: The spacing between FFT bins ($\Delta f = 1/T = f_s / N$).
3. **Why is a uniformly spaced time grid important for the FFT routines used in this project?**
   The FFT routine mathematically assumes equally spaced samples. If the grid is non-uniform, the calculated bin frequencies will not match the data.

## Part 2: Build and run

Answer:

1. **Which CSV files are created in `output/`?**
   - `good_sampling_signal.csv` / `good_sampling_spectrum.csv`
   - `undersampled_signal.csv` / `undersampled_spectrum.csv`
   - `short_record_signal.csv` / `short_record_spectrum.csv`
   - `coupled_oscillators_time.csv` / `coupled_oscillators_spectrum.csv`
2. **Which program studies pure sampling issues?**
   `src/sampling_demo.c`
3. **Which program studies a mechanics problem?**
   `src/coupled_oscillators_fft.c`
4. **What two frequencies are reported in the well-sampled case?**
   About `50 Hz` and `120 Hz` (Based on output: `peak 1: f = 50.000 Hz`, `peak 2: f = 120.000 Hz`).

## Part 3: Sampling and aliasing

Focus on `src/sampling_demo.c`.

1. **In the `undersampled` case, the sampling frequency is $f_s = 128\,\mathrm{Hz}$.**
   Given.
2. **Using $f_N = \dfrac{f_s}{2}$, what is the Nyquist frequency $f_N$ for this case?**
   $f_N = 128\,\mathrm{Hz} / 2 = 64\,\mathrm{Hz}$.
3. **Why can a $120\,\mathrm{Hz}$ component not be reconstructed faithfully here?**
   Because $120\,\mathrm{Hz} > 64\,\mathrm{Hz}$, so it exceeds the Nyquist limit and aliases into a lower frequency.
4. **The program reports an aliased peak near `8 Hz`.**
   Given. (From run output: `peak 2: f = 8.000 Hz, amplitude = 0.7000`)
5. **Explain physically and mathematically why a high-frequency signal can appear as a false low-frequency signal after sampling.**
   A sampled sinusoid is only observed at discrete times. Frequencies differing by integer multiples of the sampling frequency produce the same sampled sequence. The alias is $f_{\mathrm{alias}} = |120 - 128|\,\mathrm{Hz} = 8\,\mathrm{Hz}$.

## Part 4: Frequency resolution

Focus on the `short_record` case.

1. **The signal contains $50\,\mathrm{Hz}$ and $55\,\mathrm{Hz}$.**
   Given.
2. **Using $T = N \Delta t$, what is the total record length $T$?**
   $T = N / f_s = 64 / 512\,\mathrm{s} = 0.125\,\mathrm{s}$.
3. **Using $\Delta f = \dfrac{f_s}{N} = \dfrac{1}{T}$, what is the frequency spacing $\Delta f$ of the FFT bins?**
   $\Delta f = 1 / 0.125\,\mathrm{s} = 8\,\mathrm{Hz}$.
4. **Why does this make the two frequencies difficult to separate?**
   The two frequencies differ by only $5\,\mathrm{Hz}$, which is smaller than the FFT bin spacing ($8\,\mathrm{Hz}$), so they merge into one peak (output shows one peak at $48\,\mathrm{Hz}$).
5. **If you wanted to improve the separation, would you first change: the total acquisition time, the plotting tool, or the output file format?**
   The total acquisition time ($T$). Plotting tools and formats don't improve underlying data resolution.

## Part 5: Coupled oscillators

Focus on `src/coupled_oscillators_fft.c`.

1. **Write down the equations of motion used in the code in the form $m \ddot{x}_1 = \cdots$ and $m \ddot{x}_2 = \cdots$.**
   $m \ddot{x}_1 = -(k + k_c)x_1 + k_c x_2$ and $m \ddot{x}_2 = k_c x_1 - (k + k_c)x_2$.
2. **What physical system do these equations represent?**
   Two identical masses attached to walls and coupled by a spring.
3. **What are the two normal modes of this system?**
   An in-phase mode and an out-of-phase mode.
4. **Why does the FFT of $x_1(t)$ show more than one important frequency?**
   The chosen initial conditions excite a combination of normal modes rather than just one.
5. **Why is it useful to compare the numerical FFT peaks with the theoretical normal-mode frequencies?**
   It validates the numerical solution against analytical physics, helping detect modeling or coding mistakes.

## Part 6: Plot inspection

Answer:

1. **Which plot makes aliasing easiest to see?**
   The sampling spectra plot, specifically the `undersampled` spectrum.
2. **Which plot makes limited frequency resolution easiest to see?**
   The `short_record` spectrum plot.
3. **In the coupled-oscillator FFT plot, do the numerical peaks line up well with the theoretical reference frequencies?**
   Yes, the measured peaks ($0.79688\,\mathrm{Hz}$ and $0.99219\,\mathrm{Hz}$) line up closely with theoretical ones ($0.79577\,\mathrm{Hz}$ and $0.99392\,\mathrm{Hz}$).
4. **If they are not exactly identical, what numerical reasons could explain the small difference?**
   Finite observation time, discrete FFT bin spacing ($0.00781\,\mathrm{Hz}$), and the fact that an actual peak rarely lands exactly on a single bin center.

## Part 7: Small code modifications

### Option B: Change the observation time (Example)

1. **Increase the number of samples while keeping the interpretation clear.**
   Increasing $N$ while keeping $\Delta t$ fixed increases total time $T$.
2. **Predict how $\Delta f = \dfrac{1}{T}$ will change.**
   The bin spacing $\Delta f$ will become smaller.
3. **Check whether the $50\,\mathrm{Hz}$ and $55\,\mathrm{Hz}$ peaks become more distinguishable.**
   With smaller bin spacing, the FFT will cleanly resolve the separate $50\,\mathrm{Hz}$ and $55\,\mathrm{Hz}$ peaks.

## Part 8: Reflection

1. **One thing you learned about FFTs from this project.**
   FFTs map discrete time samples into frequency bins, and proper resolution requires sufficient observation time.
2. **One thing you learned about scientific software organization.**
   Separating standard C source files (`src/`) and post-processing Python scripts (`scripts/`) makes execution logic clean and manageable.
3. **One thing you would improve or extend in the project.**
   I would add random noise to the input signals to observe how the FFT peak detection behaves when the signal is not perfectly clean.

## Optional extension

**Example: A damped oscillator**
1. **The model**: $m \ddot{x} + c \dot{x} + kx = 0$
2. **The parameters**: A small damping factor $c > 0$.
3. **The expected spectral signature**: A broadened peak centered around the natural frequency instead of an infinitely sharp spike.
4. **Whether the FFT confirms your expectation**: The FFT of an exponentially decaying sinusoid would show energy spread across neighboring frequency bins.
