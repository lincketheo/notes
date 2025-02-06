#include "core/logging.h"
#include "core/errors.h"

#include <stdarg.h>
#include <stdio.h>

static void
ns_log_prefix (log_type type)
{
#ifdef NDEBUG
  if (type == DEBUG)
    {
      return;
    }
#endif
  switch (type)
    {
    case DEBUG:
      fprintf (stderr, "[DEBUG]: ");
      break;
    case INFO:
      fprintf (stderr, "[INFO]: ");
      break;
    case WARN:
      fprintf (stderr, "[WARN]: ");
      break;
    case ERROR:
      fprintf (stderr, "[ERROR]: ");
      break;
    default:
      break;
    }
}

void
ns_log (log_type type, const char *fmt, ...)
{
#ifdef NDEBUG
  if (type == DEBUG)
    {
      return;
    }
#endif
  ns_log_prefix (type);
  va_list args;
  va_start (args, fmt);
  vfprintf (stderr, fmt, args);
  va_end (args);
}

void
ns_log_inl (log_type type, const char *fmt, ...)
{
#ifdef NDEBUG
  if (type == DEBUG)
    {
      return;
    }
#endif
  fprintf (stderr, "\r");
  ns_log_prefix (type);
  va_list args;
  va_start (args, fmt);
  vfprintf (stderr, fmt, args);
  va_end (args);
}

void
ns_log_inl_done (log_type type)
{
#ifdef NDEBUG
  if (type == DEBUG)
    {
      return;
    }
#endif
  fprintf (stderr, "\n");
}
