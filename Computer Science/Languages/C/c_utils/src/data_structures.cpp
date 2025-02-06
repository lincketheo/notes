#include "data_structures.h"

#include "math.h"
#include "stdlibf.h"
#include "testing.h"

#include <cmath>

///////////////////////////////////////////////////////
///////// bytes
bytes
bytes_create_malloc (size_t len)
{
  bytes ret;
  ret.data = (uint8_t *)malloc_or_abort (len);
  ret.len = len;
  return ret;
}

void
bytes_free_malloced (bytes *b)
{
  ASSERT (b);
  ASSERT (b->len > 0);
  ASSERT (b->data);
  safe_free (b->data);
  b->len = 0;
}

void
bytes_resize (bytes *b, size_t newlen)
{
  ASSERT (b);
  ASSERT (newlen > 0);
  b->data = (uint8_t *)realloc_or_abort (b->data, newlen);
  b->len = newlen;
}

TEST (bytes_resize)
{
  bytes b = bytes_create_malloc (10);
  TEST_EQUAL (b.len, 10lu, "%lu");
  bytes_resize (&b, 5);
  TEST_EQUAL (b.len, 5lu, "%lu");
  bytes_resize (&b, 500);
  TEST_EQUAL (b.len, 500lu, "%lu");
  safe_free (b.data);
}

void
bytes_resize_and_zero (bytes *b, size_t newlen)
{
  ASSERT (b);
  b->data = (uint8_t *)realloc_or_abort (b->data, newlen);
  if (newlen > b->len)
    {
      memset (&b->data[b->len], 0, newlen - b->len);
    }
  b->len = newlen;
}

TEST (bytes_resize_and_zero)
{
  bytes b = bytes_create_malloc (10);
  TEST_EQUAL (b.len, 10lu, "%lu");
  bytes_resize_and_zero (&b, 5);
  TEST_EQUAL (b.len, 5lu, "%lu");
  bytes_resize_and_zero (&b, 500);
  TEST_EQUAL (b.len, 500lu, "%lu");
  safe_free (b.data);
}

void
bytes_double_until_space (bytes *b, size_t newcap)
{
  if (newcap > b->len)
    {
      size_t _newcap = b->len * dblbuf_next_2_power (b->len, newcap);
      bytes_resize (b, _newcap);
    }
}

TEST (bytes_double_until_space)
{
  bytes b = bytes_create_malloc (12);
  TEST_EQUAL (b.len, 12lu, "%lu");
  bytes_double_until_space (&b, 90);
  TEST_EQUAL (b.len, 96lu, "%lu");
  safe_free (b.data);
}

///////////////////////////////////////////////////////
///////// interval
int
intervalf_f_in_exc (intervalf i, float v)
{
  ASSERT (i.v0 <= i.vf);
  return v < i.vf && v > i.v0;
}

int
intervalf_if_in_exc (intervalf outer, intervalf inner)
{
  ASSERT (outer.v0 <= outer.vf);
  ASSERT (inner.v0 <= inner.vf);

  return intervalf_f_in_exc (outer, inner.v0) && intervalf_f_in_exc (outer, inner.vf);
}

///////////////////////////////////////////////////////
///////// trange
int
trange_parse_argstr (trange *dest, const char *str)
{
  dest->t0 = 0.0f;
  dest->tf = 0.0f;
  dest->isinf = 0;

  float t0, tf;

  if (sscanf (str, "%f:%f", &t0, &tf) == 2)
    {
      dest->t0 = t0;
      dest->tf = tf;
    }
  else if (sscanf (str, "%f:", &t0) == 1)
    {
      dest->t0 = t0;
      dest->isinf = 1;
    }
  else if (sscanf (str, ":%f", &tf) == 1)
    {
      dest->t0 = 0.0f;
      dest->tf = tf;
    }
  else if (strcmp (str, ":") == 0)
    {
      dest->isinf = 1;
    }
  else
    {
      return -1;
    }

  return 0;
}

