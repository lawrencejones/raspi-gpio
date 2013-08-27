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
// MUX TESTING
///////////////////////////////////////////////////////////////////////////////

void mux_network_print(Mux *m);
int mux_test(int argc, char** argv)
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

///////////////////////////////////////////////////////////////////////////////
// MAIN
///////////////////////////////////////////////////////////////////////////////

#define IS_FLAG(str) if (!strcmp(str, argv[1]))

int main(int argc, char** argv)
{
  // Clear a line
  printf("\n");
  if (argc > 1)
  {
    // For the multiplexer tests
    IS_FLAG("mux")
    {
      mux_test(argc, argv);
    }
    else
    {
      ERR("Test `%s` is not yet supported.\n\n", argv[1]);
    }
  }
  else
  {
  ERR("Test flag is required.\n\n");
  }
  return 0;
}
