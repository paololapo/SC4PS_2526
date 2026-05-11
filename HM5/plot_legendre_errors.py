import numpy as np
import matplotlib.pyplot as plt

# Read the data produced by the C program
data = np.loadtxt("legendre_errors.dat")

# Columns:
# 0: x
# 1: l
# 2: P_reference
# 3: P_forward
# 4: P_backward
# 5: abs_err_forward
# 6: rel_err_forward
# 7: abs_err_backward
# 8: rel_err_backward

x_values = sorted(np.unique(data[:, 0]))

def plot_error(column, ylabel, title, filename):
    plt.figure()

    for x in x_values:
        mask = np.isclose(data[:, 0], x)
        ell = data[mask, 1]
        err = data[mask, column]
        plt.plot(ell, err, marker="o", markersize=3, label=f"x = {x:g}")

    plt.yscale("log")
    plt.xlabel("degree l")
    plt.ylabel(ylabel)
    plt.title(title)
    plt.grid(True)
    plt.legend()
    plt.tight_layout()
    plt.savefig(f"./plots/{filename}", dpi=200)
    plt.close()

plot_error(
    column=6,
    ylabel="relative error",
    title="Forward recurrence: relative error",
    filename="forward_relative_error.png",
)

plot_error(
    column=8,
    ylabel="relative error",
    title="Backward recurrence experiment: relative error",
    filename="backward_relative_error.png",
)

plot_error(
    column=5,
    ylabel="absolute error",
    title="Forward recurrence: absolute error",
    filename="forward_absolute_error.png",
)

plot_error(
    column=7,
    ylabel="absolute error",
    title="Backward recurrence experiment: absolute error",
    filename="backward_absolute_error.png",
)

print("Plots saved.")