TEST (trange_parse_argstr)
{
  int res;
  trange t;

  res = trange_parse_argstr (&t, "0:100");
  TEST_EQUAL (res, 0, "%d");
  TEST_EQUAL (t.t0, 0.0, "%f");
  TEST_EQUAL (t.tf, 100.0, "%f");
  TEST_EQUAL (t.isinf, 0, "%d");

  res = trange_parse_argstr (&t, "0.123:100E8");
  TEST_EQUAL (res, 0, "%d");
  TEST_EQUAL (t.t0, 0.123f, "%f");
  TEST_EQUAL (t.tf, 100E8f, "%f");
  TEST_EQUAL (t.isinf, 0, "%d");

  res = trange_parse_argstr (&t, "0.123:");
  TEST_EQUAL (res, 0, "%d");
  TEST_EQUAL (t.t0, 0.123f, "%f");
  TEST_EQUAL (t.isinf, 1, "%d");

  res = trange_parse_argstr (&t, ":");
  TEST_EQUAL (res, 0, "%d");
  TEST_EQUAL (t.t0, 0.0f, "%f");
  TEST_EQUAL (t.isinf, 1, "%d");

  res = trange_parse_argstr (&t, ":10");
  TEST_EQUAL (res, 0, "%d");
  TEST_EQUAL (t.t0, 0.0f, "%f");
  TEST_EQUAL (t.tf, 10.0f, "%f");
  TEST_EQUAL (t.isinf, 0, "%d");
}

///////////////////////////////////////////////////////
///////// frange
int
frange_valid (frange f)
{
  if (f.df == 0.0f)
    {
      return 0;
    }

  if ((f.f1 > f.f0 && f.df <= 0.0f) || (f.f1 < f.f0 && f.df >= 0.0f))
    {
      return 0;
    }

  return 1;
}

int
frange_in_range (frange f, float fs)
{
  intervalf fsint = (intervalf){
    .v0 = -fs / 2,
    .vf = fs / 2,
  };

  return intervalf_if_in_exc (fsint, frange_to_intervalf (f));
}

int
frange_parse_argstr (frange *dest, const char *str)
{
  ASSERT (dest);
  ASSERT (str);

  float f0 = 0.0f, f1 = 0.0f, df = 0.0f;

  if (sscanf (str, "%f:%f:%f", &f0, &df, &f1) != 3)
    {
      return -1;
    }

  dest->f0 = f0;
  dest->f1 = f1;
  dest->df = df;

  if (!frange_valid (*dest))
    return -1;

  return 0;
}

TEST (frange_parse_argstr)
{
  frange result;
  int ret;

  // Valid cases
  ret = frange_parse_argstr (&result, "0.0:0.1:1.0");
  TEST_EQUAL (ret, 0, "%d");
  TEST_EQUAL (result.f0, 0.0f, "%f");
  TEST_EQUAL (result.df, 0.1f, "%f");
  TEST_EQUAL (result.f1, 1.0f, "%f");

  ret = frange_parse_argstr (&result, "1.5:-0.5:0.0");
  TEST_EQUAL (ret, 0, "%d");
  TEST_EQUAL (result.f0, 1.5f, "%f");
  TEST_EQUAL (result.df, -0.5f, "%f");
  TEST_EQUAL (result.f1, 0.0f, "%f");

  // Invalid cases
  ret = frange_parse_argstr (&result, "0.0:-0.1:1.0"); // df moves away from f1
  TEST_EQUAL (ret, -1, "%d");

  ret = frange_parse_argstr (&result, "1.0:0.1:0.0"); // df moves away from f1
  TEST_EQUAL (ret, -1, "%d");

  ret = frange_parse_argstr (&result, "5:0:-5"); // df is zero
  TEST_EQUAL (ret, -1, "%d");

  ret = frange_parse_argstr (&result, "not_a_number");
  TEST_EQUAL (ret, -1, "%d");

  ret = frange_parse_argstr (&result, "1.0:1.0"); // Missing f1
  TEST_EQUAL (ret, -1, "%d");
}

size_t
frange_size (frange f)
{
  return (size_t)floorf ((f.f1 - f.f0) / (f.df)) + 1;
}

void
frange_fprintf (FILE *ofp, frange f)
{
  fprintf (ofp, "frange:\n");
  fprintf (ofp, "  f0 (start frequency): %.2f\n", f.f0);
  fprintf (ofp, "  f1 (end frequency): %.2f\n", f.f1);
  fprintf (ofp, "  df (frequency step): %.2f\n", f.df);
}

