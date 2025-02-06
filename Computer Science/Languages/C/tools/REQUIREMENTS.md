# Requirements

## Overview

Find the time and frequency offset of a channelized signal. Use a two layered approach:

1. Low resolution CAF
2. High resolution CAF

Goals:

-   Fast
-   Very high noise floor very low signal (e.g. very low snrdb)

# Technology

-   XMidas Option tree
    -   Choosing ISO standard PIPEs to read input data and write output data
    -   UNIX environment - cpu - possibly gpu - at least multithreading supported
    -   Will be run in a docker container

## Inputs

-   **Type**: Complex Float (32 bit each - 64 total bits)
-   **Format**: No Requirements (choosing files of contiguous data)
-   **Characteristics**: White noise signal with power 1

## Outputs

-   **Time Offset**: Float
-   **Frequency Offset**: Float

### Intermediary Outputs

-   To facilitate composability, break down into two outputs:
    1. CAF outputs F32's for all correlations
    2. A interpretation layer parses F32's (stream wise to avoid memory overhead) and returns **time offset** and **frequency offset**

## Specifications

1. Input signal durations: 1, 2, 3, 5, 10 seconds.
2. Input signal bandwidth: 10 MHz.
3. Channel SNR range: -50 dB to -70 dB.
4. Breakpoint SNR:
    - Defined as the SNR such that d(peak, noise) >= 3 dB.
    - Calculated for each input signal duration.

## Deliverables

1. **Breakpoint SNR** for 1, 2, 3, 5, 10-second signals.
2. **Plots**:

-   SNR db vs Signal Duration.
-   Breakpoint SNR analysis for each duration.
-   Plot of signal and match overlayed to show how small the match is (log)

## Testing

1. Single script to:
    - Generate input signals for 1, 2, 3, 5, 10 seconds.
    - Channelize signals for SNRs ranging from -50 to 60 dB.
    - Compute CAF for all combinations of durations and SNRs.
2. Validate time and frequency offset outputs.
3. Generate plots for deliverables.
4. One unit test exe run on a gitlab ci pipeline
