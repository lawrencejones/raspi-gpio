///////////////////////////////////////////////////////////////////////////////
// Raspberry Pi GPIO Interface
// ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
// File: imu_routing.c
// PA Consulting - Lawrence Jones
///////////////////////////////////////////////////////////////////////////////

#include "i2c.h"
#include "imu_private.h"
#include "dev/mpu3300.h"
#include "dev/pca9548a.h"
#include "macros.h"

///////////////////////////////////////////////////////////////////////////////
// STATIC HELPERS
///////////////////////////////////////////////////////////////////////////////

static int verify_tokens(int *bus, short *addr, char **tokens, int argc)
{
  // If arg count is not satifactory
  if (argc < 4)
  {
    // Print err
    ERR("Incorrect arguments.\n\n");
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
      // Return failure
      return 1;
    }
    // Verify address
    if (*addr > 127 || *addr < 0)
    {
      // Print error
      ERR("I2C address range is 0-127. %d (0x%02x) is not valid.\n\n", 
        *addr, *addr);
      // Return failure
      return 1;
    }
  }
  // Return success
  return 0;
}

///////////////////////////////////////////////////////////////////////////////
// MAIN ROUTER
///////////////////////////////////////////////////////////////////////////////

// Routes are...
//           |   1   |   2   |       3       |    4     |     5
//      imu  | [dev] | [bus] |     [addr]    | [action] |  [params]
// eg.  imu  |  mpu  |   1   |  0x74/1/0x69  |  config  | "i2c_bypass:on"
//      imu  |  pca  |   0   |      0x74     |  test    |
int imu_route(char **tokens, int argc)
{
  printf("\n");
  // Check for simple run command
  if (!strcmp(tokens[1], "run"))
  {
    ERR("To be implemented.\n\n"); 
  } 
  else
  {
    // Declare variables
    int bus, err; short addr;
    // Verifies tokens and sets bus and address if found
    if ((err = verify_tokens(&bus, &addr, tokens, argc)))
    {
      // Return the error
      return err;
    }
    // Init i2c
    i2c_bus *i2c = i2c_init(bus);
    // If pca multiplexer
    if (!strcmp(tokens[1], "pca"))                                       // PCA
    {
      return imu_pca_route(i2c, addr, tokens, argc);
    }
    // Else if mpu gyro
    else if (!strcmp(tokens[1], "mpu") || !strcmp(tokens[1], "itg"))     // MPU || ITG
    {
      return imu_gyro_route(i2c, addr, tokens, argc);
    }
    // Else unsupported option
    else
    {
      // Error print for unsupported option
      ERR("Unsupported option `%s`.\n\n", tokens[1]);
      // Return exit failure
      return 1;
    }
  }
  return 0;
}
