#include "emprobe/fftcaf.h"

#include "core/data_structures.h"
#include "core/errors.h"
#include "core/io.h"
#include "core/logging.h"
#include "core/math.h"
#include "core/posixf.h"
#include "core/stdlibf.h"
#include "core/testing.h"

#include <fftw3.h>
#include <limits.h>
#include <string.h>

fftcaff_fout
fftcaff_fout_create ()
{
  fftcaff_fout ret;
  memset (&ret, 0, sizeof (ret));
  return ret;
}

size_t
fftcaff_lite_rows (size_t offsets)
{
  return 1 + 2 * offsets;
}

static inline size_t
fft_match_len (fftcaff *f)
{
  switch (f->type)
    {
    case FFTCT_LITE:
      return f->match_len;
    case FFTCT_NORMAL:
      return f->match_len * fftcaff_rows (f);
    default:
      unreachable ();
    }
  return 0;
}

void
fftcaff_fout_write_out (fftcaff *f, fftcaff_fout fout)
{
  if (fout.fft_match)
    {
      ns_log (DEBUG, "Writing out fft(match) debug\n");
      fwrite_or_abort (f->data.fft_match, sizeof *f->data.fft_match,
                       fft_match_len (f), fout.fft_match);
    }
  if (fout.data)
    {
      ns_log (DEBUG, "Writing out data debug\n");
      fwrite_or_abort (f->data.data, sizeof *f->data.data, f->match_len,
                       fout.data);
    }
  if (fout.fft_data)
    {
      ns_log (DEBUG, "Writing out fft(data) debug\n");
      fwrite_or_abort (f->data.fft_data, sizeof *f->data.fft_data, f->match_len,
                       fout.fft_data);
    }
  if (fout.fft_corr)
    {
      ns_log (DEBUG, "Writing out fft(corl) debug\n");
      fwrite_or_abort (f->data.fft_corr, sizeof *f->data.fft_corr,
                       f->match_len * fftcaff_rows (f), fout.fft_corr);
    }
  if (fout.corr)
    {
      ns_log (DEBUG, "Writing out corl debug\n");
      fwrite_or_abort (f->data.corr, sizeof *f->data.corr,
                       f->match_len * fftcaff_rows (f), fout.corr);
    }
}

long
lite_ind_to_bin (size_t offsets, size_t bin_stride, size_t binind)
{
  size_t nbins = fftcaff_lite_rows (offsets);
  long center = (long)(nbins / 2);
  long ret = ((long)binind - center) * bin_stride;
  return ret;
}

TEST (lite_ind_to_bin)
{
  // -8   -4    0     4     8
  //  0    1    2     3     4
  TEST_EQUAL (lite_ind_to_bin (2, 4, 0), -8l, "%ld");
  TEST_EQUAL (lite_ind_to_bin (2, 4, 1), -4l, "%ld");
  TEST_EQUAL (lite_ind_to_bin (2, 4, 2), 0l, "%ld");
  TEST_EQUAL (lite_ind_to_bin (2, 4, 3), 4l, "%ld");
  TEST_EQUAL (lite_ind_to_bin (2, 4, 4), 8l, "%ld");

  // -12    -8   -4    0     4     8    12
  //  0      1    2    3     4     5    6
  TEST_EQUAL (lite_ind_to_bin (3, 4, 0), -12l, "%ld");
  TEST_EQUAL (lite_ind_to_bin (3, 4, 3), 0l, "%ld");
  TEST_EQUAL (lite_ind_to_bin (3, 4, 6), 12l, "%ld");

  // -2     -1    0    1     2
  //  0      1    2    3     4
  TEST_EQUAL (lite_ind_to_bin (2, 1, 0), -2l, "%ld");
  TEST_EQUAL (lite_ind_to_bin (2, 1, 1), -1l, "%ld");
  TEST_EQUAL (lite_ind_to_bin (2, 1, 4), 2l, "%ld");

  // 0
  // 0
  TEST_EQUAL (lite_ind_to_bin (0, 10, 0), 0l, "%ld");
}

