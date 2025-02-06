#pragma once

#include "core/data_structures.h"

#include <stdio.h>
#include <stdlib.h>

class DtypeDatasource
{
public:
  DtypeDatasource (size_t elem_size) : elem_size (elem_size) {}

  virtual size_t read (void *dest, size_t n) = 0;
  virtual void write (const void *dest, size_t n) = 0;
  virtual bool done () = 0;

  const size_t elem_size;
};

class FILEDtypeDatasource : public DtypeDatasource
{
public:
  FILEDtypeDatasource (size_t elem_size, FILE *fp) : DtypeDatasource (elem_size), fp (fp) {}

  virtual size_t read (void *dest, size_t n) override;
  virtual void write (const void *src, size_t n) override;
  virtual bool done () override;

private:
  FILE *fp;
};

//////////////////////////////////////////
/// Extensions

bytes dtypeds_read_range (DtypeDatasource *dts, krange k);

size_t dtypeds_size (DtypeDatasource *dts);

size_t dtypeds_read_discard (DtypeDatasource *dts, size_t nelem);

bytes dtypeds_read_all_malloc (DtypeDatasource *dts);

bytes dtypeds_read_malloc (DtypeDatasource *dts, size_t nelem);

size_t dtypeds_copy (DtypeDatasource *src, DtypeDatasource *dest);
