#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  uint8_t* data;
  size_t len;
} buffer;

buffer read_data_stdin()
{
  size_t capacity = 10;
  size_t len = 0;
  uint8_t* ret = malloc(capacity);
  if(ret == NULL) 
    goto FAILED;

  size_t bytes;
  while((bytes = fread(&ret[len], 1, capacity - len, stdin)) != 0) {
    len += bytes;
    if(capacity - len == 0) {
      uint8_t* new_ret = ret;
      new_ret = realloc(new_ret, capacity * 2);
      if(new_ret == NULL)
        goto FAILED;
      ret = new_ret;
      capacity = 2 * capacity;
    }
  }

  uint8_t *actual_ret = malloc(len);
  if(actual_ret == NULL)
    goto FAILED;
  memcpy(actual_ret, ret, len);
  free(ret);
  return (buffer){ .data = actual_ret, .len = len };

FAILED:
  if(ret) free(ret);
  return (buffer){ .data = NULL, .len = 0 };
}

int process(uint8_t* data, size_t data_l)
{
  for (int i = 0; i < data_l; ++i) {
    printf("%02x", data[i]);
  }
  printf("\n");
  return 0;
}

int main()
{
  buffer buf = read_data_stdin();
  int ret = process(buf.data, buf.len);
  free(buf.data);
  return ret;
}