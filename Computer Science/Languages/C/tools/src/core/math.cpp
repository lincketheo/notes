#include "core/math.h"

#include "core/errors.h"
#include "core/macros.h"
#include "core/testing.h"

#include <fftw3.h>
#include <float.h>
#include <limits.h>
#include <math.h>

#ifndef NOMP
#include <omp.h>
#endif

int
ttok (float fs, float t)
{
  return floorf (fs * t);
}

float
ktot (float fs, size_t k)
{
  return (float)k / fs;
}

intervals
fftf_to_bin (intervalf fint, float fs, size_t len)
{
  ASSERT (intervalf_valid (fint));

  return (intervals){
    .s0 = freqtobin (fint.v0, fs, len),
    .s1 = freqtobin (fint.vf, fs, len),
  };
}

void
fftf_zero_frange (_Complex float *dest, intervalf f, float fs, size_t len)
{
  zerocf_range (dest, fftf_to_bin (f, fs, len));
}

size_t
next_2_power (size_t n)
{
  if (n == 0)
    return 1;

  n--;
  n |= n >> 1;
  n |= n >> 2;
  n |= n >> 4;
  n |= n >> 8;
  n |= n >> 16;

  if (sizeof (size_t) > 4)
    {
      n |= n >> 32;
    }

  n++;

  return n;
}

TEST (nearest_power_of_two)
{
  TEST_EQUAL (next_2_power (0), 1LU, "%zu");
  TEST_EQUAL (next_2_power (1), 1LU, "%zu");
  TEST_EQUAL (next_2_power (2), 2LU, "%zu");
  TEST_EQUAL (next_2_power (3), 4LU, "%zu");
  TEST_EQUAL (next_2_power (11), 16LU, "%zu");
}

size_t
dblbuf_next_2_power (size_t cur_cap, size_t new_cap)
{
  size_t twop = 1;

  while (twop * cur_cap <= new_cap)
    {
      twop *= 2;
    }

  return twop;
}

TEST (dblbuf_next_2_power)
{
  TEST_EQUAL (dblbuf_next_2_power (1, 11), 16lu, "%lu");
  TEST_EQUAL (dblbuf_next_2_power (2, 11), 8lu, "%lu");
  TEST_EQUAL (dblbuf_next_2_power (3, 11), 4lu, "%lu");
  TEST_EQUAL (dblbuf_next_2_power (2, 16), 16lu, "%lu");
  TEST_EQUAL (dblbuf_next_2_power (3, 16), 8lu, "%lu");
  TEST_EQUAL (dblbuf_next_2_power (17, 201), 16lu, "%lu");
}

int
is_power_of_2 (size_t n)
{
  return n && !(n & (n - 1));
}

TEST (is_power_of_2)
{
  TEST_EQUAL (is_power_of_2 (0), 0, "%d");
  TEST_EQUAL (is_power_of_2 (1), 1, "%d");
  TEST_EQUAL (is_power_of_2 (2), 1, "%d");
  TEST_EQUAL (is_power_of_2 (4), 1, "%d");
  TEST_EQUAL (is_power_of_2 (8), 1, "%d");
  TEST_EQUAL (is_power_of_2 (1048576), 1, "%d");
  TEST_EQUAL (is_power_of_2 (9), 0, "%d");
  TEST_EQUAL (is_power_of_2 (11), 0, "%d");
  TEST_EQUAL (is_power_of_2 (1231241), 0, "%d");
}

float
cf64_power_to_f32_std (float power)
{
  return sqrtf (power / 2.0f);
}

float
cf128_power_to_f64_std (double power)
{
  return sqrt (power / 2.0);
}

float
signal_powerf (const _Complex float *data, size_t len)
{
  ASSERT (data);
  ASSERT (len > 0);

  // Kahan summation
  float ret = 0.0f;
  float correction = 0.0f;
  float mult = 1 / (float)len;

  for (size_t i = 0; i < len; ++i)
    {
      float y = modulus_sqf (data[i]) * mult - correction;
      float t = ret + y;
      correction = (t - ret) - y;
      ret = t;
    }

  return ret;
}

double
signal_power (const _Complex double *data, size_t len)
{
  ASSERT (data);
  ASSERT (len > 0);

  // Kahan summation
  double ret = 0.0f;
  double correction = 0.0f;
  double mult = 1 / (double)len;

#ifndef NOMP
#pragma omp parallel for
#endif
  for (size_t i = 0; i < len; ++i)
    {
      double y = modulus_sq (data[i]) * mult - correction;
      double t = ret + y;
      correction = (t - ret) - y;
      ret = t;
    }

  return ret;
}

