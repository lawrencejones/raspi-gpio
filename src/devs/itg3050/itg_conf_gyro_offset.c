///////////////////////////////////////////////////////////////////////////////
// Raspberry Pi GPIO Interface
// ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
// File: itg_conf_gyro_offset.c
// PA Consulting - Lawrence Jones
///////////////////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include <string.h>
#include "itg_private.h"
#include "itg_registers.h"

///////////////////////////////////////////////////////////////////////////////
// I2C CONFIGURATION
///////////////////////////////////////////////////////////////////////////////

// Configure i2c settings
//   gyro_offset_x: N
//   gyro_offset_y: N
//   gyro_offset_z: N
uint8_t itg_config_gyro_offset(Sensor *s, KeyVal * pairs)
{
  int applied = 0;
  // Iterate through the settings
  do {
    if (!pairs->applied)
    {
      pairs->applied = 1;
      // Find the reg address
      short addr = 0;
      // If the x offset key
      if (!strcmp(pairs->key, "gyro_offset_x"))
      {
        // Set address
        addr = ITG_X_OFFS_H;
      }
      // Else if y offset key
      else if (!strcmp(pairs->key, "gyro_offset_y"))
      {
        // Set address
        addr = ITG_Y_OFFS_H;
      }
      // Else if z offset key
      else if (!strcmp(pairs->key, "gyro_offset_z"))
      {
        // Set address
        addr = ITG_Z_OFFS_H;
      }
      // Else not supported key
      else
      {
        // So set applied to false
        pairs->applied = 0;
      }
      // If key was present...
      if (pairs->applied)
      {
        // Extract the value
        int offset = atoi(pairs->val);
        uint8_t h = 0xff & (offset >> 8),
                l = 0xff & (offset);
        // Set the value
        SET_REG(addr, h);
        SET_REG(addr + 1, l);
        // Increment applied
        applied++;
      }
    }
  } while ((pairs = pairs->next));
  // Return number of register changes
  return applied;
}

