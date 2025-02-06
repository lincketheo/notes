#include "core/dtype.h"

#include "core/errors.h"
#include "core/strings.h"
#include "core/testing.h"

#include <float.h>
#include <string.h>

#define DTYPE_FOREACH_UC_ARGS(func, ...) \
  func (CHAR, __VA_ARGS__);              \
                                         \
  func (U8, __VA_ARGS__);                \
  func (U16, __VA_ARGS__);               \
  func (U32, __VA_ARGS__);               \
  func (U64, __VA_ARGS__);               \
                                         \
  func (I8, __VA_ARGS__);                \
  func (I16, __VA_ARGS__);               \
  func (I32, __VA_ARGS__);               \
  func (I64, __VA_ARGS__);               \
                                         \
  func (F32, __VA_ARGS__);               \
  func (F64, __VA_ARGS__);               \
                                         \
  func (CF64, __VA_ARGS__);              \
  func (CF128, __VA_ARGS__)

#define IF_STR_CMP_DTYPE(_dtype, in_str)             \
  if (strcmp ((in_str), #_dtype) == 0)               \
    {                                                \
      return (dtype_result){ .v = _dtype, .ok = 1 }; \
    }

#define IF_DTYPE_STR_CMP(_dtype, in_dtype) \
  if (_dtype == in_dtype)                  \
    {                                      \
      return #_dtype;                      \
    }

dtype_result
strtodtype (const char *input)
{
  DTYPE_FOREACH_UC_ARGS (IF_STR_CMP_DTYPE, input);
  return (dtype_result){
    .ok = 0,
  };
}

dtype_v_result
strtodtype_v (const char *str, dtype type)
{
  ASSERT (str);

  char cval;
  uint64_t u64val;
  int isneg = 0;
  double f64val;
  _Complex double cf128val;
  int mult;

  switch (type)
    {
    case CHAR:
      {
        if (str[0] == '\0' || str[1] != '\0')
          goto failed;
        cval = str[0];
        break;
      }
    case CASE_DTYPE_INT:
      {
        if (try_parse_u64_neg (&u64val, &isneg, str))
          goto failed;
        if (!dtype_int_in_range (type, u64val, isneg))
          goto failed;
        break;
      }
    case CASE_DTYPE_FLOAT:
      {
        if (try_parse_f64 (&f64val, str))
          goto failed;
        if (!dtype_float_in_range (type, f64val))
          goto failed;
        break;
      }
    case CASE_DTYPE_COMPLEX:
      {
        if (try_parse_cf128 (&cf128val, str))
          goto failed;
        if (!dtype_cf_in_range (type, cf128val))
          goto failed;
        break;
      }
    }

  mult = isneg ? -1 : 1;

  switch (type)
    {
    case CHAR:
      return (dtype_v_result){ .v = {
                                   .c = cval,
                                   .type = type,
                               },
                               .ok = 1 };
    case U8:
      return (dtype_v_result){ .v = {
                                   .u8 = (uint8_t)u64val,
                                   .type = type,
                               },
                               .ok = 1 };
    case U16:
      return (dtype_v_result){ .v = {
                                   .u16 = (uint16_t)u64val,
                                   .type = type,
                               },
                               .ok = 1 };
    case U32:
      return (dtype_v_result){ .v = {
                                   .u32 = (uint32_t)u64val,
                                   .type = type,
                               },
                               .ok = 1 };
    case U64:
      return (dtype_v_result){ .v = {
                                   .u64 = u64val,
                                   .type = type,
                               },
                               .ok = 1 };
    case I8:
      return (dtype_v_result){ .v = {
                                   .i8 = (int8_t)(mult * u64val),
                                   .type = type,
                               },
                               .ok = 1 };
    case I16:
      return (dtype_v_result){ .v = {
                                   .i16 = (int8_t)(mult * u64val),
                                   .type = type,
                               },
                               .ok = 1 };
    case I32:
      return (dtype_v_result){ .v = {
                                   .i32 = (int32_t)(mult * u64val),
                                   .type = type,
                               },
                               .ok = 1 };
    case I64:
      return (dtype_v_result){ .v = {
                                   .i64 = (int64_t)(mult * u64val),
                                   .type = type,
                               },
                               .ok = 1 };
    case F32:
      return (dtype_v_result){ .v = {
                                   .f32 = (float)f64val,
                                   .type = type,
                               },
                               .ok = 1 };
    case F64:
      return (dtype_v_result){ .v = {
                                   .f64 = f64val,
                                   .type = type,
                               },
                               .ok = 1 };
    case CF64:
      return (dtype_v_result){ .v = {
                                   .cf64 = cf128val,
                                   .type = type,
                               },
                               .ok = 1 };
    case CF128:
      return (dtype_v_result){ .v = {
                                   .cf128 = cf128val,
                                   .type = type,
                               },
                               .ok = 1 };
    }

failed:
  return (dtype_v_result){ .ok = 0 };
}

TEST (strtodtype_v)
{
  dtype_v_result result;

  // Test CHAR parsing
  result = strtodtype_v ("A", CHAR);
  TEST_EQUAL (result.ok, 1, "%d");
  TEST_EQUAL (result.v.c, 'A', "%c");

  result = strtodtype_v ("AB", CHAR); // Invalid CHAR
  TEST_EQUAL (result.ok, 0, "%d");

  // Test Integer Parsing (U8, I8, U16, I16, etc.)
  result = strtodtype_v ("123", U8);
  TEST_EQUAL (result.ok, 1, "%d");
  TEST_EQUAL (result.v.u8, 123, "%u");

  result = strtodtype_v ("-5", I8);
  TEST_EQUAL (result.ok, 1, "%d");
  TEST_EQUAL (result.v.i8, -5, "%d");

  result = strtodtype_v ("256", U8); // Out of range for U8
  TEST_EQUAL (result.ok, 0, "%d");

  result = strtodtype_v ("32768", I16); // Out of range for I16
  TEST_EQUAL (result.ok, 0, "%d");

  // Test Floating-Point Parsing (F32, F64)
  result = strtodtype_v ("3.14", F32);
  TEST_EQUAL (result.ok, 1, "%d");
  TEST_EQUAL (result.v.f32, (float)3.14, "%f");

  result = strtodtype_v ("3.14159", F64);
  TEST_EQUAL (result.ok, 1, "%d");
  TEST_EQUAL (result.v.f64, 3.14159, "%lf");

  result = strtodtype_v ("1e40", F32); // Out of range for F32
  TEST_EQUAL (result.ok, 0, "%d");

  // Test Complex Parsing (CF64, CF128)
  result = strtodtype_v ("1.0+i2.0", CF64);
  TEST_EQUAL (result.ok, 1, "%d");
  TEST_EQUAL (crealf (result.v.cf64), 1.0f, "%f");
  TEST_EQUAL (cimagf (result.v.cf64), 2.0f, "%f");

  result = strtodtype_v ("1.0+i2.0", CF128);
  TEST_EQUAL (result.ok, 1, "%d");
  TEST_EQUAL (creal (result.v.cf128), 1.0, "%lf");
  TEST_EQUAL (cimag (result.v.cf128), 2.0, "%lf");

  result = strtodtype_v ("1e400+i2.0", CF64); // Out of range for CF64
  TEST_EQUAL (result.ok, 0, "%d");
}

int
dtype_int_in_range (dtype type, uint64_t val, int isneg)
{
  switch (type)
    {
    case U8:
      return val <= UINT8_MAX && isneg == 0;
    case U16:
      return val <= UINT16_MAX && isneg == 0;
    case U32:
      return val <= UINT32_MAX && isneg == 0;
    case U64:
      return val <= UINT64_MAX && isneg == 0;
    case I8:
      return (isneg ? val <= (uint64_t)INT8_MAX + 1 : val <= INT8_MAX);
    case I16:
      return (isneg ? val <= (uint64_t)INT16_MAX + 1 : val <= INT16_MAX);
    case I32:
      return (isneg ? val <= (uint64_t)INT32_MAX + 1 : val <= INT32_MAX);
    case I64:
      return (isneg ? val <= (uint64_t)INT64_MAX + 1 : val <= INT64_MAX);
    default:
      unreachable ();
    }
}

TEST (dtype_int_in_range)
{
  TEST_EQUAL (dtype_int_in_range (U8, 255, 0), 1, "%d");
  TEST_EQUAL (dtype_int_in_range (U8, 256, 0), 0, "%d");

  TEST_EQUAL (dtype_int_in_range (U16, 65535, 0), 1, "%d");
  TEST_EQUAL (dtype_int_in_range (U16, 65536, 0), 0, "%d");

  TEST_EQUAL (dtype_int_in_range (I8, 127, 0), 1, "%d");
  TEST_EQUAL (dtype_int_in_range (I8, 128, 0), 0, "%d");
  TEST_EQUAL (dtype_int_in_range (I8, 128, 1), 1, "%d");
  TEST_EQUAL (dtype_int_in_range (I8, 129, 1), 0, "%d");

  TEST_EQUAL (dtype_int_in_range (I16, 32767, 0), 1, "%d");
  TEST_EQUAL (dtype_int_in_range (I16, 32768, 1), 1, "%d");

  TEST_EQUAL (dtype_int_in_range (I32, 2147483647, 0), 1, "%d");
  TEST_EQUAL (dtype_int_in_range (I32, 2147483648ULL, 1), 1, "%d");
}

int
dtype_float_in_range (dtype type, double val)
{
  switch (type)
    {
    case F32:
      return val <= FLT_MAX && val >= -FLT_MAX;
    case F64:
      return val <= DBL_MAX && val >= -DBL_MAX;
    default:
      unreachable ();
    }
}

TEST (dtype_float_in_range)
{
  // Float32
  TEST_EQUAL (dtype_float_in_range (F32, FLT_MAX), 1, "%d");
  TEST_EQUAL (dtype_float_in_range (F32, -FLT_MAX), 1, "%d");
  TEST_EQUAL (dtype_float_in_range (F32, FLT_MAX + 1e30), 0, "%d");

  // Float64
  TEST_EQUAL (dtype_float_in_range (F64, DBL_MAX), 1, "%d");
  TEST_EQUAL (dtype_float_in_range (F64, -DBL_MAX), 1, "%d");
  TEST_EQUAL (dtype_float_in_range (F64, DBL_MAX * 2), 0, "%d");
}

int
dtype_cf_in_range (dtype type, _Complex double val)
{
  switch (type)
    {
    case CF64:
      return creal (val) <= FLT_MAX && creal (val) >= -FLT_MAX && cimag (val) <= FLT_MAX && cimag (val) >= -FLT_MAX;
    case CF128:
      return creal (val) <= DBL_MAX && creal (val) >= -DBL_MAX && cimag (val) <= DBL_MAX && cimag (val) >= -DBL_MAX;
    default:
      unreachable ();
    }
}

TEST (dtype_cf_in_range)
{
  // CF64
  TEST_EQUAL (dtype_cf_in_range (CF64, 1.0 + 1.0 * I), 1, "%d");
  TEST_EQUAL (dtype_cf_in_range (CF64, FLT_MAX + 1.0 * I), 1, "%d");
  TEST_EQUAL (dtype_cf_in_range (CF64, (FLT_MAX + 1e30) + 1.0 * I), 0, "%d");

  // CF128
  TEST_EQUAL (dtype_cf_in_range (CF128, 1.0 + 1.0 * I), 1, "%d");
  TEST_EQUAL (dtype_cf_in_range (CF128, DBL_MAX + 1.0 * I), 1, "%d");
  TEST_EQUAL (dtype_cf_in_range (CF128, (DBL_MAX * 2.0) + 1.0 * I), 0, "%d");
};

static inline void
fill_buff_1 (uint8_t *dest, size_t blen, uint8_t val)
{
  for (int i = 0; i < blen; ++i)
    {
      dest[i] = val;
    }
}

static inline void
fill_buff_2 (uint16_t *dest, size_t len, uint16_t val)
{
  for (int i = 0; i < len; ++i)
    {
      dest[i] = val;
    }
}

static inline void
fill_buff_4 (uint32_t *dest, size_t len, uint32_t val)
{
  for (int i = 0; i < len; ++i)
    {
      dest[i] = val;
    }
}

static inline void
fill_buff_8 (uint64_t *dest, size_t len, uint64_t val)
{
  for (int i = 0; i < len; ++i)
    {
      dest[i] = val;
    }
}

static inline void
fill_buff_16 (_Complex double *dest, size_t len,
              _Complex double val)
{
  for (int i = 0; i < len; ++i)
    {
      dest[i] = val;
    }
}

void
fill_buff (uint8_t *dest, size_t blen, dtype_v value)
{
  ASSERT (blen % dtype_sizeof (value.type) == 0);

  switch (value.type)
    {
    case CHAR:
      fill_buff_1 (dest, blen, value.c);
      return;
    case U8:
      fill_buff_1 (dest, blen, value.u8);
      return;
    case U16:
      fill_buff_2 ((uint16_t *)dest, blen / 2, value.u16);
      return;
    case U32:
      fill_buff_4 ((uint32_t *)dest, blen / 4, value.u32);
      return;
    case U64:
      fill_buff_8 ((uint64_t *)dest, blen / 8, value.u64);
      return;
    case I8:
      fill_buff_1 (dest, blen, (uint8_t)value.i8);
      return;
    case I16:
      fill_buff_2 ((uint16_t *)dest, blen / 2, (uint16_t)value.i16);
      return;
    case I32:
      fill_buff_4 ((uint32_t *)dest, blen / 4, (uint32_t)value.i32);
      return;
    case I64:
      fill_buff_8 ((uint64_t *)dest, blen / 8, (uint64_t)value.i64);
      return;

    case F32:
      {
        uint32_t temp;
        memcpy (&temp, &value.f32, sizeof (temp));
        fill_buff_4 ((uint32_t *)dest, blen / 4, temp);
      }
      return;
    case F64:
      {
        uint64_t temp;
        memcpy (&temp, &value.f64, sizeof (temp));
        fill_buff_8 ((uint64_t *)dest, blen / 8, temp);
      }
      return;
    case CF64:
      {
        double real_part = creal (value.cf64);
        double imag_part = cimag (value.cf64);
        uint64_t real_temp, imag_temp;
        memcpy (&real_temp, &real_part, sizeof (real_temp));
        memcpy (&imag_temp, &imag_part, sizeof (imag_temp));
        fill_buff_8 ((uint64_t *)dest, blen / 8, real_temp);
        fill_buff_8 ((uint64_t *)(dest + blen / 8), blen / 8, imag_temp);
      }
      return;
    case CF128:
      {
        fill_buff_16 ((_Complex double *)dest, blen / 16, value.cf128);
      }
      return;
    }
  unreachable ();
}

size_t
dtype_sizeof (dtype type)
{
  switch (type)
    {
    case CASE_1_BYTE:
      return 1;
    case CASE_2_BYTE:
      return 2;
    case CASE_4_BYTE:
      return 4;
    case CASE_8_BYTE:
      return 8;
    case CASE_16_BYTE:
      return 16;
    }
  unreachable ();
}
