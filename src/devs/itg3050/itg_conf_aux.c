///////////////////////////////////////////////////////////////////////////////
// Raspberry Pi GPIO Interface
// ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
// File: itg_conf_fifo.c
// PA Consulting - Lawrence Jones
///////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "itg_private.h"
#include "itg_registers.h"
#include "macros.h"

///////////////////////////////////////////////////////////////////////////////
// SUB-CONFIGS
///////////////////////////////////////////////////////////////////////////////

static void process_i2c_slv_addr_key(uint8_t *reg_val, char *val)
{
  // Extract slave address key
  short addr = DEX_TO_INT(val);
  // Generate the resulting byte
  *reg_val = (0x80 & *reg_val) + (0x7f & *reg_val);
}

static void process_clkout_en_key(uint8_t *reg_val, char *val)
{
  // Toggle for on off
  yn_toggle(reg_val, 7, val);
}

///////////////////////////////////////////////////////////////////////////////
// FIFO CONFIGURATION
///////////////////////////////////////////////////////////////////////////////

// Configure fifo settings
//    i2c_slv_addr: 0xN
//    clkout_en: (on|off)
uint8_t itg_config_fifo(Sensor *s, KeyVal * pairs)
{
  // Fetch the current register value
  uint8_t byte = FETCH_REG(ITG_AUX_SLV_ADDR),
          reg_val = byte;
  // Iterate through the settings
  do {
    if (!pairs->applied)
    {
      pairs->applied = 1;
      // If the i2c_slv_addr key
      if (!strcmp(pairs->key, "i2c_slv_addr"))
      {
        // Configure the aux i2c slave addr
        process_i2c_slv_addr_key(&reg_val, pairs->val);
      }
      // Else if the clkout_en key
      else if (!strcmp(pairs->key, "clkout_en"))
      {
        // Configure the reference clock out
        process_clkout_en_key(&reg_val, pairs->val);
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
    SET_REG(ITG_AUX_SLV_ADDR, reg_val);
    // Return 1 to signify applied change
    return 1;
  }
  // Else return 0 to show no change
  return 0;
}