size_t
lite_bin_to_ind (size_t offsets, size_t bin_stride, long bin)
{
  ASSERT (bin % (long)bin_stride == 0);
  long ret = (double)bin / (double)bin_stride + offsets;
  return (size_t)ret;
}

TEST (lite_bin_to_ind)
{
  // -8   -4    0     4     8
  //  0    1    2     3     4
  TEST_EQUAL (lite_bin_to_ind (2, 4, -8), 0lu, "%lu");
  TEST_EQUAL (lite_bin_to_ind (2, 4, -4), 1lu, "%lu");
  TEST_EQUAL (lite_bin_to_ind (2, 4, 0), 2lu, "%lu");
  TEST_EQUAL (lite_bin_to_ind (2, 4, 4), 3lu, "%lu");
  TEST_EQUAL (lite_bin_to_ind (2, 4, 8), 4lu, "%lu");

  // -12    -8   -4    0     4     8    12
  //  0      1    2    3     4     5    6
  TEST_EQUAL (lite_bin_to_ind (3, 4, -12), 0lu, "%lu");
  TEST_EQUAL (lite_bin_to_ind (3, 4, 8), 5lu, "%lu");
  TEST_EQUAL (lite_bin_to_ind (3, 4, 12), 6lu, "%lu");

  // -2     -1    0    1     2
  //  0      1    2    3     4
  TEST_EQUAL (lite_bin_to_ind (2, 1, -1), 1lu, "%lu");
  TEST_EQUAL (lite_bin_to_ind (2, 1, -2), 0lu, "%lu");
  TEST_EQUAL (lite_bin_to_ind (2, 1, 2), 4lu, "%lu");

  // 0
  // 0
  TEST_EQUAL (lite_bin_to_ind (0, 10, 0), 0lu, "%lu");
}

size_t
fftcaff_cols (fftcaff *f)
{
  return f->match_len;
}

size_t
fftcaff_rows (fftcaff *f)
{
  switch (f->type)
    {
    case FFTCT_LITE:
      return fftcaff_lite_rows (f->params.lite.offsets);
    case FFTCT_NORMAL:
      return frange_size (f->params.freq_space);
    default:
      unreachable ();
    }
}

size_t
fftcaff_rows_args (fftcaff_create_args args)
{
  switch (args.type)
    {
    case FFTCT_LITE:
      return fftcaff_lite_rows (args.params.lite.f_offsets);
    case FFTCT_NORMAL:
      return frange_size (args.params.normal.f);
    default:
      unreachable ();
    }
}

size_t
fftcaff_cols_args (fftcaff_create_args args)
{
  return args.match.len;
}

static inline void
fftcaff_lite_match_init (
    fftwf_complex *dest,
    const cf64_buffer match,
    size_t f_offsets,
    int zero_middle)
{
  ASSERT (f_offsets < match.len);

  ns_log (DEBUG,
          "Creating FFTCAFF lite version with %zu fft bin offsets"
          " and %zu spreading factor\n",
          f_offsets, match.len);

  // Take the fft
  fftwf_dft_1d_one_time (
      match.len,
      (_Complex float *)match.data,
      (_Complex float *)dest,
      FFTW_FORWARD,
      FFTW_ESTIMATE);

  conjf_inl ((_Complex float *)dest, match.len);

  // Set the middle section to 0 to eliminate mid range frequencies.
  if (zero_middle)
    {
      intervals middle = (intervals){
        .s0 = match.len / 4,
        .s1 = 3 * match.len / 4,
      };
      zerocf_range ((_Complex float *)dest, middle);
    }
}

static inline fftwf_plan
match_fftwf_plan_create (
    fftwf_complex *out,
    fftwf_complex *in,
    frange f,
    int match_len)
{
  return fftwf_plan_many_dft (
      1,               // Rank (1D transform)
      &match_len,      // Pointer to the size of each dimension
      frange_size (f), // Number of transforms (batch size)
      in,              // Input array
      NULL,            // Input strides (NULL = contiguous data)
      1,               // Distance between elements in a single transform
      match_len,       // Distance between consecutive transforms
      out,             // Output array
      NULL,            // Output strides (NULL = contiguous data)
      1,               // Distance between elements in a single output transform
      match_len,       // Distance between consecutive output transforms
      FFTW_FORWARD,    // Transform direction (forward FFT)
      FFTW_ESTIMATE    // Planning mode (estimate for speed)
  );
}

