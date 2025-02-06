#pragma once

#include <complex.h>
#include <stdint.h>
#include <stdlib.h>

/**
 * A data type enum for basic C data types
 */
typedef enum
{
  CHAR,

  U8,
  U16,
  U32,
  U64,

  I8,
  I16,
  I32,
  I64,

  F32,
  F64,

  CF64,
  CF128,
} dtype;

typedef struct
{
  dtype v;
  int ok;
} dtype_result;

/*
 * A dtype value - a tagged union
 * beware using this extensively (e.g.
 * for more than one value) because it doesn't
 * takes up the space of the largest dtype and type enum
 */
typedef struct
{
  union
  {
    char c;
    uint8_t u8;
    uint16_t u16;
    uint32_t u32;
    uint64_t u64;
    int8_t i8;
    int16_t i16;
    int32_t i32;
    int64_t i64;
    float f32;
    double f64;
    _Complex float cf64;
    _Complex float cf128;
  };
  dtype type;
} dtype_v;

typedef struct
{
  dtype_v v;
  int ok;
} dtype_v_result;

/**
 * Various macros for similar case types. For example
 * case CASE_DTYPE_INT: {
 *   // do stuff
 *   break;
 * }
 * covers all int cases
 */

// All the (un)signed integer types
#define CASE_DTYPE_INT \
  U8:                  \
  case U16:            \
  case U32:            \
  case U64:            \
  case I8:             \
  case I16:            \
  case I32:            \
  case I64

// All the float types
#define CASE_DTYPE_FLOAT \
  F32:                   \
  case F64

// All the complex types
#define CASE_DTYPE_COMPLEX \
  CF64:                    \
  case CF128

// All the types with 1 byte
#define CASE_1_BYTE \
  CHAR:             \
  case I8:          \
  case U8

// All the types with 2 bytes
#define CASE_2_BYTE \
  I16:              \
  case U16

// All the types with 4 bytes
#define CASE_4_BYTE \
  I32:              \
  case U32:         \
  case F32

// All the types with 8 bytes
#define CASE_8_BYTE \
  I64:              \
  case U64:         \
  case F64:         \
  case CF64

// All the types with 16 bytes
#define CASE_16_BYTE CF128

/**
 * @brief Converts a null terminated string to a datatype value (Think "0.123", "1.1+i3")
 *
 * @param str - The null terminated string argument
 * @param type - The type to interpret this string as
 * @return - A datatype value result
 */
dtype_v_result strtodtype_v (const char *str, dtype type);

/**
 * @brief Converts a null terminated string to a datatype (Think "CF64", "U8")
 *
 * @param input The null terminated string argument
 * @return - A datatype result
 */
dtype_result strtodtype (const char *input);

/**
 * @brief All int data types are between -UINT64 and UINT64 MAX. Smaller dtypes are constrained
 * differently, for example, U32 is between 0 and U32 MAX. Checks if the provided U64 (which is
 * necessarily big enough to hold any dtype) is in range of [type]
 */
int dtype_int_in_range (dtype type, uint64_t val, int isneg);

/**
 * @brief same as dtype_int_in_range but for floats
 */
int dtype_float_in_range (dtype type, double val);

/**
 * @brief same as dtype_int_in_range but for complex floats
 */
int dtype_cf_in_range (dtype type, _Complex double val);

/**
 * @brief Fills a buffer of bytes with a value
 *
 * @param dest The destination array of length blen bytes
 * @param blen The length (in bytes, not elements) - length must be a multiple of dtype size
 * @param value The value to fill with
 */
void fill_buff (uint8_t *dest, size_t blen, dtype_v value);

/**
 * @brief The sizeof an dtype in bytes. Like sizeof, but programatic
 */
size_t dtype_sizeof (dtype type);