intervalf
frange_to_intervalf (frange f)
{
  if (f.f1 > f.f0)
    {
      return (intervalf){
        .v0 = f.f0,
        .vf = f.f1,
      };
    }

  return (intervalf){
    .v0 = f.f1,
    .vf = f.f0,
  };
}

float
frange_findtof (frange f, size_t ind)
{
  ASSERT (frange_valid (f));
  return f.f0 + f.df * ind;
}

///////////////////////////////////////////////////////
///////// krange
int
krange_parse_argstr (krange *dest, const char *str)
{
  dest->k0 = 0.0f;
  dest->kf = 0.0f;
  dest->isinf = 0;

  int k0, kf;

  if (sscanf (str, "%d:%d", &k0, &kf) == 2)
    {
      dest->k0 = k0;
      dest->kf = kf;
    }
  else if (sscanf (str, "%d:", &k0) == 1)
    {
      dest->k0 = k0;
      dest->isinf = 1;
    }
  else if (sscanf (str, ":%d", &kf) == 1)
    {
      dest->k0 = 0.0f;
      dest->kf = kf;
    }
  else if (strcmp (str, ":") == 0)
    {
      dest->isinf = 1;
    }
  else
    {
      return -1;
    }

  return 0;
}

TEST (krange_parse_argstr)
{
  int res;
  krange t;

  res = krange_parse_argstr (&t, "0:100");
  TEST_EQUAL (res, 0, "%d");
  TEST_EQUAL (t.k0, 0, "%d");
  TEST_EQUAL (t.kf, 100, "%d");
  TEST_EQUAL (t.isinf, 0, "%d");

  res = krange_parse_argstr (&t, "0:100");
  TEST_EQUAL (res, 0, "%d");
  TEST_EQUAL (t.k0, 0, "%d");
  TEST_EQUAL (t.kf, 100, "%d");
  TEST_EQUAL (t.isinf, 0, "%d");

  res = krange_parse_argstr (&t, "123:");
  TEST_EQUAL (res, 0, "%d");
  TEST_EQUAL (t.k0, 123, "%d");
  TEST_EQUAL (t.isinf, 1, "%d");

  res = krange_parse_argstr (&t, ":");
  TEST_EQUAL (res, 0, "%d");
  TEST_EQUAL (t.k0, 0, "%d");
  TEST_EQUAL (t.isinf, 1, "%d");

  res = krange_parse_argstr (&t, ":10");
  TEST_EQUAL (res, 0, "%d");
  TEST_EQUAL (t.k0, 0, "%d");
  TEST_EQUAL (t.kf, 10, "%d");
  TEST_EQUAL (t.isinf, 0, "%d");
}

krange
trange_to_krange (trange t, float fs)
{
  krange ret;
  ret.k0 = (int)(fs * t.t0);
  ret.isinf = t.isinf;
  if (!ret.isinf)
    {
      ret.kf = (int)(fs * t.tf);
    }
  return ret;
}

finite_krange
krange_to_finite_krange (krange k)
{
  ASSERT (!k.isinf);
  return (finite_krange){
    .k0 = k.k0,
    .kf = k.kf,
  };
}

void
krange_fprintf (FILE *ofp, krange k)
{
  fprintf (ofp, "krange:\n");
  fprintf (ofp, "  k0 (start sample): %d\n", k.k0);

  if (k.isinf)
    {
      fprintf (ofp, "  kf (end sample): infinity\n");
    }
  else
    {
      fprintf (ofp, "  kf (end sample): %d\n", k.kf);
    }

  fprintf (ofp, "  isinf: %s\n", k.isinf ? "true" : "false");
}

krange_next_result
krange_next (krange k, size_t desired)
{
  if (k.isinf)
    return (krange_next_result){
      .val = desired,
      .next = k,
    };

  size_t kavail = krange_size (k);
  if (desired > kavail)
    {
      return (krange_next_result){
        .val = kavail,
        .next = (krange){
            .k0 = k.kf,
            .kf = k.kf,
            .isinf = k.isinf,
        },
      };
    }

  return (krange_next_result){
    .val = desired,
    .next = (krange){
        .k0 = (int)k.k0 + (int)desired,
        .kf = k.kf,
        .isinf = k.isinf,
    },
  };
}