// Create a forward FFT plan for the provided data
static inline fftwf_plan
create_data_fplan (
    int ml,
    fftwf_complex *data,
    fftwf_complex *fft_data)
{
  return fftwf_plan_dft_1d (
      ml,           // Transform length (1D transform)
      data,         // Input data array
      fft_data,     // Output data array
      FFTW_FORWARD, // Transform direction (forward FFT)
      FFTW_ESTIMATE // Planning mode (estimate for speed)
  );
}

// Create an inverse FFT plan for correlation data
static inline fftwf_plan
create_corr_iplan (
    int len,
    int rows,
    fftwf_complex *fft_corr,
    fftwf_complex *corr)
{
  return fftwf_plan_many_dft (
      1,             // Rank (1D transform)
      &len,          // Pointer to the size of each dimension
      rows,          // Number of transforms (batch size)
      fft_corr,      // Input array
      NULL,          // Input strides (NULL = contiguous data)
      1,             // Distance between elements in a single transform
      len,           // Distance between consecutive transforms
      corr,          // Output array
      NULL,          // Output strides (NULL = contiguous data)
      1,             // Distance between elements in a single output transform
      len,           // Distance between consecutive output transforms
      FFTW_BACKWARD, // Transform direction (inverse FFT)
      FFTW_ESTIMATE  // Planning mode (estimate for speed)
  );
}

static inline void
fftcaff_normal_match_init (
    fftwf_complex *dest,
    fftwf_complex *tmp,
    cf64_buffer match,
    frange f,
    float fs)
{
  size_t rows = frange_size (f);
  size_t cols = match.len;

  ns_log (DEBUG, "Creating FFTCAFF normal version\n");

  init_tuner_emat (
      (_Complex float *)tmp,
      match.len,
      f, fs);

  cf64_row_had_mult_vec (
      (_Complex float *)tmp,
      (_Complex float *)match.data,
      rows, cols);

  // Take the fft
  fftwf_plan match_plan = match_fftwf_plan_create (dest, tmp, f, match.len);
  fftwf_execute (match_plan);
  fftwf_destroy_plan (match_plan);

  cf64_mat_conj_inl ((_Complex float *)dest, rows, cols);
}

static inline size_t
fftcaff_fft_match_len (fftcaff *caf)
{
  ASSERT (caf);
  if (caf->type == FFTCT_NORMAL)
    {
      return caf->match_len * fftcaff_rows (caf);
    }
  else if (caf->type == FFTCT_LITE)
    {
      return caf->match_len;
    }
  unreachable ();
}

static inline size_t
fftcaff_fft_match_len_args (fftcaff_create_args args)
{
  if (args.type == FFTCT_NORMAL)
    {
      return args.match.len * fftcaff_rows_args (args);
    }
  else if (args.type == FFTCT_LITE)
    {
      return args.match.len;
    }
  unreachable ();
}

