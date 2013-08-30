///////////////////////////////////////////////////////////////////////////////
// Raspberry Pi GPIO Interface
// ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
// File: mpu_conf_i2c_mst.c
// PA Consulting - Lawrence Jones
///////////////////////////////////////////////////////////////////////////////

#include "mpu_private.h"
#include "mpu_registers.h"

///////////////////////////////////////////////////////////////////////////////
// SUB-CONFIGS
///////////////////////////////////////////////////////////////////////////////

static void process_multi_mst_key(uint8_t *reg_val, char *val)
{
  // If yes in value
  if (strstr(val, "yes") != NULL)
  {
    // Set bit 7 to 1
    *reg_val |= (1 << 7);
  }
  // Else if no
  else if (strstr(val, "no") != NULL)
  {
    // Set bit 7 to 0
    *reg_val &= 0x7f;
  }
}

static void process_wait_for_aux_key(uint8_t *reg_val, char *val)
{
  // If yes in value
  if (strstr(val, "yes") != NULL)
  {
    // Set bit 6 to 1
    *reg_val |= (1 << 6);
  }
  // Else if no
  else if (strstr(val, "no") != NULL)
  {
    // Set bit 6 to 0
    *reg_val &= 0xbf;
  }
}

static void process_i2c_mst_clk_key(uint8_t *reg_val, char *val)
{
  // Extract clk setting from the val string
  int clk = atoi(val);
  // Verify clk lies within parameters
  if ((clk > 0) && (clk < 16))
  {
    // Write out the first nibble of the reg
    *reg_val = (*reg_val & 0xf0);
    // Set the clk bits in the register
    *reg_val |= (0x0f & clk);
  }
}

///////////////////////////////////////////////////////////////////////////////
// I2C MASTER CONFIGURATION
///////////////////////////////////////////////////////////////////////////////

// Configures the i2c master
//    multi_mst_en: (yes:no)
//    wait_for_aux: (yes:no)
//    i2c_mst_clk: (0..15)
uint8_t mpu_config_i2c_mst(Sensor *s, KeyVal * pairs)
{
  // Fetch current register value
  uint8_t byte = FETCH_REG(MPU_I2C_MST_CTRL),
          reg_val = byte;
  // While there are keys left
  do {
    // If not yet applied
    if (!pairs->applied)
    {
      // Initially set applied as true
      pairs->applied = 1;
      // If multiple i2c master enable key
      if (strcmp(pairs->key, "multi_mst_en"))
      {
        // Configure for multiple i2c masters
        process_multi_mst_key(&reg_val, pairs->val);
      }
      // If wait for auxiliary sensor key
      else if (strcmp(pairs->key, "wait_for_aux"))
      {
        // Configure for aux sensor
        process_wait_for_aux_key(&reg_val, pairs->val);
      }
      // If i2c master clock key
      else if (strcmp(pairs->key, "i2c_mst_clk"))
      {
        // Configure for i2c master clock
        process_i2c_mst_clk_key(&reg_val, pairs->val);
      }
      // Else unsupported key
      else
      {
        // So set applied to false
        pairs->applied = 0;
      }
    }
  } while ((pairs = pairs->next));
  // If the reg value has changed
  if (reg_val != byte)
  {
    // Then set register
    SET_REG(MPU_I2C_MST_CTRL, reg_val);
    // And return 1 to signify change
    return 1;
  }
  // Else return 0 to show no change
  return 0;
}
