///////////////////////////////////////////////////////////////////////////////
// Raspberry Pi GPIO Interface
// ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
// File: mpu_read.c
// PA Consulting - Lawrence Jones
///////////////////////////////////////////////////////////////////////////////

#include "mpu_private.h"
#include "mpu_registers.h"
#include "../shared/devs_sensor.h"

///////////////////////////////////////////////////////////////////////////////
// READ SENSOR DATA
///////////////////////////////////////////////////////////////////////////////
/*
   Only one entry point for the reading of data. The function
   `mpu_read` conforms to function type `ReadAxes` and takes the
   mpu sensor pointer along with a target for reading.

   Currently possible targets are `HOST` or `AUX`, standing
   for the native gyro axes and aux sensor respectively.
*/

// Retrieves only the mpu's gyro axes data, then returns
// the data as an Axes struct pointer.
static Axes *read_gyro(Sensor *s)
{
  // malloc new axes struct
  Axes *res = malloc(sizeof(Axes));
  // Read the results in a burst, starting from xout reg
  uint8_t *readings = i2c_read_block( s->i2c,
                                      s->i2c_addr,
                                      MPU_XOUT_H,
                                      6 );
  // Extract x y z values
  res->x = (readings[0] << 8) | readings[1];
  res->y = (readings[2] << 8) | readings[3];
  res->z = (readings[4] << 8) | readings[5];
  // Free the array
  free(readings);
  // Set type of results
  res->type = GYRO;
  // No next axes (not reading aux here)
  res->next = NULL;
  // Return the resulting readings
  return res;
}

// Retrieves only the aux sensor's axes data, then returns
// the data as an Axes struct pointer.
static Axes *read_aux(Sensor *s)
{
  return NULL;
}

// Entry point for all reads. Takes a Sensor pointer (the selected mpu)
// and a target. This is currently either `HOST` or `AUX`, as explained
// above.
Axes *mpu_read(Sensor *s, target_t t)
{
  // Check for valid sensor pointer
  if (s == NULL)
  { 
    ERR("Sensor pointer is not valid.\n\n");
    exit(EXIT_FAILURE);
  }
  // If targetting the mpu
  if (t == HOST)
  {
    // Return the gyro readings
    return read_gyro(s);
  }
  // Else if targetting the auxiliary
  else if (t == AUX)
  {
    // Return the auxiliary sensor readings
    return read_aux(s);
  }
  return NULL;
}
