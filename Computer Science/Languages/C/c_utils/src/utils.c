#include "utils/common.h"
#include <math.h>
#include <stdarg.h>
#include <stdio.h>

size_t floored_power_of_two(size_t val)
{
  float power = floorf(log2f((float)val));
  return (size_t)powf(2, power);
}

void printf_if_verbose(int verbose, const char* format, ...)
{
  if (verbose) {
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
  }
}

/**
 * int Print Failed Verb to reduce boilerplate code
 * @brief Quick wrapper around error messaging for int return type
 */
int int_pfv(int p, const char* verb_phrase)
{
  if (p)
    fprintf(stderr, "Failed to %s\n", verb_phrase);
  return p;
}

