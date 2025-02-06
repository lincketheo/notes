#pragma once

#include "core/DtypeDatasource.hpp"
#include "core/data_structures.h"
#include "emprobe/fftcaf.h"

/**
 * This system Starts with an input file stream,
 * and reads it until it finds a peak.
 *
 * Then, it a single buffer where the peak is at sample 0
 */

typedef struct
{
  DtypeDatasource *ifp;
  float thresh;
  fftcaff_create_args args;
  krange search_range;

  /**
   * The searcher will trigger on a threshold. However,
   * to avoid triggering on rising edges, you can give the
   * function a sample search range. Let's say it triggers on
   * sample 5, but the actual peak is sample 6, and decreases
   * at sample 7. You should pass sample_resolution 3 in this case.
   *
   * If you just want to trigger on the first sample you find
   * that triggers the threshold, set sample_resolution to 1.
   *
   * sample_resolution must be > 0
   *
   * Note that the search buffer is always a multiple of the
   * number of frequency bins.
   */
  size_t sample_resolution;

  /**
   * FFTCAFF is a circular convolution, so a useful way of doing it
   * is to delete the front 50% of samples for each read
   */
  float fftcaff_search_percent;
} fftcaff_peak_find_args;

typedef struct
{
  int found;

  // below is meaningless if found is 0
  size_t sample;        // The overall sample number of data_out[0]
  size_t fbin;          // The frequency bin that was hit
  cf64_buffer data_out; // data_out[0] is the peak
} fftcaff_peak_find_result;

fftcaff_peak_find_args read_until_peak (fftcaff_peak_find_args args);
