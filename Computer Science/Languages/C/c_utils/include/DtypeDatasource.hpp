#pragma once

#include "core/data_structures.h"

#include <stdio.h>
#include <stdlib.h>

//////////////////////////////////////////
/// Base class for data sources with specific element sizes.

/**
 * Abstract base class for handling data sources with a specific element size.
 */
class DtypeDatasource
{
public:
  /**
   * Constructs a data source with the given element size.
   * @param elem_size Size of each element in bytes.
   */
  DtypeDatasource (size_t elem_size) : elem_size (elem_size) {}

  /**
   * Reads up to `n` elements into the destination buffer.
   * @param dest Destination buffer to read into.
   * @param n Number of elements to read.
   * @return Number of elements successfully read.
   */
  virtual size_t read (void *dest, size_t n) = 0;

  /**
   * Writes `n` elements from the source buffer to the data source.
   * @param src Source buffer to write from.
   * @param n Number of elements to write.
   */
  virtual void write (const void *src, size_t n) = 0;

  /**
   * Checks if the data source is exhausted.
   * @return True if the data source has no more data, false otherwise.
   */
  virtual bool done () = 0;

  const size_t elem_size; // Size of each element in bytes.
};

//////////////////////////////////////////
/// FILE-backed implementation of `DtypeDatasource`.

class FILEDtypeDatasource : public DtypeDatasource
{
public:
  /**
   * Constructs a FILE-backed data source.
   * @param elem_size Size of each element in bytes.
   * @param fp Pointer to the file stream.
   */
  FILEDtypeDatasource (size_t elem_size, FILE *fp)
      : DtypeDatasource (elem_size), fp (fp) {}

  virtual size_t read (void *dest, size_t n) override;

  virtual void write (const void *src, size_t n) override;

  virtual bool done () override;

private:
  FILE *fp; // Pointer to the file stream.
};

//////////////////////////////////////////
/// Extensions for working with `DtypeDatasource`.

/**
 * Reads a range of elements defined by a `krange` into a byte buffer.
 * @param dts Pointer to the data source.
 * @param k Range of elements to read.
 * @return A `bytes` object containing the read data.
 */
bytes dtypeds_read_range (DtypeDatasource *dts, krange k);

/**
 * Computes the size of the data source in elements.
 * @param dts Pointer to the data source.
 * @return Number of elements in the data source.
 */
size_t dtypeds_size (DtypeDatasource *dts);

/**
 * Reads and discards `nelem` elements from the data source.
 * @param dts Pointer to the data source.
 * @param nelem Number of elements to discard.
 * @return Number of elements discarded.
 */
size_t dtypeds_read_discard (DtypeDatasource *dts, size_t nelem);

/**
 * Reads all remaining data from the data source into a dynamically allocated byte buffer.
 * @param dts Pointer to the data source.
 * @return A `bytes` object containing all the read data.
 */
bytes dtypeds_read_all_malloc (DtypeDatasource *dts);

/**
 * Reads `nelem` elements from the data source into a dynamically allocated byte buffer.
 * @param dts Pointer to the data source.
 * @param nelem Number of elements to read.
 * @return A `bytes` object containing the read data.
 */
bytes dtypeds_read_malloc (DtypeDatasource *dts, size_t nelem);

/**
 * Copies data from one data source to another.
 * @param src Pointer to the source data source.
 * @param dest Pointer to the destination data source.
 * @return Number of elements successfully copied.
 */
size_t dtypeds_copy (DtypeDatasource *src, DtypeDatasource *dest);