float
noise_power_from_dataf (
    const _Complex float *data_noise,
    const _Complex float *data_no_noise,
    size_t len)
{
  ASSERT (data_noise);
  ASSERT (data_no_noise);

  // Kahan summation
  float ret = 0.0f;
  float correction = 0.0f;

#ifndef NOMP
#pragma omp parallel
#endif
  {
    float thread_ret = 0.0f;
    float thread_correction = 0.0f;

#ifndef NOMP
#pragma omp for
#endif
    for (size_t i = 0; i < len; ++i)
      {
        float diff = modulus_sqf (data_noise[i] - data_no_noise[i]);
        float y = diff - thread_correction;
        float t = thread_ret + y;
        thread_correction = (t - thread_ret) - y;
        thread_ret = t;
      }

#ifndef NOMP
#pragma omp critical
#endif
    {
      float y = thread_ret - correction;
      float t = ret + y;
      correction = (t - ret) - y;
      ret = t;
    }
  }

  return len > 0 ? ret / (float)len : 0.0f;
}

void
scale_to_power_inl (_Complex float *dest, size_t len, float power)
{
  ASSERT (dest);
  ASSERT (len > 0);
  ASSERT (power >= 0);

  float _power = signal_powerf (dest, len);
  ASSERT (_power >= 0);

  float scale = 0;
  if (power > 0 && _power > 0)
    {
      scale = sqrtf (power / _power);
    }

#ifndef NOMP
#pragma omp parallel for
#endif
  for (size_t i = 0; i < len; ++i)
    {
      dest[i] *= scale;
    }
}

void
gen_awgn_from_snrdb (
    _Complex float *dest,
    size_t len,
    float sig_power,
    float snrdb)
{
  float power = sqrtf (snrdb_to_awgn_power (snrdb, sig_power));

#ifdef RESCALE_POWERS
  gaussf_vec ((float *)dest, 2 * len, 1, 0);
  scale_to_power_inl (dest, len, power);
#else
  gaussf_vec ((float *)dest, 2 * len, cf128_power_to_f64_std (power), 0);
#endif
}

void
gen_awgn_from_power (_Complex float *dest, size_t len, float noise_power)
{
#ifdef RESCALE_POWERS
  gaussf_vec ((float *)dest, 2 * len, 1, 0);
  scale_to_power_inl (dest, len, noise_power);
#else
  gaussf_vec ((float *)dest, 2 * len, cf64_power_to_f32_std (noise_power), 0);
#endif
}

static inline intervalf
fs_interval (float fs)
{
  return (intervalf){
    .v0 = -fs / 2,
    .vf = fs / 2,
  };
}

size_t
freqtobin (float f, float fs, size_t len)
{
  ASSERT (intervalf_f_in_exc (fs_interval (fs), f));
  return f * len / fs;
}

float
bintofreq (long bin, float fs, size_t fft_len)
{
  ASSERT (bin < (long)fft_len);
  ASSERT (bin > -(long)fft_len);
  return (float)bin * fs / (float)fft_len;
}

void
fftwf_dft_1d_one_time (
    size_t len,
    _Complex float *src,
    _Complex float *dest,
    int dir,
    int type)
{
  fftwf_plan match_fplan = fftwf_plan_dft_1d (
      len,
      (fftwf_complex *)src,
      (fftwf_complex *)dest,
      dir,
      type);

  fftwf_execute (match_fplan);
  fftwf_destroy_plan (match_fplan);
}

void
cf64_build_tuner_inl (
    _Complex float *dest,
    size_t len,
    float f_offset,
    float fs,
    _Complex float c0)
{
  float pinc = -2.0f * M_PI * f_offset / fs;

#ifndef NOMP
#pragma omp parallel for
#endif
  for (size_t k = 0; k < len; ++k)
    {
      dest[k] = cexpf (I * k * pinc) * c0;
    }
}

_Complex float
cf64_tune_inl (
    _Complex float *dest,
    size_t len,
    float f_offset,
    float fs,
    _Complex float c0)
{
  float pinc = -2.0f * M_PI * f_offset / fs;

#ifndef NOMP
#pragma omp parallel for
#endif
  for (size_t k = 0; k < len; ++k)
    {
      dest[k] *= cexpf (I * k * pinc) * c0;
    }

  return cexpf (I * len * pinc) * c0;
}

