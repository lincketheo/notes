#ifndef RANDOM_H
#define RANDOM_H

#include <complex.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

uint8_t random_uint8(uint8_t low, uint8_t high);

uint16_t random_uint16(uint16_t low, uint16_t high);

uint32_t random_uint32(uint32_t low, uint32_t high);

uint64_t random_uint64(uint64_t low, uint64_t high);

float random_float(float low, float high);

double random_double(double low, double high);

void random_uint8s(uint8_t* buffer, size_t length, uint8_t low, uint8_t high);

void random_uint16s(uint16_t* buffer, size_t length, uint16_t low, uint16_t high);

void random_uint32s(uint32_t* buffer, size_t length, uint32_t low, uint32_t high);

void random_uint64s(uint64_t* buffer, size_t length, uint64_t low, uint64_t high);

void random_floats(float* buffer, size_t length, float low, float high);

void random_doubles(double* buffer, size_t length, double low, double high);

void random_fixed_points(uint16_t* buffer, size_t length, float low, float high);

void random_complex_uint8s(uint8_t* buffer, size_t length, uint8_t low, uint8_t high);

void random_complex_uint16s(uint16_t* buffer, size_t length, uint16_t low, uint16_t high);

void random_complex_uint32s(uint32_t* buffer, size_t length, uint32_t low, uint32_t high);

void random_complex_uint64s(uint64_t* buffer, size_t length, uint64_t low, uint64_t high);

void random_float_complexes(float complex* buffer, size_t length, float low, float high);

void random_double_complexes(double complex* buffer, size_t length, double low, double high);

#endif
