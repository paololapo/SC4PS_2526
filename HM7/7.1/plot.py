"""
Plot script for Exercise 1 - Coin tosses and Law of Large Numbers.

This script reads the text file produced by coin_toss_lln.c and saves a plot.

Expected input file:
    coin_toss_lln.txt

Expected columns:
    toss_index    running_fraction_of_heads

Run:
    python3 plot.py
"""

import numpy as np
import matplotlib.pyplot as plt


def main():
    input_file = "coin_toss_lln.txt"
    output_plot = "coin_toss_lln.png"

    # Read the data.
    # Lines starting with "#" are ignored automatically.
    data = np.loadtxt(input_file)

    toss_index = data[:, 0]
    running_fraction = data[:, 1]

    plt.figure()

    # Plot the running fraction of heads.
    plt.plot(toss_index, running_fraction, label="running fraction of heads")

    # Add the expected value 0.5 for a fair coin.
    plt.axhline(0.5, linestyle="--", label="expected value = 0.5")

    plt.xlabel("number of tosses")
    plt.ylabel("fraction of heads")
    plt.title("Coin tosses and Law of Large Numbers")
    plt.grid(True)
    plt.legend()
    plt.tight_layout()

    plt.savefig(output_plot, dpi=200)
    plt.close()

    print(f"Plot saved to: {output_plot}")


if __name__ == "__main__":
    main()