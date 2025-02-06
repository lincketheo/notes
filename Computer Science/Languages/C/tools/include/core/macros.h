#pragma once

#include <string.h>

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN3(a, b, c) MIN (MIN (a, b), c)
#define STR_EQUAL(left, right) (strcmp (left, right) == 0)
#define TDIFF(start, end) \
  (((end).tv_sec - (start).tv_sec) + ((end).tv_nsec - (start).tv_nsec) / 1e9)

#define MHz(v) ((v) * 1e6)

// Regular colors
#define BLACK "\033[0;30m"
#define RED "\033[0;31m"
#define GREEN "\033[0;32m"
#define YELLOW "\033[0;33m"
#define BLUE "\033[0;34m"
#define MAGENTA "\033[0;35m"
#define CYAN "\033[0;36m"
#define WHITE "\033[0;37m"

// Bold colors
#define BOLD_BLACK "\033[1;30m"
#define BOLD_RED "\033[1;31m"
#define BOLD_GREEN "\033[1;32m"
#define BOLD_YELLOW "\033[1;33m"
#define BOLD_BLUE "\033[1;34m"
#define BOLD_MAGENTA "\033[1;35m"
#define BOLD_CYAN "\033[1;36m"
#define BOLD_WHITE "\033[1;37m"

// Reset
#define RESET "\033[0m"

#define ZEROS_8 0, 0, 0, 0, 0, 0, 0, 0
#define ZEROS_64 \
  ZEROS_8, ZEROS_8, ZEROS_8, ZEROS_8, ZEROS_8, ZEROS_8, ZEROS_8, ZEROS_8
#define ZEROS_92 ZEROS_64, ZEROS_8, ZEROS_8, 0, 0
#define ZEROS_256 ZEROS_64, ZEROS_64, ZEROS_64, ZEROS_64

#define TIMER_START()         \
  struct timespec start, end; \
  clock_gettime (CLOCK_MONOTONIC, &start)

#define GET_ELLAPSED() (clock_gettime (CLOCK_MONOTONIC, &end), TDIFF (start, end))
