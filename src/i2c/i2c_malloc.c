///////////////////////////////////////////////////////////////////////////////
// Raspberry Pi GPIO Interface
// ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
// File: i2c_malloc.c
// PA Consulting - Lawrence Jones
///////////////////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include "i2c_private.h"

///////////////////////////////////////////////////////////////////////////////
// I2C MALLOC
///////////////////////////////////////////////////////////////////////////////

// Allocate memory for an i2c_dev struct in the heap
i2c_dev* i2c_dev_malloc(short addr)                           // i2c_dev_malloc
{
  // malloc the dev struct
  i2c_dev *dev = malloc(sizeof(i2c_dev));
  // Verify successful malloc
  if (!dev)
  {
    ERR("Memory allocation failed (malloc) of dev.\n\n");
    exit(EXIT_FAILURE);
  }
  // Set address value
  dev->addr = addr;
  // Set the next dev to NULL
  dev->next = NULL;
  // Return dev pointer
  return dev;
}
