#!/usr/bin/env bash

prefix="../build"

# 19 bins on either side for a total of 1 + 2 * 19 bins
# 3 bin stride
# Expect bin number 17

cat signal.bin |
  "${prefix}/fftcaf_lite" match.bin 19 3 |
  "${prefix}/fout" caf_lite.bin |
  "${prefix}/dtob" SF 10MHz >caf_lite.tmp
