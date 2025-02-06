#include "emprobe/usecases.h"

#include "core/DtypeDatasource.hpp"
#include "core/data_structures.h"
#include "core/dtype.h"
#include "core/errors.h"
#include "core/logging.h"
#include "core/macros.h"
#include "core/math.h"
#include "core/memory.h"
#include "core/stdlibf.h"
#include "core/testing.h"

#include "emprobe/fftcaf.h"
#include "emprobe/fftcaf_shift_reader.h"

#include <cmath>
#include <complex.h>
#include <float.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <time.h>

#ifndef NOMP
#include <omp.h>
#endif

#define MAX_ML 9999999999999999

///////////////////////////////////////////////////////
///////// Testing utilities

static void
test_generate_tx (
    const char *fname,
    float fs,
    float t,
    float ss_power,
    float rutf,
    float rdt0)
{
  tx_args tx;
  FILE *fp = fopen_or_abort (fname, "wb");
  FILEDtypeDatasource fds (sizeof (_Complex float), fp);
  tx_args args = {
    .fs_hz = fs,
    .time_sec = t,
    .ss_power = ss_power,
    .ramp_up_tf = rutf,
    .ramp_down_t0 = rdt0,
    .rut = RU_LINEAR,
    .ofp = &fds,
    .seed = 1234,
  };
  usecase_tx (args);
  fclose_or_abort (fp);
}

static float
test_run_sigpwr_usecase (const char *fname, krange k)
{
  FILE *ifp = fopen_or_abort (fname, "rb");
  FILEDtypeDatasource fds (sizeof (_Complex float), ifp);
  float ret = usecase_sig_pwrf_full_file_ifp ((sig_pwrf_args){
      .ifp = &fds,
      .k = k,
  });
  fclose_or_abort (ifp);
  return ret;
}

static cf64_buffer
test_read_malloc (const char *fname)
{
  FILE *fp = fopen_or_abort (fname, "rb");
  FILEDtypeDatasource fds (sizeof (_Complex float), fp);
  bytes b = dtypeds_read_all_malloc (&fds);
  fclose_or_abort (fp);
  return cf64_buffer_from_bytes (b);
}

static void
test_generate_channel (const char *ifname, const char *ofname,
                       const char *nfname, float awgn_power,
                       float f_offset, float fs, size_t left_zero,
                       size_t right_zero, uint32_t seed)
{
  FILE *ifp = fopen_or_abort (ifname, "rb");
  FILE *ofp = fopen_or_abort (ofname, "wb");
  FILE *nfp = NULL;

  if (nfname)
    {
      nfp = fopen_or_abort (nfname, "wb");
    }

  FILEDtypeDatasource ifpds (sizeof (_Complex float), ifp);
  FILEDtypeDatasource ofpds (sizeof (_Complex float), ofp);
  FILEDtypeDatasource nfpds (sizeof (_Complex float), nfp);

  channel_args ch = {
    .awgn_power = awgn_power,
    .f_offset = f_offset,
    .fs = fs,
    .left_zero = left_zero,
    .right_zero = right_zero,
    .ifp = &ifpds,
    .ofp = &ofpds,
    .nfp = &nfpds,
    .seed = seed,
  };

  usecase_channel (ch);
  fclose_or_abort (ifp);
  fclose_or_abort (ofp);
  fclose_or_abort (nfp);
}

///////////////////////////////////////////////////////
///////// TX

