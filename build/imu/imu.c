///////////////////////////////////////////////////////////////////////////////
// Raspberry Pi GPIO Interface
// ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
// File: imu.c
// PA Consulting - Lawrence Jones
///////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <string.h>
#include "imu.h"
#include "../../tools/src/macros.h"

///////////////////////////////////////////////////////////////////////////////
// MAIN
///////////////////////////////////////////////////////////////////////////////

#define IS_FLAG(str) if (!strcmp(str, argv[1]))

int main(int argc, char** argv)
{
  // Clear a line
  printf("\n");
  if (argc > 1)
  {
    // For the multiplexer tests
    IS_FLAG("mux")
    {
      mux_test(argc, argv);
    }
    else
    {
      ERR("Test `%s` is not yet supported.\n\n", argv[1]);
    }
  }
  else
  {
  ERR("Test flag is required.\n\n");
  }
  return 0;
}
