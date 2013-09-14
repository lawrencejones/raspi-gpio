///////////////////////////////////////////////////////////////////////////////
// Raspberry Pi GPIO Interface
// ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
// File: mpu_conf_power.c
// PA Consulting - Lawrence Jones
///////////////////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include <string.h>
#include "mpu_private.h"
#include "mpu_registers.h"

///////////////////////////////////////////////////////////////////////////////
// SUB-CONFIGS
///////////////////////////////////////////////////////////////////////////////

static void process_sleep_key(uint8_t *reg_val, char *val)
{
  // If value is on
  if (strstr(val, "on") != NULL)
  {
    // Set bit 6 to 1
    *reg_val |= (1 << 6);
  }
  // Else if value is off
  else if (strstr(val, "off") != NULL)
  {
    // Set bit 6 to 0
    *reg_val &= 0xbf;
  }
}

static void process_temp_key(uint8_t *reg_val, char *val)
{
  // If the value is yes
  if (strstr(val, "yes") != NULL)
  {
    // Set bit 3 to 0
    *reg_val &= 0xf7;
  }
  // Else if the value is no
  else if (strstr(val, "no") != NULL)
  {
    // Set bit 3 to 1
    *reg_val |= (1 << 3);
  }
}

static void process_gyro_sleep(uint8_t *reg_val, char *val)
{
  // Extract values
  short none = (strstr(val, "NONE") != NULL),
        x = (strchr(val, 'X') != NULL) & !none,
        y = (strchr(val, 'Y') != NULL) & !none,
        z = (strchr(val, 'Z') != NULL) & !none;
  // Compute new reg value
  *reg_val = (x << 2) | (y << 1) | (z << 0);
}

///////////////////////////////////////////////////////////////////////////////
// POWER CONFIGURATION
///////////////////////////////////////////////////////////////////////////////

// Configures the mpu power management
//    sleep: (on:off)
//    temp_en: (yes:no)
//    gyro_sleep: (x|y|z|none)
uint8_t mpu_config_power(Sensor *s, KeyVal * pairs)
{
  // Fetch the current register states
  uint8_t pow_1 = FETCH_REG(MPU_POWER_MGMT_1),
          _pow_1 = pow_1,
          pow_2 = FETCH_REG(MPU_POWER_MGMT_2),
          _pow_2 = pow_2;
  do {
    if (!pairs->applied)
    {
      // Set applied to true initially
      pairs->applied = 1;
      // If the key is sleep
      if (!strcmp(pairs->key, "sleep"))
      {
        // Configure for sleep
        process_sleep_key(&_pow_1, pairs->val);
      }
      // If key is temp_em
      else if (!strcmp(pairs->key, "temp_en"))
      {
        // Configure for temp enabling
        process_temp_key(&_pow_1, pairs->val);
      }
      // If the key is gyro_sleep
      else if (!strcmp(pairs->key, "gyro_sleep"))
      {
        // Configure for gyro sleep
        process_gyro_sleep(&_pow_2, pairs->val); 
      }
      // Else the key was not valid
      else
      {
        // Set applied to false
        pairs->applied = 0;
      }
    }
  } while ((pairs = pairs->next));
  // Keep track of whether a change has been applied
  int applied = 0;
  // If power register 1 has changed
  if (pow_1 != _pow_1)
  {
    // Then set the new reg value
    SET_REG(MPU_POWER_MGMT_1, _pow_1);
    // Set applied as true
    applied = 1;
  }
  // If second register has change
  if (pow_2 != _pow_2)
  {
    // Then set the new value
    SET_REG(MPU_POWER_MGMT_2, _pow_2);
    // Set applied as true
    applied = 1;
  }
  return applied;
}
