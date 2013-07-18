///////////////////////////////////////////////////////////////////////////////
// Raspberry Pi GPIO Interface
// ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
// File: i2c_test.c
// PA Consulting - Lawrence Jones
///////////////////////////////////////////////////////////////////////////////
// Usage Examples  
// ¯¯¯¯¯¯¯¯¯¯¯¯¯¯
// i2c-test detect
// i2c-test read  0x77 4 > read_result
// i2c-test write 0x12 8 0x2020c1d3 0x11e0a248
///////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <string.h>
#include "../src/io.h"
#include "../src/print.h"

#define DEX_TO_INT(str) \
  strtoul(str, NULL, 10 + ((strlen(str) > 2) && (str[1] == 'x'))*6)
#define PRINT_BIN(byte) \
  for (int i=7;i>=0;i--) printf("%d", 1u & (byte >> i));

static inline void verify_arg_count(int expected, int argc)
{
  // If the arguments do not equal the expected
  if (argc < expected)
  {
    // Then print to standard out
    fprintf(stderr, "Error. See i2c-test --help for correct usage.\n\n");
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
    while (buffer > 0xff && count < bytes)
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
  printf("             i2c read  [addr] [noOfBytes]\n");
  printf("             i2c write [addr] [noOfBytes] [content]\n\n");
  printf("[addr]:      device address\n");
  printf("[noOfBytes]: to either read or write\n");
  printf("[content]:   to write to device. any mix of dec or hex numbers.\n\n");
}

#define MAX_CHAR_PER_LINE 100
#define MAX_ARG_PER_LINE 10

typedef struct {
  char*** lines; 
  int noOfLines;
} source_t;

source_t *init_source(int no_of_lines)
{ 
  // Initialise a source struct
  source_t *src = calloc(sizeof(Source), 1);
  // Set the number of lines in source
  src->noOfLines = no_of_lines;
  // Allocate heap memory for the content
  src->lines = calloc(no_of_lines * sizeof(char*), 1);
  // Verify success of malloc
  if (!src->lines)
  {
    fprintf(stderr, "Memory allocation error with source.\n\n");
    exit(EXIT_FAILURE);
  }
  // For all the lines in source
  for (int i = 0; i < no_of_lines; i++)
  {
    // Allocate memory
    src->lines[i] = malloc(MAX_ARG_PER_LINE * sizeof(char*));
    // Verify that malloc was successful
    if (!src->lines[i])
    {
      fprintf(stderr, "Memory allocation error (malloc) of source line.\n\n");
      exit(EXIT_FAILURE);
    }
  }
  // Return pointer to source
  return src;
}

int linesInFile(FILE* file, char* line)
{
  // Initialise line count
  int lines = 0;
  // Seek to start of file
  fseek(file, 0, SEEK_SET);
  // Calculate the no of lines
  while (fgets(line, MAX_CHAR_PER_LINE, file) != NULL) lines++;
  // Reset the seek
  fseek(file, 0, SEEK_SET);
  // Return the no of lines
  return lines;
}

FILE *open_file(char* filepath)
{
  // Open the file for reading
  FILE *file = fopen(filepath, "r");
  // Declare the temporary char array and li
  char line[MAX_CHAR_PER_LINE], *tmp;
  source_t *src = init_source(;
  // Check file opened correctly
  if (!file)
  {
    fprintf(stderr, "File (%s) failed to open. Does it exist?\n\n", filepath);
    exit(EXIT_FAILURE);
  }
  // for all the lines in the file
  for (int i = 0; i < src->noOfLines; i++)
  {
    fgets(line, MAX_CHAR_PER_LINE, file);
    // apply strtok for the first time
    tmp = strtok(line, ", \n");
    // for all the potential arguments
    for (int j = 0; j < MAX_ARG_PER_LINE; j++)
    {
      // allocate memory for the copied string, say
      // 20 characters will be max length of args
      src->lines[i][j] = malloc(sizeof(char) * 200);
      // if tmp is not null, then copy the string
      if (tmp != NULL) strcpy(src->lines[i][j], tmp );
      // else just make null
      else src->lines[i][j] = NULL;
      // repeat the strtok
      tmp = strtok(NULL, ", \n");
    }
  }
  // close the file stream
  fclose(file);
  // return the array of string arrays
  return src;
}
}

// Main function for testing purposes
// i2c-test [function] [addr] [size] [content]
// [function] = detect | read | write
// [addr] = device bus address
// [size] = no of bytes
// [content] = hex numbers representing bytes
int main(int argc, char** argv)
{
  // Clear a line
  printf("\n");
  // Initialise gpio access
  init_gpio_access();
  // Initialise the chip struct
  init_chip();
  // Initialise i2c protocol
  init_i2c();
  // Generate bus model by detecting devs
  i2c_bus *bus = i2c_bus_detect();
  // If no arguments or detect argument
  if ((argc == 1) || !(strcmp(argv[1], "detect")))
  {
    // Print bus
    print_i2c_bus(bus);
    // Print the usage message
    if (argc == 1)
    {
      print_usage();
    }
  }
  // Else if another command
  else
  { 
    // Create a dev from the given address
    i2c_dev dev  = {DEX_TO_INT(argv[2]), NULL}; 
    // Pick off the number of bytes to transfer
    int bytes = DEX_TO_INT(argv[3]);
    // Verify that address is within i2c bus range
    if (dev.addr > 127)
    {
      fprintf(stderr, "Address not in range of bus.\n\n");
      exit(EXIT_FAILURE);
    }
    // Verify that the device is on the bus
    if (!i2c_bus_addr_active(dev.addr))
    {
      fprintf(stderr, "Device not found on bus. Use `detect` to list devs.\n\n");
      exit(EXIT_FAILURE);
    }
    // General verification now finished, split on command
    // If `read` command
    if (!(strcmp(argv[1], "read")))
    // i2c-test read [addr] [bytes]
    {
      // Verify correct number of args
      verify_arg_count(/* expected */ 4, /* got */ argc);
      printf("Reading %d bytes from dev with address 0x%02x...\n\n", bytes, dev.addr);
      uint8_t *read = i2c_read_block(&dev, bytes);
      for (int i = 0; i < bytes; i++)
      {
        printf("   Byte %03d - 0x%02x - ", i, read[i]);
        PRINT_BIN(read[i]); 
        printf("\n");
      }
      printf("\nFinished read. I2C bus status is 0x%03x / ");
      PRINT_BIN(BSC_S); printf("\n");
    } 
    else if (!(strcmp(argv[1], "write")))
    // i2c-test write [addr] [bytes] [content]
    {
      // Verify the correct number of args
      verify_arg_count(/* expected */ 5, /* got */ argc);
      // Fetch no of bytes to write
      short no_of_bytes = (short) (0xff & DEX_TO_INT(argv[3]));
      // Select what args represent content
      char** tokens = (char**)(argv + 4);
      // Interpret the content
      uint8_t *content = interpret_content(tokens, (argc - 4), no_of_bytes);
      // Create an i2c transaction
      i2c_transaction *trans = malloc_transaction(dev.addr, WRITE);
      // Set content
      trans->raw->content = content;
      // Set no of bytes
      trans->raw->size = no_of_bytes;
      // Write to device
      i2c_write_block(trans);
    }
  }
  dealloc_i2c_bus(bus);
  dealloc_chip();
  printf("\n");
  return 0;
}
