///////////////////////////////////////////////////////////////////////////////
// Raspberry Pi GPIO Interface
// ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
// File: mpu_conf_int.c
// PA Consulting - Lawrence Jones
///////////////////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include <string.h>
#include "mpu_private.h"
#include "mpu_registers.h"

///////////////////////////////////////////////////////////////////////////////
// SUB-CONFIGS
///////////////////////////////////////////////////////////////////////////////

static void process_i2c_bypass_key(uint8_t *reg_val, char *val)
{
  // Toggle for on off
  yn_toggle(reg_val, 1, val);
}

static void process_fifo_overflow_en_key(uint8_t *reg_val, char *val)
{
  // Toggle for on off
  yn_toggle(reg_val, 4, val);
}

static void process_data_ready_en_key(uint8_t *reg_val, char *val)
{
  // Toggle for on off
  yn_toggle(reg_val, 0, val);
}

///////////////////////////////////////////////////////////////////////////////
// INT PIN CONFIGURATION
///////////////////////////////////////////////////////////////////////////////

// Configure the interupt pin and i2c bus
//    i2c_bypass: (on:off)
//    fifo_overflow_en: (on:off)
//    data_ready_en: (on:off)
uint8_t mpu_config_int_pin(Sensor *s, KeyVal *pairs)
{
  // Fetch current value of register
  uint8_t cfg = FETCH_REG(MPU_INT_PIN_CFG),
          _cfg = cfg,
          int_en = FETCH_REG(MPU_INT_ENABLE),
          _int_en = int_en;
  // Iterate through the setting pairs
  do {
    if (!pairs->applied)
    {
      // Initially set applied to true
      pairs->applied = 1;
      // If the i2c_bypass key
      if (!strcmp(pairs->key, "i2c_bypass"))
      {
        // Then process the bypass configuration
        process_i2c_bypass_key(&_cfg, pairs->val);
      }
      // Else if fifo_overflow_en key
      else if (!strcmp(pairs->key, "fifo_overflow_en"))
      {
        // Then process the fifo overflow enable bit
        process_fifo_overflow_en_key(&_int_en, pairs->val);
      }
      // Else if data_ready_en key
      else if (!strcmp(pairs->key, "data_ready_en"))
      {
        // Then process the data ready enable bit
        process_data_ready_en_key(&_int_en, pairs->val);
      }
      // Else unsupported key
      else
      {
        // So set applied to false
        pairs->applied = 0;
      }
    }

  } while ((pairs = pairs->next));
  // Keep track of the number of reg changes
  int applied = 0;
  // If the config reg differs
  if (cfg != _cfg)
  {
    // Then set the new value
    SET_REG(MPU_INT_PIN_CFG, _cfg);
    // Note a write
    applied++;
  }
  if (int_en != _int_en)
  {
    // Then set the new value
    SET_REG(MPU_INT_ENABLE, _int_en);
    // Note a write
    applied++;
  }
  // Return applied value
  return applied;
}
