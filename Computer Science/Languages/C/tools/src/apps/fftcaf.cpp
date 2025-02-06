#include "core/DtypeDatasource.hpp"
#include "core/data_structures.h"
#include "core/errors.h"
#include "core/logging.h"
#include "core/stdlibf.h"
#include "core/strings.h"

#include "emprobe/fftcaf.h"
#include "emprobe/usecases.h"

#include <fftw3.h>

__attribute__ ((noreturn)) void
usage_and_exit (const char *prgrm)
{
  ns_log (ERROR, "Usage: %s MATCH FRANGE FS\n", prgrm);
  exit (-1);
}

int
main (int argc, char **argv)
{
  frange f;
  float fs;

  if (argc != 4)
    {
      usage_and_exit (argv[0]);
      unreachable ();
    }

  FILEDtypeDatasource mfs (
      sizeof (_Complex float),
      fopen_or_abort (argv[1], "rb"));
  FILE *ifp = stdin;
  FILE *ofp = stdout;
  FILEDtypeDatasource dfs (sizeof (_Complex float), ifp);
  FILEDtypeDatasource ofs (sizeof (float), ofp);
  int i = frange_parse_argstr (&f, argv[2]);
  i += abs (try_parse_f32_freq_str (&fs, argv[3]));

  if (i)
    {
      usage_and_exit (argv[0]);
      unreachable ();
    }

  fftcaff_ifp_ofp_args args = {
    .params = {
        .normal = {
            .f = f,
            .fs = fs,
        },
    },
    .type = FFTCT_NORMAL,
    .samp_range = (krange){
        .k0 = 0,
        .isinf = 1,
    },
    .match_ifp = &mfs,
    .data_ifp = &dfs,
    .ofp = &ofs,
    .fout = fftcaff_fout_create (),
    .compute_fabs = 1,
    .mmap_base = NULL,
  };

  usecase_fftcaff_ifp_ofp (args);

  return 0;
}
