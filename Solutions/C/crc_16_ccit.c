/**
 * Draft work - not reference material
 */



#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * Polynomial = x^16 + x^12 + x^5 + 1 = 10001000000100001 = 0x11021
 * That's 17 bits. So we can "simulate" a 17 bit operation by testing the top bit every time
 * before shifting the crc, ignoring the top bit = 0001000000100001 = 0x1021
 */

#define POLY 0x8408
// #define POLY 0x1021
extern int crc_ok;
int crc_ok = 0x470F;

void print_bits8(uint8_t num)
{
  for (int i = 7; i >= 0; i--) {
    printf("%d", (num >> i) & 1);
  }
}

void print_bits32(uint32_t num)
{
  for (int i = 31; i >= 0; i--) {
    printf("%d", (num >> i) & 1);
  }
}

uint8_t reverse_bits(uint8_t num)
{
  uint8_t reversed_num = 0;
  for (int i = 0; i < 8; i++) {
    reversed_num |= ((num >> i) & 1) << (7 - i);
  }
  return reversed_num;
}

uint16_t calc_CRC16(const uint8_t* _ptr, size_t len_bytes)
{
  uint8_t* ptr = malloc(sizeof *_ptr * len_bytes);
  memcpy(ptr, _ptr, sizeof *_ptr * len_bytes);
  const uint16_t poly = 0x1021;
  uint16_t crc = 0xFFFF;

  for (size_t i = 0; i < len_bytes; ++i) {
    ptr[i] = reverse_bits(ptr[i]);
    crc ^= (uint16_t)ptr[i] << 8;
    for (int j = 0; j < 8; ++j) {
      crc = (crc & 0x8000) ? (crc << 1) ^ poly : (crc << 1);
    }
  }
  return crc;
}

uint16_t crc16(const uint8_t* _data_p, size_t length)
{
  uint8_t* data_p = malloc(sizeof *_data_p * length);
  memcpy(data_p, _data_p, sizeof *_data_p * length);

  uint8_t i;
  uint32_t data;
  uint32_t crc;

  crc = 0xffff;

  if (length == 0)
    return (~crc);

  do {
    i = 0;
    data = (uint32_t)0xff & *data_p; // Just making sure top 32 - 8 bits are 0

    for (; i < 8; i++, data >>= 1) {
      printf("data = Data[i]: ");
      print_bits32(data);
      printf("\n");
      printf("crc: ");
      print_bits32(crc);
      printf("\n");
      printf("crc right bit: %d\n", crc & 0x0001);
      printf("data right bit: %d\n", data & 0x0001);
      printf("xor both: %d\n", (crc & 0x0001) ^ (data & 0x0001));
      if ((crc & 0x0001) ^ (data & 0x0001))
        crc = (crc >> 1) ^ POLY;
      else
        crc >>= 1;
    }

    data_p++;
  } while (--length);

  crc = ~crc;

  data = crc;
  crc = (crc << 8) | (data >> 8 & 0xFF);

  return (crc);
}

int main()
{
  uint8_t data[3] = { 10, 5, 6 };
  for (int i = 0; i < sizeof data; ++i) {
    print_bits8(data[i]);
    printf(" ");
  }
  printf("\n");
  uint16_t crc1 = crc16(data, sizeof data);
  uint16_t crc2 = calc_CRC16(data, sizeof data);
  printf("%x %x\n", crc1, crc2);
  return 0;
}
