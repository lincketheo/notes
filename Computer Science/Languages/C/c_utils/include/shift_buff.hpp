#pragma once

#include "core/DtypeDatasource.hpp"

/**
 * Represents a buffer that supports shifting operations
 * (e.g., removing elements from the front).
 * - `data`: Pointer to the buffer's memory.
 * - `size`: Size of each element in the buffer (in bytes).
 * - `cap`: Capacity of the buffer (in elements).
 * - `len`: Current number of elements in the buffer.
 */
typedef struct
{
  void *data;
  size_t size;
  size_t cap;
  size_t len;
} shift_buf;

/**
 * Creates a `shift_buf` using a provided memory block.
 * @param data Pointer to the pre-allocated memory block.
 * @param len Initial number of elements in the buffer.
 * @param cap Total capacity of the memory block in elements.
 * @return A `shift_buf` structure initialized with the provided memory.
 */
shift_buf shift_buf_create (void *data, size_t len, size_t cap);

/**
 * Reads data from a `DtypeDatasource` into a shift buffer up to its capacity.
 * @param dest Pointer to the destination `shift_buf` structure.
 * @param ifp Pointer to the data source (`DtypeDatasource`) to read from.
 *
 * @note The function reads as much data as possible without exceeding the buffer's capacity.
 */
void shift_buf_fread_max (shift_buf *dest, DtypeDatasource *ifp);

/**
 * Removes the first `n` elements from the front of the shift buffer.
 * The remaining elements are shifted forward, and the buffer's length is updated.
 * @param dest Pointer to the `shift_buf` structure.
 * @param n Number of elements to remove from the front.
 *
 * @note The function assumes `n <= dest->len`. Behavior is undefined otherwise.
 */
void shift_buf_del_front (shift_buf *dest, size_t n);
