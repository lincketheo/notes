import os 
import numpy as np
import seaborn as sns
import matplotlib.pyplot as plt

def plt_init():
    sns.set(style='whitegrid')

    # Configure default styles
    plt.rcParams.update({
        'axes.facecolor': 'black',
        'axes.edgecolor': 'white',
        'axes.labelcolor': 'white',
        'xtick.color': 'white',
        'ytick.color': 'white',
        'text.color': 'white',
        'figure.facecolor': '#2e2e2e',
        'font.size': 12,
        'lines.linewidth': 1,
        'legend.fontsize': 10,
        'legend.facecolor': 'black',
        'legend.edgecolor': 'white'
    })

hackergreen = "#00ff00"
imagblue = "blue"

def parse_fname(fname):
    # Tests were run with symmetric rutf rdt0, so only record rutf
    _, t, rutf, _, snrdb = fname.split("_")
    t = float(t)
    rutf = float(rutf)
    snrdb = float(snrdb[0:-4])
    return t, rutf, snrdb

def caf_peaks_next(d, t, fs):
    dd = d.copy()
    peak = np.max(dd)
    peakn = np.argmax(dd)
    dd[peakn-10:peakn+10] = 0
    next = np.max(dd)
    nextn = np.argmax(dd)
    dd[nextn-10:nextn+10] = 0
    next = np.max(dd)
    nextn = np.argmax(dd)
    success = (peakn) == int(fs * t / 1000)
    return peak, next, success

def power_of(t, snrdb, rutf):
    fname = f"match_{t:.4f}_{rutf:.4f}_{(t-rutf):.4f}_{snrdb:.4f}.bin"
    dd = np.fromfile(os.path.join(data_root, fname), dtype=np.csingle)
    return np.sum(np.abs(dd)**2) / len(dd)

