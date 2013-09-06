///////////////////////////////////////////////////////////////////////////////
// Raspberry Pi GPIO Interface
// ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
// File: itg_conf_itg.c
// PA Consulting - Lawrence Jones
///////////////////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include <string.h>
#include "itg_private.h"
#include "itg_registers.h"

///////////////////////////////////////////////////////////////////////////////
// SUB-CONFIGS
///////////////////////////////////////////////////////////////////////////////

static void process_i2c_if_dis_key(uint8_t *reg_val, char *val)
{
  // Toggle for on off
  yn_toggle(reg_val, 7, val);
}

///////////////////////////////////////////////////////////////////////////////
// I2C CONFIGURATION
///////////////////////////////////////////////////////////////////////////////

// Configure i2c settings
//   i2c_if_dis: (yes:no)
uint8_t itg_config_i2c(Sensor *s, KeyVal * pairs)
{
  // Fetch the current register value
  uint8_t byte = FETCH_REG(ITG_WHO_AM_I),
          reg_val = byte;
  // Iterate through the settings
  do {
    if (!pairs->applied)
    {
      pairs->applied = 1;
      // If the i2c interface enable key
      if (!strcmp(pairs->key, "i2c_if_dis"))
      {
        // Configure for i2c availability
        process_i2c_if_dis_key(&reg_val, pairs->val);
      }
      // Else not supported key
      else
      {
        // So set applied to false
        pairs->applied = 0;
      }
    }
  } while ((pairs = pairs->next));
  // If the new reg val is different from the current
  if (reg_val != byte)
  {
    // Set the register value to reg_val
    SET_REG(ITG_WHO_AM_I, reg_val);
    // Return 1 to signify applied change
    return 1;
  }
  // Else return 0 to show no change
  return 0;
}
