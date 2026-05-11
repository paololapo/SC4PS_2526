"""Generate a standard-resolution 3D scatter plot of RANDU triples.

The script implements the historical RANDU linear congruential generator,
builds points of the form (u_n, u_{n+1}, u_{n+2}), and displays the classic
plane structure in a Matplotlib 3D scatter plot.
"""

import numpy as np
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D  # noqa: F401


def randu_uniform(seed, n):
    """Return ``n`` uniforms in ``[0, 1)`` produced by RANDU."""
    values = np.empty(n, dtype=np.uint64)
    state = np.uint64(seed & 0x7FFFFFFF)

    # RANDU recurrence: x_{n+1} = 65539 * x_n mod 2^31.
    for i in range(n):
        state = (np.uint64(65539) * state) % np.uint64(2**31)
        values[i] = state

    return values / float(2**31)


def main():
    """Generate RANDU triples and plot them in 3D."""
    # Generate enough values to create overlapping triples.
    n = 6000
    u = randu_uniform(seed=1, n=n + 2)

    # Build the 3D point cloud (u_n, u_{n+1}, u_{n+2}).
    triples = np.column_stack([u[:-2], u[1:-1], u[2:]])

    # Use Matplotlib defaults for standard output resolution.
    fig = plt.figure(figsize=(8, 6))
    ax = fig.add_subplot(111, projection="3d")

    # A light point cloud makes the plane structure easier to see.
    ax.scatter(
        triples[:3000, 0],
        triples[:3000, 1],
        triples[:3000, 2],
        s=4,
        alpha=0.45,
        color="#E45756",
    )

    ax.set_title("RANDU 3D Correlation Plot")
    ax.set_xlabel("$u_n$")
    ax.set_ylabel("$u_{n+1}$")
    ax.set_zlabel("$u_{n+2}$")
    fig.savefig("randu_3d_scatter.png", dpi=100)

    plt.tight_layout()
    plt.show()


if __name__ == "__main__":
    main()