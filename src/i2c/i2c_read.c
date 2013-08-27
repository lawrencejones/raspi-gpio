///////////////////////////////////////////////////////////////////////////////
// Raspberry Pi GPIO Interface
// ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
// File: i2c_read.c
// PA Consulting - Lawrence Jones
///////////////////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include "i2c_private.h"
#include "i2c_res.h"

///////////////////////////////////////////////////////////////////////////////
// I2C Read
///////////////////////////////////////////////////////////////////////////////

// Read a single byte from the given addr on the given bus
uint8_t i2c_read_byte(i2c_bus *i2c, short addr)                // i2c_read_byte
{
  // Clear the fifo
  BSC_C = BSC_C_CLEAR;
  // Set new address
  BSC_SLAVE_ADDR = addr;
  // Clear the bus status
  BSC_S = CLEAR_STATUS;
  // Start the bus read
  BSC_C = START_READ;
  // Wait for the bus to clear
  i2c_wait_done(i2c);
  // Return the result in the fifo
  return (uint8_t)BSC_FIFO;
}

// Read a single byte from the given register at the given
// addresss, on i2c_bus*
uint8_t i2c_read_reg(i2c_bus *i2c, short addr, short reg)       // i2c_read_reg
{
  // Read block of 1 byte
  uint8_t *result = i2c_read_block(i2c, addr, reg, 1),
          byte = result[0];
  // Free the result pointer
  free(result);
  // Return the literal uint8_t byte
  return byte;
}

// Same as the read byte, just allows specification of block
// size to read. Returns an array of uint32_t in the heap
// that contains all the information read out of the FIFO reg
uint8_t *i2c_read_block(i2c_bus *i2c,                         // i2c_read_block
    short addr, 
    short reg,
    short block_size)
{
  // Declare result array
  uint8_t *result;
  // Clear the fifo
  BSC_C = BSC_C_CLEAR;
  // Set new address
  BSC_SLAVE_ADDR = addr;
  // Set size of transfer
  BSC_DATA_LEN = 1;
  // Write the register to the fifo
  BSC_FIFO = reg;
  // Clear status
  BSC_S = CLEAR_STATUS;
  // Initiate the transfer
  BSC_C = START_WRITE;
  // Wait for acknowledgement
  i2c_wait_done(i2c);
  // Set length to block size
  BSC_DATA_LEN = block_size;
  // Clear the bus status
  BSC_S = CLEAR_STATUS;
  // TODO - Setup error checking for the reg transfer
  // Start the bus read
  BSC_C = START_READ;
  // Wait for the bus to clear
  i2c_wait_done(i2c);
  // Allocate the uint8_t array
  result = malloc(sizeof(uint8_t) * block_size);
  // Verify successful malloc
  if (!result)
  {
    ERR("Error allocating memory (malloc). Read failed.\n\n");
    exit(EXIT_FAILURE);
  }
  // Read result into the result array
  for (int i = 0; i < block_size; i++)
  {
    // printf("0x%20x\n", BSC_FIFO);
    result[i] = BSC_FIFO;
  }
  // Return the result
  // NOTE - Memory responsibility passed to calling function
  return result;
}
