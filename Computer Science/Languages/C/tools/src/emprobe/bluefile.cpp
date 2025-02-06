#include "emprobe/bluefile.h"

#include "core/errors.h"
#include "core/macros.h"
#include "core/memory.h"

#include <string.h>

static blue_header header_init = {
  'B', 'L', 'U', 'E',     // version
  0, 0, 0, 0,             // head_rep
  0, 0, 0, 0,             // data_rep
  0, 0, 0, 0,             // detached
  0, 0, 0, 0,             // protected
  0, 0, 0, 0,             // pipe
  0, 0, 0, 0,             // ext_start
  0, 0, 0, 0,             // ext_size
  0, 0, 0, 0, 0, 0, 0, 0, // data_start
  0, 0, 0, 0, 0, 0, 0, 0, // data_size
  0, 0, 0, 0,             // type
  'C', 'F',               // format
  0, 0,                   // flagmask
  0, 0, 0, 0, 0, 0, 0, 0, // timecode
  0, 0,                   // inlet
  0, 0,                   // outlets
  0, 0, 0, 0,             // outmask
  0, 0, 0, 0,             // pipeloc
  0, 0, 0, 0,             // pipesize
  0, 0, 0, 0, 0, 0, 0, 0, // in_byte
  0, 0, 0, 0, 0, 0, 0, 0, // out_byte
  0, 0, 0, 0, 0, 0, 0, 0, // outbytes
  0, 0, 0, 0,             // keylength
  ZEROS_92,               // keywords
  ZEROS_256,              // adjunct
};

int
blue_header_init (blue_header b, const char *type, float fs)
{
  if (strlen (type) != 2)
    {
      fprintf (stderr, "Invalid header: %s\n", type);
      return -1;
    }

  memcpy (b, header_init, sizeof header_init);
  memcpy (&b[52], type, 2);
  if (system_is_little_endian ())
    {
      memcpy (&b[4], "EEEI", 4);
      memcpy (&b[8], "EEEI", 4);
    }
  else
    {
      memcpy (&b[4], "IEEE", 4);
      memcpy (&b[8], "IEEE", 4);
    }

  // data start
  *(double *)&b[32] = 512.0;

  // type
  *(uint32_t *)&b[48] = 1000;

  // type 1000 header adjunct
  *(double *)&b[256 + 8] = 1 / fs;
  *(uint32_t *)&b[256 + 16] = 1; // seconds
  return 0;
}

void
blue_header_set_len (blue_header b, size_t len)
{
  *(double *)&b[40] = (double)len;
}
