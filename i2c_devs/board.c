///////////////////////////////////////////////////////////////////////////////
// Raspberry Pi GPIO Interface
// ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
// File: board.c
// PA Consulting - Lawrence Jones
///////////////////////////////////////////////////////////////////////////////

#include <stdint.h>
#include "board.h"
#include "mpu3300.h"
#include "itg3050.h"
#include "pca9548a.h"

///////////////////////////////////////////////////////////////////////////////
// TEST RUN
///////////////////////////////////////////////////////////////////////////////

int main(int argc, char** argv)
{
  // Initialise i2c protocol for bus 1
  // Assign i2c handle
  i2c_bus *i2c = i2c_init(1);
  // Init the i2c multiplexer
  Mux *m = pca_init("MUX1", 116, i2c);
  // Select channel 0
  m->set_channel(m, PCA_C0);
  // Dealloc the mux struct
  pca_dealloc(&m);
  return 0;
}