static void
linear_signal_ramp_up (
    _Complex float *sig,
    size_t k0,
    size_t len,
    size_t rkf)
{
  ASSERT (sig);

  // Point slope
  float y0 = 0.0f;
  float y1 = 1.0f;
  float x0 = 0.0f;
  float x1 = (float)rkf;
  float m = (y1 - y0) / (x1 - x0);

  for (size_t k = k0; (k - k0) < len && k < rkf; ++k)
    {
      sig[k - k0] *= (m * ((float)k - x0) + y0);
    }
}

const char *
ramp_up_to_str (ramp_up r)
{
  switch (r)
    {
    case RU_LINEAR:
      return "LINEAR";
    case RU_HAMMING:
      return "RU_HAMMING";
    case RU_QUADRATIC:
      return "RU_QUADRATIC";
    default:
      unreachable ();
    }
}

// Ramps up signal (which is a slice from [k0, kf) (len == kf - k0) and rkf is
// the final sample to ramp up on
void
signal_ramp_up (
    _Complex float *sig,
    size_t k0,
    size_t len,
    size_t rkf,
    ramp_up type)
{
  switch (type)
    {
    case RU_LINEAR:
      linear_signal_ramp_up (sig, k0, len, rkf);
      break;
    default:
      unreachable ();
    }
}

static void
linear_signal_ramp_down (
    _Complex float *sig,
    size_t k0,
    size_t len,
    size_t rk0,
    size_t klen)
{
  ASSERT (sig);

  if (klen < rk0)
    return;

  // Point slope
  float y0 = 1.0f;
  float y1 = 0.0f;
  float x0 = (float)rk0;
  float x1 = (float)klen;
  float m = (y1 - y0) / (x1 - x0);

  for (size_t k = MAX (rk0, k0); (k - k0) < len; ++k)
    {
      sig[k - k0] *= (m * ((float)k - x0) + y0);
    }
}

// Ramps down signal (which is a slice from [k0, kf) (len == kf - k0) and rk0
// is the first sample to ramp down on
void
signal_ramp_down (
    _Complex float *sig,
    size_t k0,
    size_t len,
    size_t rk0,
    size_t klen,
    ramp_up type)
{
  switch (type)
    {
    case RU_LINEAR:
      linear_signal_ramp_down (sig, k0, len, rk0, klen);
      break;
    default:
      unreachable ();
    }
}

void
init_tuner_emat (
    _Complex float *dest,
    size_t match_len,
    frange f,
    float fs)
{
  ASSERT (frange_valid (f));

  size_t num_freqs = frange_size (f);

#ifndef NOMP
#pragma omp parallel for
#endif
  for (size_t i = 0; i < num_freqs; ++i)
    {
      _Complex float *row = &dest[i * match_len];
      float fi = f.f0 + i * f.df;
      cf64_build_tuner_inl (row, match_len, fi, fs, 1.0);
    }
}

float
snrdb_to_awgn_power (float snrdb, float sig_pwr)
{
  return sig_pwr / (powf (10, snrdb / 10));
}

siggen_sum_result_k
siggen_sum_sigs_k (
    _Complex float *dest,
    size_t dlen,
    const pargs *ps,
    size_t pslen,
    const krange k,
    const float fs)
{
  siggen_sum_result_k ret;

  size_t tofill = k.isinf ? dlen : MIN (dlen, k.kf - k.k0);

  for (size_t _k = 0; _k < tofill; ++_k)
    {
      dest[_k] = 0;
      for (int s = 0; s < pslen; ++s)
        {
          dest[_k] += pargs_evalk (ps[s], k.k0 + _k, fs);
        }
    }

  ret.filled = tofill;
  ret.next = (krange){
    .k0 = k.k0 + (int)tofill,
    .kf = k.kf,
    .isinf = k.isinf,
  };

  return ret;
}

