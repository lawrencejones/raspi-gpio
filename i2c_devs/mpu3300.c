///////////////////////////////////////////////////////////////////////////////
// Raspberry Pi GPIO Interface
// ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
// File: mpu3300.c
// PA Consulting - Lawrence Jones
///////////////////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include <stdio.h>
#include "mpu3300.h"

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
   All i2c slave related configuration functionality should appear
   below. All should be static and only accessed through the mpu_configure
   function.
*/

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
Axes *mpu_read(Sensor *s, enum Target t)
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
