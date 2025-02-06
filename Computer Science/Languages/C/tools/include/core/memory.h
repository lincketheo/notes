#pragma once

#include <inttypes.h>
#include <stdlib.h>

int system_is_little_endian ();

typedef struct
{
  uint8_t *data;
  size_t cap;
  size_t len;
} linalloc;

linalloc linalloc_create_default (size_t cap);

linalloc linalloc_create_from (uint8_t *data, size_t cap);

void *linalloc_malloc (linalloc *s, size_t bytes);

void linalloc_free_default (linalloc *s);

void memmove_top_bottom (void *data, size_t size, size_t from, size_t len);

void memset_top_zero (void *data, size_t size, size_t from, size_t len);

size_t memcpy_stride (void *dest, const void *src, size_t size, size_t n,
                      size_t step);