size_t
krange_size (krange k)
{
  if (k.isinf)
    return SIZE_MAX;
  ASSERT (k.kf >= k.k0);
  return k.kf - k.k0;
}

int
krange_more (krange k)
{
  return k.isinf || k.kf > k.k0;
}

///////////////////////////////////////////////////////
///////// buffers
cf64_buffer
cf64_buffer_from_bytes (bytes b)
{
  ASSERT (b.len % sizeof (_Complex float) == 0);
  cf64_buffer ret;
  ret.len = b.len / sizeof (_Complex float);
  ret.data = (_Complex float *)b.data;
  return ret;
}

f32_buffer
f32_buffer_from_bytes (bytes b)
{
  ASSERT (b.len % sizeof (float) == 0);
  f32_buffer ret;
  ret.len = b.len / sizeof (float);
  ret.data = (float *)b.data;
  return ret;
}

///////////////////////////////////////////////////////
///////// pargs
int
pargs_parse_argstr (pargs *dest, const char *str)
{
  ASSERT (str);

  dest->a = 1.0f;
  dest->p0 = 0.0f;
  dest->f = NAN;

  const char *ptr = str;

  while (*ptr)
    {
      char *end;
      float value = strtof (ptr, &end);
      if (end == ptr)
        {
          return -1;
        }

      if (*end == 'f')
        {
          dest->f = value;
          ptr = end + 1;
        }
      else if (*end == 'a')
        {
          dest->a = value;
          ptr = end + 1;
        }
      else if (*end == 'p')
        {
          dest->p0 = value;
          ptr = end + 1;
        }
      else
        {
          ptr = end;
        }
    }

  if (isnanf (dest->f))
    {
      return -1;
    }

  return 0;
}

TEST (pargs_parse_argstr)
{
  int res;
  pargs p;

  // Base
  res = pargs_parse_argstr (&p, "5f4p1a");
  TEST_EQUAL (res, 0, "%d");
  TEST_EQUAL (p.a, 1.0, "%f");
  TEST_EQUAL (p.p0, 4.0, "%f");
  TEST_EQUAL (p.f, 5.0, "%f");

  // Order doesn't matter
  res = pargs_parse_argstr (&p, "4.123p1E9f1a");
  TEST_EQUAL (res, 0, "%d");
  TEST_EQUAL (p.a, 1.0f, "%f");
  TEST_EQUAL (p.p0, 4.123f, "%f");
  TEST_EQUAL (p.f, 1e9f, "%f");

  // Leave out a
  res = pargs_parse_argstr (&p, "1e9f4.123p");
  TEST_EQUAL (res, 0, "%d");
  TEST_EQUAL (p.a, 1.0f, "%f");
  TEST_EQUAL (p.p0, 4.123f, "%f");
  TEST_EQUAL (p.f, 1e9f, "%f");

  // Leave out p
  res = pargs_parse_argstr (&p, "-1.100f9.1a");
  TEST_EQUAL (res, 0, "%d");
  TEST_EQUAL (p.a, 9.1f, "%f");
  TEST_EQUAL (p.p0, 0.0f, "%f");
  TEST_EQUAL (p.f, -1.1f, "%f");

  // Leave out p and a
  res = pargs_parse_argstr (&p, "5.123f");
  TEST_EQUAL (res, 0, "%d");
  TEST_EQUAL (p.a, 1.0f, "%f");
  TEST_EQUAL (p.p0, 0.0f, "%f");
  TEST_EQUAL (p.f, 5.123f, "%f");

  // Error
  res = pargs_parse_argstr (&p, "");
  TEST_EQUAL (res, -1, "%d");

  res = pargs_parse_argstr (&p, "gf");
  TEST_EQUAL (res, -1, "%d");

  res = pargs_parse_argstr (&p, "5fgp");
  TEST_EQUAL (res, -1, "%d");

  res = pargs_parse_argstr (&p, "5f6p5i");
  TEST_EQUAL (res, -1, "%d");
}

