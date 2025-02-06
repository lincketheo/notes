#include "core/DtypeDatasource.hpp"

#include "core/logging.h"
#include "core/strings.h"

#include "emprobe/usecases.h"

int
main (int argc, char **argv)
{
  float fs, p, f_offset;
  size_t left, right;
  uint32_t seed = 9876;
  int index = 1;
  int i;
  channel_args args;
  FILEDtypeDatasource ifp (sizeof (_Complex float), stdin);
  FILEDtypeDatasource ofp (sizeof (_Complex float), stdout);

  if (argc != 6 && argc != 7)
    {
      goto usage_and_failed;
    }

  i = abs (try_parse_f32_freq_str (&fs, argv[index++]));
  i += abs (try_parse_f32_freq_str (&f_offset, argv[index++]));
  i += abs (try_parse_f32 (&p, argv[index++]));
  i += abs (try_parse_u64 (&left, argv[index++]));
  i += abs (try_parse_u64 (&right, argv[index++]));
  if (argc == 7)
    {
      i += abs (try_parse_u32 (&seed, argv[index]));
    }

  if (i)
    {
      goto usage_and_failed;
    }

  args = (channel_args){
    .awgn_power = p,
    .f_offset = f_offset,
    .fs = fs,
    .left_zero = left,
    .right_zero = right,
    .ifp = &ifp,
    .ofp = &ofp,
    .nfp = NULL,
    .seed = seed,
  };

  usecase_channel (args);
  return 0;

usage_and_failed:
  ns_log (ERROR, "Usage: %s FS FOFFSET POWER LEFT_PADDING RIGHT_PADDING\n", argv[0]);
  return -1;
}
