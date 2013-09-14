///////////////////////////////////////////////////////////////////////////////
// Raspberry Pi GPIO Interface
// ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
// File: itg_conf.c
// PA Consulting - Lawrence Jones
///////////////////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include "itg_private.h"
#include "dev/shared/dev_mux.h"

///////////////////////////////////////////////////////////////////////////////
// CONFIGURE GYRO SETTINGS
///////////////////////////////////////////////////////////////////////////////
/*
   Configuring settings is handled using KeyVal pairs, where these
   are structs containing a setting key and desired value.

   The `itg_configure` function is the only entry point for setting
   configuration values. All the other functions are (and for future
   changes, should) be declared as static and therefore private to
   this file.
 */

// `map` defines the link between keys and their config helper
// functions.
static ConfigFunctionMap map[] = {
  {
    "i2c_if_dis",
    &itg_config_i2c
  },
  {
    "gyro_offset_x,gyro_offset_y,gyro_offset_z",
    &itg_config_gyro_offset
  },
  {
    "fifo_selection",
    &itg_config_fifo
  },
  {
    "i2c_slv_addr,clkout_en",
    &itg_config_aux
  },
  {
    "samplerate,ext_sync_set,fs_sel,dlpf_cfg",
    &itg_config_sync
  },
  {
    "int_out,int_drive_type,latch_int_en,int_rdy_en,raw_rdy_en",
    &itg_config_int_pin
  },
  {
    "aux_burst_addr",
    &itg_config_aux
  },
  {
    "fifo_en,i2c_bypass,reset_aux_i2c,fifo_reset",
    &itg_config_user_ctrl
  },
  { // Termination element
    NULL, NULL
  }
};

// Just a proxy to pass to the general configuration function
int itg_config(Sensor *s, char *conf_str)
{
  // Call dev_config with the function map
  return dev_config(s, conf_str, map);
}
