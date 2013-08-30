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
  // If the value is a yes
  if (strstr(val, "yes") != NULL)
  {
    // Make bit 5 equal 1
    *reg_val |= (1 << 5);
  }
  // Else if the value is a no
  else if (strstr(val, "no") != NULL)
  {
    // Make bit 5 equal to 0
    *reg_val &= 0xdf;
  }
}

static void process_fifo_key(uint8_t *reg_val, char *val)
{
  // If the value is on
  if (strstr(val, "on") != NULL)
  {
    // Make bit 6 equal 1
    *reg_val |= (1 << 6);
  }
  // Else if the value is off
  else if (strstr(val, "off") != NULL)
  {
    // Make bit 6 equal 0
    *reg_val &= 0xbf;
  }
}

///////////////////////////////////////////////////////////////////////////////
// USER CONTROL CONFIGURATION
///////////////////////////////////////////////////////////////////////////////

// Configures the user settings
//    i2c_mst_en: (yes:no)
//    fifo: (on:off)
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
      // If the key is fifo
      else if (!strcmp(pairs->key, "fifo"))
      {
        // Configure for the fifo
        process_fifo_key(&reg_val, pairs->val); 
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
