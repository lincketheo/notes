#include "core/DtypeDatasource.hpp"

#include "core/data_structures.h"
#include "core/stdlibf.h"
#include "core/testing.h"

size_t
FILEDtypeDatasource::read (void *dest, size_t n)
{
  return fread_or_abort (dest, elem_size, n, fp);
}

void
FILEDtypeDatasource::write (const void *src, size_t n)
{
  return fwrite_or_abort (src, elem_size, n, fp);
}

bool
FILEDtypeDatasource::done ()
{
  return feof (fp);
}

size_t
dtypeds_read_discard (DtypeDatasource *dts, size_t nelem)
{
  ASSERT (dts->elem_size <= 1024);
  char buffer[1024];
  size_t bn = sizeof (buffer) / dts->elem_size;
  size_t _n = nelem;

  while (_n > 0)
    {
      size_t next = _n < bn ? _n : bn;
      size_t read = dts->read (buffer, next);
      _n -= read;
      if (read != next)
        {
          if (dts->done ())
            {
              break;
            }
          unreachable ();
        }
      ASSERT (nelem >= read);
    }

  ASSERT (nelem >= _n);
  return nelem - _n;
}

TEST (FILE_fread_discard)
{
  FILE *fp = tmpfile_or_abort ();
  FILEDtypeDatasource fds (sizeof (char), fp);
  const char *test_data = "1234567890abcdefghijklmnopqrstuvwxyz";
  size_t data_size = strlen (test_data);

  fds.write (test_data, data_size);

  rewind (fp);

  fseek (fp, 0, SEEK_SET);

  size_t discarded = dtypeds_read_discard (&fds, 10);
  TEST_EQUAL (discarded, 10lu, "%zu");

  char remaining[1024];
  size_t remaining_read = fds.read (remaining, sizeof (remaining));
  TEST_EQUAL (remaining_read, data_size - 10, "%zu");
  TEST_EQUAL (strncmp (remaining, test_data + 10, data_size - 10), 0, "%d");

  fclose_or_abort (fp);
}

size_t
dtypeds_size (DtypeDatasource *dts)
{
  ASSERT (dts);
  size_t len = 0;

  do
    {
      len += dtypeds_read_discard (dts, 1024);
    }
  while (!dts->done ());

  return len;
}

TEST (FILE_dtypeds_size)
{
  FILE *fp = tmpfile_or_abort ();
  FILEDtypeDatasource fds (sizeof (int), fp);

  int data[] = { 1, 2, 3, 4, 5 };
  fds.write (data, 5);

  rewind (fp);

  TEST_EQUAL (dtypeds_size (&fds), 5lu, "%lu");

  fclose_or_abort (fp);
}

bytes
dtypeds_read_all_malloc (DtypeDatasource *dts)
{
  ASSERT (dts->elem_size < 2048);
  uint8_t readb[2048];
  stack ret = stack_create_malloc (2048);

  do
    {
      size_t toread = 2048 / dts->elem_size;
      size_t read = dts->read (readb, toread);

      if (read > 0)
        stack_push_grow (&ret, readb, read * dts->elem_size);
    }
  while (!dts->done ());

  return stacktobytes (ret);

failed:
  return (bytes){ .data = NULL, .len = 0 };
}

TEST (FILE_dtypeds_read_all_malloc)
{
  FILE *fp = tmpfile_or_abort ();
  FILEDtypeDatasource fds (sizeof (int), fp);

  int data[] = { 1, 2, 3, 4, 5 };

  fds.write (data, 5);

  rewind (fp);

  bytes actual = dtypeds_read_all_malloc (&fds);
  TEST_EQUAL (actual.len, 20lu, "%lu");
  TEST_EQUAL (((int *)actual.data)[0], 1, "%d");
  TEST_EQUAL (((int *)actual.data)[1], 2, "%d");
  TEST_EQUAL (((int *)actual.data)[2], 3, "%d");
  TEST_EQUAL (((int *)actual.data)[3], 4, "%d");
  TEST_EQUAL (((int *)actual.data)[4], 5, "%d");
  safe_free (actual.data);

  fclose_or_abort (fp);
}

bytes
dtypeds_read_malloc (DtypeDatasource *dts, size_t nelem)
{
  ASSERT (dts);
  ASSERT (nelem > 0);

  bytes ret = bytes_create_malloc (nelem * dts->elem_size);
  size_t len_actual = dts->read (ret.data, nelem);
  if (len_actual != nelem)
    {
      bytes_resize (&ret, len_actual * dts->elem_size);
    }
  return ret;
}

TEST (FILE_fread_malloc)
{
  FILE *fp = tmpfile_or_abort ();
  FILEDtypeDatasource fds (sizeof (int), fp);

  int data[] = { 1, 2, 3, 4, 5 };
  fds.write (data, 5);
  rewind (fp);

  bytes actual = dtypeds_read_malloc (&fds, 3);
  TEST_EQUAL (actual.len, 12lu, "%lu");
  TEST_EQUAL (((int *)actual.data)[0], 1, "%d");
  TEST_EQUAL (((int *)actual.data)[1], 2, "%d");
  TEST_EQUAL (((int *)actual.data)[2], 3, "%d");
  safe_free (actual.data);

  fclose_or_abort (fp);
}

bytes
dtypeds_read_range (DtypeDatasource *dts, krange k)
{
  bytes ret;
  if (k.k0 > 0)
    {
      size_t read = dtypeds_read_discard (dts, k.k0);
      if (read != k.k0)
        {
          ASSERT (dts->done ());
          return (bytes){
            .data = nullptr,
            .len = 0,
          };
        }
    }

  if (k.isinf)
    {
      ret = dtypeds_read_all_malloc (dts);
    }
  else
    {
      ret = dtypeds_read_malloc (dts, krange_size (k));
    }

  return ret;
}

size_t
dtypeds_copy (DtypeDatasource *src, DtypeDatasource *dest)
{
  uint8_t buffer[1024];
  size_t total = 0;

  do
    {
      size_t read = src->read (buffer, 1024);
      if (read > 0)
        {
          total += read;
          dest->write (buffer, read);
        }
    }
  while (!src->done ());
  return total;
}

TEST (fcopy)
{
  FILE *ifp = tmpfile_or_abort ();
  FILE *ofp = tmpfile_or_abort ();

  FILEDtypeDatasource ifpds (sizeof (char), ifp);
  FILEDtypeDatasource ofpds (sizeof (char), ofp);

  const char *test_data = "This is a test data for copying.";
  ifpds.write (test_data, strlen (test_data));
  rewind (ifp);

  dtypeds_copy (&ifpds, &ofpds);

  rewind (ifp);
  rewind (ofp);

  uint8_t buffer_ifp[1024];
  uint8_t buffer_ofp[1024];
  size_t read_ifp = ifpds.read (buffer_ifp, sizeof (buffer_ifp));
  size_t read_ofp = ofpds.read (buffer_ofp, sizeof (buffer_ofp));

  TEST_EQUAL (read_ifp, read_ofp, "%zu");
  TEST_EQUAL (memcmp (buffer_ifp, buffer_ofp, read_ifp), 0, "%d");

  fclose_or_abort (ifp);
  fclose_or_abort (ofp);
}
