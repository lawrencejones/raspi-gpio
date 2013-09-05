///////////////////////////////////////////////////////////////////////////////
// Raspberry Pi GPIO Interface
// ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
// File: mpu_helpers.c
// PA Consulting - Lawrence Jones
///////////////////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "mpu_private.h"
#include "macros.h"

///////////////////////////////////////////////////////////////////////////////
// MPU ON/OFF TOGGLE
///////////////////////////////////////////////////////////////////////////////
/*
   For ease of use, two functions are provided to enable and disable
   the mpu sensor using sensible defaults and/or the configuration
   provided in KeyVal pairs
*/

// Turns the mpu on and deals with sensible defaults
int mpu_enable(Sensor *s, KeyVal *settings)
{
  return 0;
}

// Disables the mpu sensor
int mpu_disable(Sensor *s)
{
  return 0;
}
