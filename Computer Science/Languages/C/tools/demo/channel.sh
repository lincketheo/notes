#!/usr/bin/env bash

prefix="../build"

# 10MHz
# -500.7 Tune frequency
# 10 Noise power
# 1000... Left Padding
# 1000... Right Padding

cat match.bin |
  "${prefix}/channel" 10MHz -500.7 10 1000000 1000000 |
  "${prefix}/fout" signal.bin |
  "${prefix}/dtob" CF 10MHz >signal.tmp
