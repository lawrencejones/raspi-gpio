///////////////////////////////////////////////////////////////////////////////
// Raspberry Pi GPIO Interface
// ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
// File: gpio_malloc.c
// PA Consulting - Lawrence Jones
///////////////////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include "gpio_private.h"
#include "../../tools/src/macros.h"

///////////////////////////////////////////////////////////////////////////////
// MALLOC
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