TEST (siggen_sum_result_k)
{
  siggen_sum_result_k kres;

  _Complex float out[200];

  pargs ps[] = { (pargs){
                     .a = 5,
                     .p0 = 0,
                     .f = 100,
                 },
                 (pargs){
                     .a = 1,
                     .p0 = 1,
                     .f = 100,
                 },
                 (pargs){
                     .a = 0.1,
                     .p0 = 0,
                     .f = 1,
                 },
                 (pargs){
                     .a = 100,
                     .p0 = 0,
                     .f = 1,
                 } };

  // Finite
  kres = siggen_sum_sigs_k (out, 200, ps, 4,
                            (krange){ .k0 = 0, .kf = 201, .isinf = 0 }, 1);
  TEST_EQUAL (kres.filled, 200lu, "%zu");
  TEST_EQUAL (kres.next.k0, 200, "%d");
  TEST_EQUAL (kres.next.kf, 201, "%d");
  TEST_EQUAL (kres.next.isinf, 0, "%d");

  kres = siggen_sum_sigs_k (out, 200, ps, 4, kres.next, 1);
  TEST_EQUAL (kres.filled, 1lu, "%zu");
  TEST_EQUAL (kres.next.k0, 201, "%d");
  TEST_EQUAL (kres.next.kf, 201, "%d");
  TEST_EQUAL (kres.next.isinf, 0, "%d");

  kres = siggen_sum_sigs_k (out, 200, ps, 4, kres.next, 1);
  TEST_EQUAL (kres.filled, 0lu, "%zu");
  TEST_EQUAL (kres.next.k0, 201, "%d");
  TEST_EQUAL (kres.next.kf, 201, "%d");
  TEST_EQUAL (kres.next.isinf, 0, "%d");

  // Infinite
  kres = siggen_sum_sigs_k (out, 200, ps, 4, (krange){ .k0 = 21, .isinf = 1 },
                            1);
  TEST_EQUAL (kres.filled, 200lu, "%zu");
  TEST_EQUAL (kres.next.k0, 221, "%d");
  TEST_EQUAL (kres.next.isinf, 1, "%d");

  kres = siggen_sum_sigs_k (out, 200, ps, 4, kres.next, 1);
  TEST_EQUAL (kres.filled, 200lu, "%zu");
  TEST_EQUAL (kres.next.k0, 421, "%d");
  TEST_EQUAL (kres.next.isinf, 1, "%d");

  kres = siggen_sum_sigs_k (out, 200, ps, 4, kres.next, 1);
  TEST_EQUAL (kres.filled, 200lu, "%zu");
  TEST_EQUAL (kres.next.k0, 621, "%d");
  TEST_EQUAL (kres.next.isinf, 1, "%d");
}

void
cf64_rot_row_had (
    _Complex float *dest,
    const _Complex float *left,
    const _Complex float *right,
    size_t len,
    size_t rotnum)
{
  ASSERT (dest);
  ASSERT (left);
  ASSERT (right);
  ASSERT (len > 0);
  ASSERT (rotnum > 0);
  ASSERT (is_power_of_2 (len)); // a % len == a & (len - 1)

#ifndef NOMP
#pragma omp parallel for
#endif
  for (int i = 0; i < rotnum; ++i)
    {
      _Complex float *row = &dest[i * len];

      for (int j = 0; j < len; ++j)
        {
          row[j] = left[i] * right[(len + j - rotnum) & (len - 1)];
        }
    }
}

void
cf64_row_had (
    _Complex float *dest,
    const _Complex float *left,
    const _Complex float *right,
    size_t len)
{
  ASSERT (dest);
  ASSERT (left);
  ASSERT (right);
  ASSERT (len > 0);

#ifndef NOMP
#pragma omp parallel for
#endif
  for (int i = 0; i < len; ++i)
    {
      dest[i] = left[i] * right[i];
    }
}

void
cf64_mat_vec_row_had (
    _Complex float *dest,
    const _Complex float *left_mat,
    const _Complex float *right_vec,
    size_t rows,
    size_t cols)
{
  ASSERT (dest);
  ASSERT (left_mat);
  ASSERT (right_vec);
  ASSERT (rows > 0);
  ASSERT (cols > 0);

#ifndef NOMP
#pragma omp parallel for
#endif
  for (int i = 0; i < rows; ++i)
    {
      _Complex float *row = &dest[i * cols];
      const _Complex float *left_row = &left_mat[i * cols];
      for (int j = 0; j < cols; ++j)
        {
          row[j] = left_row[j] * right_vec[j];
        }
    }
}

void
cf64_row_had_right_shift (
    _Complex float *dest,
    const _Complex float *left,
    const _Complex float *right,
    size_t len,
    long right_start)
{
  ASSERT (len <= LONG_MAX);
  ASSERT (right_start > -(long)len);
  ASSERT (right_start < (long)len);
  ASSERT (dest);
  ASSERT (left);
  ASSERT (right);
  ASSERT (len > 0);
  ASSERT (is_power_of_2 (len)); // a % len == a & (len - 1)

  right_start += len;

#ifndef NOMP
#pragma omp parallel for
#endif
  for (int i = 0; i < len; ++i)
    {
      dest[i] = left[i] * right[(right_start + i) & (len - 1)];
    }
}