static inline fftcaff_data
fftcaff_data_create (fftcaff_create_args args)
{
  ASSERT (args.match.len > 0);
  size_t ml = args.match.len;
  ASSERT (is_power_of_2 (ml));

  size_t rows = fftcaff_rows_args (args);
  size_t fftml = fftcaff_fft_match_len_args (args);
  size_t size = sizeof (fftwf_complex);

  fftwf_complex *fft_match;
  fftwf_complex *data;
  fftwf_complex *fft_data;
  fftwf_complex *fft_corr;
  fftwf_complex *corr;

  if (args.mmap_base == NULL)
    {
      ns_log (DEBUG, "Creating FFTCAFF in memory\n");
      fft_match = (fftwf_complex *)fftwf_malloc (ml * rows * size);
      data = (fftwf_complex *)fftwf_malloc (ml * size);
      fft_data = (fftwf_complex *)fftwf_malloc (ml * size);
      fft_corr = (fftwf_complex *)fftwf_malloc (ml * rows * size);
      corr = (fftwf_complex *)fftwf_malloc (ml * rows * size);
    }
  else
    {
      ASSERT (args.mmap_base);
      ns_log (DEBUG, "Creating FFTCAFF with memory mapped data\n");
      fft_match = (fftwf_complex *)mmap_malloc_create_with_base_fname_wrb (
          args.mmap_base, "/fft_match.bin", sizeof *fft_match, fftml);
      data = (fftwf_complex *)mmap_malloc_create_with_base_fname_wrb (
          args.mmap_base, "/data.bin", sizeof *data, ml);
      fft_data = (fftwf_complex *)mmap_malloc_create_with_base_fname_wrb (
          args.mmap_base, "/fft_data.bin", sizeof *fft_data, ml);
      fft_corr = (fftwf_complex *)mmap_malloc_create_with_base_fname_wrb (
          args.mmap_base, "/fft_corr.bin", ml * sizeof *fft_corr, rows);
      corr = (fftwf_complex *)mmap_malloc_create_with_base_fname_wrb (
          args.mmap_base, "/corr.bin", ml * sizeof *corr, rows);
    }

  switch (args.type)
    {
    case FFTCT_LITE:
      fftcaff_lite_match_init (
          fft_match,
          args.match,
          args.params.lite.f_offsets,
          args.params.lite.zero_middle);
      break;
    case FFTCT_NORMAL:
      fftcaff_normal_match_init (
          fft_match,
          corr,
          args.match,
          args.params.normal.f,
          args.params.normal.fs);
      break;
    }

  return (fftcaff_data){
    .fft_match = fft_match,
    .data = data,
    .fft_data = fft_data,
    .fft_corr = fft_corr,
    .corr = corr,
    .mmapped = args.mmap_base != NULL,
    .fftml = fftml,
  };
}

static inline void
fftcaff_data_free (fftcaff_data data, size_t ml,
                   size_t rows)
{
  if (!data.mmapped)
    {
      ns_log (DEBUG, "Freeing memory from FFTCAFF\n");
      fftwf_free ((void *)data.fft_match);
      fftwf_free (data.data);
      fftwf_free (data.fft_data);
      fftwf_free (data.fft_corr);
      fftwf_free (data.corr);
    }
  else
    {
      ns_log (DEBUG, "Unmapping files from FFTCAFF\n");
      munmap_or_abort ((void *)data.fft_match,
                       data.fftml * sizeof *data.fft_match);
      munmap_or_abort (data.data, ml * sizeof *data.data);
      munmap_or_abort (data.fft_data, ml * sizeof *data.fft_data);
      munmap_or_abort (data.fft_corr, ml * rows * sizeof *data.fft_data);
      munmap_or_abort (data.corr, ml * rows * sizeof *data.fft_data);
    }
}

fftcaff
fftcaff_create (fftcaff_create_args args)
{
  ASSERT (args.match.len > 0);
  size_t ml = args.match.len;
  ASSERT (is_power_of_2 (ml));
  size_t rows = fftcaff_rows_args (args);
  size_t fftml = fftcaff_fft_match_len_args (args);

  // For fftwf to create corr plan
  ASSERT (args.match.len <= (size_t)INT_MAX);
  int len = args.match.len;

  fftcaff_data data = fftcaff_data_create (args);

  // Data and correlation fft plans
  fftwf_plan data_fplan = create_data_fplan (
      ml,
      data.data,
      data.fft_data);
  fftwf_plan corr_iplan = create_corr_iplan (
      args.match.len,
      rows,
      data.fft_corr,
      data.corr);

  switch (args.type)
    {
    case FFTCT_LITE:
      return (fftcaff){
        .match_len = args.match.len,
        .data = data,
        .params = {
            .lite = {
                .offsets = args.params.lite.f_offsets,
                .bin_stride = args.params.lite.bin_stride,
            },
        },
        .type = args.type,
        .data_fplan = data_fplan,
        .corr_iplan = corr_iplan,
      };
    case FFTCT_NORMAL:
      return (fftcaff){
        .match_len = args.match.len,
        .data = data,
        .params = {
            .freq_space = args.params.normal.f,
        },
        .type = args.type,
        .data_fplan = data_fplan,
        .corr_iplan = corr_iplan,
      };
    default:
      unreachable ();
    }
  unreachable ();
}

