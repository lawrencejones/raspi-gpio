///////////////////////////////////////////////////////////////////////////////
// Raspberry Pi GPIO Interface
// ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
// File: imu_routing.c
// PA Consulting - Lawrence Jones
///////////////////////////////////////////////////////////////////////////////

#include "i2c.h"
#include "imu_private.h"
#include "devs/mpu3300.h"
#include "devs/pca9548a.h"
#include "macros.h"

static int verify_tokens(int *bus, short *addr, char **tokens, int argc)
{
  // If arg count is not satifactory
  if (argc < 4)
  {
    // Return error
    return 1;
  }
  else
  {
     // Extract bus
    *bus = DEX_TO_INT(tokens[2]);
    // Extract address
    *addr = DEX_TO_INT(tokens[3]);
    // Verify correct bus
    if (!(*bus == 0 || *bus == 1))
    {
      // Print error
      ERR("Only two i2c buses available (0 or 1). %d is not valid.\n\n", 
        *bus);
      // Return exit_failure
      return 1;
    }
    // Verify address
    if (*addr > 127 || *addr < 0)
    {
      // Print error
      ERR("I2C address range is 0-127. %d (0x%02x) is not valid.\n\n", 
        *addr, *addr);
      // Return exit_failure
      return 1;
    }
  }
  // Return success
  return 0;
}

// Routes are...
//           |   1   |   2   |   3    |    4     |     5
//      imu  | [dev] | [bus] | [addr] | [action] |  [params]
// eg.  imu  |  mpu  |   1   |  0x69  |  config  | "i2c_bypass:on"
//      imu  |  pca  |   0   |  0x74  |  test    |
int imu_route(char **tokens, int argc)
{
  printf("\n");
  // Declare variables
  int bus; short addr;
  // Verifies tokens and sets bus and address if found
  if (verify_tokens(&bus, &addr, tokens, argc))
  {
    // Print error
    ERR("Invalid arguments.\n\n");
    // Return the error
    return EXIT_FAILURE;
  }
  // Init i2c
  i2c_bus *i2c = i2c_init(bus);
  // If pca multiplexer
  if (!strcmp(tokens[1], "pca"))
  {
    // If pca test
    if (!strcmp(tokens[4], "test"))
    {
      // Run pca test
      imu_pca_test(bus, addr);
    }
    // Else if set_channel
    else if (!strcmp(tokens[4], "set_channel"))
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
      // Initialise a pca
      Mux *pca = pca_init("PCA", addr, i2c);
      // Set the channel
      pca->set_channel(pca, (1 << channel));
      // Dealloc
      pca->dealloc(&pca);
    }
    // Else unsupported action
    else
    {
      // Print error
      ERR("Action `%s` is unsupported for pca.\n\n", tokens[5]);
      // Return failure
      return 1;
    }
  }
  // Else if mpu gyro
  else if (!strcmp(tokens[1], "mpu"))
  {
    // If mpu test
    if (!strcmp(tokens[4], "read_axes"))
    {
      // Run mpu test with tokens[5] as config str
      imu_mpu_test_axes(bus, addr, tokens[5]);
    }
    // Else if mpu config
    else if (!strcmp(tokens[4], "config"))
    {
      // Init mpu without config
      Sensor *mpu = mpu_init("MPU", i2c, addr, NULL);
      // If reset to defaults
      if (!strcmp(tokens[5], "reset"))
      {
        // Set the defaults on the mpu pointer
        mpu->reset(mpu);
      }
      // Else if general config
      else
      {
        // Configure the mpu
        mpu->config(mpu, tokens[5]);
      }
      // dealloc the mpu
      mpu->dealloc(&mpu);
    }
    // Else unsupported action
    else
    {
      // Print error
      ERR("Action `%s` is unsupported for mpu.\n\n", tokens[5]);
      // Return failure
      return 1;
    }
  }
  // Else unsupported option
  else
  {
    // Error print for unsupported option
    ERR("Unsupported option `%s`.\n\n", tokens[1]);
    // Return exit failure
    return EXIT_FAILURE;
  }
  return 0;
}