_Complex float
pargs_evalk (pargs p, int k, float fs)
{
  float arg = (2.0f * M_PI * p.f * (float)k / fs + p.p0);
  _Complex float c = cexpf (I * arg);
  float ff = cabsf (c);
  return c * p.a;
}

///////////////////////////////////////////////////////
///////// si_prefix
static si_prefixf prefixesf[] = { { "y", 1e-24 },
                                  { "z", 1e-21 },
                                  { "a", 1e-18 },
                                  { "f", 1e-15 },
                                  { "p", 1e-12 },
                                  { "n", 1e-9 },
                                  { "u", 1e-6 },
                                  { "m", 1e-3 },
                                  { "", 1.0 },
                                  { "k", 1e3 },
                                  { "M", 1e6 },
                                  { "G", 1e9 },
                                  { "T", 1e12 },
                                  { "P", 1e15 },
                                  { "E", 1e18 },
                                  { "Z", 1e21 },
                                  { "Y", 1e24 } };

static const int si_prefix_count = sizeof (prefixesf) / sizeof (si_prefixf);

int
si_prefixf_parse (si_prefixf *dest, const char *prefix)
{
  for (int i = 0; i < si_prefix_count; i++)
    {
      if (strcmp (prefixesf[i].prefix, prefix) == 0)
        {
          *dest = prefixesf[i];
          return 0;
        }
    }
  return -1;
}

static si_prefix prefixes[] = { { "y", 1e-24 },
                                { "z", 1e-21 },
                                { "a", 1e-18 },
                                { "f", 1e-15 },
                                { "p", 1e-12 },
                                { "n", 1e-9 },
                                { "u", 1e-6 },
                                { "m", 1e-3 },
                                { "", 1.0 },
                                { "k", 1e3 },
                                { "M", 1e6 },
                                { "G", 1e9 },
                                { "T", 1e12 },
                                { "P", 1e15 },
                                { "E", 1e18 },
                                { "Z", 1e21 },
                                { "Y", 1e24 } };

int
si_prefix_parse (si_prefix *dest, const char *prefix)
{
  for (int i = 0; i < si_prefix_count; i++)
    {
      if (strcmp (prefixes[i].prefix, prefix) == 0)
        {
          *dest = prefixes[i];
          return 0;
        }
    }
  return -1;
}

///////////////////////////////////////////////////////
///////// slice
slice
slice_default ()
{
  slice s;
  s.start = 0;
  s.step = 1;
  s.isinf = 1;

  return s;
}

size_t
slice_left (slice s)
{
  if (s.isinf)
    {
      return SIZE_MAX;
    }
  ASSERT (s.stop >= s.start);
  return s.stop - s.start;
}

int
try_parse_slice (slice *dest, const char *str)
{
  if (!dest || !str)
    {
      return 0;
    }

  *dest = slice_default ();

  size_t start, stop, step;

  if (strchr (str, ':') == NULL)
    {
      return 0;
    }

  if (sscanf (str, "%zu:%zu:%zu", &start, &stop, &step) == 3)
    {
      dest->start = start;
      dest->stop = stop;
      dest->step = step;
      dest->isinf = 0;
    }
  else if (sscanf (str, "%zu:%zu:", &start, &stop) == 2)
    {
      dest->start = start;
      dest->stop = stop;
      dest->step = 1;
      dest->isinf = 0;
    }
  else if (sscanf (str, ":%zu:%zu", &stop, &step) == 2)
    {
      dest->start = 0;
      dest->stop = stop;
      dest->step = step;
      dest->isinf = 0;
    }
  else if (sscanf (str, "%zu::%zu", &start, &step) == 2)
    {
      dest->start = start;
      dest->step = step;
      dest->isinf = 1;
    }
  else if (sscanf (str, ":%zu", &stop) == 1)
    {
      dest->start = 0;
      dest->stop = stop;
      dest->step = 1;
      dest->isinf = 0;
    }
  else if (sscanf (str, "%zu:", &start) == 1)
    {
      dest->start = start;
      dest->step = 1;
      dest->isinf = 1;
    }
  else if (strcmp (str, "::") == 0)
    {
      dest->start = 0;
      dest->step = 1;
      dest->isinf = 1;
    }
  else
    {
      return -1;
    }

  if (dest->start > dest->stop)
    {
      return -1;
    }

  return 0;
}

