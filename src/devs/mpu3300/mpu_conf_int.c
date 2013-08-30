///////////////////////////////////////////////////////////////////////////////
// Raspberry Pi GPIO Interface
// ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
// File: mpu_conf_int.c
// PA Consulting - Lawrence Jones
///////////////////////////////////////////////////////////////////////////////

#include "mpu_private.h"

///////////////////////////////////////////////////////////////////////////////
// SUB-CONFIGS
///////////////////////////////////////////////////////////////////////////////

static void process_i2c_bypass_key(uint8_t *reg_val, char *val)
{
  // Toggle for on off
  yn_toggle(reg_val, 1, val);
}

///////////////////////////////////////////////////////////////////////////////
// INT PIN CONFIGURATION
///////////////////////////////////////////////////////////////////////////////

// Configure the interupt pin and i2c bus
//    i2c_bypass: (on:off)
uint8_t mpu_config_int_pin(Sensor *s, KeyVal * pairs)
{
  // Fetch current value of register
  uint8_t byte = FETCH_REG(MPU_INT_PIN_CFG),
          reg_val = byte;
  // Iterate through the setting pairs
  do {
    if (!pairs->applied)
    {
      // Initially set applied to true
      pairs->applied = 1;
      // If the i2c_bypass key
      if (!strcmp(pairs->key, "i2c_bypass"))
      {
        // The process the bypass configuration
        process_i2c_bypass_key(&reg_val, pairs->val);
      }
      // Else unsupported key
      else
      {
        // So set applied to false
        pairs->applied = 0;
      }
    }

  } while ((pairs = pairs->next));
  // If the register value differs
  if (reg_val != byte)
  {
    // The set the new value
    SET_REG(MPU_INT_PIN_CFG, reg_val);
    // Return 1 to signify reg write
    return 1;
  }
  // Else return 0 for no change
  return 0;
}