void
cf64_row_had_mult_vec (
    _Complex float *dest,
    const _Complex float *vec,
    size_t rows,
    size_t cols)
{
  ASSERT (dest);
  ASSERT (vec);
  ASSERT (rows > 0);
  ASSERT (cols > 0);

#ifndef NOMP
#pragma omp parallel for
#endif
  for (size_t i = 0; i < rows; ++i)
    {
      _Complex float *row = &dest[i * cols];
      for (size_t j = 0; j < cols; ++j)
        {
          row[j] *= vec[j];
        }
    }
}

void
cf64_matmul_mat_vec_cf64abs (
    float *col_dest,
    const _Complex float *mat,
    const _Complex float *col_vec,
    size_t rows,
    size_t cols)
{
  ASSERT (col_dest);
  ASSERT (mat);
  ASSERT (col_vec);
  ASSERT (rows > 0);
  ASSERT (cols > 0);

#ifndef NOMP
#pragma omp parallel for
#endif
  for (size_t i = 0; i < rows; ++i)
    {
      _Complex float sum = 0.0f + 0.0f * I;

      for (size_t j = 0; j < cols; ++j)
        {
          sum += mat[i * cols + j] * col_vec[j];
        }

      col_dest[i] = cabsf (sum);
    }
}

void
f64_vec_argmax (float *col, size_t len, float *dest, size_t *rdest)
{
  ASSERT (col);
  ASSERT (len > 0);

  float max = -FLT_MAX;
  ssize_t argmax = -1;

#ifndef NOMP
#pragma omp parallel
  {
    float lmax = -FLT_MAX;
    ssize_t largmax = -1;

#pragma omp for nowait
    for (size_t i = 0; i < len; ++i)
      {
        if (col[i] > lmax)
          {
            lmax = col[i];
            largmax = i;
          }
      }

#pragma omp critical
    {
      if (lmax > max)
        {
          max = lmax;
          argmax = largmax;
        }
    }
  }
#else
  for (size_t i = 0; i < len; ++i)
    {
      if (col[i] > max)
        {
          max = col[i];
          argmax = i;
        }
    }
#endif

  ASSERT (argmax >= 0 && (size_t)argmax < len);

  if (dest)
    *dest = max;
  if (rdest)
    *rdest = argmax;
}

void
cf64_mat_reverse_rows_inl (_Complex float *dest, size_t rows, size_t cols)
{
#ifndef NOMP
#pragma omp parallel for
#endif
  for (int i = 0; i < rows; ++i)
    {
      _Complex float *row = &dest[i * cols];
      reverse_inl (row, cols, sizeof *row);
    }
}

void
cf64_mat_conj_inl (_Complex float *dest, size_t rows, size_t cols)
{
#ifndef NOMP
#pragma omp parallel for
#endif
  for (int i = 0; i < rows; ++i)
    {
      _Complex float *row = &dest[i * cols];
      conjf_inl (row, cols);
    }
}

_Complex float
cf64_vec_inner_prod (
    const _Complex float *left,
    const _Complex float *right,
    size_t len)
{
  _Complex float ret = 0.0f + 0.0f * I;

#ifndef NOMP
#pragma omp parallel for reduction(+ : ret)
#endif
  for (int i = 0; i < len; ++i)
    {
      ret += left[i] * conjf (right[i]);
    }
  return ret;
}

float
cf64_abs_sum (const _Complex float *arr, size_t len)
{
  float ret = 0.0f;

#ifndef NOMP
#pragma omp parallel for reduction(+ : ret)
#endif
  for (int i = 0; i < len; ++i)
    {
      ret += crealf (arr[i] * conjf (arr[i]));
    }
  return ret;
}

void
cf64_transpose_cf64_col_limit (
    _Complex float *dest,
    const _Complex float *src,
    size_t rows,
    size_t cols,
    size_t col_limit)
{
#ifndef NOMP
#pragma omp parallel for
#endif
  for (size_t i = 0; i < rows; ++i)
    {
      for (size_t j = 0; j < col_limit; ++j)
        {
          dest[j * rows + i] = src[i * cols + j];
        }
    }
}

void
cf64_transpose_f32_abs_sqrd_col_limit (
    float *dest,
    const _Complex float *src,
    size_t rows,
    size_t cols,
    size_t col_limit)
{
#ifndef NOMP
#pragma omp parallel for
#endif
  for (size_t i = 0; i < rows; ++i)
    {
      for (size_t j = 0; j < col_limit; ++j)
        {
          dest[j * rows + i] = modulus_sqf (src[i * cols + j]);
        }
    }
}

void
conjf_inl (_Complex float *data, size_t len)
{
#ifndef NOMP
#pragma omp parallel for
#endif
  for (int i = 0; i < len; ++i)
    data[i] = crealf (data[i]) - cimagf (data[i]) * I;
}

