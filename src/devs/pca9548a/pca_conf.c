///////////////////////////////////////////////////////////////////////////////
// Raspberry Pi GPIO Interface
// ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
// File: pca_conf.c
// PA Consulting - Lawrence Jones
///////////////////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include "pca_private.h"
#include "macros.h"

///////////////////////////////////////////////////////////////////////////////
// CONFIGURING CHANNEL
///////////////////////////////////////////////////////////////////////////////
/*
   The mux is only ever used directly to access it's current
   channel register or to write to it.
   As such, there are only two functions below to achieve this.
 */

// Helper to avoid duplicating validity checks
void verify_mux(Mux *m)
{
  // Verify that we have a valid mux pointer
  if (m == NULL)
  {
    // Print error message
    ERR("Invalid pointer for mux struct (m).\n\n");
    // Exit with failure
    exit(EXIT_FAILURE);
  }
  // Verify that we have a valid i2c handle
  if (m->i2c == NULL)
  {
    // Print error message
    ERR("Invalid i2c handle attached to mux `%s`.\
         Cannot continue.\n\n", m->name);
    // Exit with failure
    exit(EXIT_FAILURE);
  }
}

// Fetches the current channel code from the given mux,
// then updates the field inside the given struct and
// returns the code as the value
uint8_t pca_get_channel(Mux *m)
{
  // Verify that the pointer is healthy
  verify_mux(m);
  // Once verified, read the channel code from the mux 
  // into the channel field
  m->channel = i2c_read_byte(m->i2c, m->i2c_addr);
  // Return the value read
  return m->channel;
}

// Sets the pca control channel and verifies write
// success
void pca_set_channel(Mux *m, uint8_t c)
{
  // Verify arg health
  verify_mux(m);
  // Once verified, transfer the channel code to
  // the device
  i2c_write_byte(m->i2c, m->i2c_addr, (uint8_t)c);
  // Once written, verify that write has been successful
  m->get_channel(m);
  // After fetching the channel, write has been successful
  // iff the new channel field is equal to the desired parameter
  if (c != m->channel)
  {
    // If not equal then write has failed
    ERR("Write to mux named `%s` has failed.\n\n", m->name);
    // Exit with error
    exit(EXIT_FAILURE);
  }
}
