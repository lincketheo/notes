#include "core/logging.h"

#include <stdio.h>

#define BUFFER_SIZE 1024
#define MAX_FILES 10

int
main (int argc, char *argv[])
{
  if (argc < 2)
    {
      ns_log (ERROR, "Usage: %s FILE1 [FILE2 ...] (up to %d files)\n", argv[0], MAX_FILES);
      return 1;
    }

  if (argc - 1 > MAX_FILES)
    {
      ns_log (ERROR, "Error: Too many files. Maximum allowed is %d.\n", MAX_FILES);
      return 1;
    }

  FILE *output_files[MAX_FILES];
  int file_count = argc - 1;

  for (int i = 0; i < file_count; i++)
    {
      output_files[i] = fopen (argv[i + 1], "w");
      if (!output_files[i])
        {
          perror ("Failed to open output file");
          for (int j = 0; j < i; j++)
            {
              fclose (output_files[j]);
            }
          return 1;
        }
    }

  char buffer[BUFFER_SIZE];
  size_t read_size;

  while ((read_size = fread (buffer, 1, BUFFER_SIZE, stdin)) > 0)
    {
      fwrite (buffer, 1, read_size, stdout);

      for (int i = 0; i < file_count; i++)
        {
          fwrite (buffer, 1, read_size, output_files[i]);
        }
    }

  if (ferror (stdin))
    {
      perror ("Error reading from stdin");
    }

  for (int i = 0; i < file_count; i++)
    {
      fclose (output_files[i]);
    }

  return 0;
}
