///////////////////////////////////////////////////////////////////////////////
// Raspberry Pi GPIO Interface
// ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
// File: imu_pca_test.c
// PA Consulting - Lawrence Jones
///////////////////////////////////////////////////////////////////////////////

#include "i2c.h"
#include "devs/pca9548a.h"

///////////////////////////////////////////////////////////////////////////////
// MUX TESTING
///////////////////////////////////////////////////////////////////////////////

// PRE - Sanitised params
int imu_pca_test(int bus, short addr)
{
  // Initialise i2c protocol for bus 1
  // Assign i2c handle
  i2c_bus *i2c = i2c_init(bus);
  // Init the i2c multiplexer
  Mux *pca = pca_init("PCA", addr, i2c);
  // Select channel 0
  pca->set_channel(pca, PCA_C0);
  // Print mux network
  mux_network_print(pca);
  // Dealloc the mux struct
  pca_dealloc(&pca);
  return 0;
}
