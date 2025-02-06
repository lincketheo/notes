#ifndef UTILS_H
#define UTILS_H
#include <stdlib.h>
#include "str.h"

/**
 * Null unwrap with lambda expr.
 * Example:
 * int* a = unwrap(malloc(10 * sizeof*a), ({ return 1; }));
 *
 * is shorthand for:
 *
 * int *a = malloc(10 * sizeof*a);
 * if(a == NULL)
 *  return 1;
 */
#define unwrap(expr, fail_expr) \
  ({                            \
    if ((expr) == NULL)         \
      (fail_expr);              \
    expr;                       \
  })

#define printf_err(fmt, ...) \
  fprintf(stderr, BOLD_RED "%s: " RESET fmt, __func__, ##__VA_ARGS__)

#define malloc_err(arg, name, expr) ({                      \
  unwrap(malloc(arg), ({                                    \
    printf_err("Failed to allocate memory for %s\n", name); \
    expr;                                                   \
  }));                                                      \
})

#define calloc_err(arg, len, name, expr) ({                  \
  unwrap(calloc(arg, len), ({                                \
    printf_err("Failed to callocate memory for %s\n", name); \
    expr;                                                    \
  }));                                                       \
})

// I find myself "creating" things a lot. This is just shorthand
#define create_err(expr, name, fail_expr) ({   \
  unwrap((expr), ({                            \
    printf_err("Failed to create %s\n", name); \
    fail_expr;                                 \
  }));                                         \
})

#define re_im(c) creal(c), cimag(c)

/**
 * Returns the closest power of two less than [val]
 */
size_t floored_power_of_two(size_t val);

/**
 * Nice readable print only if verbose. So you don't have to do if(verbose) printf....
 */
void printf_if_verbose(int verbose, const char* format, ...);

/**
 * int Print Failed Verb to reduce boilerplate code
 * @brief Quick wrapper around error messaging for int return type
 */
int int_pfv(int p, const char* verb_phrase);

#endif
