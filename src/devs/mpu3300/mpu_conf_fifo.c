///////////////////////////////////////////////////////////////////////////////
// Raspberry Pi GPIO Interface
// ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
// File: mpu_conf_fifo.c
// PA Consulting - Lawrence Jones
///////////////////////////////////////////////////////////////////////////////

#include "mpu_private.h"

///////////////////////////////////////////////////////////////////////////////
// SUB-CONFIGS
///////////////////////////////////////////////////////////////////////////////

static void process_fifo_selection_key(uint8_t *reg_val, char *_val)
{
  // Make upper case to avoid case sensitivity
  char *val = make_upper(_val);
  // Find if the config value contains
  short tmp = (strstr(val, "TMP") != NULL) << 7,
        xg  = (strstr(val, "XG")  != NULL) << 6,
        yg  = (strstr(val, "YG")  != NULL) << 5,
        zg  = (strstr(val, "ZG")  != NULL) << 4,
        xa  = (strstr(val, "XA")  != NULL) << 2,
        ya  = (strstr(val, "YA")  != NULL) << 1,
        za  = (strstr(val, "ZA")  != NULL) << 0;
  // Generate the resulting byte
  *reg_val = tmp | xg | yg | zg | xa | ya | za;
  // Free the malloced string
  free(val);
}

///////////////////////////////////////////////////////////////////////////////
// FIFO CONFIGURATION
///////////////////////////////////////////////////////////////////////////////

// Configure fifo settings
//    fifo_selection: (tmp|xg|yg|zg|xa|ya|za)
uint8_t mpu_config_fifo(Sensor *s, KeyVal * pairs)
{
  // Fetch the current register value
  uint8_t byte = FETCH_REG(MPU_FIFO_EN),
          reg_val = byte;
  // Iterate through the settings
  do {
    if (!pairs->applied)
    {
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
    SET_REG(MPU_FIFO_EN, reg_val);
    // Return 1 to signify applied change
    return 1;
  }
  // Else return 0 to show no change
  return 0;
}