void
usecase_tx (tx_args args)
{
  srand (args.seed);

  // The temp buffer to write out to
  _Complex float out_buffer[2048];

  const size_t samples = args.fs_hz * args.time_sec;
  size_t rukf, rdk0;

  // Get start / stop time for ramp up / down
  if (ru_present (args))
    {
      rukf = args.fs_hz * args.ramp_up_tf;
      ns_log (DEBUG,
              "TX Ramp up last sample "
              "(%f seconds at %f Hz) is %zu\n",
              args.ramp_up_tf, args.fs_hz, rukf);
    }

  if (rd_present (args))
    {
      rdk0 = args.fs_hz * args.ramp_down_t0;
      ns_log (DEBUG,
              "TX Ramp down starting sample "
              "(%f seconds at %f Hz) is %zu\n",
              args.ramp_down_t0, args.fs_hz, rdk0);
    }

  size_t k = 0;

  float std = cf64_power_to_f32_std (args.ss_power);

  while (k < samples)
    {
      size_t next_samps = MIN (samples - k, 2048);

      gaussf_vec ((float *)out_buffer, next_samps * 2, std, 0);

      // Heads up, r[u|d]_present isn't in the "beefy computation area"
      // so I'm comfortable calling it for each buffer
      if (ru_present (args))
        {
          signal_ramp_up (out_buffer, k, next_samps, rukf, RU_LINEAR);
        }
      if (rd_present (args))
        {
          signal_ramp_down (out_buffer, k, next_samps, rdk0, samples, RU_LINEAR);
        }

      args.ofp->write (out_buffer, next_samps);

      k += next_samps;

      ns_log_inl (
          DEBUG,
          "Samples generated: %zu/%zu. Signal Time: %f",
          k,
          samples,
          roundf ((float)k / args.fs_hz));

      ASSERT (k <= samples);
    }
  ns_log_inl_done (DEBUG);
}

TEST (usecase_tx)
{
  test_generate_tx ("test.bin", 1000, 2, 1, NAN, NAN);
  cf64_buffer sig = test_read_malloc ("test.bin");
  float power = signal_powerf (sig.data, sig.len);

  TEST_EQUAL (sig.len, (size_t)(1000 * 2), "%zu");
  TEST_EQUAL (fabs (power - 1) < 0.05, 1, "%d");

  safe_free (sig.data);
  remove ("test.bin");
}

///////////////////////////////////////////////////////
///////// Channel

// Writes just awgn to an output file [ofp]
static void
fwrite_awgn_cf64 (
    DtypeDatasource *ofp,
    size_t samples,
    float power)
{
  ASSERT (ofp->elem_size == sizeof (_Complex float));

  _Complex float buffer[2048];
  const size_t _samples = samples;

  while (samples > 0)
    {
      size_t next = MIN (samples, 2048);
      gen_awgn_from_power (buffer, next, power);
      ASSERT (next <= samples);
      ofp->write (buffer, next);
      samples -= next;
      ns_log_inl (
          DEBUG,
          "Generating 0 signal. Samples: %zu/%zu",
          _samples - samples,
          _samples);
    }

  ns_log_inl_done (DEBUG);
}

// Writes just awgn to 2 output files [ofp1, ofp2]
static void
fwrite2_awgn_cf64 (
    DtypeDatasource *ofp1,
    DtypeDatasource *ofp2,
    size_t samples,
    float power)
{
  ASSERT (ofp1->elem_size == sizeof (_Complex float));
  ASSERT (ofp2->elem_size == sizeof (_Complex float));

  _Complex float buffer[2048];
  const size_t _samples = samples;

  while (samples > 0)
    {
      size_t next = MIN (samples, 2048);
      gen_awgn_from_power (buffer, next, power);
      ASSERT (next <= samples);
      ofp1->write (buffer, next);
      ofp2->write (buffer, next);
      samples -= next;
      ns_log_inl (
          DEBUG,
          "Generating 0 signal. Samples: %zu/%zu",
          _samples - samples,
          _samples);
    }

  ns_log_inl_done (DEBUG);
}

static void
fcopy_cf64_channel (
    DtypeDatasource *ifp,
    DtypeDatasource *ofp,
    float noise_power,
    float f_offset,
    float fs)
{
  ASSERT (ifp->elem_size == sizeof (_Complex float));
  ASSERT (ofp->elem_size == sizeof (_Complex float));

  _Complex float buffer[2048];
  _Complex float noise[2048];
  _Complex float phasor = 1.0f;
  size_t total = 0;

  do
    {
      size_t read = ifp->read (buffer, 2048);
      total += read;

      if (read > 0)
        {
          gen_awgn_from_power (noise, read, noise_power);
          cf64_add_inl (buffer, noise, read);
          phasor = cf64_tune_inl (buffer, read, f_offset, fs, phasor);
          ofp->write (buffer, read);
          ns_log_inl (DEBUG, "Adding awgn to signal. Samples: %zu", total);
        }
    }
  while (!ifp->done ());

  ns_log_inl_done (DEBUG);
}

