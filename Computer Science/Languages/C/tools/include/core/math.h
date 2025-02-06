#pragma once

#include "core/data_structures.h"

#include <stdio.h>

///////////////////////////////////////////////////////
///////// Math

#define modulus_sq(z) (creal (z) * creal (z) + cimag (z) * cimag (z))

#define modulus_sqf(z) (float)(crealf (z) * crealf (z) + cimagf (z) * cimagf (z))

#define modulus(z) (sqrt (modulus_sq (z)))

#define modulusf(z) (sqrtf (modulus_sqf (z)))

#define cf_unwrap(c) creal (c), cimag (c)

int ttok(float fs, float t);

float ktot(float fs, size_t k);

intervals fftf_to_bin (intervalf fint, float fs, size_t len);

void fftf_zero_frange (_Complex float *dest, intervalf fint, float fs, size_t len);

size_t next_2_power (size_t n);

size_t dblbuf_next_2_power (size_t cur, size_t newlen);

int is_power_of_2 (size_t s);

float cf64_power_to_f32_std (float power);

float signal_powerf (const _Complex float *data, size_t len);

// Estimates noise power from two arrays: data summed with noise and data without noise
float noise_power_from_dataf (
    const _Complex float *data_noise,
    const _Complex float *data_no_noise,
    size_t len);

double signal_power (const _Complex double *data, size_t len);

void scale_to_power_inl (_Complex float *dest, size_t len, float power);

void gen_awgn_from_snrdb (
    _Complex float *dest,
    size_t len,
    float sig_power,
    float snrdb);

void gen_awgn_from_power (
    _Complex float *dest,
    size_t len,
    float noise_power);

size_t freqtobin (float f, float fs, size_t len);

float bintofreq (long bin, float fs, size_t fft_len);

void fftwf_dft_1d_one_time (
    size_t len,
    _Complex float *src,
    _Complex float *dest,
    int dir,
    int type);

/**
 * Creates a 2d matrix (assumes dest is the correct size)
 * Each row is e^(i * (2pi * f * k/fs)) where k is the column number
 * and f is the frequency bin from f.f0 to f.f1
 *
 * rows: frange_size(f)
 * cols: slen
 */
void init_tuner_emat (_Complex float *dest, size_t slen, frange f, float fs);

/**
 * Builds a "tuner" (doesn't "tune", just overrides dest)
 * dest[k] = exp(-2pi * I * f/fs * k)
 */
void cf64_build_tuner_inl (
    _Complex float *dest,
    size_t len,
    float f_offset,
    float fs,
    _Complex float c0);

/**
 * Tunes a _Complex float array by f_offset with sample frequency fs
 * Starts tuner at phasor. Usually phasor should be 1, and if you have
 * multiple tunes (multiple buffers) with continuous phase,
 * you keep passing phasor to the function
 */
_Complex float cf64_tune_inl (
    _Complex float *dest,
    size_t len, float f_offset,
    float fs,
    _Complex float phasor);

float snrdb_to_awgn_power (float snrdb, float sig_pwr);

typedef enum
{
  RU_LINEAR,
  RU_HAMMING,
  RU_QUADRATIC,
} ramp_up;

const char *ramp_up_to_str (ramp_up r);

// Ramps up signal (which is a slice from [k0, kf) (len == kf - k0) and rkf is
// the final sample to ramp up on
void signal_ramp_up (
    _Complex float *sig,
    size_t k0,
    size_t len,
    size_t rkf,
    ramp_up type);

// Ramps down signal (which is a slice from [k0, kf) (len == kf - k0) and rk0
// is the first sample to ramp down on. Also need total signal length (klen)
void signal_ramp_down (
    _Complex float *sig,
    size_t k0,
    size_t len,
    size_t rk0,
    size_t klen,
    ramp_up type);

typedef struct
{
  krange next;
  size_t filled;
} siggen_sum_result_k;

/**
 * @brief Generates and sums a set of _Complex waves within a specified k range.
 *
 * Fills the `dest` buffer with the sum of _Complex waves generated based on the
 *
 * @param dest Pointer to the destination buffer where the generated _Complex
 * wave sum will be stored.
 * @param dlen Length of the destination buffer in samples.
 * @param ps Array of `pargs` structures, each specifying the amplitude, phase,
 * and frequency for a wave.
 * @param pslen Length of the `ps` array, indicating the number of wave
 * components to generate.
 * @param k Frequency range constraint (as `krange`) to limit the generated
 * signal frequencies.
 * @param fs Sampling frequency of the signal, used to compute each wave’s
 * sampling rate.
 * @return A `siggen_sum_result_k` struct containing:
 *         - `next`: The next `krange` constraint for further processing.
 *         - `filled`: The number of samples filled in the `dest` buffer.
 *
 * @note Each wave in `ps` is generated as a _Complex float and added to `dest`,
 *       producing a summed signal of all specified waves within the frequency
 * range defined by `k`. Ensure `dest` has sufficient space to store `dlen`
 * samples.
 */
