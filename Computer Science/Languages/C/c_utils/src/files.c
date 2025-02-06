#include <assert.h>
#include <complex.h>
#include <unistd.h>

#include "utils/files.h"

int is_file_size_multiple_of(const char* filename, size_t size)
{
  size_t len = get_file_size_bytes(filename);
  return len % size == 0;
}

ssize_t get_num_elements(const char* file, size_t elem_size)
{
  ssize_t len = get_file_size_bytes(file);
  if (len == -1)
    return -1;

  if (len % elem_size != 0) {
    fprintf(stderr, "File size of: %s is not a multiple of %zu", file, elem_size);
    return -1;
  }

  return len / elem_size;
}

ssize_t get_file_size_bytes(const char* file)
{
  assert(file);

  FILE* fp = fopen_err(file, "rb", ({ return -1; }));

  ssize_t res;

  if (fseek(fp, 0L, SEEK_END) != 0) {
    fprintf(stderr, "Failed to seek file: %s\n", file);
    res = -1;
  } else if ((res = ftell(fp)) == -1L) {
    fprintf(stderr, "Failed to tell file: %s\n", file);
    res = -1;
  }

  if (fclose(fp)) {
    fprintf(stderr, "Failed to close file: %s\n", file);
    res = -1;
  }

  return res;
}

ssize_t fp_fcopy(FILE* fp_dest, FILE* fp_src)
{
  assert(fp_dest);
  assert(fp_src);

  unsigned char buffer[256];
  size_t bytes_read;
  while ((bytes_read = fread(buffer, 1, 256, fp_src)) > 0)
    fwrite(buffer, 1, bytes_read, fp_dest);

  return bytes_read;
}

ssize_t copy_file(const char* src, const char* dest)
{
  assert(src);
  assert(dest);

  FILE* srcf = fopen_err(src, "r", ({ return -1; }));
  FILE* destf = fopen_err(dest, "wb", ({ fclose(srcf); return -1; }));

  ssize_t ret = fp_fcopy(destf, srcf);

  fclose(srcf);
  fclose(destf);

  return ret;
}

ssize_t truncate_into(const char* input, const char* output, size_t target_size)
{
  assert(input);
  assert(output);

  ssize_t bytes = copy_file(input, output);
  if (bytes < 0) {
    fprintf(stderr, "Failed to copy file %s to %s\n", input, output);
    return -1;
  }

  if (truncate(output, target_size)) {
    fprintf(stderr, "Failed to resize file %s to %zu\n", input, target_size);
    return -1;
  }

  return get_file_size_bytes(input);
}

int write_data_to_file(
    const char* fname,
    const char* mode,
    const void* data,
    size_t bytes_len)
{
  assert(fname);
  assert(data);

  FILE* fp = fopen_err(fname, mode, ({ return -1; }));

  int ret = 0;
  if (fwrite(data, 1, bytes_len, fp) != bytes_len) {
    fprintf(stderr, "Failed to write data to file: %s\n", fname);
    ret = -1;
  };

  if (fclose(fp) != 0) {
    fprintf(stderr, "Failed to close file: %s\n", fname);
    ret = -1;
  };

  return ret;
}

int read_data_from_file(const char* fname, void* dest, size_t bytes_len)
{
  assert(dest);
  assert(fname);

  if (get_file_size_bytes(fname) != bytes_len) {
    fprintf(stderr, "Reading file: %s but size is "
                    "not expected length: %zu bytes\n",
        fname, bytes_len);
    return -1;
  }

  FILE* fp = fopen_err(fname, "r", ({ return -1; }));

  int ret = 0;
  if (fread(dest, 1, bytes_len, fp) != bytes_len) {
    fprintf(stderr, "Didn't read expected "
                    "%zu bytes from file: %s",
        bytes_len, fname);
    ret = -1;
  }

  if (fclose(fp) != 0) {
    fprintf(stderr, "Failed to close file: %s\n", fname);
    ret = -1;
  }

  return ret;
}

int empty_file(const char* fname)
{
  FILE* file = fopen_err(fname, "w", ({ return -1; }));
  return fclose(file);
}
