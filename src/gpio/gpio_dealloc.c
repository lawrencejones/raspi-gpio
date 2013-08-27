///////////////////////////////////////////////////////////////////////////////
// Raspberry Pi GPIO Interface
// ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
// File: gpio_dealloc.c
// PA Consulting - Lawrence Jones
///////////////////////////////////////////////////////////////////////////////

#include "gpio_private.h"
#include "../../tools/src/macros.h"

///////////////////////////////////////////////////////////////////////////////
// DEALLOC
///////////////////////////////////////////////////////////////////////////////

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
