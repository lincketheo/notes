#include "core/posixf.h"

#include <string.h>

FILE *
fifoopen_or_abort (const char *path, const char *mode)
{

  mkfifo_or_abort (path, 0666);

  int flags;
  if (strcmp (mode, "r") == 0 || strcmp (mode, "rb") == 0)
    {
      flags = O_RDONLY;
    }
  else if (strcmp (mode, "w") == 0 || strcmp (mode, "wb") == 0)
    {
      flags = O_WRONLY;
    }
  else if (strcmp (mode, "r+") == 0 || strcmp (mode, "rb+") == 0 || strcmp (mode, "w+") == 0 || strcmp (mode, "wb+") == 0)
    {
      flags = O_RDWR;
    }
  else
    {
      unreachable ();
    }

  int fd = open_or_abort (path, flags);

  return fdopen_or_abort (fd, mode);
}

FILE *
fifoopen (const char *path, const char *mode)
{
  if (mkfifo (path, 0666))
    return NULL;

  int flags;
  if (strcmp (mode, "r") == 0 || strcmp (mode, "rb") == 0)
    {
      flags = O_RDONLY;
    }
  else if (strcmp (mode, "w") == 0 || strcmp (mode, "wb") == 0)
    {
      flags = O_WRONLY;
    }
  else if (strcmp (mode, "r+") == 0 || strcmp (mode, "rb+") == 0 || strcmp (mode, "w+") == 0 || strcmp (mode, "wb+") == 0)
    {
      flags = O_RDWR;
    }
  else
    {
      unreachable ();
    }

  int fd = open (path, flags);

  if (fd == -1)
    return NULL;

  return fdopen (fd, mode);
}