void
conj_inl (_Complex double *data, size_t len)
{
#ifndef NOMP
#pragma omp parallel for
#endif
  for (int i = 0; i < len; ++i)
    data[i] = creal (data[i]) - cimag (data[i]) * I;
}

#define _reverse_inl(_tdata, len, type) \
  do                                    \
    {                                   \
      type *tdata = (type *)_tdata;     \
      int l = 0;                        \
      int r = len - 1;                  \
      while (l < r)                     \
        {                               \
          type temp = tdata[l];         \
          tdata[l] = tdata[r];          \
          tdata[r] = temp;              \
          l++;                          \
          r--;                          \
        }                               \
    }                                   \
  while (0)

void
reverse_inl (void *data, size_t len, size_t esize)
{
  switch (esize)
    {
    case 1:
      _reverse_inl (data, len, uint8_t);
      break;
    case 2:
      _reverse_inl (data, len, uint16_t);
      break;
    case 4:
      _reverse_inl (data, len, uint32_t);
      break;
    case 8:
      _reverse_inl (data, len, uint64_t);
      break;
    default:
      fprintf (stderr, "Unsupported element size: %zu", esize);
      break;
    }
}

// Uses statement expressions
#define _test_reverse_inline_type(type, fmt)              \
  {                                                       \
    type data_odd[] = { 5, 6, 7, 8, 9 };                  \
    type expected_odd[] = { 9, 8, 7, 6, 5 };              \
                                                          \
    type data_even[] = { 5, 6, 7, 8, 9, 10 };             \
    type expected_even[] = { 10, 9, 8, 7, 6, 5 };         \
                                                          \
    reverse_inl (data_odd, 5, sizeof *data_odd);          \
    reverse_inl (data_even, 6, sizeof *data_even);        \
                                                          \
    for (int i = 0; i < 5; ++i)                           \
      {                                                   \
        TEST_EQUAL (data_odd[i], expected_odd[i], fmt);   \
      }                                                   \
    for (int i = 0; i < 6; ++i)                           \
      {                                                   \
        TEST_EQUAL (data_even[i], expected_even[i], fmt); \
      }                                                   \
  }

TEST (reverse_inl)
{
  _test_reverse_inline_type (uint8_t, "%d");
  _test_reverse_inline_type (uint16_t, "%d");
  _test_reverse_inline_type (uint32_t, "%d");
  _test_reverse_inline_type (uint64_t, "%zu");
}

void
roll_inline (void *data, int64_t rollby, size_t len, size_t esize)
{
  uint8_t *bdata = (uint8_t *)data;
  size_t _rollby = rollby % len;
  reverse_inl (data, len, esize);
  reverse_inl (data, _rollby, esize);
  reverse_inl (bdata + esize * _rollby, len - _rollby, esize);
}

// Uses statement expressions
#define _test_roll_inl_with_type(type, fmt)               \
  {                                                       \
    type data_odd[] = { 5, 6, 7, 8, 9 };                  \
    type expected_odd[] = { 7, 8, 9, 5, 6 };              \
                                                          \
    type data_even[] = { 5, 6, 7, 8, 9, 10 };             \
    type expected_even[] = { 8, 9, 10, 5, 6, 7 };         \
                                                          \
    roll_inline (data_odd, 3, 5, sizeof *data_odd);       \
    roll_inline (data_even, 3, 6, sizeof *data_even);     \
                                                          \
    for (int i = 0; i < 5; ++i)                           \
      {                                                   \
        TEST_EQUAL (data_odd[i], expected_odd[i], fmt);   \
      }                                                   \
    for (int i = 0; i < 6; ++i)                           \
      {                                                   \
        TEST_EQUAL (data_even[i], expected_even[i], fmt); \
      }                                                   \
  }

TEST (roll_inline)
{
  _test_roll_inl_with_type (uint8_t, "%d");
  _test_roll_inl_with_type (uint16_t, "%d");
  _test_roll_inl_with_type (uint32_t, "%d");
  _test_roll_inl_with_type (uint64_t, "%zu");
}

void
cf64_add_inl (_Complex float *dest, _Complex float *right, size_t len)
{
#ifndef NOMP
#pragma omp parallel for
#endif
  for (int i = 0; i < len; ++i)
    {
      dest[i] += right[i];
    }
}

void
zerocf_range (_Complex float *dest, intervals s)
{
  ASSERT (s.s1 >= s.s0);
  if (s.s1 > s.s0)
    memset (&dest[s.s0], 0, (s.s1 - s.s0) * sizeof *dest);
}