static void
fcopy_and_isol_noise_cf64_channel (
    DtypeDatasource *ifp,
    DtypeDatasource *sofp,
    DtypeDatasource *nofp,
    float noise_power,
    float f_offset,
    float fs)
{
  ASSERT (ifp->elem_size == sizeof (_Complex float));
  ASSERT (sofp->elem_size == sizeof (_Complex float));
  ASSERT (nofp->elem_size == sizeof (_Complex float));

  _Complex float buffer[2048];
  _Complex float noise[2048];
  _Complex float phasor = 1.0f;
  size_t total = 0;

  do
    {
      size_t read = ifp->read (buffer, 2048);
      total += read;

      if (read > 0)
        {
          gen_awgn_from_power (noise, read, noise_power);
          cf64_add_inl (buffer, noise, read);
          phasor = cf64_tune_inl (buffer, read, f_offset, fs, phasor);
          sofp->write (buffer, read);
          nofp->write (noise, read);
          ns_log_inl (DEBUG, "Adding awgn to signal. Samples: %zu", total);
        }
    }
  while (!ifp->done ());

  ns_log_inl_done (DEBUG);
}

void
usecase_channel (channel_args args)
{
  srand (args.seed);

  // Write out to noise and signal file (seperate file for noise)
  // But it's the same noise as applied to the signal
  if (args.nfp != NULL)
    {
      fwrite2_awgn_cf64 (
          args.ofp,
          args.nfp,
          args.left_zero,
          args.awgn_power);

      fcopy_and_isol_noise_cf64_channel (
          args.ifp,
          args.ofp,
          args.nfp,
          args.awgn_power,
          args.f_offset,
          args.fs);

      fwrite2_awgn_cf64 (
          args.ofp,
          args.nfp,
          args.right_zero,
          args.awgn_power);
    }
  else
    {
      fwrite_awgn_cf64 (
          args.ofp,
          args.left_zero,
          args.awgn_power);

      fcopy_cf64_channel (
          args.ifp,
          args.ofp,
          args.awgn_power,
          args.f_offset,
          args.fs);

      fwrite_awgn_cf64 (
          args.ofp,
          args.right_zero,
          args.awgn_power);
    }
}

TEST (usecase_channel)
{
  test_generate_tx ("test.bin", 1000, 2, 1, NAN, NAN);
  cf64_buffer match = test_read_malloc ("test.bin");
  float power = signal_powerf (match.data, match.len);

  float snrdb = -61.123;
  float awgn_power = snrdb_to_awgn_power (snrdb, power);

  test_generate_channel ("test.bin", "test.signal.bin", "test.noise.bin",
                         awgn_power, 0, 1000, 100, 1123, 1234);

  cf64_buffer signal = test_read_malloc ("test.signal.bin");
  cf64_buffer noise = test_read_malloc ("test.noise.bin");
  float _awgn_power = signal_powerf (noise.data, noise.len);

  TEST_EQUAL (signal.len, (size_t)(1000 * 2 + 100 + 1123), "%zu");
  TEST_EQUAL (noise.len, (size_t)(1000 * 2 + 100 + 1123), "%zu");
  TEST_FLOAT_EQUAL (10 * log10f (power / _awgn_power), snrdb, 0.5);

  safe_free (match.data);
  safe_free (signal.data);
  safe_free (noise.data);
  remove ("test.bin");
  remove ("test.noise.bin");
  remove ("test.signal.bin");
}

///////////////////////////////////////////////////////
///////// sig_pwr

float
usecase_sig_pwrf_full_file_ifp (sig_pwrf_args args)
{
  bytes data = dtypeds_read_range (args.ifp, args.k);
  if (data.len == 0)
    {
      return 0.0f;
    }

  float power = signal_powerf (
      (_Complex float *)data.data,
      data.len / sizeof (_Complex float));
  safe_free (data.data);

  return power;
}

TEST (usecase_sig_pwrf_full_file_ifp)
{
  test_generate_tx ("test.bin", 1000, 2, 1, NAN, NAN);
  cf64_buffer sig = test_read_malloc ("test.bin");

  float power = signal_powerf (sig.data, sig.len);
  float act_power = test_run_sigpwr_usecase (
      "test.bin",
      (krange){
          .k0 = 0,
          .isinf = 1,
      });

  TEST_FLOAT_EQUAL (power, act_power, 0);

  safe_free (sig.data);
  remove ("test.bin");
}

