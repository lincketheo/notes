#pragma once

#include <complex.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>

// bytes: A structure for managing byte arrays.
typedef struct
{
  uint8_t *data; // Pointer to the byte array.
  size_t len;    // Length of the byte array.
} bytes;

/**
 * Creates a `bytes` structure with a specified length, allocating memory.
 * @param len Length of the byte array.
 * @return A `bytes` structure with allocated memory.
 */
bytes bytes_create_malloc (size_t len);

/**
 * Resizes the byte array, reallocating memory as needed.
 * @param newlen New length for the byte array.
 */
void bytes_resize (bytes *b, size_t newlen);

/**
 * Resizes the byte array and zeroes new memory if expanding.
 * @param newlen New length for the byte array.
 */
void bytes_resize_and_zero (bytes *b, size_t newlen);

/**
 * Doubles the size of the byte array until it meets or exceeds a target capacity.
 * @param newcap Target capacity for the byte array.
 */
void bytes_double_until_space (bytes *b, size_t newcap);

///////////////////////////////////////////////////////
// interval: Represent intervals of floats or sizes.

typedef struct
{
  float v0; // Start of the interval.
  float vf; // End of the interval.
} intervalf;

typedef struct
{
  size_t s0; // Start of the interval.
  size_t s1; // End of the interval.
} intervals;

/**
 * Computes the center point of a float interval.
 * @param f Float interval.
 * @return Center of the interval.
 */
static inline float
intervalf_center (intervalf f)
{
  return (f.vf - f.v0) / 2;
}

/**
 * Checks if a float interval is valid.
 * @param f Float interval.
 * @return 1 if valid, 0 otherwise.
 */
static inline int
intervalf_valid (intervalf f)
{
  return f.v0 <= f.vf;
}

/**
 * Checks if a value is within a float interval (exclusive bounds).
 * @param i Float interval.
 * @param v Value to check.
 * @return 1 if value is within the interval, 0 otherwise.
 */
int intervalf_f_in_exc (intervalf i, float v);

/**
 * Checks if one interval is within another (exclusive bounds).
 * @param outer Outer interval.
 * @param inner Inner interval.
 * @return 1 if inner interval is within the outer, 0 otherwise.
 */
int intervalf_if_in_exc (intervalf outer, intervalf inner);

/**
 * Computes the inclusive-exclusive length of a size interval.
 * @param s Size interval.
 * @return Length of the interval.
 */
static inline size_t
intervals_len_inc_exc (intervals s)
{
  return s.s1 - s.s0;
}

///////////////////////////////////////////////////////
// trange: Represent time ranges.

typedef struct
{
  float t0;  // Start of the range.
  float tf;  // End of the range.
  int isinf; // Flag indicating if the range is infinite.
} trange;

/**
 * Parses a time range from a string.
 * @param str String to parse.
 * @return 0 on success, non-zero on failure.
 */
int trange_parse_argstr (trange *dest, const char *str);

///////////////////////////////////////////////////////
// frange: Represent ranges with fixed increments.

// [f0, df, f1]
typedef struct
{
  float f0; // Start of the range.
  float f1; // End of the range.
  float df; // Increment between values.
} frange;

int frange_valid (frange f);                             // Checks if f0 + df x N for some N passes f1 - (invalid = {f0 = 0, df = -1, f1 = 1})
int frange_in_range (frange f, float fs);                // Checks if a value is within the range.
int frange_parse_argstr (frange *dest, const char *str); // Parses a range from a string.
size_t frange_size (frange f);                           // Computes the number of elements in the range
void frange_fprintf (FILE *ofp, frange f);               // Prints the range to a file.
intervalf frange_to_intervalf (frange f);                // Converts a range to an interval.
float frange_findtof (frange f, size_t ind);             // Finds a value at a specific index in the range.

///////////////////////////////////////////////////////
// krange: Represent ranges with integer values, with possible infinity.

typedef struct
{
  int k0;    // Start of the range.
  int kf;    // End of the range.
  int isinf; // Flag indicating if the range is infinite.
} krange;

