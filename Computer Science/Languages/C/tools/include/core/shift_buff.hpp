#pragma once

#include "core/DtypeDatasource.hpp"

typedef struct
{
  void *data;
  size_t size;
  size_t cap;
  size_t len;
} shift_buf;

shift_buf shift_buf_create (void *data, size_t len, size_t cap);

void shift_buf_fread_max (shift_buf *dest, DtypeDatasource *ifp);

void shift_buf_del_front (shift_buf *dest, size_t n);
