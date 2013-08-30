///////////////////////////////////////////////////////////////////////////////
// Raspberry Pi GPIO Interface
// ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
// File: mpu_dealloc.c
// PA Consulting - Lawrence Jones
///////////////////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include <string.h>
#include "mpu_private.h"

///////////////////////////////////////////////////////////////////////////////
// DEALLOC MPU
///////////////////////////////////////////////////////////////////////////////
/*
   After consideration, the current implementation will both reset
   and disable an mpu when the struct memory is released.
   All memory releases should be achieved through the below `mpu_dealloc`
   function to avoid the user missing other dependencies.
  
   IMPORTANT - Any auxiliary sensors connected to the mpu will
               also have their respective dealloc function called
               should the i2c_bypass setting not be in use.
               Once again, to avoid user errors where aux sensors
               are left enabled but unaccessable.
*/

void mpu_dealloc(Sensor **s)
{
  // If sensor is already null
  if ((*s) == NULL)
  {
    // The return without doing anything
    return;
  }
  // Free the device name
  free((*s)->name);
  // Free the controlling mux
  if ((*s)->mux)
  {
    free((*s)->mux);
  }
  // Free the sensor struct
  free(*s);
  // Null the pointer
  *s = NULL;
}