void
usecase_sig_pwrf_full_file_ifp_and_print (sig_pwrf_args args)
{
  float power = usecase_sig_pwrf_full_file_ifp (args);
  fprintf (stdout, "%f\n", power);
}

///////////////////////////////////////////////////////
///////// FFTCAFF

typedef struct
{
  fftcaff_create_args fargs;
  DtypeDatasource *ifp;
  DtypeDatasource *ofp;
  fftcaff_fout fout;
  krange samp_range;
  int compute_abs;
  size_t nthreads;
  const char *mmap_base;
} fftcaff_common_args;

static const cf64_buffer
read_match (DtypeDatasource *mfp)
{
  ns_log (DEBUG, "Reading match file from disk\n");

  bytes match = dtypeds_read_all_malloc (mfp);
  ASSERT (match.len % sizeof (_Complex float) == 0);

  size_t sf = match.len / sizeof (_Complex float);
  ns_log (DEBUG, "Match len is: %zu\n", sf);

  if (!is_power_of_2 (sf))
    {
      size_t newlen = next_2_power (sf);
      bytes_resize_and_zero (&match, newlen * sizeof (_Complex float));
      ns_log (DEBUG, "Match len extended to: %zu\n", newlen);
    }

  return cf64_buffer_from_bytes (match);
}

static void
free_match (const cf64_buffer *match)
{
  safe_free (match->data);
}

#define FFTC_MEASURE_TIME

static int
usecase_fftcaff_ifp_ofp_common (fftcaff_common_args args)
{
  dtypeds_read_discard (args.ifp, (size_t)args.samp_range.k0);
  fftcaff_shift_reader caf = fftcaff_shift_reader_create (
      args.fargs,
      args.fout,
      args.compute_abs);
  free_match (&args.fargs.match);

  size_t kleft;
  size_t avail;
  size_t ml = fftcsml (&caf);

  while (1)
    {
#ifdef FFTC_MEASURE_TIME
      TIMER_START ();
#endif

      fftcaff_shift_reader_load_max (&caf, args.ifp);
      kleft = krange_size (args.samp_range);
      avail = fftcsavail (&caf);

      if (kleft == 0 || avail == 0)
        {
          break;
        }

      size_t towrite = MIN3 (kleft, avail, ml / 2);
      fftcaff_shift_reader_run (&caf, args.ofp, towrite);
      args.samp_range.k0 += towrite;

#ifdef FFTC_MEASURE_TIME
      double time = GET_ELLAPSED ();
      float fsproc = (float)towrite / time;
      ns_log (INFO, "FFTCAFF Processing speed: %f MHz\n", fsproc / MHz (1));
#endif
    }

  ns_log (DEBUG, "Freeing resources from FFTCAFF\n");
  fftcaff_shift_reader_free (caf);

  return 0;
}

int
usecase_fftcaff_ifp_ofp (fftcaff_ifp_ofp_args args)
{
  const cf64_buffer match = read_match (args.match_ifp);
  fftcaff_create_args fargs = { 0 };

  switch (args.type)
    {
    /////////////////////////// LITE
    case FFTCT_LITE:
      {
        fftcaff_create_args _fargs = {
          .match = match,
          .params = {
              .lite = {
                  .f_offsets = args.params.lite.offsets,
                  .bin_stride = args.params.lite.bin_stride,
                  .zero_middle = args.params.lite.zero_middle,
              },
          },
          .type = FFTCT_LITE,
          .mmap_base = args.mmap_base,
        };
        memcpy (&fargs, &_fargs, sizeof (fargs));
        break;
      }

    /////////////////////////// NORMAL
    case FFTCT_NORMAL:
      {
        fftcaff_create_args _fargs = {
          .match = match,
          .params = {
              .normal = {
                  .f = args.params.normal.f,
                  .fs = args.params.normal.fs,
              },
          },
          .type = FFTCT_NORMAL,
          .mmap_base = args.mmap_base,
        };
        memcpy (&fargs, &_fargs, sizeof (fargs));
        break;
      }
    default:
      unreachable ();
    }

  return usecase_fftcaff_ifp_ofp_common ((fftcaff_common_args){
      .fargs = fargs,
      .ifp = args.data_ifp,
      .ofp = args.ofp,
      .fout = args.fout,
      .samp_range = args.samp_range,
      .compute_abs = args.compute_fabs,
#ifndef NOMP
      .nthreads = args.nthreads,
#endif
      .mmap_base = args.mmap_base,
  });
}

