#!/usr/bin/env bash

prefix="../build"

# Search length = 1024
# Threshold = 4e22
# Sample frequency 10MHz
# Caf type = LITE
# NBins = 19
# bin stride = 3
# FFT Size = 1048576

cat caf_lite.bin |
  "${prefix}/caf_interpret" signal.bin 1024 4e22 10MHz LITE 19 3 1048576
