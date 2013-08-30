///////////////////////////////////////////////////////////////////////////////
// Raspberry Pi GPIO Interface
// ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
// File: mpu_conf_samplerate.c
// PA Consulting - Lawrence Jones
///////////////////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include <string.h>
#include "mpu_private.h"
#include "mpu_registers.h"

///////////////////////////////////////////////////////////////////////////////
// SUB-CONFIGS
///////////////////////////////////////////////////////////////////////////////

static void process_samplerate_key(uint8_t *reg_val, uint8_t dlpf, char *val)
{
  // Initialise required vars
  int outr = 1000, samplerate = 0;
  // If dlpf is disabled
  if ((dlpf == 0) || (dlpf == 7))
  {
    outr = 8000;
  }
  // Extract sample rate from config pair
  // atoi will stop processing the number at any non digit
  samplerate = atoi(val);
  // Calculate the new sdiv (reg) value
  *reg_val = outr/samplerate - 1;
}

///////////////////////////////////////////////////////////////////////////////
// SAMPLERATE CONFIGURATION
///////////////////////////////////////////////////////////////////////////////

// Configures the sample rate of the sensor //////////////
//    samplerate: Nhz
uint8_t mpu_config_samplerate(Sensor *s, KeyVal *pairs)
{
  // Fetch the current reg value
  uint8_t byte = FETCH_REG(MPU_SMPLRT_DIV),
          reg_val = byte;
  do {
    // If not already applied
    if (!pairs->applied)
    {
      // Set applied to true initially
      pairs->applied = 1;
      // If the config is a samplerate key
      if (!strcmp(pairs->key, "samplerate"))
      {
        // Fetch the current digital low pass filter setting
        uint8_t dlpf = FETCH_REG(MPU_CONFIG) & 0x7u;
        // Configure for sample rate
        process_samplerate_key(&reg_val, dlpf, pairs->val);
      }
      // Else not supported key
      else
      {
        // Set applied to false
        pairs->applied = 0;
      }
    }
  } while ((pairs = pairs->next));
  // If the register is not already correct
  if (reg_val != byte)
  {
    // The set it's new value
    SET_REG(MPU_SMPLRT_DIV, reg_val);
    // Return 1 to show change
    return 1;
  }
  // Else return 0
  return 0;
}
