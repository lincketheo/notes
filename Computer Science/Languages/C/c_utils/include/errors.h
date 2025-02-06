#pragma once

#include <errno.h>
#include <stdio.h>

/**
 * @brief Print an error message then exit
 */
void fatal_error (const char *format, ...) __attribute__ ((noreturn));

#define fatal_error_if(expr, ...)                                         \
  do                                                                      \
    {                                                                     \
      if (__builtin_expect (!!(expr), 0))                                 \
        {                                                                 \
          if (errno)                                                      \
            perror (#expr);                                               \
          fatal_error ("Expression: %s failed. Aborting early\n", #expr); \
        }                                                                 \
    }                                                                     \
  while (0)

#define unreachable() \
  fatal_error ("BUG! Unreachable! (%s):%s:%d\n", __FILE__, __func__, __LINE__);

#ifndef NDEBUG

#define ASSERT(expr)                                        \
  do                                                        \
    {                                                       \
      if (!(expr))                                          \
        {                                                   \
          fatal_error ("BUG! ASSERTION: " #expr " failed. " \
                       "Location: (%s):%s:%d\n",            \
                       __FILE__, __func__, __LINE__);       \
        }                                                   \
    }                                                       \
  while (0)

#define TODO(msg)                                 \
  do                                              \
    {                                             \
      fatal_error ("TODO! " msg " "               \
                   "Location: (%s):%s:%d\n",      \
                   __FILE__, __func__, __LINE__); \
    }                                             \
  while (0)

#else

#define ASSERT(expr)

// TODO left undefined for compile time error
#endif
