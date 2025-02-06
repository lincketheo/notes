#!/usr/bin/env bash

prefix="../build"

# Search length = 1024
# Threshold = 4e22
# Sample frequency 10MHz
# Caf type = NORMAL

cat caf_normal.bin |
  "${prefix}/caf_interpret" signal.bin 1024 4e22 10MHz NORMAL -600:100:700
