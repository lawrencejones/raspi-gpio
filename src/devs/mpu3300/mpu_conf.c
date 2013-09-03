///////////////////////////////////////////////////////////////////////////////
// Raspberry Pi GPIO Interface
// ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
// File: mpu_conf.c
// PA Consulting - Lawrence Jones
///////////////////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include "mpu_private.h"
#include "devs/shared/dev_mux.h"

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
    "i2c_mst_en,fifo_en",
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

// Specific mpu config function to be attached as the config
// field of an mpu array. Hands majority of work to the dev_config
// function (dev/shared/dev_config.c).
int mpu_config(Sensor *s, char *conf_str)
{
  // If there is a multiplexer, configure for access
  if (s->mux)
  {
    // Set the channel
    s->mux->set_channel(s->mux, s->mux_channel);
  }
  // Call dev_config with the function map
  return dev_config(s, conf_str, map);
}