float
fvec_add (const float *dat, size_t len)
{
  ASSERT (dat);
  float ret = 0;

#ifndef NOMP
#pragma omp parallel for
#endif
  for (int i = 0; i < len; ++i)
    {
      ret += dat[i];
    }
  return ret;
}

float
fvec_mean (const float *dat, size_t len)
{
  ASSERT (dat);
  float ret = 0;
  const float invlen = 1.0f / (float)len;

#ifndef NOMP
#pragma omp parallel for
#endif
  for (int i = 0; i < len; ++i)
    {
      ret += dat[i] * invlen;
    }
  return ret;
}

float
fvec_mean_fsqrd (const float *dat, size_t len)
{
  ASSERT (dat);
  float ret = 0;
  float invlen = 1.0f / (float)len;

#ifndef NOMP
#pragma omp parallel for
#endif
  for (int i = 0; i < len; ++i)
    {
      ret += dat[i] * dat[i] * invlen;
    }
  return ret;
}

#ifdef GAUSS_DETERMINISTIC
void
gauss_vec (double *dest, size_t len, double std, double mean)
{
  ASSERT (len % 2 == 0); // Ensure even length for paired generation
  double mult = 1.0 / (double)RAND_MAX;

  for (int i = 0; i < len; i += 2)
    {
      double S, v1, v2, u1, u2;

      do
        {
          u1 = (double)rand () * mult;
          u2 = (double)rand () * mult;
          v1 = 2 * u1 - 1;
          v2 = 2 * u2 - 1;
          S = v1 * v1 + v2 * v2;
        }
      while (S >= 1);

      double v3 = std * sqrt ((-2.0 * log (S)) / S);

      dest[i] = v1 * v3 + mean;
      dest[i + 1] = v2 * v3 + mean;
    }
}

void
gaussf_vec (float *dest, size_t len, float std, float mean)
{
  ASSERT (len % 2 == 0); // Ensure even length for paired generation
  float mult = 1.0f / (float)RAND_MAX;

  for (int i = 0; i < len; i += 2)
    {
      float S, v1, v2, u1, u2;

      do
        {
          u1 = (float)rand () * mult;
          u2 = (float)rand () * mult;
          v1 = 2.0f * u1 - 1.0f;
          v2 = 2.0f * u2 - 1.0f;
          S = v1 * v1 + v2 * v2;
        }
      while (S >= 1.0f || S == 0.0f);

      float v3 = std * sqrtf ((-2.0f * logf (S)) / S);

      dest[i] = v1 * v3 + mean;
      dest[i + 1] = v2 * v3 + mean;
    }
}
#else
void
gauss_vec (double *dest, size_t len, double std, double mean)
{
  ASSERT (len % 2 == 0);
  double mult = 1.0 / (double)RAND_MAX;

  unsigned int base_seed = (unsigned int)rand ();

#pragma omp parallel
  {
    int thread_num = omp_get_thread_num ();
    unsigned int seed = base_seed + (unsigned int)thread_num;

#pragma omp for
    for (int i = 0; i < (int)len; i += 2)
      {
        double S, v1, v2, u1, u2;
        do
          {
            u1 = (double)rand_r (&seed) * mult;
            u2 = (double)rand_r (&seed) * mult;
            v1 = 2.0 * u1 - 1.0;
            v2 = 2.0 * u2 - 1.0;
            S = v1 * v1 + v2 * v2;
          }
        while (S >= 1.0 || S == 0.0);

        double v3 = std * sqrt ((-2.0 * log (S)) / S);
        dest[i] = v1 * v3 + mean;
        dest[i + 1] = v2 * v3 + mean;
      }
  }
}

void
gaussf_vec (float *dest, size_t len, float std, float mean)
{
  ASSERT (len % 2 == 0);
  float mult = 1.0f / (float)RAND_MAX;

  unsigned int base_seed = (unsigned int)rand ();

#pragma omp parallel
  {
    int thread_num = omp_get_thread_num ();
    unsigned int seed = base_seed + (unsigned int)thread_num;

#pragma omp for
    for (int i = 0; i < (int)len; i += 2)
      {
        float S, v1, v2, u1, u2;
        do
          {
            u1 = (float)rand_r (&seed) * mult;
            u2 = (float)rand_r (&seed) * mult;
            v1 = 2.0f * u1 - 1.0f;
            v2 = 2.0f * u2 - 1.0f;
            S = v1 * v1 + v2 * v2;
          }
        while (S >= 1.0f || S == 0.0f);

        float v3 = std * sqrtf ((-2.0f * logf (S)) / S);
        dest[i] = v1 * v3 + mean;
        dest[i + 1] = v2 * v3 + mean;
      }
  }
}
#endif

