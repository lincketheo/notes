#pragma once

#include <inttypes.h>
#include <stdlib.h>

/**
 * Checks if the system architecture is little-endian.
 * @return 1 if the system is little-endian, 0 otherwise.
 */
int system_is_little_endian ();

/**
 * Represents a linear allocator structure for managing memory allocations.
 * - `data`: Pointer to the allocated memory block.
 * - `cap`: Capacity of the memory block in bytes.
 * - `len`: Current usage of the memory block in bytes.
 */
typedef struct
{
  uint8_t *data; // Pointer to the allocated memory block.
  size_t cap;    // Total capacity of the memory block.
  size_t len;    // Current used size of the memory block.
} linalloc;

/**
 * Creates a linear allocator with a specified initial capacity.
 * @param cap Capacity of the allocator in bytes.
 * @return A `linalloc` structure with allocated memory.
 */
linalloc linalloc_create_default (size_t cap);

/**
 * Creates a linear allocator using pre-allocated memory.
 * @param data Pointer to the pre-allocated memory.
 * @param cap Capacity of the memory in bytes.
 * @return A `linalloc` structure using the provided memory.
 */
linalloc linalloc_create_from (uint8_t *data, size_t cap);

/**
 * Allocates a specified number of bytes from the linear allocator.
 * @param s Pointer to the `linalloc` structure.
 * @param bytes Number of bytes to allocate.
 * @return Pointer to the allocated memory, or `NULL` if insufficient space.
 */
void *linalloc_malloc (linalloc *s, size_t bytes);

/**
 * Frees memory managed by the linear allocator if it was created
 * using `linalloc_create_default`.
 * @param s Pointer to the `linalloc` structure to free.
 */
void linalloc_free_default (linalloc *s);

/**
 * Moves a block of memory within a region, from the top to the bottom.
 * @param data Pointer to the memory region.
 * @param size Size of each element in bytes.
 * @param from Starting index of the block to move.
 * @param len Length of the block to move in elements.
 */
void memmove_top_bottom (
    void *data,
    size_t size,
    size_t from,
    size_t len);

/**
 * Sets a block of memory at the top of a region to zero.
 * @param data Pointer to the memory region.
 * @param size Size of each element in bytes.
 * @param from Starting index of the block to set.
 * @param len Length of the block to set in elements.
 */
void memset_top_zero (
    void *data,
    size_t size,
    size_t from,
    size_t len);

/**
 * Copies elements from a source array to a destination array with a specified stride.
 * @param dest Pointer to the destination array.
 * @param src Pointer to the source array.
 * @param size Size of each element in bytes.
 * @param n Number of elements to copy.
 * @param step Stride (distance between elements in the source array) in elements.
 * @return Number of bytes copied.
 */
size_t memcpy_stride (
    void *dest,
    const void *src,
    size_t size,
    size_t n,
    size_t step);
