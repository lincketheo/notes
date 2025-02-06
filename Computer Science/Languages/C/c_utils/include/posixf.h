#pragma once

#include "core/errors.h"

#include <errno.h>
#include <fcntl.h>
#include <stdarg.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

/**
 * Facades for POSIX calls
 */

static inline void
mkfifo_or_abort (const char *path, __mode_t mode)
{
  errno = 0;
  fatal_error_if (mkfifo (path, mode), "mkfifo");
}

static inline int
open_or_abort (const char *path, int flags, ...)
{
  errno = 0;
  int fd;

  if (flags & O_CREAT)
    {
      va_list args;
      va_start (args, flags);
      mode_t mode = va_arg (args, mode_t);
      fd = open (path, flags, mode);
      va_end (args);
    }
  else
    {
      fd = open (path, flags);
    }

  fatal_error_if (fd == -1, "open");
  return fd;
}

static inline FILE *
fdopen_or_abort (int fd, const char *modes)
{
  errno = 0;
  FILE *fp = fdopen (fd, modes);
  fatal_error_if (fp == NULL, "fdopen");
  return fp;
}

static inline int
fileno_or_abort (FILE *fp)
{
  errno = 0;
  int fd = fileno (fp);
  fatal_error_if (fd == -1, "fileno");
  return fd;
}

static inline void *
mmap_or_abort (void *addr, size_t len, int prot, int flags,
               int fd, off_t offset)
{
  errno = 0;
  void *ret = mmap (addr, len, prot, flags, fd, offset);
  fatal_error_if (ret == MAP_FAILED, "mmap\n");
  return ret;
}

static inline void
close_or_abort (int fd)
{
  errno = 0;
  fatal_error_if (close (fd), "close\n");
}

static inline void
munmap_or_abort (void *data, size_t len)
{
  errno = 0;
  fatal_error_if (munmap (data, len), "munmap\n");
}

static inline void
ftruncate_or_abort (int fd, off_t length)
{
  errno = 0;
  fatal_error_if (ftruncate (fd, length));
}

static void
mkdir_or_abort (const char *fname, mode_t mode)
{
  errno = 0;
  fatal_error_if (mkdir (fname, mode), "mkdir\n");
}

static void
rmdir_or_abort (const char *fname)
{
  errno = 0;
  fatal_error_if (rmdir (fname), "rmdir\n");
}

static size_t
read_or_abort (int fd, void *buf, size_t bytes)
{
  ssize_t ret = read (fd, buf, bytes);
  errno = 0;
  fatal_error_if (ret < 0, "read");
  return ret;
}

FILE *fifoopen (const char *path, const char *mode);
