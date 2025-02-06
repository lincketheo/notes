#include "core/shift_buff.hpp"

#include "core/errors.h"

#include <cstring>

shift_buf
shift_buf_create (void *data, size_t size, size_t cap)
{
  ASSERT (data);
  ASSERT (size > 0);
  ASSERT (cap > 0);

  shift_buf buf;
  buf.data = data;
  buf.size = size;
  buf.cap = cap;
  buf.len = 0;

  return buf;
}

void
shift_buf_fread_max (shift_buf *dest, DtypeDatasource *ifp)
{
  ASSERT (dest->len <= dest->cap);
  ASSERT (dest->size == ifp->elem_size);

  if (dest->len == dest->cap)
    return;

  size_t space = dest->cap - dest->len;
  char *head = (char *)dest->data + dest->len * dest->size;
  size_t read = ifp->read (head, space);

  dest->len += read;
}

void
shift_buf_del_front (shift_buf *dest, size_t n)
{
  ASSERT (dest);
  ASSERT (n > 0);
  ASSERT (dest->len <= dest->cap);

  if (n > dest->len)
    n = dest->len;

  memmove (dest->data, (char *)dest->data + n * dest->size,
           (dest->len - n) * dest->size);

  dest->len -= n;
}
