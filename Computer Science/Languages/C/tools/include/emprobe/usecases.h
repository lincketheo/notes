#pragma once

#include "core/DtypeDatasource.hpp"
#include "core/math.h"

#include "emprobe/fftcaf.h"

///////////////////////////////////////////////////////
///////// Channel

// Reads input file, adds noise and zero padding
// on left and write, writes to output file
// (noise after zero padding)
typedef struct
{
  float awgn_power; // Use common snr formula if you want to use snr
  float f_offset;   // Tune frequency
  float fs;

  size_t left_zero;  // Number of zeros to insert on the left
  size_t right_zero; // Number of zeros to insert on the right

  DtypeDatasource *ifp; // Input file
  DtypeDatasource *ofp; // Output file
  DtypeDatasource *nfp; // Noise file (can be null)
  uint32_t seed;
} channel_args;

void usecase_channel (channel_args args);

///////////////////////////////////////////////////////
///////// RX

typedef struct
{
  // Global
  const char *mmap_base;
  trange t;
  float fs;

  struct
  {
    size_t offsets;
    size_t bin_stride;
    float thresh;
  } caf1;

  struct
  {
    frange f;
    float thresh2;
  } caf2;

  // Data
  DtypeDatasource *ifp;       // Input data file
  DtypeDatasource *match_ifp; // Input match file
} rx_args;

void usecase_rx (rx_args args);

///////////////////////////////////////////////////////
///////// TX

// Writes a white gaussian noise signal to (ofp)
typedef struct
{
  float fs_hz;    // Sample frequency output (Hz)
  float time_sec; // Time in seconds
  float ss_power; // Power of steady state (note, not total)

  float ramp_up_tf;   // Time for ramp up
  float ramp_down_t0; // Time to start ramp down
  ramp_up rut;

  DtypeDatasource *ofp;
  uint32_t seed;
} tx_args;

// Is ramp up / down present
#define ru_present(args) (!isnanf ((args).ramp_up_tf))
#define rd_present(args) (!isnanf ((args).ramp_up_tf))

void usecase_tx (tx_args args);

///////////////////////////////////////////////////////
///////// sig_pwr

// Calculates signal power over a sample range (k)
typedef struct
{
  DtypeDatasource *ifp;
  krange k; // Range of samples to calculate over
} sig_pwrf_args;

// Returns the power
float usecase_sig_pwrf_full_file_ifp (sig_pwrf_args args);

// Prints the power
void usecase_sig_pwrf_full_file_ifp_and_print (sig_pwrf_args args);

///////////////////////////////////////////////////////
///////// FFTCAFF

// Runs an fftcaff on the input file
// and outputs a stream of F32's if compute_abs is 1
// else CF64's if compute_abs is 0
typedef struct
{
  union
  {
    // Lite parameters
    struct
    {
      size_t offsets;
      size_t bin_stride;
      int zero_middle;
    } lite;

    // Normal parameters
    struct
    {
      frange f;
      float fs;
    } normal;
  } params;

  fftcaff_type type;
  krange samp_range; // What range to compute over

  DtypeDatasource *match_ifp; // Match file (original signal)
  DtypeDatasource *data_ifp;  // Data file (with noise probably)
  DtypeDatasource *ofp;       // Output file for correlations

  fftcaff_fout fout; // Intermediary files
  int compute_fabs;  // If 1 computes |z| for each correlation
  const char
      *mmap_base; // If not null, uses memory mapping instead of in memory for
                  // fftcaff data structures (they can get pretty large)

#ifndef NOMP
  size_t nthreads;
#endif

} fftcaff_ifp_ofp_args;

int usecase_fftcaff_ifp_ofp (fftcaff_ifp_ofp_args args);

///////////////////////////////////////////////////////
///////// CAF Interpret

// "Interprets" CAF
// Reads an input file, and when a correlation has
// crossed the threshold, it will print that sample
// number and frequency row number in the caf matrix.
// It is the caller's responsibility to convert
// frequency row number to actual frequency
//
// Note: After the first hit is found,
// caf_interpret will look forward [search_len]
// in the unfolded caf - this is so that rising edges
// aren't detected by default. [search_len] is the length of the buffer
// you take the max from. Ideally, once you cross the threshold,
// it would return something after because the threshold won't be
// exactly on the peak
typedef struct
{
  DtypeDatasource *ifp;
  FILE *res_ofp;
  DtypeDatasource *data_ifp;
  DtypeDatasource *data_ofp;

  size_t search_len; // Number of samples to search per max - also the search
                     // resolution of the max
  float thresh;      // Threshold to search for

  float fs;
  union
  {
    struct
    {
      size_t offsets;
      size_t bin_stride;
      size_t fft_size;
    } lite;
    struct
    {
      frange f;
    } normal;
  } params;
  fftcaff_type type;
} caf_interpret_args;

void usecase_caf_interpret (caf_interpret_args args);

///////////////////////////////////////////////////////
///////// Generate signal, and it's CAF and truncate

// A simple one time run
// that does the following:
// 1. Runs tx
// 2. Runs channel
// 3. Runs fftcaff_lite
// 4. Runs truncate
typedef struct
{
  // Global
  float fs;

  // TX
  float t; // Time length of the signal
  float rutf;
  float rdt0;
  const char *match_fname;

  // Channel
  float snrdb;
  const char *signal_fname;
  const char *noise_fname;

  // CAF
  size_t nbins;
  const char *cafresult_fname;       // Output caf file name
  const char *trunc_cafresult_fname; // The output file name of the truncated
                                     // caf results
  size_t stride;                     // Stride for output truncation array slice
} gen_sig_args;

// Data collected in one run
typedef struct
{
  // Independent Variables
  size_t slen;
  float actual_power;
  float fs;
  float t;
  float rutf;
  float rdt0;
  float snrdb;

  // Timing
  double tx_time;
  double sigpwr_time;
  double channel_time;
  double fftcaff_lite_time;
} gen_sig_row;

void gen_sig_row_fprintf_csv_row (FILE *ofp, gen_sig_row row);

void gen_sig_row_fprintf_csv_header (FILE *ofp);

gen_sig_row usecase_gen_sig (gen_sig_args args);
