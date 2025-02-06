#!/usr/bin/env bash

prefix="../build"

# match.bin match file
# -600:100:700 Frequency search range (600 exclusive) 
# 10MHz fs
# Expect bin number 17

cat signal.bin |
  "${prefix}/fftcaf" match.bin -600:100:700 10MHz |
  "${prefix}/fout" caf_normal.bin |
  "${prefix}/dtob" SF 10MHz >caf_normal.tmp