typedef struct
{
  int k0; // Start of the range.
  int kf; // End of the range.
} finite_krange;

/**
 * Computes the size of a finite range.
 * @param k Finite range.
 * @return Size of the range.
 */
static inline size_t
finite_krange_size (finite_krange k)
{
  return (size_t)abs (k.kf - k.k0);
}

/**
 * Creates a default `krange` with k0 = 0 and isinf = 1.
 * @return Default `krange`.
 */
static inline krange
krange_default ()
{
  krange k;
  k.k0 = 0;
  k.isinf = 1;
  return k;
}

/**
 * Parses a `krange` from a string.
 * @param dest Pointer to the destination `krange`.
 * @param str String to parse.
 * @return 0 on success, non-zero on failure.
 */
int krange_parse_argstr (krange *dest, const char *str);

/**
 * Converts a `trange` to a `krange` using a sampling frequency.
 * @param t Input `trange`.
 * @param fs Sampling frequency.
 * @return Converted `krange`.
 */
krange trange_to_krange (trange t, float fs);

/**
 * Converts an infinite or finite `krange` to a strictly finite `finite_krange`.
 * @param k Input `krange`.
 * @return Equivalent `finite_krange`.
 */
finite_krange krange_to_finite_krange (krange k);

/**
 * Prints a `krange` to a file.
 * @param ofp Output file pointer.
 * @param k `krange` to print.
 */
void krange_fprintf (FILE *ofp, krange k);

/**
 * Represents the result of advancing a `krange`.
 * @param val Current value in the range.
 * @param next Remaining range after advancing.
 */
typedef struct
{
  size_t val;
  krange next;
} krange_next_result;

/**
 * Advances a `krange` to the next value up to a desired target.
 * @param k Input `krange`.
 * @param desired Desired target value.
 * @return Result of the advancement, including the next value and remaining range.
 */
krange_next_result krange_next (krange k, size_t desired);

/**
 * Computes the size of a `krange`.
 * @param k Input `krange`.
 * @return Size of the range.
 */
size_t krange_size (krange k);

/**
 * Checks if a `krange` has more values to iterate over.
 * @param k Input `krange`.
 * @return 1 if more values exist, 0 otherwise.
 */
int krange_more (krange k);

///////////////////////////////////////////////////////
// buffers: Managing bytes - and other useful utilities.

/**
 * Represents a buffer of complex float values.
 */
typedef struct
{
  _Complex float *data; // Pointer to the buffer data.
  size_t len;           // Length of the buffer.
} cf64_buffer;

/**
 * Represents a buffer of float values.
 */
typedef struct
{
  float *data; // Pointer to the buffer data.
  size_t len;  // Length of the buffer.
} f32_buffer;

/**
 * Represents a 2D buffer of complex float values.
 */
typedef struct
{
  _Complex float *data; // Pointer to the buffer data.
  size_t rows;          // Number of rows.
  size_t cols;          // Number of columns.
} cf64_2d_buffer;

/**
 * Converts a `bytes` structure to a complex float buffer.
 * @param b Input `bytes`.
 * @return Complex float buffer representation.
 */
cf64_buffer cf64_buffer_from_bytes (bytes b);

/**
 * Converts a `bytes` structure to a float buffer.
 * @param b Input `bytes`.
 * @return float buffer representation.
 */
f32_buffer f32_buffer_from_bytes (bytes b);

///////////////////////////////////////////////////////
// pargs: Represent parameters for evaluation.

/**
 * Represents parameters for evaluation functions.
 */
typedef struct
{
  float a;  // Amplitude.
  float p0; // Initial phase.
  float f;  // Frequency.
} pargs;

/**
 * Parses parameters from a string.
 * @param dest Pointer to the destination `pargs`.
 * @param str String to parse.
 * @return 0 on success, non-zero on failure.
 */
int pargs_parse_argstr (pargs *dest, const char *str);

