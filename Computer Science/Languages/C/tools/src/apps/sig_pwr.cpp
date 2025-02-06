#include "core/DtypeDatasource.hpp"
#include "core/data_structures.h"
#include "core/logging.h"

#include "emprobe/usecases.h"

int
main (int argc, char **argv)
{
  krange k = krange_default ();
  sig_pwrf_args args;
  FILEDtypeDatasource ifs (sizeof (_Complex float), stdin);

  if (argc == 2)
    {
      if (krange_parse_argstr (&k, argv[1]))
        {
          goto usage_and_failed;
        }
    }
  else if (argc != 1)
    {
      goto usage_and_failed;
    }

  args = {
    .ifp = &ifs,
    .k = k,
  };
  usecase_sig_pwrf_full_file_ifp_and_print (args);
  return 0;

usage_and_failed:
  ns_log (ERROR, "Usage: %s [START:END]\n", argv[0]);
  return -1;
}
