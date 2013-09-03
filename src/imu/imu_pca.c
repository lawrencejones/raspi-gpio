///////////////////////////////////////////////////////////////////////////////
// Raspberry Pi GPIO Interface
// ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
// File: imu_pca.c
// PA Consulting - Lawrence Jones
///////////////////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include "i2c.h"
#include "imu_private.h"
#include "devs/pca9548a.h"
#include "macros.h"

///////////////////////////////////////////////////////////////////////////////
// PCA TESTING
///////////////////////////////////////////////////////////////////////////////

// PRE - Sanitised params
static int imu_pca_test(Mux *pca)
{
  // Select channel 0
  pca->set_channel(pca, PCA_C0);
  // Verify that the channel has been set
  pca->get_channel(pca);
  // Print mux network
  mux_network_print(pca);
  return 0;
}

///////////////////////////////////////////////////////////////////////////////
// PCA SET CHANNEL
///////////////////////////////////////////////////////////////////////////////

// Route for setting pca channel
static int imu_pca_set_channel(Mux *pca, char **tokens)
{
  // Extract channel
  short channel = atoi(tokens[5]);
  // Verify channel
  if (channel < 0 || channel > 3)
  {
    // Print error
    ERR("Invalid pca channel value (%d).\n\n", channel);
    // Return failure
    return EXIT_FAILURE;
  }
  // Set the pca channel
  pca->set_channel(pca, 1 << channel);
  // Return 0
  return 0;
}

///////////////////////////////////////////////////////////////////////////////
// PCA ROUTE
///////////////////////////////////////////////////////////////////////////////

// Route for all the pca actions
int imu_pca_route(i2c_bus *i2c, short addr, char **tokens, int argc)
{
  // Init a pca struct
  Mux *pca = pca_init(i2c, addr);
  // If pca test
  if (!strcmp(tokens[4], "test"))
  {
    // Run pca test
    imu_pca_test(pca);
  }
  // Else if set_channel
  else if (!strcmp(tokens[4], "set_channel"))
  {
    // Set the pca channel
    imu_pca_set_channel(pca, tokens);
  }
  // Else unsupported action
  else
  {
    // Print error
    ERR("Action `%s` is unsupported for pca.\n\n", tokens[4]);
    // Return failure
    return 1;
  }
  // Dealloc the mux struct
  pca_dealloc(&pca);
  // Return success
  return 0;
}
