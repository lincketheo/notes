#ifndef FILE_UTILS_H
#define FILE_UTILS_H

#include <stdio.h>
#include <stdlib.h>

#include "common.h"

/**
 * Same as unwrap, but adds a print statement to the expr.
 */
#define fopen_err(file, mode, expr) ({                      \
  unwrap(fopen(file, mode), ({                              \
    fprintf(stderr, "Failed to open filename: %s\n", file); \
    expr;                                                   \
  }));                                                      \
})

/**
 * @brief Checks if file has elements of size [size]. For example,
 * to see if file could be interpreted as a series of 4 byte ints.
 */
#define does_file_consist_of(fn, type) is_file_size_multiple_of((fn), sizeof(type))
int is_file_size_multiple_of(const char* filename, size_t size);

#define get_file_size_of(fn, type) get_num_elements((fn), sizeof(type))
ssize_t get_num_elements(const char* file, size_t elem_size);

ssize_t fp_fcopy(FILE* fp_dest, FILE* fp_src);

ssize_t copy_file(const char* src, const char* dest);

ssize_t truncate_into(const char* input, const char* output, size_t target_size);

ssize_t get_file_size_bytes(const char* filename);

int write_data_to_file(
    const char* fname,
    const char* mode,
    const void* data,
    size_t bytes_len);

#define write_and_free_data_to_file(fname, mode, data, bytes_len) ({ \
  int ret = write_data_to_file(fname, mode, data, bytes_len);        \
  free(data);                                                        \
  ret;                                                               \
})

int read_data_from_file(const char* fname, void* dest, size_t bytes_len);

int empty_file(const char* fname);

#define empty_file_if_not_null(fname) \
  ({                                  \
    int ret = 0;                      \
    if (fname) {                      \
      ret = empty_file(fname);        \
    }                                 \
    ret;                              \
  })

#endif
