///////////////////////////////////////////////////////////////////////////////
// Raspberry Pi GPIO Interface
// ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
// File: pca_dealloc.c
// PA Consulting - Lawrence Jones
///////////////////////////////////////////////////////////////////////////////

#include "pca_private.h"
#include "macros.h"

///////////////////////////////////////////////////////////////////////////////
// DEALLOC PCA
///////////////////////////////////////////////////////////////////////////////
/*
   Deallocing the pca is much less significant than any sensors
   due to the passive nature of the device. Any pointers to
   i2c devices (which may or may not be sensors) have the potential
   to be leaked if not dealt with carefully though.
*/

void pca_dealloc(Mux **m)
{
  // If either the pointer to pointer of the pointer to the
  // mux is null then...
  if (!(m && *m))
  {
    // Output error message
    ERR("Not a valid Mux pointer. Possibly already free?\n\n");
    // Exit with error
    exit(EXIT_FAILURE);
  }
  // m is now a valid pointer, so proceed to free fields
  // Free the mux name string
  free((*m)->name);
  // Free the struct
  free(*m);
}
