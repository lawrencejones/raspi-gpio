///////////////////////////////////////////////////////////////////////////////
// Raspberry Pi GPIO Interface
// ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
// File: gpio_init.c
// PA Consulting - Lawrence Jones
///////////////////////////////////////////////////////////////////////////////

#include "gpio_private.h"
#include "shared/io.h"

///////////////////////////////////////////////////////////////////////////////
// INSTRUCTIONS
///////////////////////////////////////////////////////////////////////////////
/*
   The process to go from x to pin(g) on, is shown below...

    INP_GPIO(g)  =>  OUT_GPIO(g)  =>  CLR_PIN(g)  =>  SET_PIN(g)

   ...which first sets the pin to input mode to clear the current
   setting. This is entirely optional and is only present to highlight
   the possible entry at step 2, regardless of previous states.
   The clear pin is required to prep the pi for changing the said pins
   status. Once cleared, any value may be sent to the SET_PIN macro.
*/

// Define the entry point for gpios
volatile unsigned *gpio;

///////////////////////////////////////////////////////////////////////////////
// INITIALISATION
///////////////////////////////////////////////////////////////////////////////

// Initialises gpio access specifically
volatile unsigned* init_gpio_access()                       // init_gpio_access
{
  // Use the get_mmap function to pull from the devmem page
  // Initialise with the GPIO_BASE
  // Use volatile pointer to allow shared access of the gpio locations
  // Slows performance but refreshing is required
  gpio = (volatile unsigned *)get_mmap(GPIO_BASE);
  return gpio;
}
