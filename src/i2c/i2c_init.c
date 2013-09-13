///////////////////////////////////////////////////////////////////////////////
// Raspberry Pi GPIO Interface
// ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
// File: i2c_init.c
// PA Consulting - Lawrence Jones
///////////////////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include "i2c_private.h"
#include "i2c_res.h"
#include "gpio.h"
#include "shared/io.h"

///////////////////////////////////////////////////////////////////////////////
// INITIALISATION
///////////////////////////////////////////////////////////////////////////////

// TODO - Set macros to auto fix pins for various board revisions
// Initialises pins to prepare for the I2C protocol
volatile unsigned* i2c_init(int bus)                                // i2c_init
{
  // If gpio isn't init'd, attempt an init
  volatile unsigned *gpio = init_gpio_access();
  // Open the devmem and fetch from the i2c base
  volatile unsigned* i2c = get_mmap(I2C_BASE(bus));
  // Initialise clock and data ints
  int clk, data;
  // Dependent on which bus, select the correct pins
  switch (bus)
  {
    // For i2c_0, gpio pins 28 and 29
    case 0: 
      data = 28;
      clk = 29; 
      INP_GPIO(0);
      INP_GPIO(1);
      break;
      // For i2c_1, gpio pins 2 and 3
    case 1:
      data = 2;
      clk = 3;
      break;
  }
  // Rememeber to set gpio's to INP initially to reset
  // the current alt setting
  // Set gpio data to input
  INP_GPIO(data);
  // Set the gpio data pin to it's alternate state - in this case,
  // we want state 0, SDA0, the I2C data line
  SET_GPIO_ALT(data, 0);
  // Set gpio clk pin to input
  INP_GPIO(clk);
  // Set gpio clk pin to alternate state, SCL0
  SET_GPIO_ALT(clk, 0);
  // Return the i2c pointer
  return i2c;
}
