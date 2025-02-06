#pragma once

#include "core/data_structures.h"

#include <stdio.h>

///////////////////////////////////////////////////////
///////// Math

/**
 * Computes the squared modulus of a complex number.
 * @param z Complex number.
 * @return Squared modulus of `z`.
 */
#define modulus_sq(z) (creal (z) * creal (z) + cimag (z) * cimag (z))

/**
 * Computes the squared modulus of a complex float.
 * @param z Complex float number.
 * @return Squared modulus of `z`.
 */
#define modulus_sqf(z) (float)(crealf (z) * crealf (z) + cimagf (z) * cimagf (z))

/**
 * Computes the modulus of a complex number.
 * @param z Complex number.
 * @return Modulus of `z`.
 */
#define modulus(z) (sqrt (modulus_sq (z)))

/**
 * Computes the modulus of a complex float.
 * @param z Complex float number.
 * @return Modulus of `z`.
 */
#define modulusf(z) (sqrtf (modulus_sqf (z)))

/**
 * Unwraps a complex number into its real and imaginary parts.
 * @param c Complex number.
 * @return Real and imaginary parts of `c`.
 */
#define cf_unwrap(c) creal (c), cimag (c)

/**
 * Converts a time value `t` to a sample index `k` using the sampling frequency `fs`.
 * @param fs Sampling frequency in Hz.
 * @param t Time in seconds.
 * @return Corresponding sample index `k`.
 */
int ttok (float fs, float t);

/**
 * Converts a sample index `k` to a time value `t` using the sampling frequency `fs`.
 * @param fs Sampling frequency in Hz.
 * @param k Sample index.
 * @return Corresponding time in seconds.
 */
float ktot (float fs, size_t k);

/**
 * Converts a frequency interval to an FFT bin interval.
 * @param fint Frequency interval in Hz.
 * @param fs Sampling frequency in Hz.
 * @param len FFT length.
 * @return Corresponding bin interval.
 */
intervals fftf_to_bin (
    intervalf fint,
    float fs,
    size_t len);

/**
 * Zeros out the bins of an FFT result corresponding to a specified frequency range.
 * @param dest FFT output buffer.
 * @param fint Frequency interval to zero out.
 * @param fs Sampling frequency in Hz.
 * @param len FFT length.
 */
void fftf_zero_frange (
    _Complex float *dest,
    intervalf fint,
    float fs,
    size_t len);

/**
 * Finds the next power of 2 greater than or equal to the input value.
 * @param n Input value.
 * @return Next power of 2.
 */
size_t next_2_power (size_t n);

/**
 * Finds the next power of 2 based on the current and required buffer lengths.
 * @param cur Current buffer length.
 * @param newlen New required buffer length.
 * @return Next power of 2.
 */
size_t dblbuf_next_2_power (
    size_t cur,
    size_t newlen);

/**
 * Checks if a number is a power of 2.
 * @param s Input number.
 * @return 1 if `s` is a power of 2, 0 otherwise.
 */
int is_power_of_2 (size_t s);

/**
 * Converts a double-precision power value to a single-precision standard form.
 * @param power Input power value.
 * @return Standardized single-precision power.
 */
float cf64_power_to_f32_std (float power);

/**
 * Calculates the power of a signal represented by complex float data.
 * @param data Signal data.
 * @param len Length of the signal data.
 * @return Calculated power of the signal.
 */
float signal_powerf (
    const _Complex float *data,
    size_t len);

/**
 * Estimates noise power from signal data with and without noise.
 * @param data_noise Signal data containing noise.
 * @param data_no_noise Signal data without noise.
 * @param len Length of the signal data.
 * @return Estimated noise power.
 */
float noise_power_from_dataf (
    const _Complex float *data_noise,
    const _Complex float *data_no_noise,
    size_t len);

/**
 * Calculates the power of a signal represented by complex double data.
 * @param data Signal data.
 * @param len Length of the signal data.
 * @return Calculated power of the signal.
 */
double signal_power (
    const _Complex double *data,
    size_t len);

/**
 * Scales a signal to a specified power in place.
 * @param dest Signal buffer to scale.
 * @param len Length of the buffer.
 * @param power Desired power value.
 */
void scale_to_power_inl (
    _Complex float *dest,
    size_t len,
    float power);

/**
 * Generates additive white Gaussian noise (AWGN) based on a given signal-to-noise ratio (SNR) in dB.
 * @param dest Destination buffer for the generated noise.
 * @param len Length of the buffer.
 * @param sig_power Signal power in the buffer.
 * @param snrdb Signal-to-noise ratio in dB.
 */
