#pragma once

#include <stdint.h>
#include <stdlib.h>

typedef uint8_t blue_header[512];

int blue_header_init (blue_header b, const char *type, float fs);

void blue_header_set_len (blue_header b, size_t len);
