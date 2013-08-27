///////////////////////////////////////////////////////////////////////////////
// Raspberry Pi GPIO Interface
// ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
// File: i2c_cli_display.c
// PA Consulting - Lawrence Jones
///////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include "i2c_cli_private.h"

///////////////////////////////////////////////////////////////////////////////
// CLI SPECIFIC
///////////////////////////////////////////////////////////////////////////////

// Print binary byte to stdout
void print_binary_byte(uint8_t byte)                      // print_binary_byte
{
  for (int i=8;i>=0;i--) printf("%d", 1u & (byte >> i));
}

// Prints the i2c cli usage to stdout
void print_usage(int extended)                                  // print_usage
{
  printf("Usage:       i2c (optional) detect\n");
  printf("             i2c read  [addr] [reg] [noOfBytes]\n");
  printf("             i2c write [addr] [reg] [noOfBytes] [content]\n");
  printf("             i2c file  [filename]\n\n");
  if (!extended) return;
  printf("[bus]:       optional flag: supply `bus N`\n");
  printf("[addr]:      device address\n");
  printf("[reg]:       data register\n");
  printf("[noOfBytes]: to either read or write\n");
  printf("[content]:   to write to device. any mix of dec or hex numbers.\n");
  printf("[filename]:  the filename containing commands\n\n");
}
