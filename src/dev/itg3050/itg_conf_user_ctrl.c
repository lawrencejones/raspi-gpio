///////////////////////////////////////////////////////////////////////////////
// Raspberry Pi GPIO Interface
// ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
// File: itg_conf_user_ctrl.c
// PA Consulting - Lawrence Jones
///////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "itg_private.h"
#include "itg_registers.h"

///////////////////////////////////////////////////////////////////////////////
// INT PIN CONFIGURATION
///////////////////////////////////////////////////////////////////////////////

// Configure the interupt pin and i2c bus
//    fifo_en: (yes|no)
//    i2c_bypass: (on|off)
//    reset_aux_i2c: (yes|no)
//    fifo_reset: (yes|no)
uint8_t itg_config_user_ctrl(Sensor *s, KeyVal *pairs)
{
  // Fetch current value of register
  uint8_t user_ctrl = FETCH_REG(ITG_USER_CTRL),
          _user_ctrl = user_ctrl;
  // Iterate through the setting pairs
  do {
    if (!pairs->applied)
    {
      // Initially set applied to true
      pairs->applied = 1;
      // If the fifo_en key
      if (!strcmp(pairs->key, "fifo_en"))
      {
        // Then process the fifo enable bit
        yn_toggle(&_user_ctrl, 6, pairs->val);
      }
      // If the i2c_bypass key
      else if (!strcmp(pairs->key, "i2c_bypass"))
      {
        // If you want to bypass
        if (strstr("yes,on,en", pairs->val) != NULL)
        {
          _user_ctrl &= 0xdf;
        }
        else
        {
          _user_ctrl |= 0x20;
        }
      }
      // If the int_rdy_en key
      else if (!strcmp(pairs->key, "int_rdy_en"))
      {
        // Then process the interrupt when device is ready
        yn_toggle(&_user_ctrl, 2, pairs->val);
      }
      // If key is fifo_reset
      else if (!strcmp(pairs->key, "fifo_reset"))
      {
        // Reset fifo
        yn_toggle(&_user_ctrl, 1, pairs->val);
      }
      // If the raw_rdy_en key
      else if (!strcmp(pairs->key, "raw_rdy_en"))
      {
        // Then process the raw data ready status
        yn_toggle(&_user_ctrl, 0, pairs->val);
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
  // If the value has changed
  if (user_ctrl != _user_ctrl)
  {
    // Then set the new value
    SET_REG(ITG_USER_CTRL, _user_ctrl);
    // Note a write
    applied++;
  }
  // Return applied value
  return applied;
}
