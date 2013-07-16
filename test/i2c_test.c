///////////////////////////////////////////////////////////////////////////////
// Raspberry Pi GPIO Interface
// ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
// File: i2c_test.c
// PA Consulting - Lawrence Jones
///////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include "../src/io.h"
#include "../src/print.h"

void print_i2c_bus(i2c_bus *bus)
{
  int count = 0;
  i2c_dev *dev = bus->first;
  do {
    // printf("  Dev %d  -  Addr 0x%02x\n", count++, dev->addr);
  } while (dev = dev->next);
}


// Main function for testing purposes
int main(int argc, char** argv)
{
  init_gpio_access();
  init_chip();
  init_i2c();
  i2c_bus *bus = i2c_bus_refresh();
  print_i2c_bus(bus);
  dealloc_i2c_bus(bus);
  dealloc_chip();
  return 0;
}