void gen_awgn_from_snrdb (
    _Complex float *dest,
    size_t len,
    float sig_power,
    float snrdb);

/**
 * Generates additive white Gaussian noise (AWGN) based on a given noise power.
 * @param dest Destination buffer for the generated noise.
 * @param len Length of the buffer.
 * @param noise_power Desired noise power.
 */
void gen_awgn_from_power (
    _Complex float *dest,
    size_t len,
    float noise_power);

/**
 * Converts a frequency to its corresponding FFT bin index.
 * @param f Frequency in Hz.
 * @param fs Sampling frequency in Hz.
 * @param len FFT length.
 * @return FFT bin index corresponding to the frequency.
 */
size_t freqtobin (
    float f,
    float fs,
    size_t len);

/**
 * Converts an FFT bin index to its corresponding frequency.
 * @param bin Bin index.
 * @param fs Sampling frequency in Hz.
 * @param fft_len FFT length.
 * @return Frequency in Hz corresponding to the bin index.
 */
float bintofreq (
    long bin,
    float fs,
    size_t fft_len);

/**
 * Performs a one-time FFT or IFFT operation on a 1D array.
 * @param len Length of the FFT.
 * @param src Source data array.
 * @param dest Destination data array for the result.
 * @param dir Direction of transformation (1 for FFT, -1 for IFFT).
 * @param type Implementation-dependent FFT type.
 */
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
    size_t len,
    float f_offset,
    float fs,
    _Complex float phasor);

// Converts snrdb and signal power to noise power
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

/**
 * Reverses the rows of a matrix in place.
 * @param dest Pointer to the matrix data.
 * @param rows Number of rows in the matrix.
 * @param cols Number of columns in the matrix.
 */
void cf64_mat_reverse_rows_inl (
    _Complex float *dest,
    size_t rows,
    size_t cols);

/**
 * Computes the complex conjugate of each element in a matrix.
 * @param dest Pointer to the matrix data.
 * @param rows Number of rows in the matrix.
 * @param cols Number of columns in the matrix.
 */
void cf64_mat_conj_inl (
    _Complex float *dest,
    size_t rows,
    size_t cols);

/**
 * Finds the index of the maximum value in a float array.
 * @param col Input array of floats.
 * @param len Length of the array.
 * @param dest Pointer to store the maximum value.
 * @param rdest Pointer to store the index of the maximum value.
 */
void f64_vec_argmax (
    float *col,
    size_t len,
    float *dest,
    size_t *rdest);

/**
 * Computes the inner product of two complex float vectors.
 * @param left First input vector.
 * @param right Second input vector.
 * @param len Length of the vectors.
 * @return Inner product as a complex float.
 */
_Complex float cf64_vec_inner_prod (
    const _Complex float *left,
    const _Complex float *right,
    size_t len);

/**
 * Calculates the sum of the absolute values of elements in a complex float array.
 * @param arr Input array of complex floats.
 * @param len Length of the array.
 * @return Sum of the absolute values.
 */
float cf64_abs_sum (
    const _Complex float *arr,
    size_t len);

/**
 * Transposes a complex float matrix with a column limit.
 * @param dest Destination matrix.
 * @param src Source matrix.
 * @param rows Number of rows in the source matrix.
 * @param cols Number of columns in the source matrix.
 * @param col_limit Maximum number of columns to transpose.
 */
void cf64_transpose_cf64_col_limit (
    _Complex float *dest,
    const _Complex float *src,
    size_t rows,
    size_t cols,
    size_t col_limit);

/**
 * Transposes a complex float matrix,
 * computing squared absolute values as floats, with a column limit.
 * @param dest Destination matrix for squared absolute values.
 * @param src Source complex float matrix.
 * @param rows Number of rows in the source matrix.
 * @param cols Number of columns in the source matrix.
 * @param col_limit Maximum number of columns to transpose.
 *
 * For example, if we have 10 columns and col_limit is 5, then it
 * only takes the first 5 values of each row to create the next columns
 */
void cf64_transpose_f32_abs_sqrd_col_limit (
    float *dest,
    const _Complex float *src,
    size_t rows,
    size_t cols,
    size_t col_limit);

/**
 * Computes the complex conjugate of each element in a complex float array.
 * @param data Input/output array of complex floats.
 * @param len Length of the array.
 */
void conjf_inl (
    _Complex float *data,
    size_t len);

/**
 * Computes the complex conjugate of each element in a complex double array.
 * @param data Input/output array of complex doubles.
 * @param len Length of the array.
 */
void conj_inl (
    _Complex double *data,
    size_t len);

