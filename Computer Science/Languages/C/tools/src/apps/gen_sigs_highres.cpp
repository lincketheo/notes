
#include "core/logging.h"
#include "core/stdlibf.h"
#include "core/strings.h"

#include "emprobe/usecases.h"

#include <omp.h>
#include <stdio.h>

typedef struct
{
  float ts;
  float snr_min;
  float snr_max;
  float snr_step;
} trial;

trial trials[] = {
  (trial){
      .ts = 10,
      .snr_min = -70,
      .snr_max = -60,
      .snr_step = 10000,
  },
  (trial){
      .ts = 5,
      .snr_min = -65,
      .snr_max = -50,
      .snr_step = 100000,
  },
};

int
main ()
{
  ns_log (INFO, "Setting number of threads to: %zu\n", 15);
  omp_set_num_threads (15);
  fftwf_init_threads ();
  fftwf_plan_with_nthreads (15);

  char signalfname[2048];
  char matchfname[2048];
  char truncfname[2048];
  char noisefname[2048];

  // const char *base = "/warehouse/tjl/high_res";
  const char *base = ".";

  // Output result file
  FILE *fes = fopen_or_abort ("results.csv", "w");
  gen_sig_row_fprintf_csv_header (fes);

  // Search range
  for (int i = 0; i < sizeof (trials) / sizeof (trial); ++i)
    {
      for (float snrdb = trials[i].snr_min; snrdb < trials[i].snr_max;
           snrdb += trials[i].snr_step)
        {
          float t = trials[i].ts;
          float rutf = 0;
          float rdt0 = t;

          snprintf (matchfname, sizeof (matchfname),
                    "%s/match_%.4f_%.4f_%.4f_%.4f.bin", base, t, rutf, rdt0, snrdb);
          snprintf (signalfname, sizeof (signalfname),
                    "%s/signal_%.4f_%.4f_%.4f_%.4f.bin", base, t, rutf, rdt0, snrdb);
          snprintf (truncfname, sizeof (truncfname),
                    "%s/caftrunc_%.4f_%.4f_%.4f_%.4f.bin", base, t, rutf, rdt0,
                    snrdb);
          snprintf (noisefname, sizeof (noisefname),
                    "%s/noise_%.4f_%.4f_%.4f_%.4f.bin", base, t, rutf, rdt0, snrdb);

          fname_dottop (matchfname);
          fname_dottop (signalfname);
          fname_dottop (truncfname);
          fname_dottop (noisefname);

          gen_sig_row r = usecase_gen_sig ((gen_sig_args){
              // Global
              .fs = 1e7,

              // TX
              .t = t,
              .rutf = rutf,
              .rdt0 = rdt0,
              .match_fname = matchfname,

              // Channel
              .snrdb = snrdb,
              .signal_fname = signalfname,
              .noise_fname = noisefname,

              // CAF
              .nbins = 1,

              // not saving (by choice)
              .cafresult_fname = "/warehouse/tjl/out/cafres.bin",
              .trunc_cafresult_fname = truncfname,

              // Array slice
              .stride = 1000,
          });

          gen_sig_row_fprintf_csv_row (fes, r);
        }
    }

  fclose_or_abort (fes);

  return 0;
}
