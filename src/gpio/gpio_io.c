///////////////////////////////////////////////////////////////////////////////
// Raspberry Pi GPIO Interface
// ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
// File: gpio_io.c
// PA Consulting - Lawrence Jones
///////////////////////////////////////////////////////////////////////////////

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

#include <stdlib.h>
#include "gpio_io.h"
#include "gpio_res.h"
#include "../shared/io.h"

// Define the entry point for gpios
volatile unsigned *gpio;
static int memToGpioLabel(int p);

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

///////////////////////////////////////////////////////////////////////////////
// GPIO READ
///////////////////////////////////////////////////////////////////////////////

// Given a --PHYSICAL-- chip pin index, return a
// pin struct containing the current information
Pin* get_pin_status(int p)                                    // get_pin_status
{
  // Convert the physical chip pin index to it's memory index
  int g = chip_index_to_mem(p);
  Pin *pin = malloc_pin(p);
  // Update the pin with it's current values
  update_pin_status(pin);
  // Note mem dealloc responsibility
  return pin;
}

// Helper function to prevent reallocation of memory
// Given the pin struct pointer, will update it's value
Pin* update_pin_status(Pin* pin)                           // update_pin_status
{
  int g = pin->memIndex;
  // Extract pin control code (3 bits signifying current state)
  // by retrieving the control word, shifting it the appropriate
  // distance to the right, then masking for the first 3 bits.
  int ctrlCode  = ((PIN_CONTROL_WORD(g) >> PIN_SHIFT(g)) & 7),
      isHigh    = ((VAL_WORD & (1u << g)) != 0);
  // Using the ctrlCode (setting in or out) and the current state,
  // return the state_t type that represents the pins current status
  pin->state = 1u & ctrlCode;
  pin->value = isHigh;
  return pin;
}

// Updates all the pin values in the chip struct
Pin** update_all_pins()                                      // update_all_pins
{
  for (int i = 0; i < NO_OF_PINS; i++)
    update_pin_status(chip->pins[i]);
  return chip->pins;
}

///////////////////////////////////////////////////////////////////////////////
// GPIO WRITE
///////////////////////////////////////////////////////////////////////////////

// Given a word, apply it to the set mem address to
// set multiple pins in one go
void  set_with_word(uint32_t w)                                // set_with_word
{
  SET_WORD = w;
}

// As above, given a word, apply to the clearing
// memory address to turn multiple pins off at once
void  clr_with_word(uint32_t w)                                // clr_with_word
{
  CLR_WORD = w;
}

// Given a --PHYSICAL-- chip pin index, set it's output
// value to v
void set_pin_value(int p, int v)                               // set_pin_value
{
  int g = chip_index_to_mem(p);
  if (v == 0)
    // Clear the pin
    GPIO_CLR(g);
  else
    // Now set it
    GPIO_SET(g,v);
}

void set_pin_state(int p, int v)                               // set_pin_state
{
  int g = chip_index_to_mem(p);
  INP_GPIO(g);
  // If output
  if (v) OUT_GPIO(g);
}

///////////////////////////////////////////////////////////////////////////////
// MALLOC / DEALLOC
///////////////////////////////////////////////////////////////////////////////

// Allocate memory to a chip struct that will represent the
// current state of the raspberry pi pins
Chip *init_chip()                                                  // init_chip
{
  // Allocate heap memory for the chip
  chip = malloc(sizeof(Chip));
  // Verify successful malloc of chip
  if (!chip)
  {
    ERR("Memory allocation failed (malloc) of chip.\n\n");
    exit(EXIT_FAILURE);
  }
  // For all the pins on the chip
  for (int i = 1; i <= NO_OF_PINS; i++)
  {
    // Initialise the pin struct
    chip->pins[i - 1] = malloc_pin(i);
  }
  // Return the pointer to the chip
  return chip;
}

// Given the --PHYSICAL-- chip pin index, create a
// pin struct and return the pointer
Pin* malloc_pin(int p)                                            // malloc_pin
{
  // Allocate space in heap for the Pin struct
  Pin *pin = malloc(sizeof(Pin));
  // Verify successful malloc
  if (!pin)
  {
    ERR("Memory allocation failed (malloc) of pin.\n\n");
    exit(EXIT_FAILURE);
  }
  // Set the chip index (chip pin, physical 1-26)
  pin->chipIndex = p;
  // Set the memory index
  pin->memIndex = chip_index_to_mem(p);
  // Fetch current pin data
  update_pin_status(pin);
  // Return the pin pointer
  return pin;
}

// Doesn't take a chip parameter as it is already
// globally accessable
void dealloc_chip()                                             // dealloc_chip
{
  for (int i = 0; i < NO_OF_PINS; i++)
    free(chip->pins[i]);
  free(chip);
}

// Takes p, the physical pin number and translates to the
// memory address pin location
int chip_index_to_mem(int p)                               // chip_index_to_mem
{
  if ((p <= NO_OF_PINS) && (p > 0)) return chipPinToMem(p);
  ERR("Not a valid physical pin number.\n");
  return NA;
}

///////////////////////////////////////////////////////////////////////////////
// HELPERS
///////////////////////////////////////////////////////////////////////////////

// Takes the pin memory address index and returns
// it's gpio label
static int memToGpioLabel(int p)
{
  for (int i = 0; i < 17; i++)
    if (gpioIndexes[i] == p) return i;
  return -1;
}

int chipPinToMem(int p) { 
  return memIndexes[p]; 
}

Chip* get_chip()
{ 
  return chip; 
}

void printHeader()
{
  printf("+------------------------------------------------+\n");
  printf("| PIN  | MEM ADDR  | GPIO NAME | STATE  | VALUE  |\n"); 
  printf("+------------------------------------------------+\n");
}

void printPin(Pin *pin)
{
  int gpio = memToGpioLabel(pin->memIndex);
  printf("| %02d   |    %02d     | ", 
    pin->chipIndex, pin->memIndex);
  if (gpio >= 0) printf("Gpio %2d   ", gpio);
  else printf("          ");
  printf("| %s    |  %sV    |\n",
    yn[pin->state], vals[pin->value]);
}

void printAll()
{
  printHeader();
  for (int i = 0; i < NO_OF_PINS; i++)
    printPin(chip->pins[i]);
  printf("+------------------------------------------------+\n");
}