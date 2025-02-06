#pragma once

#include "core/errors.h"
#include "core/logging.h"
#include "core/macros.h"

#include <string.h>

#ifndef NTEST
// Macro to define a test case
// Usage:
// TEST(foo) {
//    TEST_EQUAL(1, 1, "%d");
// }
#define TEST(name)                                             \
  static void test_##name ();                                  \
  static void wrapper_test_##name ()                           \
  {                                                            \
    ns_log (INFO, "Running Test %s\n", #name);                 \
    test_##name ();                                            \
    ns_log (INFO, BOLD_GREEN "PASSED\n" RESET);                \
  }                                                            \
  __attribute__ ((constructor)) static void register_##name () \
  {                                                            \
    if (test_count >= MAX_TESTS_LEN)                           \
      {                                                        \
        fatal_error ("Increase MAX_TESTS_LEN");                \
      }                                                        \
    tests[test_count++] = wrapper_test_##name;                 \
  }                                                            \
  static void test_##name ()

#else

#define TEST(name) static void test_##name ()

#endif

#define MAX_TESTS_LEN 2048
typedef void (*test_func) ();
extern test_func tests[MAX_TESTS_LEN];
extern size_t test_count;

#define TEST_REPORT() \
  fprintf (stderr, "%s(%s:%d)\n", __FILE__, __func__, __LINE__);

#define TEST_ASSERT_STR_EQUAL(str1, str2)                                       \
  if (strcmp (str1, str2) != 0)                                                 \
    {                                                                           \
      fprintf (stderr, BOLD_RED "FAILED: " RESET RED "\"%s\" != \"%s\" " RESET, \
               str1, str2);                                                     \
      TEST_REPORT ();                                                           \
      ASSERT (0);                                                               \
    }

#define TEST_EQUAL(left, right, fmt)                                       \
  if (left != right)                                                       \
    {                                                                      \
      fprintf (stderr,                                                     \
               BOLD_RED "FAILED: " RESET RED "%s != %s -- " fmt " != " fmt \
                        "\n" RESET,                                        \
               #left, #right, left, right);                                \
      TEST_REPORT ();                                                      \
      ASSERT (0);                                                          \
    }

#define TEST_FLOAT_EQUAL(left, right, tol)                                        \
  if (fabs (left - right) > tol)                                                  \
    {                                                                             \
      fprintf (stderr,                                                            \
               BOLD_RED "FAILED: " RESET RED "%s != %s, %f != %f\n" RESET, #left, \
               #right, left, right);                                              \
      TEST_REPORT ();                                                             \
      ASSERT (0);                                                                 \
    }
