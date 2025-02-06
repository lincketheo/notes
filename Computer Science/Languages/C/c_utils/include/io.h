#pragma once

#include "core/data_structures.h"

#include <stdio.h>

///////////////////////////////////////////////////////
///////// File utils

/**
 * Checks if a file is seekable.
 * @param fp File pointer.
 * @return 1 if the file is seekable, 0 otherwise.
 */
int is_seekable (FILE *fp);

/**
 * Copies data from one file to another.
 * @param ifp Input file pointer.
 * @param ofp Output file pointer.
 * @return Number of bytes copied.
 */
size_t fcopy (FILE *ifp, FILE *ofp);

/**
 * Reads `n` elements from a file and fills the top portion of the
 * allocated memory with zeros if the file data is smaller.
 * @param ptr Destination buffer.
 * @param size Size of each element.
 * @param n Number of elements to read.
 * @param fp File pointer.
 * @return Number of elements successfully read.
 */
size_t fread_zero_top (void *ptr, size_t size, size_t n, FILE *fp);

/**
 * Reads `n` elements from a file and shifts the buffer by half its size.
 * @param ptr Destination buffer.
 * @param size Size of each element.
 * @param n Length of ptr buffer
 * @param fp File pointer.
 * @return Number of elements successfully read.
 */
size_t fread_shift_half (void *ptr, size_t size, size_t n, FILE *fp);

/**
 * Reads `n` elements from a file, fills the top portion of the allocated memory with zeros, and shifts the buffer by half its size.
 * @param ptr Destination buffer.
 * @param size Size of each element.
 * @param n Number of elements to read.
 * @param fp File pointer.
 * @return Number of elements successfully read.
 */
size_t fread_zero_top_shift_half (void *ptr, size_t size, size_t n, FILE *fp);

/**
 * Creates a memory-mapped file for writing with a specified size and
 * returns a byte buffer.
 * @param fname File name.
 * @param bytes Number of bytes to allocate.
 * @return A `bytes` object representing the memory-mapped region.
 */
bytes mmap_malloc_create_wrb (const char *fname, size_t bytes);

/**
 * Creates a memory-mapped file with a base name and additional file name,
 * allowing for writing with a specified size and length.
 * @param base Base directory or file prefix.
 * @param fname File name.
 * @param size Size of each element.
 * @param len Total number of elements.
 * @return Pointer to the memory-mapped region.
 */
void *mmap_malloc_create_with_base_fname_wrb (
    const char *base,
    const char *fname,
    size_t size,
    size_t len);

/**
 * Checks if a given folder path is valid and accessible.
 * @param fname Folder path.
 * @return 1 if the folder is valid, 0 otherwise.
 */
int is_valid_folder (const char *fname);
