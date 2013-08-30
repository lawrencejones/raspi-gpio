///////////////////////////////////////////////////////////////////////////////
// Raspberry Pi GPIO Interface
// ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
// File: mpu_conf_gyro.c
// PA Consulting - Lawrence Jones
///////////////////////////////////////////////////////////////////////////////

#include "mpu_private.h"

///////////////////////////////////////////////////////////////////////////////
// SUB-CONFIGS
///////////////////////////////////////////////////////////////////////////////

static void process_selftest_key(uint8_t *reg_val, char *_val)
{
  // Make all uppercase to avoid case errors
  char *val = make_upper(_val);
  // Find if the value contains 
  short off = (strstr(val, "off") != NULL),
        x = (strchr(val, 'X') != NULL) & !off,
        y = (strchr(val, 'Y') != NULL) & !off,
        z = (strchr(val, 'Z') != NULL) & !off;
  // Assign new x y z values
  *reg_val |= (x << 7) | (y << 6) | (z << 5);
  // Free the malloced string
  free(val);
}

static void process_fs_range_key(uint8_t *reg_val, char *val)
{
  // Extract new fs_range value
  int fs = atoi(val);
  // Verify one of allowed options
  if (fs == 225)
  {
    // Make bits 3 & 4 = 0
    *reg_val &= 0xe7u;
  }
  else if (fs == 450)
  {
    // Wipe bits 3 & 4
    *reg_val &= 0xe7u;
    // Set bit 3 to 1
    *reg_val |= 0x08u; 
  }
}

///////////////////////////////////////////////////////////////////////////////
// GYRO CONFIGURATION
///////////////////////////////////////////////////////////////////////////////

// Configures gyro settings
//    selftest: (x|y|z|off)
//    fs_range:(225:450)
uint8_t mpu_config_gyro(Sensor *s, KeyVal * pairs)
{
  // Fetch the current reg value
  uint8_t byte = FETCH_REG(MPU_GYRO_CONFIG),
          reg_val = byte;
  // Iterate through the settings
  do {
    if (!pairs->applied)
    {
      // Set applied to true initially
      pairs->applied = 1;
      // If configuring the selftest properties
      if (!strcmp(pairs->key, "selftest"))
      {
        // Configure for selftest
        process_selftest_key(&reg_val, pairs->val);
      }
      // If configuring full scale range
      else if (!strcmp(pairs->key, "fs_range"))
      {
        // Configure for full scale range
        process_fs_range_key(&reg_val, pairs->val);
      }
      // Else not supported key
      else
      {
        // So set applied to false
        pairs->applied = 0;
      }
    }
  } while ((pairs = pairs->next));
  // If reg value has changed, update it
  if (reg_val != byte)
  {
    // Set the register
    SET_REG(MPU_GYRO_CONFIG, reg_val);
    // Return 1 to signify change applied
    return 1;
  }
  // Else return 0 to signify no applied changes
  return 0;
}
