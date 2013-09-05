///////////////////////////////////////////////////////////////////////////////
// Raspberry Pi GPIO Interface
// ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
// File: mpu_conf_gyro_offset.c
// PA Consulting - Lawrence Jones
///////////////////////////////////////////////////////////////////////////////

#include "itg_private.h"
#include "itg_registers.h"

///////////////////////////////////////////////////////////////////////////////
// I2C CONFIGURATION
///////////////////////////////////////////////////////////////////////////////

// Configure i2c settings
//   gyro_offset_x: N
//   gyro_offset_y: N
//   gyro_offset_z: N
uint8_t mpu_config_i2c(Sensor *s, KeyVal * pairs)
{
  int applied;
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
        // Set the value
        SET_REG(addr, 0xff & (offset >> 16));
        SET_REG(addr + 1, 0xff & offset);
        // Increment applied
        applied++;
      }
    }
  } while ((pairs = pairs->next));
  // Return number of register changes
  return applied;
}

