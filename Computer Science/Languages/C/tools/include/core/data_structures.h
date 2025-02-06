#pragma once

#include <complex.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h> // size_t

///////////////////////////////////////////////////////
///////// bytes

typedef struct
{
  uint8_t *data;
  size_t len;
} bytes;

bytes bytes_create_malloc (size_t len);

void bytes_resize (bytes *b, size_t newlen);

void bytes_resize_and_zero (bytes *b, size_t newlen);

void bytes_double_until_space (bytes *b, size_t newcap);

///////////////////////////////////////////////////////
///////// interval

typedef struct
{
  float v0;
  float vf;
} intervalf;

typedef struct
{
  size_t s0;
  size_t s1;
} intervals;

static inline float
intervalf_center (intervalf f)
{
  return (f.vf - f.v0) / 2;
}

static inline int
intervalf_valid (intervalf f)
{
  return f.v0 <= f.vf;
}

int intervalf_f_in_exc (intervalf i, float v);

int intervalf_if_in_exc (intervalf outer, intervalf inner);

static inline size_t
intervals_len_inc_exc (intervals s)
{
  return s.s1 - s.s0;
}

///////////////////////////////////////////////////////
///////// trange
typedef struct
{
  float t0;
  float tf;
  int isinf;
} trange;

int trange_parse_argstr (trange *dest, const char *str);

///////////////////////////////////////////////////////
///////// frange
typedef struct
{
  float f0;
  float f1;
  float df;
} frange;

int frange_valid (frange f);

int frange_in_range (frange f, float fs);

int frange_parse_argstr (frange *dest, const char *str);

size_t frange_size (frange f);

void frange_fprintf (FILE *ofp, frange f);

intervalf frange_to_intervalf (frange f);

float frange_findtof(frange f, size_t ind);

///////////////////////////////////////////////////////
///////// krange
typedef struct
{
  int k0;
  int kf;
  int isinf;
} krange;

typedef struct
{
  int k0;
  int kf;
} finite_krange;

static inline size_t
finite_krange_size (finite_krange k)
{
  return (size_t)abs (k.kf - k.k0);
}

static inline krange
krange_default ()
{
  return (krange){
    .k0 = 0,
    .isinf = 1,
  };
}

int krange_parse_argstr (krange *dest, const char *str);

krange trange_to_krange (trange t, float fs);

finite_krange krange_to_finite_krange (krange k);

void krange_fprintf (FILE *ofp, krange k);

typedef struct
{
  size_t val;
  krange next;
} krange_next_result;

krange_next_result krange_next (krange k, size_t desired);

size_t krange_size (krange k);

int krange_more (krange k);

///////////////////////////////////////////////////////
///////// buffers
typedef struct
{
  _Complex float *data;
  size_t len;
} cf64_buffer;

typedef struct
{
  _Complex float *data;
  size_t rows;
  size_t cols;
} cf64_2d_buffer;

cf64_buffer cf64_buffer_from_bytes (bytes b);

///////////////////////////////////////////////////////
///////// pargs
typedef struct
{
  float a;
  float p0;
  float f;
} pargs;

int pargs_parse_argstr (pargs *dest, const char *str);

_Complex float pargs_evalk (pargs p, int k, float fs);

///////////////////////////////////////////////////////
///////// si_prefix
typedef struct
{
  const char *prefix;
  float multiplier;
} si_prefixf;

typedef struct
{
  const char *prefix;
  double multiplier;
} si_prefix;

int si_prefixf_parse (si_prefixf *dest, const char *prefix);

int si_prefix_parse (si_prefix *dest, const char *prefix);

///////////////////////////////////////////////////////
///////// slice
typedef struct
{
  size_t start;
  size_t stop;
  size_t step;
  int isinf;
} slice;

slice slice_default ();

size_t slice_left (slice s);

int try_parse_slice (slice *dest, const char *str);

///////////////////////////////////////////////////////
///////// stack
typedef struct
{
  uint8_t *data;
  size_t dcap;
  size_t head;
} stack;

stack stack_create_malloc (size_t initial_cap);

stack stack_create_from (uint8_t *data, size_t initial_cap);

size_t stack_cap_left (stack *s);

bytes stacktobytes (stack s);

bytes stacktobytes_clip (stack s);

void stack_push_grow (stack *s, void *src, size_t slen);

void stack_push_dont_grow (stack *s, void *src, size_t slen);
