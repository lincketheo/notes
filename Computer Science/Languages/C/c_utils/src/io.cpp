#include "core/io.h"

#include "core/data_structures.h"
#include "core/errors.h"
#include "core/memory.h"
#include "core/posixf.h"
#include "core/stdlibf.h"
#include "core/strings.h"
#include "core/testing.h"

int
is_seekable (FILE *fp)
{
  if (fp == NULL)
    {
      return 0;
    }
  if (fseek (fp, 0, SEEK_CUR) == 0)
    {
      return 1;
    }
  else
    {
      return 0;
    }
}

TEST (is_seekable)
{
  FILE *fp = fopen_or_abort ("tmpfile", "wb");
  TEST_EQUAL (is_seekable (fp), 1, "%d");

  fclose_or_abort (fp);
  remove_or_abort ("tmpfile");

  TEST_EQUAL (is_seekable (stdin), 0, "%d");
}

size_t
fread_zero_top (void *ptr, size_t size, size_t n, FILE *fp)
{
  size_t ret = fread_or_abort (ptr, size, n, fp);
  ASSERT (ret <= n);
  if (ret < n)
    {
      memset_top_zero (ptr, size, ret, n);
    }
  return ret;
}

TEST (fread_zero_top)
{
  FILE *fp = fopen_or_abort ("testfile", "wb+");

  uint8_t data[] = { 1, 2, 3, 4, 5 };
  fwrite_or_abort (data, 1, sizeof (data), fp);
  fclose_or_abort (fp);

  fp = fopen_or_abort ("testfile", "rb");

  uint8_t buffer[10] = { 0 };
  size_t elems_read = fread_zero_top (buffer, 1, 10, fp);

  TEST_EQUAL (elems_read, 5lu, "%zu");
  for (size_t i = 5; i < 10; i++)
    {
      TEST_EQUAL (buffer[i], 0, "%d");
    }

  fclose_or_abort (fp);
  remove_or_abort ("testfile");
}

size_t
fread_shift_half (void *buff, size_t size, size_t n, FILE *fp)
{
  ASSERT (buff);
  ASSERT (size > 0);
  ASSERT (n > 0);
  ASSERT (fp);

  ASSERT (n % 2 == 0);

  memmove_top_bottom (buff, size, n / 2, n);
  return fread_or_abort (((uint8_t *)buff) + size * n / 2, size, n / 2, fp);
}

TEST (fread_shift_half)
{
  FILE *fp = fopen_or_abort ("testfile", "wb+");

  uint8_t data[] = { 1, 2, 3, 4, 5, 6 };
  fwrite_or_abort (data, 1, sizeof (data), fp);
  fclose_or_abort (fp);

  fp = fopen_or_abort ("testfile", "rb");

  uint8_t buffer[6] = { 0 };
  size_t elems_read = fread_shift_half (buffer, 1, 6, fp);

  TEST_EQUAL (elems_read, 3lu, "%zu");

  TEST_EQUAL (buffer[3], 1, "%d");
  TEST_EQUAL (buffer[4], 2, "%d");
  TEST_EQUAL (buffer[5], 3, "%d");

  elems_read = fread_shift_half (buffer, 1, 6, fp);

  TEST_EQUAL (elems_read, 3lu, "%zu");

  TEST_EQUAL (buffer[0], 1, "%d");
  TEST_EQUAL (buffer[1], 2, "%d");
  TEST_EQUAL (buffer[2], 3, "%d");

  TEST_EQUAL (buffer[3], 4, "%d");
  TEST_EQUAL (buffer[4], 5, "%d");
  TEST_EQUAL (buffer[5], 6, "%d");

  elems_read = fread_shift_half (buffer, 1, 6, fp);

  TEST_EQUAL (elems_read, 0lu, "%zu");

  TEST_EQUAL (buffer[0], 4, "%d");
  TEST_EQUAL (buffer[1], 5, "%d");
  TEST_EQUAL (buffer[2], 6, "%d");

  // Top remains unchanged
  TEST_EQUAL (buffer[3], 4, "%d");
  TEST_EQUAL (buffer[4], 5, "%d");
  TEST_EQUAL (buffer[5], 6, "%d");

  fclose_or_abort (fp);
  remove_or_abort ("testfile");
}

