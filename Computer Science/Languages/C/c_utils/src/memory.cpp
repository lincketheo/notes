#include "core/memory.h"

#include "core/errors.h"
#include "core/stdlibf.h"
#include "core/testing.h"

int
system_is_little_endian ()
{
  unsigned int num = 1;
  return *(char *)&num == 1;
}

linalloc
linalloc_create_default (size_t cap)
{
  ASSERT (cap > 0);
  linalloc ret;
  ret.data = (uint8_t *)malloc_or_abort (cap);
  ret.cap = cap;
  ret.len = 0;
  return ret;
}

linalloc
linalloc_create_from (uint8_t *data, size_t cap)
{
  ASSERT (data);
  ASSERT (cap > 0);
  linalloc ret;
  ret.data = data;
  ret.cap = cap;
  ret.len = 0;
  return ret;
}

static inline void
linalloc_ensure_space_or_fail (linalloc *s, size_t newcap)
{
  ASSERT (s);
  ASSERT (s->cap > 0);
  ASSERT (newcap > 0);

  if (s->cap < newcap)
    {
      fatal_error ("Linear allocator overflow."
                   "Requesting: %zu bytes, capacity is: %zu bytes "
                   "and linallocator is not dynamic\n",
                   newcap, s->cap);
      unreachable ();
    }
}

void *
linalloc_malloc (linalloc *s, size_t bytes)
{
  ASSERT (s);
  ASSERT (bytes > 0);

  linalloc_ensure_space_or_fail (s, s->len + bytes);

  uint8_t *ret = s->data + s->len;
  s->len += bytes;
  return (void *)ret;
}

TEST (linalloc_malloc)
{
  linalloc s = linalloc_create_default (100);

  void *head1 = linalloc_malloc (&s, 5);
  memcpy (head1, "foob", 5);
  TEST_EQUAL (s.len, 5lu, "%zu");

  void *head2 = linalloc_malloc (&s, 10);
  memcpy (head2, "foobarbaz", 10);
  TEST_EQUAL (s.len, 15lu, "%zu");

  TEST_ASSERT_STR_EQUAL ("foob", (char *)head1);
  TEST_ASSERT_STR_EQUAL ("foobarbaz", (char *)head2);
  linalloc_free_default (&s);
}

void
linalloc_free_default (linalloc *s)
{
  safe_free (s->data);
  s->len = 0;
  s->cap = 0;
}

void
memmove_top_bottom (void *data, size_t size, size_t from, size_t len)
{
  ASSERT (data);
  ASSERT (size > 0);
  ASSERT (from < len && from > 0);

  uint8_t *top = (uint8_t *)data + from * size;
  memmove (data, top, size * (len - from));
}

void
memset_top_zero (void *data, size_t size, size_t from, size_t len)
{
  ASSERT (data);
  ASSERT (size > 0);
  ASSERT (from < len && from >= 0);

  uint8_t *top = (uint8_t *)data + from * size;
  memset (top, 0, size * (len - from));
}

size_t
memcpy_stride (void *dest, const void *src, size_t size, size_t n,
               size_t step)
{
  ASSERT (dest);
  ASSERT (src);
  ASSERT (size > 0);
  ASSERT (n > 0);
  ASSERT (step > 0);
  uint8_t *_dest = (uint8_t *)dest;
  const uint8_t *_src = (uint8_t *)src;

  size_t s = 0, d = 0;
  for (; s < n && d < n; s += step, ++d)
    {
      _dest[d * size] = _src[s * size];
    }
  return d;
}
