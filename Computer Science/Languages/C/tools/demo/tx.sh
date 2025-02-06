#!/usr/bin/env bash

prefix="../build"

# 10MHz
# 0.1 Seconds
# Power = 1
# Linear ramp up end = 0.04 seconds
# Linear ramp down start = 0.06 seconds

"${prefix}/tx" 10MHz 0.1 1 0.04 0.06 |
  "${prefix}/fout" match.bin |
  "${prefix}/dtob" CF 10MHz >match.tmp
