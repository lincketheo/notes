#include "bit_serialize.h"
#include <assert.h>
#include <stdio.h>

#if defined(__cplusplus)
extern "C" {
#endif

int bits_data_init(bits_data* data)
{
  assert(data);
  assert(!data->data); // Questionable - someone might not zero out

  data->data_len_bits = 0;
  data->byte_i = 0;
  data->data_capacity_bytes = STARTING_CAPACITY_BYTES;
  data->data = calloc(data->data_capacity_bytes, sizeof *data->data);

  return 0;
}

void bits_data_free(bits_data* data)
{
  if (data) {
    if (data->data)
      free(data->data);
    data->data = NULL;
  }
}

/**
static void print_bits(uint8_t num)
{
  for (int i = 7; i >= 0; --i) {
    printf("%d", (num >> i) & 1);
  }
  printf("\n");
}
*/

/**
 * Gets uint8 such that bit [bit] is at the far left.
 * If [bit] < 8, then fills the right with zeros
 */
static uint8_t get_u8_bit_left(
    const uint64_t _elem,
    const size_t bit)
{
  if (bit < 8) {
    return _elem << (8 - bit) & 0xFF;
  } else {
    return _elem >> (bit - 8);
  }
}

static int double_data_capacity(bits_data* data)
{
  uint8_t* data_prev = data->data;
  size_t new_capacity = data->data_capacity_bytes * 2;
  data->data = realloc(data->data, new_capacity * sizeof *data->data);
  if (data->data == NULL) {
    data->data = data_prev;
    printf("Failed to double size of data\n");
    return -1;
  }
  data->data_capacity_bytes = new_capacity;
  return 0;
}

static int push_u8(
    bits_data* data,
    const uint8_t num,
    const size_t nbits)
{
  if ((data->data_len_bits + 8) >= data->data_capacity_bytes * 8)
    if (double_data_capacity(data))
      return -1;

  size_t tail_filled = data->data_len_bits % 8;

  if (tail_filled == 0) {
    data->data[data->byte_i] = num;
    data->byte_i += (nbits == 8);
    data->data_len_bits += nbits;
    return 0;
  }

  data->data[data->byte_i] |= num >> tail_filled;
  data->data[data->byte_i + 1] = num << (8 - tail_filled);

  if ((data->data_len_bits + nbits) / 8 != data->data_len_bits / 8)
    data->byte_i++;

  data->data_len_bits += nbits;
  return 0;
}

int push(bits_data* data, const uint64_t _elem, const size_t _nbits)
{
  assert(_elem >> _nbits == 0 || _nbits == 64);

  int nbits = _nbits;

  while (nbits > 0) {
    uint8_t dat = get_u8_bit_left(_elem, nbits);
    push_u8(data, dat, nbits >= 8 ? 8 : nbits);
    nbits -= 8;
  }

  return -1;
}

uint64_t get_data(const bits_data* data, const size_t _start, const size_t _nbits)
{
  assert(_nbits <= 64);
  uint64_t ret = 0;

  size_t nbits = _nbits;
  size_t start = _start;

  size_t take = 8 - (_start % 8);
  take = take > nbits ? nbits : take;

  if (start / 8 == (start + take) / 8) {
    uint8_t mask = ~(~0 << (8 - start % 8));
    mask &= (~0 << (8 - start % 8 - take));
    uint8_t byte = data->data[start / 8] & mask;
    ret |= byte >> (8 - start % 8 - take);
    return ret;
  }

  if (take > 0) {
    uint8_t byte = data->data[start / 8] & ~(~0 << take);
    ret |= ((uint64_t)byte & 0xFF) << (nbits - take);
    start += take;
    nbits -= take;
    take = 8; //- (start % 8);
    take = take > nbits ? nbits : take;
  }

  while (take == 8) {
    uint8_t byte = data->data[start / 8];
    ret |= ((uint64_t)byte & 0xFF) << (nbits - take);
    start += take;
    nbits -= take;
    take = take > nbits ? nbits : take;
  }

  if (take) {
    uint8_t byte = data->data[start / 8] & (~0 >> take);
    byte >>= (8 - take);
    ret |= ((uint64_t)byte & 0xFF);
  }

  return ret;
}

int unpack(bits_data* data, uint8_t* str, size_t len)
{
  assert(len == data->data_len_bits);
  ssize_t byte = -1;
  for (int i = 0; i < data->data_len_bits; ++i) {
    size_t bit = i % 8;
    if (bit == 0)
      byte++;
    uint8_t dat = data->data[byte];
    str[i] = (dat >> (7 - bit)) & 1;
  }
  return -1;
}

int bitn(bits_data* data, size_t n)
{
  return -1;
}

#if defined(__cplusplus)
}
#endif
