///////////////////////////////////////////////////////////////////////////////
// Raspberry Pi GPIO Interface
// ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
// File: pca_init.c
// PA Consulting - Lawrence Jones
///////////////////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include "pca_private.h"
#include "macros.h"

///////////////////////////////////////////////////////////////////////////////
// INITIALISATION
///////////////////////////////////////////////////////////////////////////////
/*
   The PCA9548A is an I2C multiplexer, with the ability to split
   a single I2C bus four ways.

   The IMU board currently utilises two of these multiplexers
   on either side of the board. The devices are distributed
   between the multiplexers channels like so...

      ====================================================
      |  Channel   |   Sensors                           |
      ====================================================
      |   0x01     |   MPU  (0x69)   ITG  (0x68)         |
      |   0x02     |   ITG  (0x69)   L3G4 (0x68)         |
      |   0x04     |   L3G4 (0x69)   MAG  (0x0e)         |
      ====================================================
   
   ...whilst there are, as stated, 4 different channels, only 
   three are in use. As of present, no MAG's are attached 
   either.

   The mux works very simply. The channels are labelled by the
   value the sensors require written to them in order to enable 
   that specific channel.

   The only complex part of this device is preventing any
   confusion between devices of the same model with identical
   addresses. As such, reading the current value of the mux
   easily is required by the device to correlate against 
   other device drivers and prevent such mixups.

   These pca's operate with an i2c slave address of 0x74.
 */

// Given name, i2c addr and i2c handle, allocates & initialises
// a new Mux struct.
Mux *pca_init(char* name,
              short i2c_addr,
              i2c_bus *i2c  )
{
  // malloc the sensor struct
  Mux *m = malloc(sizeof(Mux));
  // Verify malloc success
  if (!m)
  {
    // malloc has failed, print error and exit
    ERR("Failed to allocate memory (malloc) for Mux *m.\n");
    exit(EXIT_FAILURE);
  }
  // Assign given sensor properties
  m->name = malloc(sizeof(char) * strlen(name) + 1);
  // Make copies of strings to avoid stack expiry
  strcpy(m->name, name);
  // Assign the i2c addr
  m->i2c_addr = i2c_addr;
  // Assign the i2c handle
  m->i2c = i2c;
  // Assign the enum for model
  m->model = PCA9548A;
  // Set the channel fetch function
  m->fetch_channel = &pca_fetch_channel;
  // Assign the channel set function
  m->set_channel = &pca_set_channel;
  // Assign the mux networking function
  //m->get_devs = &pca_get_devs;
  // Set the channel to 0 initially
  m->set_channel(m, 0);
  // Assign dealloc function
  m->dealloc = &pca_dealloc;
  // Return the struct
  return m;
}
