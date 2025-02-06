import numpy as np
import matplotlib.pyplot as plt
from crtzt import *
import numpy as np
import matplotlib.pyplot as plt
from scipy.optimize import curve_fit

# Initialize plots and colors
plt_init()

file_path = "outputs/snr_test_results.txt"

def parse_data(file_path):
    data = {"t": [], "SNRDB": [], "SNRDB Dist Peak": []}
    
    with open(file_path, 'r') as file:
        for line in file:
            parts = line.split(", ")
            t = float(parts[0].split(": ")[1])
            snrdb = float(parts[1].split(": ")[1])
            snrdb_dist_peak = float(parts[2].split(": ")[1])
            
            data["t"].append(t)
            data["SNRDB"].append(snrdb)
            data["SNRDB Dist Peak"].append(snrdb_dist_peak)
    
    return data

def interpolate_x(x, y, target_y):
    for i in range(len(y) - 1):
        if (y[i] - target_y) * (y[i + 1] - target_y) <= 0:  # Check for crossing
            # Linear interpolation
            x0, y0 = x[i], y[i]
            x1, y1 = x[i + 1], y[i + 1]
            return x0 + (target_y - y0) * (x1 - x0) / (y1 - y0)
    return None

def plot_data(data):
    unique_t = sorted(set(data["t"]))
    plt.figure(figsize=(10, 6))
    
    for t in unique_t:
        x = [data["SNRDB"][i] for i in range(len(data["t"])) if data["t"][i] == t]
        y = [data["SNRDB Dist Peak"][i] for i in range(len(data["t"])) if data["t"][i] == t]
        
        plt.plot(x, y, label=f"t = {t:.1f}")
        
        # Interpolate intersections for y=0 and y=3
        x_intersection_0 = interpolate_x(x, y, 0)
        x_intersection_3 = interpolate_x(x, y, 3)
        
        if x_intersection_0 is not None:
            text_x = x_intersection_0 + (0.5 if t == 1 else 0)  # Shift right for t=1
            plt.text(text_x, 0, f"{x_intersection_0:.2f}", color="white",
                     fontsize=8, ha="center", va="bottom",
                     bbox=dict(boxstyle="round", alpha=0.5, color="red"), fontweight="bold")
        if x_intersection_3 is not None:
            plt.text(x_intersection_3, 3, f"{x_intersection_3:.2f}", color="white",
                     fontsize=8, ha="center", va="bottom",
                     bbox=dict(boxstyle="round", alpha=0.5, color="green"), fontweight="bold")
    
    # Add horizontal lines
    plt.axhline(y=0, color="red", linestyle="--", label="Breakpoint SNR (dB)")
    plt.axhline(y=3, color="green", linestyle="--", label="Reasonable Threshold SNR (dB)")
    
    # Add title, labels, and legend
    plt.title("Peak to Next Peak Distance (db) vs Applied Channel SNRdb")
    plt.xlabel("Applied Channel SNRDB (dB)")
    plt.ylabel("10log10(Actual Peak) - 10log10(Noise Floor Peak)")
    plt.legend(title="Time (t)")
    plt.grid(True)
    
    # Show the plot
    plt.show()

# Example usage
data = parse_data(file_path)
plot_data(data)

##########################################
# Data
x = np.array([1, 2, 3, 5, 10])  # Independent variable
y1 = np.array([-55.69, -56.51, -58.37, -60.64, -65.51])  # Reasonable Threshold SNR (dB)
y2 = np.array([-59.29, -59.76, -61.64, -63.45, -69.26])  # Breakpoint SNR (dB)

# Define a linear model for fitting
def linear_model(x, a, b):
    return a * x + b

# Fit the data for y1 (Reasonable Threshold)
params1, _ = curve_fit(linear_model, x, y1)

# Fit the data for y2 (Breakpoint)
params2, _ = curve_fit(linear_model, x, y2)

# Extrapolate for a larger range
x_extrapolated = np.linspace(0, 20, 100)
y1_extrapolated = linear_model(x_extrapolated, *params1)
y2_extrapolated = linear_model(x_extrapolated, *params2)

# Plot
plt.figure(figsize=(10, 6))

# Plot Reasonable Threshold SNR (y1)
plt.plot(x_extrapolated, y1_extrapolated, label="Reasonable Threshold SNR (dB) (Fit)", color="blue", linestyle="-")
plt.scatter(x, y1, label="Reasonable Threshold SNR (dB) (Data)", color="blue", marker="o")

# Plot Breakpoint SNR (y2)
plt.plot(x_extrapolated, y2_extrapolated, label="Breakpoint SNR (dB) (Fit)", color="yellow", linestyle="-")
plt.scatter(x, y2, label="Breakpoint SNR (dB) (Data)", color="yellow", marker="o")

# Labels and legend
plt.xlabel("Signal Duration (s)")
plt.ylabel("Applied Channel SNR (dB)")
plt.title("Peak to Noise Distance (dB) vs Signal Duration (s)")
plt.legend()
plt.grid(True)

# Show the plot
plt.show()
