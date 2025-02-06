#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils/str.h"

char* str_plus(const char* front, const char* back)
{
  assert(front);
  assert(back);

  char* actual = malloc(strlen(front) + strlen(back) + 1);
  if (!actual) {
    fprintf(stderr, "Failed to allocate memory for str_plus(%s, %s)\n", front, back);
    return NULL;
  }

  strcpy(actual, front);
  strcat(actual, back);

  return actual;
}
