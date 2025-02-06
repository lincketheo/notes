#include "core/logging.h"
#include "core/stdlibf.h"
#include "core/strings.h"

#include "emprobe/bluefile.h"

#include <stdio.h>
#include <stdlib.h>

int
main (int argc, char **argv)
{
  float fs;
  blue_header header;
  char buffer[1024];
  size_t read_size, total_size = 0;
  FILE *temp_file = tmpfile_or_abort ();
  const char *type = argv[1];

  if (argc != 3)
    {
      goto usage_and_failed;
    }

  if (try_parse_f32_freq_str (&fs, argv[2]))
    {
      goto usage_and_failed;
    }

  if (blue_header_init (header, type, fs))
    {
      goto usage_and_failed;
    }

  while ((read_size = fread_or_abort (buffer, 1, 1024, stdin)) > 0)
    {
      total_size += read_size;
      fwrite_or_abort (buffer, 1, read_size, temp_file);
    }
  blue_header_set_len (header, total_size);

  fwrite_or_abort (header, 1, sizeof (header), stdout);

  rewind (temp_file);
  while ((read_size = fread_or_abort (buffer, 1, 1024, temp_file)) > 0)
    {
      fwrite_or_abort (buffer, 1, read_size, stdout);
    }

  fclose (temp_file);
  return 0;

usage_and_failed:
  ns_log (ERROR, "Usage: %s DTYPE FS\n", argv[0]);
  return -1;
}