///////////////////////////////////////////////////////
///////// RX

class RX;

class RXState
{
public:
  virtual void init () {};
  virtual void execute () = 0;
};

class RX
{
public:
  void
  change_state (RXState *newstate)
  {
    safe_free (state);
    state = newstate;
  }

  void
  init ()
  {
    state->init ();
  }

  void
  execute ()
  {
    state->execute ();
  }

private:
  RXState *state;
};

class RXEnd : public RXState
{
  RXEnd () {}

  virtual void
  execute ()
  {
    exit (1);
  }
};

typedef struct rx_search_state_args_s
{
  RX *rx;
  DtypeDatasource *ifp;
  DtypeDatasource *mfp;
  fftcaff_create_args cargs;
  trange t;
  float fs;
  float thresh;

  void
  populate_match ()
  {
    ASSERT (cargs.match.len == 0 && cargs.match.data == NULL);
    cargs.match = read_match (mfp);
  }

  void
  free_match ()
  {
    safe_free (cargs.match.data);
  }
} rx_search_state_args;

class RXSearchState : RXState
{
  RXSearchState (rx_search_state_args args)
      : rx (args.rx),
        ifp (args.ifp),
        caf ({ 0 }),
        samp_range (trange_to_krange (args.t, args.fs)),
        thresh (args.thresh)
  {
    args.populate_match ();

    // Create the stateful caf
    fftcaff_shift_reader _caf = fftcaff_shift_reader_create (
        args.cargs,
        fftcaff_fout_create (),
        1);

    args.free_match ();

    memcpy (&caf, &_caf, sizeof (caf));
  }

  ~RXSearchState ()
  {
    fftcaff_shift_reader_free (caf);
  }

  virtual void execute () override;

private:
  void
  transition_end ()
  {
    RXEnd *next = (RXEnd *)malloc_or_abort (sizeof *next);
    rx->change_state (next);
    unreachable ();
  }

  size_t
  next_max_shift_length ()
  {
    size_t kleft = krange_size (samp_range);
    size_t avail = fftcsavail (&caf);
    size_t ml = fftcsml (&caf);
    return MIN3 (kleft, avail, ml / 2);
  }

  ssize_t
  find_peak ()
  {
    size_t kf = next_max_shift_length ();

    float *corrs = (float)caf.corrs_out.f32_corrs_T;
    for (size_t k = 0; k < kf; ++k)
      {
      }
  }

  size_t
  shift ()
  {
    fftcaff_shift_reader_shift (&caf, shift);

    if (kleft == 0 || avail == 0)
      {
        ns_log (WARN, "Reached the end in first CAF\n");
        transition_end ();
        unreachable ();
      }

    samp_range.k0 += shift;
    return shift;
  }

  RX *rx;
  DtypeDatasource *ifp;
  fftcaff_shift_reader caf;
  krange samp_range;
  float thresh;
};

void
RXSearchState::execute ()
{
  while (1)
    {
#ifdef FFTC_MEASURE_TIME
      TIMER_START ();
#endif

      fftcaff_shift_reader_load_max (&caf, ifp);

      fftcaff_shift_reader_run (&caf);

      // Search for peak

      // No peak found, shift
      size_t _shift = shift ();

#ifdef FFTC_MEASURE_TIME
      double time = GET_ELLAPSED ();
      float fsproc = (float)_shift / time;
      ns_log (
          INFO,
          "FFTCAFF 1 Processing speed: %f MHz\n",
          fsproc / MHz (1));
#endif
    }
}

void
usecase_rx (rx_args args)
{
}

///////////////////////////////////////////////////////
///////// CAF Interpret

static inline size_t
caf_interpret_nbins (caf_interpret_args args)
{
  switch (args.type)
    {
    case FFTCT_LITE:
      return fftcaff_lite_rows (args.params.lite.offsets);
    case FFTCT_NORMAL:
      return frange_size (args.params.normal.f);
    default:
      unreachable ();
    }
}

