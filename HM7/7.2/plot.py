"""
Plot script for Exercise 2 - Monte Carlo estimate of pi.

This script reads the text file produced by monte_carlo_pi.c and saves plots.

Expected input file:
    monte_carlo_pi.txt

Expected columns:
    N    pi_estimate    absolute_error    relative_error

Run:
    python3 plot.py
"""

import numpy as np
import matplotlib.pyplot as plt


def main():
    input_file = "monte_carlo_pi.txt"

    data = np.loadtxt(input_file)

    N = data[:, 0]
    pi_estimate = data[:, 1]
    absolute_error = data[:, 2]
    relative_error = data[:, 3]

    # ------------------------------------------------------------
    # Plot 1: pi estimate versus N
    # ------------------------------------------------------------
    plt.figure()

    plt.plot(N, pi_estimate, marker="o", label="Monte Carlo estimate")
    plt.axhline(np.pi, linestyle="--", label="true pi")

    plt.xscale("log")
    plt.xlabel("number of samples N")
    plt.ylabel("pi estimate")
    plt.title("Monte Carlo estimate of pi")
    plt.grid(True)
    plt.legend()
    plt.tight_layout()

    plt.savefig("monte_carlo_pi_estimate.png", dpi=200)
    plt.close()

    # ------------------------------------------------------------
    # Plot 2: absolute error versus N
    # ------------------------------------------------------------
    plt.figure()

    plt.plot(N, absolute_error, marker="o", label="absolute error")

    plt.xscale("log")
    plt.yscale("log")
    plt.xlabel("number of samples N")
    plt.ylabel("absolute error")
    plt.title("Monte Carlo error for pi estimate")
    plt.grid(True)
    plt.legend()
    plt.tight_layout()

    plt.savefig("monte_carlo_pi_error.png", dpi=200)
    plt.close()

    # ------------------------------------------------------------
    # Plot 3: relative error versus N
    # ------------------------------------------------------------
    plt.figure()

    plt.plot(N, relative_error, marker="o", label="relative error")

    plt.xscale("log")
    plt.yscale("log")
    plt.xlabel("number of samples N")
    plt.ylabel("relative error")
    plt.title("Monte Carlo relative error for pi estimate")
    plt.grid(True)
    plt.legend()
    plt.tight_layout()

    plt.savefig("monte_carlo_pi_relative_error.png", dpi=200)
    plt.close()

    print("Plots saved:")
    print("  monte_carlo_pi_estimate.png")
    print("  monte_carlo_pi_error.png")
    print("  monte_carlo_pi_relative_error.png")


if __name__ == "__main__":
    main()