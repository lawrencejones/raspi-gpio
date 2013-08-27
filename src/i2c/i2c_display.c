///////////////////////////////////////////////////////////////////////////////
// Raspberry Pi GPIO Interface
// ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
// File: i2c_display.c
// PA Consulting - Lawrence Jones
///////////////////////////////////////////////////////////////////////////////

#include "i2c_private.h"

///////////////////////////////////////////////////////////////////////////////
// I2C DISPLAY FUNCTIONS
///////////////////////////////////////////////////////////////////////////////

// Given a pointer to a i2c_dev and a 
// Prints the devices with their addresses on the given bus
void i2c_print_bus(i2c_dev *dev)                               // i2c_print_bus
{
  // Set count to 0
  int count = 0;
  PRINTC(GREEN, "Printing i2c bus devices...\n\n");
  // Print the device while one exists
  do {
    printf("  Dev %d  -  Addr 0x%02x\n", count++, dev->addr);
  } while ((dev = dev->next));
  // Clear a line
  PRINTC(GREEN, "\n...done.\n\n");
}