void
fftcaff_free (fftcaff *caf)
{
  ASSERT (caf);
  fftcaff_data_free (caf->data, caf->match_len, fftcaff_rows (caf));
  fftwf_destroy_plan (caf->data_fplan);
  fftwf_destroy_plan (caf->corr_iplan);
}

static inline void
lite_compute_fft_corr (fftcaff *caf)
{
  ASSERT (caf->params.lite.offsets < LONG_MAX);

  long start_bin = -(long)caf->params.lite.offsets * caf->params.lite.bin_stride;

  ns_log (
      DEBUG,
      "Computing FFT(correlations) elementwise "
      "multiplication in the frequency domain\n");

  _Complex float *fft_corr = (_Complex float *)caf->data.fft_corr;

#ifndef NOMP
#pragma omp parallel for
#endif
  for (size_t i = 0; i < fftcaff_rows (caf); ++i)
    {
      long bin0 = start_bin + i * caf->params.lite.bin_stride;

      cf64_row_had_right_shift (
          &fft_corr[i * caf->match_len],
          (_Complex float *)caf->data.fft_data,
          (_Complex float *)caf->data.fft_match,
          caf->match_len,
          bin0);
    }
}

static inline void
normal_compute_fft_corr (fftcaff *caf)
{

  ns_log (DEBUG, "Computing FFT(correlations) elementwise "
                 "multiplication in the frequency domain\n");

  cf64_mat_vec_row_had ((_Complex float *)caf->data.fft_corr,
                        (_Complex float *)caf->data.fft_match,
                        (_Complex float *)caf->data.fft_data,
                        frange_size (caf->params.freq_space), caf->match_len);
}

void
fftcaff_run (fftcaff *caf)
{
  ASSERT (caf);
  ASSERT (caf->params.lite.offsets < LONG_MAX);

  ns_log (DEBUG, "Executing forward fft for data\n");

  // FFT(data)
  fftwf_execute (caf->data_fplan);

  // Compute FFT(corr) (Using row wise multiplication)
  switch (caf->type)
    {
    case FFTCT_LITE:
      lite_compute_fft_corr (caf);
      break;
    case FFTCT_NORMAL:
      normal_compute_fft_corr (caf);
      break;
    }

  ns_log (DEBUG, "Computing IFFT(correlations)\n");

  fftwf_execute (caf->corr_iplan);
}

void
fftcaff_get_abscorrs_for_k (float *dest, fftcaff *caf, size_t k)
{
  ASSERT (dest);
  ASSERT (caf);
  ASSERT (caf->data.corr);
  ASSERT (k < caf->match_len);

  _Complex float *corr = (_Complex float *)caf->data.corr;

#ifndef NOMP
#pragma omp parallel for // simd
#endif
  for (int i = 0; i < fftcaff_rows (caf); ++i)
    {
      dest[i] = modulus_sqf (corr[i * caf->match_len + k]);
    }
}

void
fftcaff_get_corrs_for_k (_Complex float *dest, fftcaff *caf, size_t k)
{
  ASSERT (dest);
  ASSERT (caf);
  ASSERT (caf->data.corr);
  ASSERT (k < caf->match_len);

  _Complex float *corr = (_Complex float *)caf->data.corr;

#ifndef NOMP
#pragma omp parallel for // simd
#endif
  for (int i = 0; i < fftcaff_rows (caf); ++i)
    {
      dest[i] = corr[i * caf->match_len + k];
    }
}

void
fftcaff_fout_write_first_match_and_close (fftcaff *f, fftcaff_fout *fout)
{
  if (fout->fft_match)
    {
      ns_log (DEBUG, "Writing out match\n");
      fftcaff_fout first = { 0 };
      first.fft_match = fout->fft_match;
      fftcaff_fout_write_out (f, first);
      fclose_or_abort (fout->fft_match);
      fout->fft_match = NULL;
    }
}
