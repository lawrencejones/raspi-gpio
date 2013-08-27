///////////////////////////////////////////////////////////////////////////////
// Raspberry Pi GPIO Interface
// ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
// File: i2c_dev_mgmt.c
// PA Consulting - Lawrence Jones
///////////////////////////////////////////////////////////////////////////////

#include "i2c_res.h"
#include "i2c_private.h"

///////////////////////////////////////////////////////////////////////////////
// I2C DEVICE MANAGEMENT
///////////////////////////////////////////////////////////////////////////////

// Given an i2c_bus and an address, ping the dev at
// the addr and verify that there is a response
int i2c_bus_addr_active(i2c_bus *i2c, short addr)        // i2c_bus_addr_active
{
  // Set new slave address
  BSC_SLAVE_ADDR = addr;
  // Clear current bus status
  BSC_S = CLEAR_STATUS;
  // Only wish to read a single byte
  BSC_DATA_LEN = 1;
  // Initiate read using bus control
  BSC_C = START_READ;
  // Wait for bus to clear
  i2c_wait_done(i2c);
  return !(BSC_S & BSC_S_ERR);
}

// Detect all devices on the current bus and return a
// pointer to an i2c_dev struct linked to all discovered devices
i2c_dev *i2c_dev_detect(i2c_bus *i2c)                         // i2c_dev_detect
{
  // Create new dummy dev at addr 0
  i2c_dev *dev = i2c_dev_malloc(0);
  // For all of the available addresses
  for (short addr = 1; addr < 128; addr++)
  {
    // If the i2c addr is active
    if (i2c_bus_addr_active(i2c, addr))
    {
      // Add the current address to the chain of devs
      i2c_dev_append(dev, addr);
    }
  }
  // Return the initial i2c_dev pointer
  return dev;
}

// Add an i2c device to an existing i2c_dev list
void i2c_dev_append(i2c_dev *dev, short addr)                 // i2c_dev_append
{
  // bus->first will never be NULL due to ctrl dummy
  // so iterate till we get to a dev that has no next
  while (dev->next)
  { // Assign the next device
    dev = dev->next;
  }
  // Once final device has been found, malloc new
  dev->next = i2c_dev_malloc(addr);
}
