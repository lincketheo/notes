#pragma once

typedef enum
{
  DEBUG,
  INFO,
  WARN,
  ERROR,
} log_type;

#ifndef NLOG

void ns_log (log_type type, const char *fmt, ...);

// Logs with \r - call _done when you're done to move on
void ns_log_inl (log_type type, const char *fmt, ...);
void ns_log_inl_done (log_type type);

#else
#define ns_log(...)
#define ns_log_inl(...)
#define ns_log_inl_done(...)
#endif
