#include "core/DtypeDatasource.hpp"
#include "core/data_structures.h"
#include "core/errors.h"
#include "core/logging.h"
#include "core/stdlibf.h"
#include "core/strings.h"

#include "emprobe/fftcaf.h"
#include "emprobe/usecases.h"

#include <cstdlib>
#include <cstring>

__attribute__ ((noreturn)) void
usage_and_exit (const char *prgrm)
{
  ns_log (ERROR, "Usage: %s DATA_SOURCE SEARCH_LEN"
                 "THRESH FS LITE|NORMAL (NBINS BIN_STRIDE FFT_SIZE)"
                 "|FRANGE\n",
          prgrm);
  exit (-1);
}

int
main (int argc, char **argv)
{
  if (argc != 7 && argc != 9)
    {
      usage_and_exit (argv[0]);
    }

  size_t search_len;
  float thresh;
  fftcaff_type type;

  // NORMAL
  frange f;

  // LITE
  size_t offsets;
  size_t bin_offset;
  float fs;
  size_t fft_size;

  FILE *_data_ifp = fopen_or_abort (argv[1], "rb");
  int i = abs (try_parse_u64 (&search_len, argv[2]));
  i += abs (try_parse_f32 (&thresh, argv[3]));
  i += abs (try_parse_f32_freq_str (&fs, argv[4]));
  if (strcmp (argv[5], "LITE") == 0)
    {
      type = FFTCT_LITE;
      if (argc != 9)
        {
          usage_and_exit (argv[0]);
        }
      i += abs (try_parse_u64 (&offsets, argv[6]));
      i += abs (try_parse_u64 (&bin_offset, argv[7]));
      i += abs (try_parse_u64 (&fft_size, argv[8]));
    }
  else if (strcmp (argv[5], "NORMAL") == 0)
    {
      type = FFTCT_NORMAL;
      if (argc != 7)
        {
          usage_and_exit (argv[0]);
        }
      i += abs (frange_parse_argstr (&f, argv[6]));
    }
  else
    {
      usage_and_exit (argv[0]);
    }

  FILEDtypeDatasource ifp (sizeof (float), stdin);
  FILEDtypeDatasource data_ifp (sizeof (_Complex float), _data_ifp);
  FILEDtypeDatasource data_ofp (sizeof (_Complex float), stdout);

  caf_interpret_args args = (caf_interpret_args){
    .ifp = &ifp,
    .res_ofp = stdout,
    .data_ifp = &data_ifp,
    .data_ofp = &data_ofp,
    .search_len = search_len,
    .thresh = thresh,
    .fs = fs,
    .type = type,
  };

  switch (type)
    {
    case FFTCT_LITE:
      args.params.lite.fft_size = fft_size;
      args.params.lite.offsets = offsets;
      args.params.lite.bin_stride = bin_offset;
      break;
    case FFTCT_NORMAL:
      args.params.normal.f = f;
      break;
    default:
      unreachable ();
    }

  usecase_caf_interpret (args);

  return 0;
}
