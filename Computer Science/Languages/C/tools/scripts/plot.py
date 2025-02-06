#!/usr/bin/env python3

import sys
import numpy as np
import matplotlib.pyplot as plt

# Define a mapping of data types to numpy dtypes
DTYPE_MAP = {
    "CHAR": np.int8,
    "U8": np.uint8,
    "U16": np.uint16,
    "U32": np.uint32,
    "U64": np.uint64,
    "I8": np.int8,
    "I16": np.int16,
    "I32": np.int32,
    "I64": np.int64,
    "F32": np.float32,
    "F64": np.float64,
    "CF64": np.complex64,
    "CF128": np.complex128,
}


def usage():
    print("Usage: ./plot.py [FILE] DTYPE")
    print("       DTYPE: One of CHAR, U8, U16, U32, U64, I8, I16, I32, I64, F32, F64, CF64, CF128")
    sys.exit(1)


def read_data(dtype, file):
    if file == '-' or file is None:
        return np.frombuffer(sys.stdin.buffer.read(), dtype=dtype)
    else:
        return np.fromfile(file, dtype=dtype)


def plot_data(data, dtype_str):
    if np.iscomplexobj(data):
        # Four plots for complex data: real, imaginary, phase, and absolute values
        _, axs = plt.subplots(4, 1, figsize=(10, 8))
        axs[0].plot(data.real)
        axs[0].set_title(f"{dtype_str} - Real Part")
        axs[1].plot(data.imag)
        axs[1].set_title(f"{dtype_str} - Imaginary Part")
        axs[2].plot(np.angle(data))
        axs[2].set_title(f"{dtype_str} - Phase")
        axs[3].plot(np.abs(data))
        axs[3].set_title(f"{dtype_str} - Magnitude")

        for ax in axs:
            ax.set_xlabel("Sample Index")
            ax.set_ylabel("Amplitude")

        plt.tight_layout()
    else:
        plt.plot(data)
        plt.title(f"{dtype_str} Data")
        plt.xlabel("Sample Index")
        plt.ylabel("Value")

    plt.show()


def main():
    if len(sys.argv) < 2:
        usage()

    file = None
    dtype_str = sys.argv[1] if len(sys.argv) == 2 else sys.argv[2]

    if not dtype_str or dtype_str not in DTYPE_MAP:
        usage()

    file = sys.argv[1] if len(sys.argv) == 3 else None
    dtype = DTYPE_MAP[dtype_str]
    data = read_data(dtype, file)
    plot_data(data, dtype_str)


if __name__ == "__main__":
    main()
