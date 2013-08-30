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

int main(int argc, char** argv)
{
  // TODO - Sanitise the arguments first
  // Route with the given args
  imu_route(argv, argc);
  return 0;
}
