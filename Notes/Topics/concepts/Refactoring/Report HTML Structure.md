

# XCVR <LABEL>

# Sample Plot of single tone at Receiver LO frequency 320 MHz

if exists:
    /jpgroot/{device_name}_{sn}_strx_{xcvr num}0_320mhz_spectrum.jpg
else if exists:
    /jpgroot/{device_name}_{sn}_strx_{xcvr num}1_320mhz_spectrum.jpg
else:
    "Data N/A"

# Gain Flatness

if exists:
    /jpgroot/{device_name}_{sn}_xcvr{xcvr num}_rx_gf_both_chan_{config.test_single_tone_rx.full_sweep.start_freq}mhz_{config.test_single_tone_rx.full_sweep.stop_freq}mhz.jpg

TODO