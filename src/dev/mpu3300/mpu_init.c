///////////////////////////////////////////////////////////////////////////////
// Raspberry Pi GPIO Interface
// ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
// File: mpu_init.c
// PA Consulting - Lawrence Jones
///////////////////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include <string.h>
#include "mpu_private.h"
#include "macros.h"

///////////////////////////////////////////////////////////////////////////////
// INITIALISATION
///////////////////////////////////////////////////////////////////////////////
/*
   The mpu3300 is a 3-axis gyro, with an onboard i2c master for interfacing
   with an auxiliary accelerometer (or other sensor).
   The implementation below assumes i2c as the connection interface
   and makes use of an i2c library from github.com/LawrenceJones/raspi-gpio.

   The process of using an mpu goes as follows...

     1. Enable the chip.
     2. Configure gyro settings.
     3. Configure auxiliary sensor settings (opt)
     4. Read data from sensor (repeat)
     5. Disable the chip.

   This process breaks down naturally into different actions. In
   this implementation, they are...

     Allocate and initialise mpu details such as name, addr, etc.
     ============================================================
     Toggle sensor power mode
     Configure specific gyro settings
     Configure auxiliary sensor settings
     Read data from sensor

   The allocation of memory and initialisation return a struct of type
   Sensor which has all the required function pointers for mpu config
   and data reading.
 */

char mpu_default_config[] = \
" samplerate:200hz\
, fifo_en:on\
, fifo_overflow_en:on\
, fifo_selection:xg|yg|zg\
, data_ready_en:on\
, selftest:off\
, fs_range:225\
, i2c_bypass:off";

// Set the default values of mpu3300
void mpu_reset(Sensor *s)
{
  // If default is not null
  if (mpu_default_config != NULL)
  {
    // Apply default config
    mpu_config(s, mpu_default_config);
  }
}

// Set the i2c_bypass on
void mpu_i2c_bypass(Sensor *s, int bypass)
{
  if (bypass)
  {
    s->config(s, "i2c_bypass:on,i2c_mst_en:off");
  }
  else
  {
    s->config(s, "i2c_bypass:off");
  }
}

// Given name and i2c address, allocates and initialises a mpu
// struct with the given config options. `config` represents
// an array of KeyVal pairs which correspond to mpu configuration
// options. Supported config options are listed in config array.
Sensor *mpu_init(    i2c_bus*    i2c,
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
    exit(EXIT_FAILURE);
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
  s->model = MPU3300;
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
  // Assign pipe_running as false
  s->pipe_running = 0;
  ///////////////////////////////////////////////
  // Assign reset
  s->reset = &mpu_reset;                                    // RESET
  // Assign bypass switch
  s->i2c_bypass = &mpu_i2c_bypass;                          // I2C_BYPASS
  // Assign the read function
  s->read = &mpu_read;                                      // READ
  // Assign config
  s->config = &mpu_config;                                  // CONFIG
  // Assign selftest
  s->selftest = &mpu_selftest;                              // SELFTEST
  // Assign the pipe function
  s->pipe = &dev_pipe;                                      // PIPE
  // Assign the fifo capacity function
  s->fifo_capacity = &mpu_fifo_capacity;                    // FIFO_CAPACITY
  // Assign dealloc
  s->dealloc = &mpu_dealloc;                                // DEALLOC
  ///////////////////////////////////////////////
  // If config is not null
  if (config != NULL)
  {
    // Apply custom configuration
    mpu_config(s, config);
  }
  return s;
}
