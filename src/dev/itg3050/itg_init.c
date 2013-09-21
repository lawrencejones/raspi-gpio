///////////////////////////////////////////////////////////////////////////////
// Raspberry Pi GPIO Interface
// ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
// File: itg_init.c
// PA Consulting - Lawrence Jones
///////////////////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include <string.h>
#include "itg_private.h"
#include "macros.h"

///////////////////////////////////////////////////////////////////////////////
// INITIALISATION
///////////////////////////////////////////////////////////////////////////////
/*
   The itg3050 is a 3-axis gyro, with an onboard i2c master for interfacing
   with an auxiliary accelerometer (or other sensor).
   The implementation below assumes i2c as the connection interface
   and makes use of an i2c library from github.com/LawrenceJones/raspi-gpio.
 */

char itg_default_config[] = \
" fifo_selection:xg|yg|zg\
, samplerate:200hz\
, fs_sel:250\
, fifo_en:yes\
, i2c_bypass:yes";

// Set the default values of itg3050
void itg_reset(Sensor *s)
{
  // If default is not null
  if (itg_default_config != NULL)
  {
    // Apply default config
    itg_config(s, itg_default_config);
  }
}

// Set the i2c_bypass on
void itg_i2c_bypass(Sensor *s, int bypass)
{
  if (bypass)
  {
    s->config(s, "i2c_bypass:on");
  }
  else
  {
    s->config(s, "i2c_bypass:off");
  }
}

// Given name and i2c address, allocates and initialises a itg
// struct with the given config options. `config` represents
// an array of KeyVal pairs which correspond to itg configuration
// options. Supported config options are listed in config array.
Sensor *itg_init(    i2c_bus*    i2c,
                     int         i2c_addr,
                     Mux*        mux,
                     int         mux_channel,
                     char*       config  )
{
  // malloc the sensor struct
  Sensor *s = malloc(sizeof(Sensor));
  // Verify malloc success
  if (!s)
  {
    // malloc has failed, print error and exit
    ERR("Failed to allocate memory (malloc) for Sensor *s.\n");
    return NULL;
  }

  // Assign for error
  int err = 0;
  // Verify the bus
  if ((err = dev_fails_to_respond(i2c, i2c_addr, mux, mux_channel)))
  {
    // Handle errors if necessary, then return null pointer
    return NULL;
  }

  ///////////////////////////////////////////////
  // Assign the enum for model
  s->model = ITG3050;
  // Assign enum for sensor type
  s->type = GYRO;
  ///////////////////////////////////////////////
  // Assign pointer to the i2c bus
  s->i2c = i2c;
  // Assign the i2c addr
  s->i2c_addr = i2c_addr;
  // Assign the mux handle, null or otherwise
  s->mux = mux;
  // Assign the mux channel setting
  s->mux_channel = mux_channel;
  ///////////////////////////////////////////////
  // Assign reset
  s->reset = &itg_reset;                                    // RESET
  // Assign i2c bypass switch
  s->i2c_bypass = &itg_i2c_bypass;                          // I2C_BYPASS
  // Assign the read function
  s->read = &itg_read;                                      // READ
  // Assign config
  s->config = &itg_config;                                  // CONFIG
  // Assign the pipe function
  s->pipe = &dev_pipe;                                      // PIPE
  // Assign the fifo capacity function
  s->fifo_capacity = &itg_fifo_capacity;                    // FIFO_CAPACITY
  // Assign dealloc
  s->dealloc = &itg_dealloc;                                // DEALLOC
  ///////////////////////////////////////////////

  // If config is not null
  if (config != NULL)
  {
    // Apply custom configuration
    itg_config(s, config);
  }
  return s;
}
