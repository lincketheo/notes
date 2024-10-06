#include <stdio.h>
#include <complex.h>
#include <stdlib.h>
#include <math.h>

#define SIZE 10000000

complex float* complex_func() {
  complex float* ret = malloc(SIZE * sizeof* ret);
  for(int i = 0; i < SIZE; ++i) {
    ret[i] = cexpf(2 * M_PI * I * 10 * i/1000);
  }
  return ret;
}

int main() {
  complex float* data = complex_func();
  for(int i = 0; i < 1000; ++i) {
    printf("%f %f\n", creal(data[i]), cimag(data[i]));
  }
  free(data);
  return 0;
}
