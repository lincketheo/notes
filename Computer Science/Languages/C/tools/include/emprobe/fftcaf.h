#pragma once

#include "core/data_structures.h"

#include <complex.h>
#include <fftw3.h>

/**
 * fftcaff is a float (hence the extra f) implementation of
 * an fftcaf
 */

// The file output pointers for each
// data element of fftcaf
typedef struct
{
  FILE *fft_match;
  FILE *data;
  FILE *fft_data;
  FILE *fft_corr;
  FILE *corr;
} fftcaff_fout;

// Creates an empty fout
fftcaff_fout fftcaff_fout_create ();

typedef enum
{
  FFTCT_LITE,
  FFTCT_NORMAL,
} fftcaff_type;

// Just the data of an fftcaff
typedef struct
{
  const fftwf_complex *fft_match;
  fftwf_complex *data;
  fftwf_complex *fft_data;
  fftwf_complex *fft_corr;
  fftwf_complex *corr;
  int mmapped;
  size_t fftml;
} fftcaff_data;

// The tagged union fftcaff
typedef struct
{
  const size_t match_len;
  fftcaff_data data;

  union
  {
    const frange freq_space;
    struct
    {
      const size_t offsets;
      const size_t bin_stride;
    } lite;
  } params;

  const fftcaff_type type;
  const fftwf_plan data_fplan;
  const fftwf_plan corr_iplan;
} fftcaff;

/**
 * The input arguments to
 * create a caff
 *
 * - A tagged union for lite or normal
 */
typedef struct
{
  cf64_buffer match;
  union
  {
    struct
    {
      frange f;
      float fs;
    } normal;
    struct
    {
      size_t f_offsets;
      size_t bin_stride;
      int zero_middle;
    } lite;
  } params;
  fftcaff_type type;
  const char *mmap_base;
} fftcaff_create_args;

fftcaff fftcaff_create (fftcaff_create_args args);

// Writes internal buffers to a file specified by fout
void fftcaff_fout_write_out (fftcaff *f, fftcaff_fout fout);

// Bins are mapped [-bin:0:bin] -> [0:bin_stride:2*bin]
// (2*bin is not a mistake, there are a total of 2*bin + 1 outputs - to include 0)
// These two functions execute this mapping and reverse
size_t lite_bin_to_ind (size_t offsets, size_t bin_stride, long bin);
long lite_ind_to_bin (size_t offsets, size_t bin_stride, size_t binind);

// Writes out fout only the match - then sets the match to NULL
void fftcaff_fout_write_first_match_and_close (fftcaff *f, fftcaff_fout *fout);

// Calculates Number of frequency offsets
size_t fftcaff_rows (fftcaff *f);

size_t fftcaff_cols (fftcaff *f);

// Same as above but using args
size_t fftcaff_rows_args (fftcaff_create_args args);
size_t fftcaff_cols_args (fftcaff_create_args args);

// Rows of a lite fftcaff
size_t fftcaff_lite_rows (size_t offsets);

void fftcaff_free (fftcaff *caf);

void fftcaff_run (fftcaff *caf);

// dest is an array of length [rows]
// Writes cabsf(c) for each c for sample k - order is
// from minimum f offset to maximum f offset - essentially
// just takes column k of the cor matrix
void fftcaff_get_abscorrs_for_k (float *dest, fftcaff *caf, size_t k);

// Same as above, but doesn't take cabsf
void fftcaff_get_corrs_for_k (_Complex float *dest, fftcaff *caf, size_t k);
