///////////////////////////////////////////////////////////////////////////////
// Raspberry Pi GPIO Interface
// ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
// File: imu_mux_test.c
// PA Consulting - Lawrence Jones
///////////////////////////////////////////////////////////////////////////////

#include "i2c.h"
#include "devs/pca9548a.h"

///////////////////////////////////////////////////////////////////////////////
// MUX TESTING
///////////////////////////////////////////////////////////////////////////////

int mux_test(void)
{
  // Initialise i2c protocol for bus 1
  // Assign i2c handle
  i2c_bus *i2c = i2c_init(1);
  // Init the i2c multiplexer
  Mux *m = pca_init("MUX1", 116, i2c);
  // Select channel 0
  m->set_channel(m, PCA_C0);
  // Print mux network
  mux_network_print(m);
  // Dealloc the mux struct
  pca_dealloc(&m);
  return 0;
}