///////////////////////////////////////////////////////
///////// stack
stack
stack_create_malloc (size_t initial_cap)
{
  ASSERT (initial_cap > 0);
  stack ret;
  ret.data = (uint8_t *)malloc_or_abort (initial_cap);
  ret.dcap = initial_cap;
  ret.head = 0;
  return ret;
}

stack
stack_create_from (uint8_t *data, size_t initial_cap)
{
  ASSERT (initial_cap > 0);
  stack ret;
  ret.data = data;
  ret.dcap = initial_cap;
  ret.head = 0;
  return ret;
}

size_t
stack_cap_left (stack *s)
{
  ASSERT (s);
  ASSERT (s->head <= s->dcap);
  return s->dcap - s->head;
}

static inline void
stack_ensure_space (stack *s, size_t next_slen)
{
  bytes b = (bytes){ .data = s->data, .len = s->dcap };
  bytes_double_until_space (&b, s->head + next_slen);
  s->data = b.data;
  s->dcap = b.len;
}

void
stack_push_grow (stack *dest, void *src, size_t slen)
{
  ASSERT (dest);
  ASSERT (src);
  ASSERT (slen > 0);
  ASSERT (dest->dcap > 0);
  ASSERT (dest->data);

  stack_ensure_space (dest, slen);
  memcpy (dest->data + dest->head, src, slen);
  dest->head += slen;
}

TEST (stack_push_stackalloc)
{
  stack b = stack_create_malloc (10);
  int d = 1;
  stack_push_grow (&b, &d, sizeof (int));
  d = 2;
  stack_push_grow (&b, &d, sizeof (int));
  d = 10;
  stack_push_grow (&b, &d, sizeof (int));
  d = 123;
  stack_push_grow (&b, &d, sizeof (int));
  d = 5;
  stack_push_grow (&b, &d, sizeof (int));

  int *dd = (int *)b.data;
  TEST_EQUAL (dd[0], 1, "%d");
  TEST_EQUAL (dd[1], 2, "%d");
  TEST_EQUAL (dd[2], 10, "%d");
  TEST_EQUAL (dd[3], 123, "%d");
  TEST_EQUAL (dd[4], 5, "%d");

  safe_free (b.data);
}

void
stack_push_dont_grow (stack *dest, void *src, size_t slen)
{
  ASSERT (dest);
  ASSERT (src);
  ASSERT (slen > 0);
  ASSERT (dest->dcap > 0);
  ASSERT (dest->data);

  if (dest->dcap < dest->head + slen)
    {
      fatal_error ("Stack overflow\n");
      unreachable ();
    }

  memcpy (dest->data + dest->head, src, slen);
  dest->head += slen;
}

bytes
stacktobytes (stack s)
{
  return (bytes){
    .data = s.data,
    .len = s.head,
  };
}

bytes
stacktobytes_clip (stack s)
{
  bytes ret = stacktobytes (s);
  bytes_resize (&ret, s.head);
  return ret;
}

TEST (stacktobytes_clip)
{
  stack b = stack_create_malloc (10);
  int d = 1;
  stack_push_grow (&b, &d, sizeof (int));
  d = 2;
  stack_push_grow (&b, &d, sizeof (int));
  d = 10;
  stack_push_grow (&b, &d, sizeof (int));
  d = 123;
  stack_push_grow (&b, &d, sizeof (int));
  d = 5;
  stack_push_grow (&b, &d, sizeof (int));

  bytes bb = stacktobytes_clip (b);
  TEST_EQUAL (bb.len, 5 * sizeof (int), "%lu");

  int *dd = (int *)bb.data;
  TEST_EQUAL (dd[0], 1, "%d");
  TEST_EQUAL (dd[1], 2, "%d");
  TEST_EQUAL (dd[2], 10, "%d");
  TEST_EQUAL (dd[3], 123, "%d");
  TEST_EQUAL (dd[4], 5, "%d");

  safe_free (bb.data);
}
