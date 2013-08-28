///////////////////////////////////////////////////////////////////////////////
// Raspberry Pi GPIO Interface
// ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
// File: i2c_cli_private.h
// PA Consulting - Lawrence Jones
///////////////////////////////////////////////////////////////////////////////

#ifndef I2C_CLI_PRIVATE_HEADER_INC
#define I2C_CLI_PRIVATE_HEADER_INC

#include "i2c.h"
#include "../../tools/src/macros.h"

///////////////////////////////////////////////////////////////////////////////
// PRIVATE I2C CLI FUNCTION STUBS
///////////////////////////////////////////////////////////////////////////////

// Interprets the content delivered via stdin/file on the cli
uint8_t *interpret_content (  char **args,
                              int no_of_tokens, 
                              short bytes  );
// Verifies correct number of arguments
void verify_arg_count      (  int expected,
                              int argc  );
// Verifies that i2c details are correct for the current bus state
void verify_i2c_details    (  i2c_bus  *i2c,
                              short    addr,
                              int      reg  );
// Print the given binary byte to stdout
void print_binary_byte     (  uint8_t  byte  );
// Process the current line as an i2c cli command
void process_command       (  i2c_bus  *i2c,
                              char     **tokens,
                              int      no_of_tokens  );
// Print the command line tool usage
void print_usage           (  int extended  );

#endif
