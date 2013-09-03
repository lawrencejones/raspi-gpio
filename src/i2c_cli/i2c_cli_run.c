///////////////////////////////////////////////////////////////////////////////
// Raspberry Pi GPIO Interface
// ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
// File: i2c_cli_run.c
// PA Consulting - Lawrence Jones
///////////////////////////////////////////////////////////////////////////////

#include "i2c/i2c_res.h"
#include "i2c_cli_private.h"

///////////////////////////////////////////////////////////////////////////////
// RUN COMMAND FUNCTIONS
///////////////////////////////////////////////////////////////////////////////

// Given an i2c handle and an array of strings, process the tokens
// to interpret the lines into i2c commands, all to be run on the
// same bus (for now)
// TODO - Implement storing values, bus switching
void process_command(i2c_bus *i2c,                           // process_command
                     char** tokens,
                     int no_of_tokens)
{
  // Declare variables with sensible defaults
  short addr = 0;
  int reg = 0, bytes = 1;
  switch (no_of_tokens)
  {
    case 5: case 6:
    case 4: // Extract no of bytes in transfer
      bytes = DEX_TO_INT(tokens[3]);
    case 3: // Extract the register address
      reg = DEX_TO_INT(tokens[2]);
    case 2: // Extract i2c dev address
      addr = DEX_TO_INT(tokens[1]);
      break;
  }
  // Verify i2c dev is present
  verify_i2c_details(i2c, addr, reg);
  // General verification now finished, split on command
  // If `read` command
  if (!strcmp(tokens[0], "read")) // **READ**
  // read [addr] [reg] [bytes]
  {
    // Verify correct number of args - at least 2
    verify_arg_count(/* expected */ 2, /* got */ no_of_tokens);
    PRINTC(GREEN, "Reading %d bytes from dev 0x%02x at \
register 0x%02x...\n\n", bytes, addr, reg);
    // Initiate read from dev
    uint8_t *read = i2c_read_block(i2c, addr, reg, bytes);
    // For all bytes received
    for (int i = 0; i < bytes; i++)
    {
      // Print to stdout the byte received
      printf("   Byte %03d - 0x%02x - ", i, read[i]);
      print_binary_byte(read[i]);
      printf("\n");
    }
    PRINTC(GREEN, "\nFinished read. I2C bus status is \
0x%03x / ", BSC_S);
    PRINT_BIN_BYTE(BSC_S, "\n\n");
    // Free the reading array
    free(read);
  } 
  if (!strcmp(tokens[0], "write")) // **WRITE**
  // write [addr] [reg] [bytes] [content]
  {
    // Verify the correct number of args
    // 3 minimum required
    verify_arg_count(/* expected */ 3, /* got */ no_of_tokens);
    // If a single byte write
    if (no_of_tokens == 3)
    {
      // ...then write a single byte!
      i2c_write_byte(i2c, addr, reg);
    }
    else
    {
      // Fetch no of bytes to write
      short no_of_bytes = (short) (0xff & DEX_TO_INT(tokens[3]));
      // Select what args represent content
      char** content_args = (char**)(tokens + 4);
      // Interpret the content
      uint8_t *content = interpret_content(content_args, 
          (no_of_tokens - 4), no_of_bytes);
      // Write to device
      i2c_write_reg(i2c, addr, reg, content, no_of_bytes);
    }
    // Print success
    PRINTC(GREEN, "Write successful.\n\n");
  }
}
