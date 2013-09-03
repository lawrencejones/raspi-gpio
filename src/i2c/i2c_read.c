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

// Given an i2c bus, an array of bytes for storing the fifo
// contents and the number of bytes left to store, pull the
// sixteen bytes from the fifo buffer into data.
static void flush_fifo_cache( i2c_bus *i2c,
                              uint8_t *data,
                              int *bytes_left )
{
  // Initialise a count variable
  int count = 16;
  // While bytes_left does not equal 0, copy the next fifo
  // byte into data
  while ((*bytes_left != 0) && count--)
  {
    // Get next byte
    *data = BSC_FIFO;
    // Increment the pointer
    data++;
    // Decrement the number of bytes left
    (*bytes_left)--;
  }
}

// Same as the read byte, just allows specification of block
// size to read. Returns an array of uint32_t in the heap
// that contains all the information read out of the FIFO reg
uint8_t *i2c_read_block(i2c_bus *i2c,                         // i2c_read_block
                        short addr, 
                        short reg,
                        int   block_size)
{
  // Declare result array
  uint8_t *result = malloc(sizeof(uint8_t) * block_size);
  // Verify successful malloc
  if (!result)
  {
    ERR("Error allocating memory (malloc). Read failed.\n\n");
    exit(EXIT_FAILURE);
  }
  // Prep the bus ////////////////////////////////////////////
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

  // Read block //////////////////////////////////////////////
  // TODO - Setup error checking for the reg transfer
  // Start the bus read
  BSC_C = START_READ;
  // Initialise a count of how many times fifo has been flushed
  // Keep track of the bytes left to transfer
  int count = 0,
      bytes_left = block_size,
      code = 0;
  // While there are bytes left
  while (bytes_left > 0) {
    // Wait for the fifo buffer to fill
    switch ((code = i2c_wait_fifo(i2c)))
    {
      // If the transfer has complete
      case FIFO_DONE:
      // Or the first in first out is full
      case FIFO_FULL:
        // Flush the fifo cache into the next result chunk of 16 bytes
        flush_fifo_cache( i2c, 
                          (result + (16 * count++)), 
                          &bytes_left );
        // Break from the switch
        break;
      // Or is the fifo has timed out
      case FIFO_TIMEOUT:
        ERR("%s%s%s", 
            "Fifo has timed out.",
          "\nThis means the fifo did not fill, ",
            "nor did the transfer complete.\n\n");
        exit(EXIT_FAILURE);
        break;
      // Or if an unknown error
      case FIFO_ERR:
        ERR("Unknown fifo error.\n\n");
        exit(EXIT_FAILURE);
        break;
    }
  };
  
  // Return the result
  // NOTE - Memory responsibility passed to calling function
  return result;
}
