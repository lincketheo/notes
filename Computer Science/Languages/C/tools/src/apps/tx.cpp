#include "core/DtypeDatasource.hpp"
#include "core/logging.h"
#include "core/strings.h"

#include "emprobe/usecases.h"

int
main (int argc, char **argv)
{
  float fs, time, p, rutf, rdt0;
  uint32_t seed = 1234;
  tx_args args;
  int i;
  FILEDtypeDatasource ofs (sizeof (_Complex float), stdout);

  if (argc != 6 && argc != 7)
    {
      goto usage_and_failed;
    }

  i = abs (try_parse_f32_freq_str (&fs, argv[1]));
  i += abs (try_parse_f32 (&time, argv[2]));
  i += abs (try_parse_f32 (&p, argv[3]));
  i += abs (try_parse_f32 (&rutf, argv[4]));
  i += abs (try_parse_f32 (&rdt0, argv[5]));
  if (argc == 7)
    {
      i += abs (try_parse_u32 (&seed, argv[6]));
    }

  if (i)
    {
      goto usage_and_failed;
    }

  args = {
    .fs_hz = fs,
    .time_sec = time,
    .ss_power = p,
    .ramp_up_tf = rutf,
    .ramp_down_t0 = rdt0,
    .rut = RU_LINEAR,
    .ofp = &ofs,
    .seed = seed,

  };

  usecase_tx (args);

  return 0;

usage_and_failed:
  ns_log (ERROR, "Usage: %s FS TIME POWER RUTF RDT0\n", argv[0]);
  return -1;
}
