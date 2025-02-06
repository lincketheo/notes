# CAF Processor

## Build Instructions

```
$ mkdir build
$ cd build
$ cmake .. -DCMAKE_BUILD_TYPE=Release   # Build in release mode
$ cmake .. -DCMAKE_BUILD_TYPE=Debug     # Build in debug mode
$ make
$ ./test
$ make clean
```

## Architecture

-   This project is broken up into `core`, `emprobe` and `apps`
    -   `core`
        -   This is my (Theo) standard library I've build over time with
            only the selected modules that I felt would be helpful in this project.
            I haven't wrapped up my standard library in a clean project yet,
            so it's just the source code in this repository.
    -   `emprobe`
        -   The business logic for emprobe (this software).
            This is where most changes should happen
        -   Take note of `emprobe/usecases`. This is where all the business actions
            happen
    -   `apps`
        -   A list of command line executable apps for emprobe

## Usage

See `demo` folder for all of these examples. Just run `all.sh` to run all of them at once

-   Generate a match signal and a simulated signal with 0 padding
    -   Sample Frequency: 10MHz
    -   Time: 1 seconds
    -   Steady State Power: 1
    -   Ramp up final time: 1 second
    -   Ramp down start time: 9 seconds
-   Fan out to save file as a binary file `match.bin`
-   Pipe it to convert it to an xmidas file (not detached - there's no support)

```
$ tx 10MHz 1 1 1 9 | fout match.bin | dtob CF 10MHz > match.tmp
```

-   Run the file through a mini channel
    1. Adds 10000000 samples of zero padding on the left
    2. Adds 10000000 samples of zero padding on the right
    3. Adds an additive gaussian white noise with power = 10 (`-p 10`)
    4. Tunes the signal by -500.123 Hz
-   Fan out to save file as a binary file `match.bin`
-   Pipe it to convert it to an xmidas file (not detached - there's no support)

```
$ cat match.bin | ./build/channel 10MHz -500.123 10 10000000 10000000 | ./build/fout signal.bin | ./build/dtob CF 10MHz > signal.tmp
```

-   Compute the sample and frequency offset of the signal
-   At -500.123 Hz tuned, we'd expect a firing at bin number
    -   (-500.123 \_ `next_2_power(10000000)` / (10e6))
    -   = -500.123 \_ 16,777,216 / 10e6 = -839
-   Set the bin span to 100 and number of bins to 10 (21 total rows)
-   In the caf laid out horizontally - that's a peak at
    -   10000000 \* 21 + 2 (-1000th bin + 2 = -800)
    -   = caf sample 210000002

```
$ cat signal.bin | ./build/fftcaf_lite match.bin 10 100 | ./build/fout caf.bin | ./build/dtob SF 10MHz > caf.tmp
```

-   Run Caf Interpret to get [sample number / caf frequency bin] of the output