/**
 * Reverses an array in place.
 * @param data Pointer to the array.
 * @param len Length of the array.
 * @param esize Size of each element in bytes.
 */
void reverse_inl (
    void *data,
    size_t len,
    size_t esize);

/**
 * Rolls an array by a specified offset.
 * @param data Pointer to the array.
 * @param rollby Offset by which to roll the array.
 * @param len Length of the array.
 * @param esize Size of each element in bytes.
 */
void roll_inline (
    void *data,
    int64_t rollby,
    size_t len,
    size_t esize);

/**
 * Adds two complex float arrays element-wise.
 * @param dest Destination array (also serves as the first operand).
 * @param right Second operand array.
 * @param len Length of the arrays.
 */
void cf64_add_inl (
    _Complex float *dest,
    _Complex float *right,
    size_t len);

/**
 * Zeros out a range of elements in a float array.
 * @param dest Pointer to the array.
 * @param s Interval specifying the range to zero.
 */
void zerof32_range (float *dest, intervals s);

/**
 * Zeros out a range of elements in a complex float array.
 * @param dest Pointer to the array.
 * @param s Interval specifying the range to zero.
 */
void zerocf_range (
    _Complex float *dest,
    intervals s);

/**
 * Calculates the sum of all elements in a float array.
 * @param dat Input array of floats.
 * @param len Length of the array.
 * @return Sum of the elements.
 */
float fvec_add (
    const float *dat,
    size_t len);

/**
 * Calculates the mean of a float array.
 * @param dat Input array of floats.
 * @param len Length of the array.
 * @return Mean value of the elements.
 */
float fvec_mean (
    const float *dat,
    size_t len);

/**
 * Calculates the mean of the squared values in a float array.
 * @param dat Input array of floats.
 * @param len Length of the array.
 * @return Mean of the squared values.
 */
float fvec_mean_fsqrd (
    const float *dat,
    size_t len);

///////////////////////////////////////////////////////
///////// Random

/**
 * Generates a vector of random Gaussian-distributed floats.
 * @param dest Destination array for the random values.
 * @param len Length of the array.
 * @param std Standard deviation of the distribution.
 * @param mean Mean of the distribution.
 */
void gaussf_vec (
    float *dest,
    size_t len,
    float std,
    float mean);

/**
 * Generates a vector of random Gaussian-distributed doubles.
 * @param dest Destination array for the random values.
 * @param len Length of the array.
 * @param std Standard deviation of the distribution.
 * @param mean Mean of the distribution.
 */
void gauss_vec (
    double *dest,
    size_t len,
    double std,
    double mean);

///////////////////////////////////////////////////////
///////// Stats

/**
 * Represents basic statistical measures for a dataset.
 * - mean: Mean value of the dataset.
 * - std: Standard deviation.
 * - max: Maximum value.
 * - min: Minimum value.
 * - argmax: Index of the maximum value.
 * - argmin: Index of the minimum value.
 */
typedef struct
{
  double mean;  // Mean value of the dataset.
  double std;   // Standard deviation of the dataset.
  double max;   // Maximum value in the dataset.
  double min;   // Minimum value in the dataset.
  long argmax;  // Index of the maximum value.
  long argmin;  // Index of the minimum value.
} stats;

/**
 * Prints a human-readable summary of the statistics.
 * @param ofp Output file pointer where the statistics will be printed.
 * @param s Stats structure containing the statistics.
 */
void stats_fprint_friendly (FILE *ofp, stats s);

/**
 * Prints the title row for a CSV file containing statistics.
 * @param ofp Output file pointer where the title row will be printed.
 * @param prefix Prefix for column names.
 * @param end_comma If non-zero, adds a trailing comma at the end of the row.
 */
void stats_fprint_csv_title (FILE *ofp, const char *prefix, int end_comma);

/**
 * Prints a row of statistics in CSV format.
 * @param ofp Output file pointer where the statistics row will be printed.
 * @param s Stats structure containing the statistics.
 * @param end_comma If non-zero, adds a trailing comma at the end of the row.
 */
void stats_fprint_csv_row (FILE *ofp, stats s, int end_comma);

/**
 * Calculates statistics for an array of floating-point values.
 * @param data Array of floating-point values.
 * @param len Length of the array.
 * @return A `stats` structure containing the calculated statistics.
 */
stats stats_calculate (const float *data, size_t len);

/**
 * Calculates statistics for the absolute values of a complex float array.
 * @param data Array of complex float values.
 * @param len Length of the array.
 * @return A `stats` structure containing the calculated statistics.
 */
stats stats_calculate_abs_cf (const _Complex float *data, size_t len);