static inline float
caf_interpret_fbintof (caf_interpret_args args, size_t fbin)
{
  // Convert to global values
  switch (args.type)
    {
    case FFTCT_LITE:
      {
        long fft_bin = lite_ind_to_bin (
            args.params.lite.offsets,
            args.params.lite.bin_stride,
            fbin);
        return bintofreq (fft_bin, args.fs, args.params.lite.fft_size);
      }
    case FFTCT_NORMAL:
      {
        return frange_findtof (args.params.normal.f, fbin);
      }
    default:
      unreachable ();
    }
}

typedef struct
{
  float max;
  size_t argmax;
  size_t argsmax;
  float argfmax;
} caf_point;

static inline void
caf_point_fprintlnf (FILE *ofp, caf_point c)
{
}

caf_point
caf_point_initial ()
{
  return (caf_point){
    .max = FLT_MIN,
    .argmax = 0,
    .argsmax = 0,
    .argfmax = FLT_MIN,
  };
}

static void
caf_interpret_max_in_this_buffer (
    float *caf_buffer,
    size_t cblen,
    size_t k0,
    caf_point *dest,
    caf_interpret_args args)
{
  ASSERT (dest);

  *dest = caf_point_initial ();

  size_t nbins = caf_interpret_nbins (args);
  f64_vec_argmax (caf_buffer, cblen, &dest->max, &dest->argmax);

  dest->argsmax = (k0 + dest->argmax) / nbins;
  size_t argfbinmax = (k0 + dest->argmax) % nbins;
  dest->argfmax = caf_interpret_fbintof (args, argfbinmax);
}

static float
caf_interpret_fres (caf_interpret_args args)
{
  // Convert to global values
  switch (args.type)
    {
    case FFTCT_LITE:
      {
        return args.params.lite.bin_stride * args.fs / (float)args.params.lite.fft_size;
      }
    case FFTCT_NORMAL:
      {
        return args.params.normal.f.df;
      }
    default:
      unreachable ();
    }
}

void
usecase_caf_interpret (caf_interpret_args args)
{
  ASSERT (args.ifp->elem_size == sizeof (float));
  float *caf_buffer = (float *)malloc_or_abort (args.search_len * sizeof *caf_buffer);

  size_t read;
  float fres = caf_interpret_fres (args);

  caf_point global = caf_point_initial ();
  caf_point local = caf_point_initial ();

  size_t k0 = 0;

  do
    {
      read = args.ifp->read (caf_buffer, args.search_len);
      if (read > 0)
        {
          caf_interpret_max_in_this_buffer (caf_buffer, read, k0, &local, args);
          k0 += read;

          if (local.max > global.max)
            {
              global = local;
            }

          if (global.max > args.thresh)
            {
              ASSERT (read > global.argmax);

              // Shift so that max is on the left edge of the buffer
              memmove_top_bottom (caf_buffer, sizeof (float), global.argmax, read);
              k0 += global.argmax;

              // Then read the top right half
              size_t len = (read - global.argmax);
              float *head = caf_buffer + len;
              ASSERT (args.search_len > len);
              read = len + args.ifp->read (head, args.search_len - len);

              caf_interpret_max_in_this_buffer (caf_buffer, read, k0, &local, args);

              ns_log (INFO, "Sample: %zu\n", global.argsmax);
              ns_log (
                  INFO,
                  "Frequency: %f (Within [%f, %f])\n",
                  global.argfmax,
                  global.argfmax - fres / 2,
                  global.argfmax + fres / 2);
              ns_log (INFO, "Max value: %e\n", global.max);
              goto theend;
            }
        }
    }
  while (!args.ifp->done ());

  ns_log (WARN, "No peak found above supplied threshold: %e\n", args.thresh);
  ns_log (WARN, "Using global max\n");
  ns_log (WARN, "Sample: %zu\n", global.argsmax);
  ns_log (
      WARN,
      "Frequency: %f (Within [%f, %f])\n",
      global.argfmax,
      global.argfmax - fres / 2,
      global.argfmax + fres / 2);
  ns_log (WARN, "Max value: %e\n", global.max);

theend:
  safe_free (caf_buffer);
}

