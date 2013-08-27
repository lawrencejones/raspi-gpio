///////////////////////////////////////////////////////////////////////////////
// Raspberry Pi GPIO Interface
// ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
// File: gpio_write.c
// PA Consulting - Lawrence Jones
///////////////////////////////////////////////////////////////////////////////

#include "gpio_private.h"

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
