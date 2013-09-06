///////////////////////////////////////////////////////////////////////////////
// Raspberry Pi GPIO Interface
// ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
// File: itg_conf_int.c
// PA Consulting - Lawrence Jones
///////////////////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include <string.h>
#include "itg_private.h"
#include "itg_registers.h"

///////////////////////////////////////////////////////////////////////////////
// INT PIN CONFIGURATION
///////////////////////////////////////////////////////////////////////////////

// Configure the interupt pin and i2c bus
//    int_out: (on|off)
//    latch_int_en: (yes|no)
//    int_rdy_en: (yes|no)
//    raw_rdy_en: (yes|no)
uint8_t itg_config_int_pin(Sensor *s, KeyVal *pairs)
{
  // Fetch current value of register
  uint8_t int_en = FETCH_REG(ITG_INT_CFG),
          _int_en = int_en;
  // Iterate through the setting pairs
  do {
    if (!pairs->applied)
    {
      // Initially set applied to true
      pairs->applied = 1;
      // If the int_out key
      if (!strcmp(pairs->key, "int_out"))
      {
        // Then process logic level for the interupt output pin
        yn_toggle(&_int_en, 7, pairs->val);
      }
      // If the latch_int_en key
      else if (!strcmp(pairs->key, "latch_int_en"))
      {
        // Then process the latch mode
        yn_toggle(&_int_en, 5, pairs->val);
      }
      // If the int_rdy_en key
      else if (!strcmp(pairs->key, "int_rdy_en"))
      {
        // Then process the interrupt when device is ready
        yn_toggle(&_int_en, 2, pairs->val);
      }
      // If the raw_rdy_en key
      else if (!strcmp(pairs->key, "raw_rdy_en"))
      {
        // Then process the raw data ready status
        yn_toggle(&_int_en, 0, pairs->val);
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
  if (int_en != _int_en)
  {
    // Then set the new value
    SET_REG(ITG_INT_CFG, _int_en);
    // Note a write
    applied++;
  }
  // Return applied value
  return applied;
}
