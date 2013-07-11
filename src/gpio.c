///////////////////////////////////////////////////////////////////////////////
// Raspberry Pi GPIO Interface
// ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
// File: gpio.c
// PA Consulting - Lawrence Jones
///////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include "io.h"
#include "print.c"

// Main function for testing purposes
int main(int argc, char** argv)
{
	printf("Not yet implemented C interface.\n");
  initialiseGpioAccess();
  initialiseChip();
  printAll();
  deallocChip();
  return 0;
}
