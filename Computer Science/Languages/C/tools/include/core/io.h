#pragma once

#include "core/data_structures.h"

#include <stdio.h>

///////////////////////////////////////////////////////
///////// File utils

bytes fread_all_mmap_seek (FILE *fp, size_t size, int prot, int flags);

int is_seekable (FILE *fp);

size_t fcopy (FILE *ifp, FILE *ofp);

size_t fread_zero_top (void *ptr, size_t size, size_t n, FILE *fp);

size_t fread_shift_half (void *ptr, size_t size, size_t n, FILE *fp);

size_t fread_zero_top_shift_half (void *ptr, size_t size, size_t n, FILE *fp);

bytes mmap_malloc_create_wrb (const char *fname, size_t bytes);

void *mmap_malloc_create_with_base_fname_wrb (const char *base, const char *fname, size_t size, size_t len);

int is_valid_folder (const char *fname);
