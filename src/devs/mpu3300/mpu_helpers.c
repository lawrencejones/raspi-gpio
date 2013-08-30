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

///////////////////////////////////////////////////////////////////////////////
// HELPERS
///////////////////////////////////////////////////////////////////////////////

// Helper function to uppercase a string
char* make_upper(char* str)
{
  // malloc result
  char *res = malloc(sizeof(char) * strlen(str) + 1);
  // Copy the string
  strcpy(res, str);
  // Uppercase it
  int i = 0;
  // While not end of string
  while (str[i])
  {
    // Uppercase the string
    str[i] = toupper(str[i]);
    i++;
  }
  // Return the uppercased result
  return res;
}

///////////////////////////////////////////////////////////////////////////////
// CONFIG HELPERS
///////////////////////////////////////////////////////////////////////////////

void yn_toggle(uint8_t *reg, int bit, char *yn)
{
  // If the string is yes|on
  if (strstr("yes,on", yn) != NULL)
  {
    // Set the value of the byte to a 1 on the given bit
    *reg |= (1 << bit);
  }
  // Else if a no|off
  else if (strstr("no,off", yn) != NULL)
  {
    // Set the value of the byte to 0 on the given bit
    *reg &= ~(1 << bit);
  }
}

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
