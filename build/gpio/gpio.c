///////////////////////////////////////////////////////////////////////////////
// Raspberry Pi GPIO Interface
// ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
// File: gpio.c
// PA Consulting - Lawrence Jones
///////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include "gpio.h"

// Main function for testing purposes
int main(void)
{
	printf("Not yet implemented C interface.\n");
  init_gpio_access();
  init_chip();
  printAll();
  dealloc_chip();
  return 0;
}
