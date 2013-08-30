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
  char *conf_str = NULL;
  // If passed another argument
  if (argc > 2)
  {
    // Modify it so that the str is not immutable
    conf_str = (char*)malloc(strlen(argv[2]) + 1);
    strcpy(conf_str, argv[2]);
  }
  // Clear a line
  printf("\n");
  if (argc > 1)
  {
    // For the multiplexer tests
    IS_FLAG("mux")
    {
      mux_test();
    }
    // For mpu tests
    else IS_FLAG("mpu")
    {
      mpu_test(conf_str);
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
  if (conf_str)
  {
    free(conf_str);
  }
  return 0;
}
