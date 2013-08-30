///////////////////////////////////////////////////////////////////////////////
// Raspberry Pi GPIO Interface
// ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
// File: mpu_conf.c
// PA Consulting - Lawrence Jones
///////////////////////////////////////////////////////////////////////////////

#include "mpu_private.h"

///////////////////////////////////////////////////////////////////////////////
// CONFIGURE GYRO SETTINGS
///////////////////////////////////////////////////////////////////////////////
/*
   Configuring settings is handled using KeyVal pairs, where these
   are structs containing a setting key and desired value.

   The `mpu_configure` function is the only entry point for setting
   configuration values. All the other functions are (and for future
   changes, should) be declared as static and therefore private to
   this file.
 */

// `map` defines the link between keys and their config helper
// functions.
static ConfigFunctionMap map[] = {
  {
    "samplerate", 
    &mpu_config_samplerate
  },
  {
    "selftest,fsrange",
    &mpu_config_gyro
  },
  {
    "fifo_selection",
    &mpu_config_fifo
  },
  {
    "i2c_mst_en,fifo",
    &mpu_config_user_ctrl
  },
  {
    "sleep,temp_en,gyro_sleep",
    &mpu_config_power
  },
  {
    "multi_mst_en,wait_for_aux,i2c_mst_clk",
    &mpu_config_i2c_mst
  },
  {
    "i2c_slave_rw,i2c_slave_addr,i2c_slave_reg,i2c_slave_len, \
     i2c_slave_en,i2c_slave_byte_swap,i2c_slave_reg_dis,i2c_slave_grp",
    &mpu_config_i2c_slv
  },
  {
    "i2c_bypass",
    &mpu_config_int_pin
  },
  { // Termination element
    NULL, NULL
  }
};

///////////////////////////////////////////////////////////////////////////////
// ENTRY FUNCTION FOR CONFIG
///////////////////////////////////////////////////////////////////////////////

// Given an array of KeyVal structs, iterate through all pairs and
// if this setting is implemented, then modify the mpu settings.
// Sensor *s is a pointer to the mpu that should be configured.
// Returns number of detected register changes made
int mpu_configure(Sensor *s, char *conf_str)
{
  // Generate the keyval
  KeyVal *k = str_to_keyval(conf_str);
  // If k is null
  if (k == NULL)
  {
    // Return 0 for no changes
    return 0;
  }
  // Initialise counters
  int applied = 0;
  // While there is another keyval
  do 
  {
    // If the current keyval has not already been applied
    if (!k->applied)
    {
      // Declare a config helper function pointer
      Configs handle = NULL;
      // Counter for the config map
      int j = 0;
      // Iterate through the map while there are valid keys
      // and a config helper has not been found/called
      do {
        // If the current key is present in the keys
        if (strstr(map[j].keys, k->key) != NULL)
        {
          // Assign the handle value
          handle = map[j].helper;
          // Call the handle function and add to applied
          applied += handle(s, k);
        }
      } while (!handle && map[++j].keys);
    }
  } while ((k = k->next));
  // Dealloc the keyval
  keyval_dealloc(&k);
  return applied;
}
