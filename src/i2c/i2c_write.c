///////////////////////////////////////////////////////////////////////////////
// Raspberry Pi GPIO Interface
// ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
// File: i2c_write.c
// PA Consulting - Lawrence Jones
///////////////////////////////////////////////////////////////////////////////

#include "i2c_private.h"
#include "i2c_res.h"

///////////////////////////////////////////////////////////////////////////////
// I2C Write
///////////////////////////////////////////////////////////////////////////////

// Write a single byte to the addr given, on the supplied
// i2c bus
uint32_t i2c_write_byte(i2c_bus *i2c,                         // i2c_write_byte
    short addr,
    uint8_t byte)
{
  // Write nothing to a register
  // Counts as a single byte write
  return i2c_write_reg(i2c, addr, byte, NULL, 0);
}

// Write to the register of the device specified the contents
// of the `bytes` array
uint32_t i2c_write_reg(i2c_bus *i2c,                           // i2c_write_reg
    short addr, 
    short reg, 
    uint8_t *bytes, 
    short size)
{
  // Generate content package
  uint8_t content[size + 1];
  // First byte is reg number
  content[0] = reg;
  // Copy in the content
  for (int i = 1; i <= size; i++)
  {
    content[i] = bytes[i - 1];
  }
  // Write the block with the included reg
  uint32_t status = i2c_write_block(i2c, addr, size + 1, content);
  // Return the i2c bus status
  return status;
}

uint32_t i2c_write_block(i2c_bus *i2c,                       // i2c_write_block
    short addr, 
    short size, 
    uint8_t *content)
{
  // Verify that the addressed device is currently active and registered
  // on the bus
  if (!i2c_bus_addr_active(i2c, addr))
  {
    ERR("No device found at current address (0x%02x)\n\n", addr);
    exit(EXIT_FAILURE);
  }
  // Clear the current fifo
  // TODO - Investigate if this is actually the best method
  BSC_C = BSC_C_CLEAR;
  // Set dev address
  BSC_SLAVE_ADDR = addr;
  // Clear the current status
  BSC_S = CLEAR_STATUS;
  // Set the length of the transfer + addr byte
  BSC_DATA_LEN = size;
  // Load the content into the fifo buffer
  for (int i = 0; i < size; i++)
  {
    BSC_FIFO = content[i];
  }
  // Start the write
  BSC_C = START_WRITE;
  // Wait for the i2c transfer to finish
  i2c_wait_done(i2c);
  // Return the value of the status register
  return BSC_S;
}