void
stats_fprint_friendly (FILE *ofp, stats s)
{
  ASSERT (ofp);
  fprintf (ofp, "Mean: %.2f\n", s.mean);
  fprintf (ofp, "Standard Deviation: %.2f\n", s.std);
  fprintf (ofp, "Max: %.2f (Index: %zu)\n", s.max, s.argmax);
  fprintf (ofp, "Min: %.2f (Index: %zu)\n", s.min, s.argmin);
}

void
stats_fprint_csv_title (FILE *ofp, const char *prefix, int end_comma)
{
  fprintf (ofp, "%s_mean,%s_std,%s_max,%s_min,%s_argmax,%s_argmin", prefix,
           prefix, prefix, prefix, prefix, prefix);
  if (end_comma)
    fprintf (ofp, ",");
}

void
stats_fprint_csv_row (FILE *ofp, stats s, int end_comma)
{
  fprintf (ofp, "%f,%f,%f,%f,%zu,%zu", s.mean, s.std, s.max, s.min, s.argmax,
           s.argmin);
  if (end_comma)
    fprintf (ofp, ",");
}

stats
stats_calculate (const float *data, size_t len)
{
  ASSERT (data);
  ASSERT (len > 0);

  stats result = { 0 };

  if (len == 0)
    {
      result.mean = NAN;
      result.std = NAN;
      result.max = NAN;
      result.min = NAN;
      result.argmax = 0;
      result.argmin = 0;
      return result;
    }

  double mean = 0.0f;
  double sum = 0.0f;
  double sum_correction = 0.0f; // For Kahan summation
  double mean_diff = 0.0f;      // For incremental variance
  double variance = 0.0f;
  double variance_correction = 0.0f; // For Kahan summation in variance

  double max = data[0];
  double min = data[0];
  size_t argmax = 0;
  size_t argmin = 0;

  for (size_t i = 0; i < len; i++)
    {
      double value = data[i];

      // Kahan summation for sum
      double y = value - sum_correction;
      double t = sum + y;
      sum_correction = (t - sum) - y;
      sum = t;

      // Update min, max, argmin, argmax
      if (value > max)
        {
          max = value;
          argmax = i;
        }
      if (value < min)
        {
          min = value;
          argmin = i;
        }
    }

  mean = sum / len;

  // Second pass: compute variance with division inside the loop
  for (size_t i = 0; i < len; i++)
    {
      double value = data[i];
      double diff = value - mean;
      double squared_diff_over_len = (diff * diff) / len;

      // Kahan summation for variance
      double y = squared_diff_over_len - variance_correction;
      double t = variance + y;
      variance_correction = (t - variance) - y;
      variance = t;
    }

  result.mean = mean;
  result.std = sqrt (variance);
  result.max = max;
  result.min = min;
  result.argmax = argmax;
  result.argmin = argmin;

  return result;
}

stats
stats_calculate_abs_cf (const _Complex float *data, size_t len)
{
  ASSERT (data);
  ASSERT (len > 0);

  stats result = { 0 };

  double mean = 0.0f;
  double sum = 0.0f;
  double sum_correction = 0.0f; // For Kahan summation
  double mean_diff = 0.0f;      // For incremental variance
  double variance = 0.0f;
  double variance_correction = 0.0f; // For Kahan summation in variance

  double max = cabsf (data[0]);
  double min = cabsf (data[0]);
  size_t argmax = 0;
  size_t argmin = 0;

  for (size_t i = 0; i < len; i++)
    {
      double value = cabsf (data[i]);

      // Kahan summation for sum
      double y = value - sum_correction;
      double t = sum + y;
      sum_correction = (t - sum) - y;
      sum = t;

      // Update min, max, argmin, argmax
      if (value > max)
        {
          max = value;
          argmax = i;
        }
      if (value < min)
        {
          min = value;
          argmin = i;
        }
    }

  mean = sum / len;

  // Second pass: compute variance with division inside the loop
  for (size_t i = 0; i < len; i++)
    {
      double value = cabsf (data[i]);
      double diff = value - mean;
      double squared_diff_over_len = (diff * diff) / len;

      // Kahan summation for variance
      double y = squared_diff_over_len - variance_correction;
      double t = variance + y;
      variance_correction = (t - variance) - y;
      variance = t;
    }

  result.mean = mean;
  result.std = sqrt (variance);
  result.max = max;
  result.min = min;
  result.argmax = argmax;
  result.argmin = argmin;

  return result;
}
