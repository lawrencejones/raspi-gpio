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

///////////////////////////////////////////////////////////////////////////////
// SUB-CONFIGS
///////////////////////////////////////////////////////////////////////////////

static void process_fifo_selection_key(uint8_t *reg_val, char *val)
{
  // Find if the config value contains
  short tmp = (strstr(val, "tmp") != NULL) << 7,
        xg  = (strstr(val, "xg")  != NULL) << 6,
        yg  = (strstr(val, "yg")  != NULL) << 5,
        zg  = (strstr(val, "zg")  != NULL) << 4,
        xa  = (strstr(val, "xa")  != NULL) << 3,
        ya  = (strstr(val, "ya")  != NULL) << 2,
        za  = (strstr(val, "za")  != NULL) << 1,
        ff  = (strstr(val, "ff")  != NULL) << 0;
  // Generate the resulting byte
  *reg_val = tmp | xg | yg | zg | xa | ya | za | ff;
}

///////////////////////////////////////////////////////////////////////////////
// FIFO CONFIGURATION
///////////////////////////////////////////////////////////////////////////////

// Configure fifo settings
//    fifo_selection: (tmp|xg|yg|zg|xa|ya|za|ff)
uint8_t itg_config_fifo(Sensor *s, KeyVal * pairs)
{
  // Fetch the current register value
  uint8_t byte = FETCH_REG(ITG_FIFO_EN),
          reg_val = byte;
  // Iterate through the settings
  do {
    if (!pairs->applied)
    {
      pairs->applied = 1;
      // If the fifo_selection key
      if (!strcmp(pairs->key, "fifo_selection"))
      {
        // Configure for fifo selection
        process_fifo_selection_key(&reg_val, pairs->val);
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
    SET_REG(ITG_FIFO_EN, reg_val);
    // Return 1 to signify applied change
    return 1;
  }
  // Else return 0 to show no change
  return 0;
}
