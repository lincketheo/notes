#include "utils/random.h"

uint8_t random_uint8(uint8_t low, uint8_t high)
{
  return low + rand() % (high - low + 1);
}

uint16_t random_uint16(uint16_t low, uint16_t high)
{
  return low + rand() % (high - low + 1);
}

uint32_t random_uint32(uint32_t low, uint32_t high)
{
  return low + rand() % (high - low + 1);
}

uint64_t random_uint64(uint64_t low, uint64_t high)
{
  return low + (uint64_t)(rand() % (high - low + 1));
}

float random_float(float low, float high)
{
  return low + ((float)rand() / RAND_MAX) * (high - low);
}

double random_double(double low, double high)
{
  return low + ((double)rand() / RAND_MAX) * (high - low);
}

void random_uint8s(uint8_t* buffer, size_t length, uint8_t low, uint8_t high)
{
  for (size_t i = 0; i < length; ++i) {
    buffer[i] = random_uint8(low, high);
  }
}

void random_uint16s(uint16_t* buffer, size_t length, uint16_t low, uint16_t high)
{
  for (size_t i = 0; i < length; ++i) {
    buffer[i] = random_uint16(low, high);
  }
}

void random_uint32s(uint32_t* buffer, size_t length, uint32_t low, uint32_t high)
{
  for (size_t i = 0; i < length; ++i) {
    buffer[i] = random_uint32(low, high);
  }
}

void random_uint64s(uint64_t* buffer, size_t length, uint64_t low, uint64_t high)
{
  for (size_t i = 0; i < length; ++i) {
    buffer[i] = random_uint64(low, high);
  }
}

void random_floats(float* buffer, size_t length, float low, float high)
{
  for (size_t i = 0; i < length; ++i) {
    buffer[i] = random_float(low, high);
  }
}

void random_doubles(double* buffer, size_t length, double low, double high)
{
  for (size_t i = 0; i < length; ++i) {
    buffer[i] = random_double(low, high);
  }
}

void random_fixed_points(uint16_t* buffer, size_t length, float low, float high)
{
  fprintf(stderr, "Unimplemented\n");
  exit(1);
}

void random_complex_uint8s(uint8_t* buffer, size_t length, uint8_t low, uint8_t high)
{
  for (size_t i = 0; i < length; ++i) {
    buffer[2 * i] = random_uint8(low, high);
    buffer[2 * i + 1] = random_uint8(low, high);
  }
}

void random_complex_uint16s(uint16_t* buffer, size_t length, uint16_t low, uint16_t high)
{
  for (size_t i = 0; i < length; ++i) {
    buffer[2 * i] = random_uint16(low, high);
    buffer[2 * i + 1] = random_uint16(low, high);
  }
}

void random_complex_uint32s(uint32_t* buffer, size_t length, uint32_t low, uint32_t high)
{
  for (size_t i = 0; i < length; ++i) {
    buffer[2 * i] = random_uint32(low, high);
    buffer[2 * i + 1] = random_uint32(low, high);
  }
}

void random_complex_uint64s(uint64_t* buffer, size_t length, uint64_t low, uint64_t high)
{
  for (size_t i = 0; i < length; ++i) {
    buffer[2 * i] = random_uint64(low, high);
    buffer[2 * i + 1] = random_uint64(low, high);
  }
}

void random_float_complexes(float complex* buffer, size_t length, float low, float high)
{
  for (size_t i = 0; i < length; ++i) {
    float real = random_float(low, high);
    float imag = random_float(low, high);
    buffer[i] = real + imag * I;
  }
}

void random_double_complexes(double complex* buffer, size_t length, double low, double high)
{
  for (size_t i = 0; i < length; ++i) {
    double real = random_double(low, high);
    double imag = random_double(low, high);
    buffer[i] = real + imag * I;
  }
}
