#pragma once

#include "core/DtypeDatasource.hpp"
#include "core/shift_buff.hpp"
#include "fftcaf.h"

/**
 * This system reads in data, runs an fftcaff,
 * then writes the correlation or "abscorrelation"
 * to a buffer and writes to a file pointer
 *
 * Main struct is fftcaff_shift_reader
 * Main method is fftcaff_shift_reader_run
 */

typedef struct
{
  union
  {
    float *f32_corrs_T;
    _Complex float *cf64_corrs_T;
  };

  size_t rows;
  size_t size;
  size_t len;

  int is_float;
  int mmaped;
} fftcaff_writer;

typedef struct
{
  fftcaff caf;
  fftcaff_fout fout;
  fftcaff_writer writer;
  size_t total_samples;
  shift_buf data;
} fftcaff_shift_reader;

fftcaff_shift_reader fftcaff_shift_reader_create (
    fftcaff_create_args fargs,
    fftcaff_fout fout,
    int compute_abs);

void fftcaff_shift_reader_free (fftcaff_shift_reader caf);

size_t fftcsml (fftcaff_shift_reader *caf);

size_t fftcsavail (fftcaff_shift_reader *caf);

void fftcaff_shift_reader_load_max (
    fftcaff_shift_reader *caf,
    DtypeDatasource *ifp);

void fftcaff_shift_reader_run (
    fftcaff_shift_reader *caf,
    DtypeDatasource *ofp,
    size_t towrite);
