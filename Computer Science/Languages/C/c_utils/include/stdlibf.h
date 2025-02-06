#pragma once

#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "core/errors.h"

/**
 * Facades for ISO C Standard library
 */

static inline FILE *
fopen_or_abort (const char *fname, const char *mode)
{
  FILE *fp = fopen (fname, mode);
  fatal_error_if (fp == NULL, "fopen %s %s\n", fname, mode);
  return fp;
}

static inline void *
malloc_or_abort (size_t len)
{
  void *ret = malloc (len);
  fatal_error_if (ret == NULL, "malloc: %zu\n", len);
  return ret;
}

static inline void *
realloc_or_abort (void *ptr, size_t newlen)
{
  void *ret = realloc (ptr, newlen);
  fatal_error_if (ptr == NULL, "realloc: %zu\n", newlen);
  return ret;
}

static inline void
fseek_or_abort (FILE *fp, long int offset, int whence)
{
  fatal_error_if (fseek (fp, offset, whence), "fseek\n");
}

static inline size_t
ftell_or_abort (FILE *fp)
{
  long ret = ftell (fp);
  fatal_error_if (ret < 0, "ftell\n");
  return (size_t)ret;
}

static inline void
fclose_or_abort (FILE *fp)
{
  fatal_error_if (fclose (fp), "fclose\n");
}

static inline char *
fgets_or_abort (char *s, int n, FILE *fp)
{
  char *ret = fgets (s, n, fp);
  fatal_error_if (ferror (fp), "fgets\n");
  return ret;
}

static inline void
remove_or_abort (const char *fname)
{
  fatal_error_if (remove (fname), "remove\n");
}

static inline void
fwrite_or_abort (const void *ptr, size_t size, size_t n,
                 FILE *s)
{
#ifndef NDEBUG
  errno = 0;
  size_t written = fwrite (ptr, size, n, s);
  fatal_error_if (written != n, "fwrite failed\n");
  fatal_error_if (fflush (s), "fflush\n");
#else
  errno = 0;
  size_t written = fwrite (ptr, size, n, s);
  fatal_error_if (written != n, "fwrite failed\n");
#endif
}

#define MAX_FREAD 10

static inline FILE *
tmpfile_or_abort ()
{
  errno = 0;
  FILE *temp_file = tmpfile ();
  fatal_error_if (temp_file == NULL, "tmpfile\n");
  return temp_file;
}

static inline size_t
fread_or_abort (void *ptr, size_t size, size_t n,
                FILE *fp)
{
  errno = 0;
  size_t read = fread (ptr, size, n, fp);
  if (read != n)
    {
      fatal_error_if (ferror (fp), "fread failed\n");
    }
  return read;
}

static inline void
fread_expect_all_or_abort (void *ptr, size_t size, size_t n,
                           FILE *fp)
{
  errno = 0;
  size_t read = fread (ptr, size, n, fp);
  fatal_error_if (read != n, "fread (expecting all)\n");
}

static inline void
safe_free (void *ptr)
{
  ASSERT (ptr);
  free (ptr);
}

static inline FILE *
fopen_log (const char *fname, const char *mode)
{
  FILE *fp = fopen (fname, mode);
  if (fp == NULL)
    {
      fprintf (stderr, "Failed to open file: %s\n", fname);
      perror ("fopen\n");
    }
  return fp;
}
