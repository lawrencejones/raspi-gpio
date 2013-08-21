///////////////////////////////////////////////////////////////////////////////
// Raspberry Pi GPIO Interface
// ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
// File: mpu3300.c
// PA Consulting - Lawrence Jones
///////////////////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include <stdio.h>
#include "mpu3300.h"
#include "../tools/src/macros.h"

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

// Given name and i2c address, allocates and initialises a mpu
// struct with the given config options. `config` represents
// an array of KeyVal pairs which correspond to mpu configuration
// options. Supported config options are listed in config array.
Sensor *mpu_init(char*   name, 
                 int     i2c_addr,
                 KeyVal* config)
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
  // Assign given sensor properties
  s->name = malloc(sizeof(char) * strlen(name) + 1);
  // Make copies of strings to avoid stack expiry
  strcpy(s->name, name);
  // Assign the i2c addr
  s->i2c_addr = i2c_addr;
  // Assign the enum for model
  s->model = MPU3300;
  // Assign enum for sensor type
  s->type = GYRO;
  // Assign the function pointer for an enable method
  s->enable = &mpu_enable;
  // Assign the read function
  s->read = &mpu_read;
  // Assign the prep function
  s->prep = &prep_board;
  return NULL;
}

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

//////////////////////////////////////////////////////////
// Configures the sample rate of the sensor //////////////
//    samplerate: Nhz
static uint8_t mpu_config_samplerate(KeyVal *pairs);

//////////////////////////////////////////////////////////
// Configures gyro settings //////////////////////////////
//    selftest: (x|y|z|off)
//    fs_range:(225:450)
static uint8_t mpu_config_gyro(KeyVal *pairs);

//////////////////////////////////////////////////////////
// Configures the fifo settings //////////////////////////
//    fifo_selection: (tmp|xg|yg|zg|xa|ya|za)
static uint8_t mpu_config_fifo(KeyVal *pairs);

//////////////////////////////////////////////////////////
// Configures the user settings //////////////////////////
//    i2c_mst_en: (yes:no)
//    fifo: (on:off)
static uint8_t mpu_config_user_ctrl(KeyVal *pairs);

//////////////////////////////////////////////////////////
// Configures the mpu power management ///////////////////
//    sleep: (on:off)
//    temp_en: (yes:no)
//    gyro_sleep: (x|y|z|none)
static uint8_t mpu_config_power(KeyVal *pairs);

///////////////////////////////////////////////////////////////////////////////
// CONFIGURE AUX SENSOR SETTINGS
///////////////////////////////////////////////////////////////////////////////
/*
   All i2c slave related configuration functionality should appear
   below. All should be static and only accessed through the mpu_configure
   function.
*/

//////////////////////////////////////////////////////////
// Configures the i2c master /////////////////////////////
//    multi_mst_en: (yes:no)
//    wait_for_aux: (yes:no)
//    i2c_mst_clk: (0..15)
static uint8_t mpu_config_i2c_mst(KeyVal *pairs);

//////////////////////////////////////////////////////////
// Configures the i2c slave (0) //////////////////////////
//    i2c_slave_rw: (read|write) [r|w] optional
//    i2c_slave_addr: (hex)
//    i2c_slave_reg: (hex)
//    i2c_slave_len: (hex) 4-bit
//    i2c_slave_en: (yes:no)
//    i2c_slave_byte_swap: (yes:no)
//    i2c_slave_reg_dis: (yes:no)
//    i2c_slave_grp: (evenodd|oddeven) [e|o] optional
static uint8_t mpu_config_i2c_slv(KeyVal *pairs);

//////////////////////////////////////////////////////////
// Configures the interupts and i2c bus //////////////////
//    i2c_bypass: (on:off)
static uint8_t mpu_config_int_pin(KeyVal *pairs);

// Given an array of KeyVal structs, iterate through all pairs and
// if this setting is implemented, then modify the mpu settings.
// Sensor *s is a pointer to the mpu that should be configured.
int mpu_configure(Sensor *s, KeyVal *settings)
{
  return 0;
}

///////////////////////////////////////////////////////////////////////////////
// CONFIGURE AUX SENSOR SETTINGS
///////////////////////////////////////////////////////////////////////////////
/*
   For ease of use, two functions are provided to enable and disable
   the mpu sensor using sensible defaults and/or the configuration
   provided in KeyVal pairs
*/

// Turns the mpu on and deals with sensible defaults
int mpu_enable(Sensor *s, KeyVal *settings)
{
  return 0;
}

// Disables the mpu sensor
int mpu_disable(Sensor *s)
{
  return 0;
}

///////////////////////////////////////////////////////////////////////////////
// READ SENSOR DATA
///////////////////////////////////////////////////////////////////////////////
/*
   Only one entry point for the reading of data. The function
   `mpu_read` conforms to function type `ReadAxes` and takes the
   mpu sensor pointer along with a target for reading.

   Currently possible targets are `HOST` or `AUX`, standing
   for the native gyro axes and aux sensor respectively.
*/

// Entry point for all reads. Takes a Sensor pointer (the selected mpu)
// and a target. This is currently either `HOST` or `AUX`, as explained
// above.
Axes *mpu_read(Sensor *s, target_t t)
{
  return NULL;
}

// Retrieves only the mpu's gyro axes data, then returns
// the data as an Axes struct pointer.
static Axes *read_gyro(Sensor *s)
{
  return NULL;
}

// Retrieves only the aux sensor's axes data, then returns
// the data as an Axes struct pointer.
static Axes *read_aux(Sensor *s)
{
  return NULL;
}

///////////////////////////////////////////////////////////////////////////////
// DEALLOC MPU
///////////////////////////////////////////////////////////////////////////////
/*
   After consideration, the current implementation will both reset
   and disable an mpu when the struct memory is released.
   All memory releases should be achieved through the below `mpu_dealloc`
   function to avoid the user missing other dependencies.
  
   IMPORTANT - Any auxiliary sensors connected to the mpu will
               also have their respective dealloc function called
               should the i2c_bypass setting not be in use.
               Once again, to avoid user errors where aux sensors
               are left enabled but unaccessable.
*/

void mpu_dealloc(Sensor **s)
{
}
