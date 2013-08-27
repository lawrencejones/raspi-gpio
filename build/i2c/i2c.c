///////////////////////////////////////////////////////////////////////////////
// Raspberry Pi GPIO Interface
// ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
// File: i2c.c
// PA Consulting - Lawrence Jones
///////////////////////////////////////////////////////////////////////////////
// Usage Examples  
// ¯¯¯¯¯¯¯¯¯¯¯¯¯¯
// i2c detect
// i2c read  0x77 4 > read_result
// i2c write 0x12 8 0x2020c1d3 0x11e0a248
// i2c file  test_file.i2c
///////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <string.h>
#include "i2c.h"
#include "i2c_cli.h"
#include "../tools/src/macros.h"
#include "../tools/src/tokeniser.h"

///////////////////////////////////////////////////////////////////////////////
// MAIN FUNCTION
///////////////////////////////////////////////////////////////////////////////

// Main function for testing purposes
// i2c-test [function] [addr] [size] [content]
// [function] = detect | read | write | file
// [addr] = device bus address
// [size] = no of bytes
// [content] = hex numbers representing bytes
int main(int argc, char** argv)                                        // main
{
  // Clear a line
  printf("\n");
  // Set default i2c bus
  int bus_select = 1, offset = 0;
  // Scan args for a bus flag
  for (int i = 1; (i + offset) < argc; i++)
  {
    argv[i] = argv[i+offset];
    // If bus flag found
    if (!strcmp("bus", argv[i]))
    {
      // Take next argument as the chosen bus
      bus_select = atoi(argv[i+1]);
      // Adjust offset
      offset = 2;
      // Decrement i
      i--;
    }
  }
  // Reduce argc by offset
  argc -= offset;
  // Initialise i2c access
  i2c_bus *i2c = i2c_init(bus_select);
  // Generate list of devices
  i2c_dev *dev = i2c_dev_detect(i2c);
  // If no arguments or detect argument
  if ((argc == 1) || !(strcmp(argv[1], "detect")))
  {
    // Print bus
    i2c_print_bus(dev);
    // Print the usage message
    if (argc == 1)
    {
      // Print small usage message
      print_usage(0);
    }
  }
  // If sourcing commands from file
  else if ((argc == 3) && !(strcmp(argv[1], "file")))
  {
    // TODO - Consider adding syntax checker (in own time!)
    // Tokenise the file
    source_t *src = tokenise_file(argv[2]);
    // For all lines in the file, process them
    for (int i = 0; i < src->noOfLines; i++)
    {
      // Run command
      process_command(i2c, src->lines[i], src->sizes[i]);
      // Echo line to stdout
      printf("\n");
    } 
  }
  // Else if single read or write
  else if (!(strcmp(argv[1], "read")) || !(strcmp(argv[1], "write")))
  {
    process_command(i2c, argv + 1, argc - 1);
  }
  else
  {
    // Notify of incorrect usage
    ERR("Incorrect usage.\n\n");
    // Print the usage guidelines - extended version
    print_usage(1);
  }
  // Deallocate the bus
  i2c_dev_dealloc(&dev);
  return 0;
}