size_t
fread_zero_top_shift_half (void *buff, size_t size, size_t n, FILE *fp)
{
  ASSERT (buff);
  ASSERT (size > 0);
  ASSERT (n > 0);
  ASSERT (fp);

  ASSERT (n % 2 == 0);

  memmove_top_bottom (buff, size, n / 2, n);
  return fread_zero_top (((uint8_t *)buff) + size * n / 2, size, n / 2, fp);
}
TEST (fread_zero_top_shift_half)
{
  FILE *fp = fopen_or_abort ("testfile", "wb+");

  uint8_t data[] = { 1, 2, 3, 4, 5, 6 };
  fwrite_or_abort (data, 1, sizeof (data), fp);
  fclose_or_abort (fp);

  fp = fopen_or_abort ("testfile", "rb");

  uint8_t buffer[6] = { 0 };
  size_t elems_read = fread_zero_top_shift_half (buffer, 1, 6, fp);

  TEST_EQUAL (elems_read, 3lu, "%zu");

  TEST_EQUAL (buffer[3], 1, "%d");
  TEST_EQUAL (buffer[4], 2, "%d");
  TEST_EQUAL (buffer[5], 3, "%d");

  elems_read = fread_zero_top_shift_half (buffer, 1, 6, fp);

  TEST_EQUAL (elems_read, 3lu, "%zu");

  TEST_EQUAL (buffer[0], 1, "%d");
  TEST_EQUAL (buffer[1], 2, "%d");
  TEST_EQUAL (buffer[2], 3, "%d");

  TEST_EQUAL (buffer[3], 4, "%d");
  TEST_EQUAL (buffer[4], 5, "%d");
  TEST_EQUAL (buffer[5], 6, "%d");

  elems_read = fread_zero_top_shift_half (buffer, 1, 6, fp);

  TEST_EQUAL (elems_read, 0lu, "%zu");

  TEST_EQUAL (buffer[0], 4, "%d");
  TEST_EQUAL (buffer[1], 5, "%d");
  TEST_EQUAL (buffer[2], 6, "%d");

  TEST_EQUAL (buffer[3], 0, "%d");
  TEST_EQUAL (buffer[4], 0, "%d");
  TEST_EQUAL (buffer[5], 0, "%d");

  fclose_or_abort (fp);
  remove_or_abort ("testfile");
}

bytes
mmap_malloc_create_wrb (const char *fname, size_t blen)
{
  int fd = open_or_abort (fname, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
  ftruncate_or_abort (fd, blen);
  int pflags = PROT_READ | PROT_WRITE;
  void *ret = mmap_or_abort (NULL, blen, pflags, MAP_SHARED, fd, 0);
  close_or_abort (fd);
  return (bytes){
    .data = (uint8_t *)ret,
    .len = blen,
  };
}

TEST (mmap_malloc_create_wrb)
{
  const char *fname = "testfile";
  size_t size = 1024;

  // Test valid mmap creation
  bytes b = mmap_malloc_create_wrb (fname, size);
  TEST_EQUAL (b.len, size, "%zu");

  struct stat st;
  stat (fname, &st);
  TEST_EQUAL ((size_t)st.st_size, size, "%zu");

  // Write and verify data in the mmap region
  memset (b.data, 0xAB, size);
  munmap (b.data, size);

  int fd = open_or_abort (fname, O_RDONLY);
  uint8_t buffer[1024];
  read_or_abort (fd, buffer, size);
  for (size_t i = 0; i < size; i++)
    {
      TEST_EQUAL (buffer[i], 0xAB, "%x");
    }
  close_or_abort (fd);
  remove_or_abort (fname);
}

void *
mmap_malloc_create_with_base_fname_wrb (const char *base,
                                        const char *fname, size_t size,
                                        size_t len)
{
  char dest[2048];

  fatal_error_if (construct_path (base, fname, dest, sizeof (dest)));
  bytes b = mmap_malloc_create_wrb (dest, len * size);
  ASSERT (b.len == len * size);

  return b.data;
}

int
is_valid_folder (const char *fname)
{
  ASSERT (fname);

  struct stat sb;
  if (stat (fname, &sb) == 0 && S_ISDIR (sb.st_mode))
    {
      return 1;
    }
  return 0;
}

TEST (is_valid_folder)
{
  mkdir_or_abort ("testdir", 0755);
  TEST_EQUAL (is_valid_folder ("testdir"), 1, "%d");
  rmdir_or_abort ("testdir");

  TEST_EQUAL (is_valid_folder ("nonexistent_dir"), 0, "%d");

  FILE *fp = fopen_or_abort ("testfile", "w");
  uint8_t data[] = { 1, 2, 3 };
  fwrite_or_abort (data, 1, sizeof (data), fp);
  fclose_or_abort (fp);
  TEST_EQUAL (is_valid_folder ("testfile"), 0, "%d");
  remove_or_abort ("testfile");
}
