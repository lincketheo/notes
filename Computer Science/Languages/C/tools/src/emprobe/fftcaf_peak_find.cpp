#include "emprobe/fftcaf_peak_find.h"

#include "core/DtypeDatasource.hpp"
#include "core/errors.h"
#include "core/memory.h"
#include "emprobe/fftcaf.h"

#include <fftw3.h>
#include <string.h>

// Outline
// fftcaff_peak_find_args
// read_until_peak (fftcaff_peak_find_args args)
//{
//   do {
//    Read max caf data
//    Run caf
//    Search output correlation for peak > thresh
//    if found {
//      shift data so that peak sample is at data 0
//      Read max caf data
//      run caf
//      Search for peak
//      Search output correlation for GLOBAL max
//      Shift data so that peak sample is at data 0
//      return found and stuff;
//    }
//    delete % front
//   } while(!args.ifp->done());
//   return not found;
// }

class fftcpf_private
{
public:
  fftcpf_private ()
      : caf ({ 0 }),
        is_corrl_populated (0),
        loaded (0)
  {
  }

  void fill_fftcaff_data (DtypeDatasource *ifp);
  void run_fftcaff ();

  // Must run caf first (asserts is_corrl_populated)
  ssize_t find_sample_that_crosses_thresh (float thresh);
  size_t find_sample_max ();
  void delete_percent (float percent);
  void delete_up_to (size_t sample);

private:
  void get_next_read (fftwf_complex **head, size_t *toread);

  fftcaff caf;
  size_t loaded;
  int is_corrl_populated; // For assertions
};

void
fftcpf_private::get_next_read (fftwf_complex **head, size_t *toread)
{
  ASSERT (head);
  ASSERT (toread);
  ASSERT (caf.match_len >= loaded);

  *head = caf.data.data + loaded;
  *toread = caf.match_len - loaded;
}

void
fftcpf_private::fill_fftcaff_data (DtypeDatasource *ifp)
{
  ASSERT (ifp);
  ASSERT (loaded < caf.match_len);
  ASSERT (ifp->elem_size == sizeof *caf.data.data);

  fftwf_complex *head;
  size_t toread;

  get_next_read (&head, &toread);

  loaded += ifp->read (head, toread);

  // Fill top with zeros if done
  if (loaded < caf.match_len)
    {
      get_next_read (&head, &toread);
      memset (head, 0, toread * sizeof *head);
    }
}

void
fftcpf_private::run_fftcaff ()
{
  fftcaff_run (&caf);
  is_corrl_populated = 1;
}

void
fftcpf_private::delete_percent (float pcnt)
{
  ASSERT (is_corrl_populated);
  ASSERT (pcnt > 0 && pcnt <= 1);

  size_t samp = pcnt * caf.match_len;
  delete_up_to (samp);
}

void
fftcpf_private::delete_up_to (size_t samp)
{
  ASSERT (is_corrl_populated);
  ASSERT (samp > 0 && samp <= caf.match_len);

  memmove_top_bottom (
      caf.data.data,
      sizeof *caf.data.data,
      samp,
      caf.match_len);
  is_corrl_populated = 0;
}

size_t
fftcpf_private::find_sample_max ()
{
  ASSERT (is_corrl_populated);
  for (int k = 0; k < caf.match_len; ++k)
    {
      for (int f = 0; f < fftcaff_cols (&caf); ++f)
        {
          float c = cabsf(); 
        }
    }
}
