///////////////////////////////////////////////////////////////////////////////
// Raspberry Pi GPIO Interface
// ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
// File: mpu_conf_user_ctrl.c
// PA Consulting - Lawrence Jones
///////////////////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include <string.h>
#include "mpu_private.h"
#include "mpu_registers.h"

///////////////////////////////////////////////////////////////////////////////
// SUB-CONFIGS
///////////////////////////////////////////////////////////////////////////////

static void process_i2c_mst_key(uint8_t *reg_val, char *val)
{
  // Toggle for on off
  yn_toggle(reg_val, 5, val);
}

static void process_fifo_en_key(uint8_t *reg_val, char *val)
{
  // Toggle for on off
  yn_toggle(reg_val, 6, val);
}

///////////////////////////////////////////////////////////////////////////////
// USER CONTROL CONFIGURATION
///////////////////////////////////////////////////////////////////////////////

// Configures the user settings
//    i2c_mst_en: (yes:no)
//    fifo_en: (on:off)
uint8_t mpu_config_user_ctrl(Sensor *s, KeyVal * pairs)
{
  // Fetch the current register value
  uint8_t byte = FETCH_REG(MPU_USER_CTRL),
          reg_val = byte;
  // Iterate through the setting keys
  do {
    // If this setting is not yet applied
    if (!pairs->applied)
    {
      // Set applied to true initially
      pairs->applied = 1;
      // If the key is i2c_mst_en
      if (!strcmp(pairs->key, "i2c_mst_en"))
      {
        // Configure for i2c master enable
        process_i2c_mst_key(&reg_val, pairs->val);
      }
      // If the key is fifo_en
      else if (!strcmp(pairs->key, "fifo_en"))
      {
        // Configure for the fifo
        process_fifo_en_key(&reg_val, pairs->val); 
      }
      // Else key not recognised
      else
      {
        // So set applied to false
        pairs->applied = 0;
      }
    }
  } while ((pairs = pairs->next));
  // If reg value differs
  if (reg_val != byte)
  {
    // Set reg value
    SET_REG(MPU_USER_CTRL, reg_val);
    // Return 1 to show change
    return 1;
  }
  // Else return 0 for no change
  return 0;
}
