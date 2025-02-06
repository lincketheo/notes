#include "core/DtypeDatasource.hpp"
#include "core/logging.h"
#include "core/strings.h"

#include "emprobe/usecases.h"

#include "core/stdlibf.h"

__attribute__ ((noreturn)) void
usage_and_exit (const char *prgrm)
{
  ns_log (ERROR, "Usage: %s MATCH_FILE OFFSETS BIN_STRIDE\n", prgrm);
  exit (-1);
}

int
main (int argc, char **argv)
{
  if (argc != 4)
    {
      usage_and_exit (argv[0]);
    }

  size_t offsets;
  size_t bin_stride;
  int i = abs (try_parse_u64 (&offsets, argv[2]));
  i += abs (try_parse_u64 (&bin_stride, argv[3]));

  if (i)
    {
      usage_and_exit (argv[0]);
    }

  FILE *mfp = fopen_or_abort (argv[1], "rb");
  fftcaff_ifp_ofp_args args;
  FILEDtypeDatasource mfs (sizeof (_Complex float), mfp);
  FILEDtypeDatasource dfs (sizeof (_Complex float), stdin);
  FILEDtypeDatasource ofs (sizeof (float), stdout);

  args = (fftcaff_ifp_ofp_args){
    .params = {
        .lite = {
            .offsets = offsets,
            .bin_stride = bin_stride,
        },
    },
    .type = FFTCT_LITE,
    .samp_range = (krange){
        .k0 = 0,
        .isinf = 1,
    },
    .match_ifp = &mfs,
    .data_ifp = &dfs,
    .ofp = &ofs,
    .fout = fftcaff_fout_create (),
    .compute_fabs = 1,
    .mmap_base = ".",
  };

  usecase_fftcaff_ifp_ofp (args);

  return 0;
}
