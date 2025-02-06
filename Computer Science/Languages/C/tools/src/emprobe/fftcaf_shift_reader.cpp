#include "emprobe/fftcaf_shift_reader.h"

#include "core/DtypeDatasource.hpp"
#include "core/io.h"
#include "core/logging.h"
#include "core/math.h"
#include "core/posixf.h"
#include "core/stdlibf.h"

#include "emprobe/fftcaf.h"

static inline void *
fftcaff_writer_head (fftcaff_writer data)
{
  if (data.is_float)
    return data.f32_corrs_T;
  return data.cf64_corrs_T;
}

static void
fftcaff_writer_free (fftcaff_writer data)
{
  if (data.mmaped)
    {
      munmap_or_abort (fftcaff_writer_head (data), data.len * data.size);
    }
  else
    {
      safe_free (fftcaff_writer_head (data));
    }
}

static fftcaff_writer
fftcaff_writer_create (
    fftcaff_create_args fargs,
    int compute_abs)
{
  size_t size = compute_abs ? sizeof (float) : sizeof (_Complex float);
  size_t cols = fftcaff_cols_args (fargs);
  size_t rows = fftcaff_rows_args (fargs);
  size_t len = rows * cols;

  fftcaff_writer ret = {
    .rows = rows,
    .size = size,
    .len = len,
    .is_float = compute_abs,
  };

  if (fargs.mmap_base == NULL)
    {
      ret.mmaped = 0;
      if (compute_abs)
        {
          ret.f32_corrs_T = (float *)malloc_or_abort (len * size);
        }
      else
        {
          ret.cf64_corrs_T = (_Complex float *)malloc_or_abort (len * size);
        }
    }
  else
    {
      ASSERT (fargs.mmap_base);
      ret.mmaped = 1;
      if (compute_abs)
        {
          ret.f32_corrs_T = (float *)mmap_malloc_create_with_base_fname_wrb (
              fargs.mmap_base, "/write_out_temp.bin", size, len);
        }
      else
        {
          ret.cf64_corrs_T = (_Complex float *)mmap_malloc_create_with_base_fname_wrb (
              fargs.mmap_base, "/write_out_temp.bin", ret.size, ret.len);
        }
    }

  return ret;
}

static inline void
fftcaff_writer_write_out (
    size_t num_samps,
    DtypeDatasource *ofp,
    fftcaff_writer w,
    fftcaff *caf)
{
  if (w.is_float)
    {
      ns_log (
          DEBUG,
          "Transposing and taking the abs of "
          "fftcaff corelation data as floats.\n");

      ns_log (
          DEBUG,
          "Only writing %zu/%zu samples (%f\% of the buffer)\n",
          num_samps,
          fftcaff_cols (caf),
          (float)num_samps / (float)fftcaff_cols (caf));

      // Take the transpose of the caf matrix, limit col by num_samps
      cf64_transpose_f32_abs_sqrd_col_limit (
          (float *)fftcaff_writer_head (w),
          (_Complex float *)caf->data.corr,
          fftcaff_rows (caf),
          fftcaff_cols (caf),
          num_samps);
    }
  else
    {
      ns_log (
          DEBUG,
          "Transposing fftcaff corelation data as _Complex floats.\n");

      ns_log (
          DEBUG,
          "Only writing %zu/%zu samples (%f\% of the buffer)\n)",
          num_samps,
          fftcaff_cols (caf),
          (float)num_samps / (float)fftcaff_cols (caf));

      // Take the transpose of the caf matrix, limit col by num_samps
      cf64_transpose_cf64_col_limit (
          (_Complex float *)fftcaff_writer_head (w),
          (_Complex float *)caf->data.corr,
          fftcaff_rows (caf),
          fftcaff_cols (caf),
          num_samps);
    }

  ns_log (DEBUG, "Writing out %zu FFTCAFF results\n", num_samps);
  ofp->write (fftcaff_writer_head (w), num_samps * w.rows);
}

typedef union
{
  _Complex float *write_outcf;
  float *write_outf;
} write_out;

static inline write_out
write_out_create (int compute_abs, fftcaff *caf)
{
  write_out w;
  if (compute_abs)
    {
      w.write_outf = (float *)malloc_or_abort (
          fftcaff_rows (caf) * sizeof *w.write_outf);
    }
  else
    {
      w.write_outcf = (_Complex float *)malloc_or_abort (
          fftcaff_rows (caf) * sizeof *w.write_outcf);
    }
  return w;
}

static inline void
write_out_free (int compute_abs, write_out w)
{
  if (compute_abs)
    {
      safe_free (w.write_outf);
    }
  else
    {
      safe_free (w.write_outcf);
    }
}

fftcaff_shift_reader
fftcaff_shift_reader_create (
    fftcaff_create_args fargs,
    fftcaff_fout fout,
    int compute_abs)
{
  // Create fftcaff
  fftcaff caf = fftcaff_create (fargs);

  // Create the output write buffer
  fftcaff_writer w = fftcaff_writer_create (fargs, compute_abs);

  // Write out the match filter if there is one (once)
  fftcaff_fout_write_first_match_and_close (&caf, &fout);

  _Complex float *data = (_Complex float *)caf.data.data;

  return (fftcaff_shift_reader){
    .caf = caf,
    .fout = fout,
    .writer = w,
    .total_samples = 0,
    .data = shift_buf_create (data, sizeof *data, caf.match_len),
  };
}

void
fftcaff_shift_reader_load_max (fftcaff_shift_reader *caf, DtypeDatasource *ifp)
{
  ns_log (DEBUG, "fftcaff before read: %zu/%zu\n", caf->data.len, caf->data.cap);

  shift_buf_fread_max (&caf->data, ifp);

  ns_log (DEBUG, "fftcaff After read: %zu/%zu\n", caf->data.len, caf->data.cap);
}

size_t
fftcsml (fftcaff_shift_reader *caf)
{
  return caf->caf.match_len;
}

size_t
fftcsavail (fftcaff_shift_reader *caf)
{
  return caf->data.len;
}

void
fftcaff_shift_reader_run (fftcaff_shift_reader *caf, DtypeDatasource *ofp, size_t n)
{
  ns_log (DEBUG, "Running FFTCAFF\n");

  fftcaff_run (&caf->caf);
  fftcaff_fout_write_out (&caf->caf, caf->fout);
  fftcaff_writer_write_out (n, ofp, caf->writer, &caf->caf);
  shift_buf_del_front (&caf->data, n);
  caf->total_samples += n;

  ns_log (
      DEBUG,
      "Total samples processed by FFTCAFF: %zu\n",
      caf->total_samples);
}

void
fftcaff_shift_reader_free (fftcaff_shift_reader caf)
{
  fftcaff_free (&caf.caf);
  fftcaff_writer_free (caf.writer);
}
