///////////////////////////////////////////////////////////////////////////////
// Raspberry Pi GPIO Interface
// ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
// File: i2c_cli_helpers.c
// PA Consulting - Lawrence Jones
///////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include "i2c_cli_private.h"

///////////////////////////////////////////////////////////////////////////////
// VALIDATION
///////////////////////////////////////////////////////////////////////////////

// Verifies that the expected argument count matches that
// given. If it doesn't then exit program with error.
void verify_arg_count(int expected,                         // verify_arg_count
                      int argc) 
{
  // If the arguments do not equal the expected
  if (argc < expected)
  {
    // Then print to standard err
    ERR("Error. See i2c-test --help for correct usage.\n\n");
    // And exit with failure
    exit(EXIT_FAILURE);
  }
}

// Verifies that the address is currently active and that
// the register is within a sensible range
void verify_i2c_details(i2c_bus *i2c,
                        short addr,
                        int reg)
{
  // Verify that address is within i2c bus range
  if (addr > 127)
  {
    ERR("Address (0x%02x not in range of bus.\n\n", addr);
    exit(EXIT_FAILURE);
  }
  // Verify that the device is on the bus
  if (!i2c_bus_addr_active(i2c, addr))
  {
    // If it's not, then exit with error
    ERR("Device (0x%02x) not found on bus. \
Use `detect` to list devs.\n\n", addr);
    exit(EXIT_FAILURE);
  }
  // Verify register is within reasonable limit
  if (!(reg >= 0 && reg < 256))
  {
    // Else exit with error
    ERR("Register 0x%02x is out of range.\n\n", reg);
    exit(EXIT_FAILURE);
  }
}
