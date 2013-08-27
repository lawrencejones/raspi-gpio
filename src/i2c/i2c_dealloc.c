///////////////////////////////////////////////////////////////////////////////
// Raspberry Pi GPIO Interface
// ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
// File: i2c_init.c
// PA Consulting - Lawrence Jones
///////////////////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include "i2c_private.h"

///////////////////////////////////////////////////////////////////////////////
// I2C DEALLOC
///////////////////////////////////////////////////////////////////////////////

// Deallocate all the heap memory required for the
// bus, including all it's devices
void i2c_dev_dealloc(i2c_dev **a)                            // i2c_dev_dealloc
{
  // Declare placeholder
  i2c_dev *b = *a;
  // While there is a i2c_dev
  while ((*a = b))
  {
    // Advance
    b = b->next;
    // Free previous
    free(*a);
  }
  // Null the callee's pointer up the stack
  *a = NULL;
}
