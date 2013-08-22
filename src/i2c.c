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
#include "io.h"
#include "print.h"
#include "../tools/src/macros.h"
#include "../tools/src/tokeniser.h"

static inline void verify_arg_count(int expected, int argc)
{
  // If the arguments do not equal the expected
  if (argc < expected)
  {
    // Then print to standard err
    ERR("Error. See i2c-test --help for correct usage.\n\n");
    // And exit with failure
    exit(EXIT_FAILURE);
  }
}

// Content can come as any mix of hex/decimal numbers
// This is valid - 0x66f8ae 22 1 0x9a
// As is this - 0xffff812a
// PRE - No token may exceed a 32 bit int
// NB - Tokens will be interpretted in byte chunks
uint8_t *interpret_content(char **args, int no_of_tokens, short bytes)
{
  uint8_t *content = malloc(sizeof(uint8_t) * bytes);
  int count = 0, i = 0;
  uint32_t buffer = 0;
  // While the count of bytes interpretted is less than the number
  // required to be collected and we have arguments left to process
  while (count < bytes && i < no_of_tokens)
  {
    buffer = DEX_TO_INT(args[i]);
    while (count < bytes)
    {
      content[count++] = buffer & 0xff;
      buffer >>= 8u;
    }
  }
  return content;
}

void print_usage()
{
  printf("Usage:       i2c (optional) detect\n");
  printf("             i2c read  [addr] [reg] [noOfBytes]\n");
  printf("             i2c write [addr] [reg] [noOfBytes] [content]\n");
  printf("             i2c file  [filename]\n\n");
  printf("[addr]:      device address\n");
  printf("[reg]:       data register\n");
  printf("[noOfBytes]: to either read or write\n");
  printf("[content]:   to write to device. any mix of dec or hex numbers.\n");
  printf("[filename]:  the filename containing commands\n\n");
}

void process_command(i2c_bus *i2c, char** tokens, int no_of_tokens)
{
  // Extract i2c dev address
  short addr  = DEX_TO_INT(tokens[1]); 
  // Pick off the number of bytes to transfer
  int reg = DEX_TO_INT(tokens[2]), bytes = DEX_TO_INT(tokens[3]);
  // Verify that address is within i2c bus range
  if (addr > 127)
  {
    ERR("Address (0x%02x not in range of bus.\n\n", addr);
    exit(EXIT_FAILURE);
  }
  // Verify that the device is on the bus
  if (!i2c_bus_addr_active(i2c, addr))
  {
    // If it's not, then exit with error
    ERR("Device (0x%02x) not found on bus. \
      Use `detect` to list devs.\n\n", addr);
    exit(EXIT_FAILURE);
  }
  // Verify register is within reasonable limit
  if (!(reg >= 0 && reg < 256))
  {
    // Else exit with error
    ERR("Register 0x%02x is out of range.\n\n", reg);
    exit(EXIT_FAILURE);
  }
  // General verification now finished, split on command
  // If `read` command
  if (!(strcmp(tokens[0], "read")))
  // read [addr] [reg] [bytes]
  {
    // Verify correct number of args
    verify_arg_count(/* expected */ 4, /* got */ no_of_tokens);
    // TODO - Update for new color variadic function
    printf(GREEN);
    printf("Reading %d bytes from dev 0x%02x at \
        register 0x%02x...\n\n", bytes, addr, reg);
    printf(CLRCOL);
    // Initiate read from dev
    uint8_t *read = i2c_read_block(i2c, addr, reg, bytes);
    // For all bytes received
    for (int i = 0; i < bytes; i++)
    {
      // Print to stdout the byte received
      printf("   Byte %03d - 0x%02x - ", i, read[i]);
      PRINT_BIN_BYTE(read[i]); 
      printf("\n");
    }
    printf(GREEN);
    printf("\nFinished read. I2C bus status is 0x%03x / ", BSC_S);
    PRINT_BIN_BYTE(BSC_S); printf("\n");
    printf(CLRCOL);
  } 
  else if (!(strcmp(tokens[0], "write")))
  // write [addr] [reg] [bytes] [content]
  {
    // Verify the correct number of args
    verify_arg_count(/* expected */ 5, /* got */ no_of_tokens);
    // Fetch no of bytes to write
    short no_of_bytes = (short) (0xff & DEX_TO_INT(tokens[3]));
    // Select what args represent content
    char** content_args = (char**)(tokens + 4);
    // Interpret the content
    uint8_t *content = interpret_content(content_args, 
        (no_of_tokens - 4), no_of_bytes);
    // Write to device
    i2c_write_reg(i2c, addr, reg, content, no_of_bytes);
    // Print success
    PRINTC("Write successful.\n\n", GREEN);
  }
}

// Main function for testing purposes
// i2c-test [function] [addr] [size] [content]
// [function] = detect | read | write | file
// [addr] = device bus address
// [size] = no of bytes
// [content] = hex numbers representing bytes
int main(int argc, char** argv)
{
  // Clear a line
  printf("\n");
  // Initialise i2c access
  i2c_bus *i2c = i2c_init(1);
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
      print_usage();
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
    ERR("Incorrent usage.\n\n");
    // Print the usage guidelines
    print_usage();
  }
  // Deallocate the bus
  i2c_dev_dealloc(&dev);
  // Deallocate the chip
  dealloc_chip();
  return 0;
}