/**
 * Evaluates a parameterized function at a given index and sampling frequency.
 * @param p Input parameters.
 * @param k Index.
 * @param fs Sampling frequency.
 * @return Result of the evaluation as a complex float.
 */
_Complex float pargs_evalk (pargs p, int k, float fs);

///////////////////////////////////////////////////////
// si_prefix: Represent SI prefixes for scaling.

/**
 * Represents an SI prefix and its multiplier (float precision).
 */
typedef struct
{
  const char *prefix; // SI prefix (e.g., "k", "M").
  float multiplier;   // Corresponding multiplier.
} si_prefixf;

/**
 * Represents an SI prefix and its multiplier (double precision).
 */
typedef struct
{
  const char *prefix; // SI prefix (e.g., "k", "M").
  double multiplier;  // Corresponding multiplier.
} si_prefix;

/**
 * Parses an SI prefix into a `si_prefixf` structure.
 * @param dest Pointer to the destination `si_prefixf`.
 * @param prefix Input prefix string.
 * @return 0 on success, non-zero on failure.
 */
int si_prefixf_parse (si_prefixf *dest, const char *prefix);

/**
 * Parses an SI prefix into a `si_prefix` structure.
 * @param dest Pointer to the destination `si_prefix`.
 * @param prefix Input prefix string.
 * @return 0 on success, non-zero on failure.
 */
int si_prefix_parse (si_prefix *dest, const char *prefix);

///////////////////////////////////////////////////////
// slice: Represent array slices.

/**
 * Represents an array slice with optional infinite bounds.
 */
typedef struct
{
  size_t start; // Start index.
  size_t stop;  // Stop index.
  size_t step;  // Step size.
  int isinf;    // Flag indicating if bounds are infinite.
} slice;

/**
 * Creates a default `slice` with start = 0, stop = 0, step = 1, and isinf = 0.
 * @return Default `slice`.
 */
slice slice_default ();

/**
 * Computes the leftmost index of a slice.
 * @param s Input slice.
 * @return Leftmost index.
 */
size_t slice_left (slice s);

/**
 * Attempts to parse a slice from a string.
 * @param dest Pointer to the destination `slice`.
 * @param str String to parse.
 * @return 0 on success, non-zero on failure.
 */
int try_parse_slice (slice *dest, const char *str);

///////////////////////////////////////////////////////
// stack: Represent a resizable stack.

/**
 * Represents a resizable stack of bytes.
 */
typedef struct
{
  uint8_t *data; // Pointer to the stack data.
  size_t dcap;   // Capacity of the stack.
  size_t head;   // Current position of the stack head.
} stack;

/**
 * Creates a stack with an initial allocated capacity.
 * @param initial_cap Initial capacity of the stack.
 * @return Created stack.
 */
stack stack_create_malloc (size_t initial_cap);

/**
 * Creates a stack from pre-allocated memory.
 * @param data Pre-allocated memory for the stack.
 * @param initial_cap Capacity of the stack.
 * @return Created stack.
 */
stack stack_create_from (uint8_t *data, size_t initial_cap);

/**
 * Computes the remaining capacity of a stack.
 * @param s Pointer to the stack.
 * @return Remaining capacity.
 */
size_t stack_cap_left (stack *s);

/**
 * Converts a stack to a `bytes` structure.
 * @param s Input stack.
 * @return `bytes` representation of the stack.
 */
bytes stacktobytes (stack s);

/**
 * Converts a stack to a `bytes` structure, clipping excess capacity.
 * @param s Input stack.
 * @return `bytes` representation of the stack.
 */
bytes stacktobytes_clip (stack s);

/**
 * Pushes data onto the stack, growing its capacity if necessary.
 * @param s Pointer to the stack.
 * @param src Source data to push.
 * @param slen Length of the source data.
 */
void stack_push_grow (stack *s, void *src, size_t slen);

/**
 * Pushes data onto the stack without growing its capacity.
 * @param s Pointer to the stack.
 * @param src Source data to push.
 * @param slen Length of the source data.
 */
void stack_push_dont_grow (stack *s, void *src, size_t slen);