///////////////////////////////////////////////////////
///////// gen sig

static inline void
gen_sig_tx (gen_sig_args args, gen_sig_row *t)
{
  // TX
  FILE *match = fopen_or_abort (args.match_fname, "wb");
  FILEDtypeDatasource mds (sizeof (_Complex float), match);

  tx_args targs = (tx_args){
    .fs_hz = args.fs,
    .time_sec = args.t,
    .ss_power = 1,
    .ramp_up_tf = args.rutf,
    .ramp_down_t0 = args.rdt0,
    .rut = RU_LINEAR,
    .ofp = &mds,
    .seed = 1234,
  };

  TIMER_START ();

  usecase_tx (targs);

  t->tx_time = GET_ELLAPSED ();

  fclose_or_abort (match);
}

static inline size_t
gen_sig_siglen (gen_sig_args args)
{
  FILE *match = fopen_or_abort (args.match_fname, "rb");
  FILEDtypeDatasource dts (sizeof (_Complex float), match);
  size_t nelems = dtypeds_size (&dts);
  fclose_or_abort (match);
  return nelems;
}

static inline size_t
gen_sig_cafout_len (gen_sig_args args)
{
  FILE *match = fopen_or_abort (args.cafresult_fname, "rb");
  FILEDtypeDatasource dts (sizeof (float), match);
  size_t nelems = dtypeds_size (&dts);
  fclose_or_abort (match);
  return nelems;
}

static inline float
gen_sig_power (gen_sig_args args, size_t nelems,
               gen_sig_row *t)
{
  ASSERT (nelems == (size_t)(args.fs * args.t));

  FILE *match = fopen_or_abort (args.match_fname, "rb");
  FILEDtypeDatasource dts (sizeof (_Complex float), match);

  sig_pwrf_args pwr_args = (sig_pwrf_args){
    .ifp = &dts,
    .k = (krange){
        .k0 = 0,
        .isinf = 1,
    },
  };

  TIMER_START ();

  float power = usecase_sig_pwrf_full_file_ifp (pwr_args);

  t->sigpwr_time = GET_ELLAPSED ();

  fclose_or_abort (match);

  return power;
}

static inline void
gen_sig_channel (gen_sig_args args, size_t nelems,
                 float power, gen_sig_row *t)
{
  ASSERT (nelems == (size_t)(args.fs * args.t));

  ns_log (INFO, "Writing channelized signal to: %s\n", args.signal_fname);
  if (args.noise_fname)
    {
      ns_log (INFO, "Writing raw noise to: %s\n", args.noise_fname);
    }

  FILE *match = fopen_or_abort (args.match_fname, "rb");
  FILE *signal = fopen_or_abort (args.signal_fname, "wb");
  FILE *noise = NULL;

  if (args.noise_fname)
    noise = fopen_or_abort (args.noise_fname, "wb");

  FILEDtypeDatasource mds (sizeof (_Complex float), match);
  FILEDtypeDatasource sds (sizeof (_Complex float), signal);
  FILEDtypeDatasource nds (sizeof (_Complex float), noise);

  channel_args cargs = (channel_args){
    .awgn_power = snrdb_to_awgn_power (args.snrdb, power),
    .f_offset = 0,
    .fs = args.fs,
    .left_zero = nelems,
    .right_zero = nelems,
    .ifp = &mds,
    .ofp = &sds,
    .nfp = &nds,
    .seed = 9876,
  };

  TIMER_START ();

  usecase_channel (cargs);

  t->channel_time = GET_ELLAPSED ();

  fclose_or_abort (match);
  fclose_or_abort (signal);
  if (noise)
    fclose_or_abort (noise);
}

void
gen_sig_row_fprintf_csv_row (FILE *ofp, gen_sig_row row)
{
  ASSERT (ofp);
  fprintf (ofp, "%zu,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,", row.slen, row.actual_power,
           row.fs, row.t, row.rutf, row.rdt0, row.snrdb);
  fprintf (ofp, "%.9f,%.9f,%.9f,%.9f\n", row.tx_time, row.sigpwr_time,
           row.channel_time, row.fftcaff_lite_time);
}