siggen_sum_result_k siggen_sum_sigs_k (
    _Complex float *dest,
    size_t dlen,
    const pargs *ps,
    size_t pslen,
    const krange k,
    const float fs);

///////////////////////////////////////////////////////
///////// Matrix Math

// left: vector (len)
// right: vector (len)
// dest: matrix (rotnum x len)
//
// dest[i] = left (o) roll(right, i)
void cf64_rot_row_had (
    _Complex float *dest,
    const _Complex float *left,
    const _Complex float *right,
    size_t len,
    size_t rotnum);

// left: vector (len)
// right: vector (len)
// dest: vector (len)
//
// dest = left (o) right
void cf64_row_had (
    _Complex float *dest,
    const _Complex float *left,
    const _Complex float *right,
    size_t len);

// left: matrix (rows x cols)
// right: vector (cols)
// dest: vector (rows x cols)
//
// dest[i] = left[i] (o) right
void cf64_mat_vec_row_had (
    _Complex float *dest,
    const _Complex float *left_mat,
    const _Complex float *right_vec,
    size_t rows,
    size_t cols);

// left: vector (len)
// right: vector (len)
// dest: vector (len)
//
// dest = left (o) roll(right, right_start)
void cf64_row_had_right_shift (
    _Complex float *dest,
    const _Complex float *left,
    const _Complex float *right,
    size_t len,
    long right_start);

// dest: matrix (rows x cols)
// vec: vector (cols)
//
// dest[i] = dest[i] (o) vec
void cf64_row_had_mult_vec (
    _Complex float *dest,
    const _Complex float *vec,
    size_t rows,
    size_t cols);

// col_dest: vector (rows)
// mat: matrix (rows x cols)
// col_vec: vector (cols)
//
// col_dest = cabsf(mat x col_vec)
void cf64_matmul_mat_vec_cf64abs (
    float *col_dest,
    const _Complex float *mat,
    const _Complex float *col_vec,
    size_t rows,
    size_t cols);

void cf64_mat_reverse_rows_inl (_Complex float *dest, size_t rows, size_t cols);

void cf64_mat_conj_inl (_Complex float *dest, size_t rows, size_t cols);

void f64_vec_argmax (float *col, size_t len, float *dest, size_t *rdest);

_Complex float cf64_vec_inner_prod (
    const _Complex float *left,
    const _Complex float *right,
    size_t len);

float cf64_abs_sum (const _Complex float *arr, size_t len);

void cf64_transpose_cf64_col_limit (
    _Complex float *dest,
    const _Complex float *src,
    size_t rows,
    size_t cols,
    size_t col_limit);

void cf64_transpose_f32_abs_sqrd_col_limit (
    float *dest,
    const _Complex float *src,
    size_t rows,
    size_t cols,
    size_t col_limit);

void conjf_inl (_Complex float *data, size_t len);

void conj_inl (_Complex double *data, size_t len);

void reverse_inl (void *data, size_t len, size_t esize);

void roll_inline (void *data, int64_t rollby, size_t len, size_t esize);

void cf64_add_inl (_Complex float *dest, _Complex float *right, size_t len);

void zerocf_range (_Complex float *dest, intervals s);

float fvec_add (const float *dat, size_t len);

float fvec_mean (const float *dat, size_t len);

float fvec_mean_fsqrd (const float *dat, size_t len);

///////////////////////////////////////////////////////
///////// Random

void gaussf_vec (float *dest, size_t len, float std, float mean);

void gauss_vec (double *dest, size_t len, double std, double mean);

///////////////////////////////////////////////////////
///////// Stats

typedef struct
{
  double mean;
  double std;
  double max;
  double min;
  size_t argmax;
  size_t argmin;
} stats;

void stats_fprint_friendly (FILE *ofp, stats s);

void stats_fprint_csv_title (FILE *ofp, const char *prefix, int end_comma);

void stats_fprint_csv_row (FILE *ofp, stats s, int end_comma);

stats stats_calculate (const float *data, size_t len);

stats stats_calculate_abs_cf (const _Complex float *data, size_t len);