void
gen_sig_row_fprintf_csv_header (FILE *ofp)
{
  ASSERT (ofp);
  fprintf (ofp, "slen,actual_power,fs,t,rutf,rdt0,snrdb,");
  fprintf (ofp, "tx_time,sigpwr_time,channel_time,fftcaff_lite_time\n");
}

static inline void
gen_sig_fftcaff_lite (gen_sig_args args, gen_sig_row *t)
{
  ns_log (INFO, "Match: %s\n", args.match_fname);
  ns_log (INFO, "Signal: %s\n", args.signal_fname);
  ns_log (INFO, "Result: %s\n", args.cafresult_fname);

  // FFTCAFF
  FILE *out = fopen_or_abort (args.cafresult_fname, "wb");
  FILE *match = fopen_or_abort (args.match_fname, "rb");
  FILE *signal = fopen_or_abort (args.signal_fname, "rb");

  FILEDtypeDatasource ods (sizeof (float), out);
  FILEDtypeDatasource mds (sizeof (_Complex float), match);
  FILEDtypeDatasource sds (sizeof (_Complex float), signal);

  fftcaff_ifp_ofp_args fargs = {
    .params = {
        .lite = {
            .offsets = 0,
            .bin_stride = 1,
            .zero_middle = 0,
        },
    },
    .type = FFTCT_LITE,
    .samp_range = (krange){
        .k0 = 0,
        .isinf = 1,
    },
    .match_ifp = &mds,
    .data_ifp = &sds,
    .ofp = &ods,
    .fout = { 0 },
    .compute_fabs = 1,
    .mmap_base = NULL,
#ifndef NOMP
    .nthreads = (size_t)omp_get_max_threads (),
#endif
  };

  TIMER_START ();

  usecase_fftcaff_ifp_ofp (fargs);

  t->fftcaff_lite_time = GET_ELLAPSED ();

  fclose_or_abort (match);
}

static inline void
gen_sig_truncate (gen_sig_args args)
{
  ns_log (INFO, "Reading caf results from: %s\n", args.cafresult_fname);
  ns_log (INFO, "Truncating with stride: %zu to file %s\n", args.stride,
          args.cafresult_fname);

  slice s = {
    .start = 0,
    .step = args.stride,
    .isinf = 1,
  };
  FILE *ifp = fopen_or_abort (args.cafresult_fname, "rb");
  FILE *ofp = fopen_or_abort (args.trunc_cafresult_fname, "wb");
  dtype type = F32;

  uint8_t *read = (uint8_t *)malloc_or_abort (s.step * dtype_sizeof (type));

  do
    {
      size_t next = MIN (slice_left (s), s.step);
      size_t nread = fread_or_abort (read, dtype_sizeof (type), next, ifp);
      fwrite_or_abort (read, dtype_sizeof (type), 1, ofp);
      s.start += next;
    }
  while (slice_left (s) > 0 && !feof (ifp));

  safe_free (read);
  fclose_or_abort (ifp);
  fclose_or_abort (ofp);
}

gen_sig_row
usecase_gen_sig (gen_sig_args args)
{
  gen_sig_row t;

  // Copy
  t.fs = args.fs;
  t.t = args.t;
  t.rutf = args.rutf;
  t.rdt0 = args.rdt0;
  t.snrdb = args.snrdb;

  // TX
  ns_log (INFO, "Generating Signal to file: %s\n", args.match_fname);
  gen_sig_tx (args, &t);

  // Siglen (not necessary, but just for sanity)
  ns_log (INFO, "Calculating Signal Length\n");
  size_t siglen = gen_sig_siglen (args);
  t.slen = siglen;
  ns_log (INFO, "Signal Length: %zu\n", siglen);

  // Power
  ns_log (INFO, "Calculating Signal Power\n");
  float power = gen_sig_power (args, siglen, &t);
  t.actual_power = power;
  ns_log (INFO, "Signal Power: %f\n", t.actual_power);

  // Channel
  ns_log (INFO, "Running Channel\n");
  gen_sig_channel (args, siglen, power, &t);

  // RX
  ns_log (INFO, "Running FFTCAFF LITE\n");
  gen_sig_fftcaff_lite (args, &t);

  // Truncate CAF
  ns_log (INFO, "Truncating signal\n");
  gen_sig_truncate (args);

  return t;
}
