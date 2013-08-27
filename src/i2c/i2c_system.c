///////////////////////////////////////////////////////////////////////////////
// Raspberry Pi GPIO Interface
// ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
// File: i2c_system.c
// PA Consulting - Lawrence Jones
///////////////////////////////////////////////////////////////////////////////

#include <time.h>
#include "i2c_private.h"
#include "i2c_res.h"

///////////////////////////////////////////////////////////////////////////////
// I2C System Functions
///////////////////////////////////////////////////////////////////////////////

// Wait for the i2c bus to register DONE status
// Pauses execution of the thread using nanosleep
void i2c_wait_done(i2c_bus *i2c)                               // i2c_wait_done
{
  // Pause should be 5x10^4 nanoseconds, timeout should
  // be 1x10^9/pause
  int pause = 50000, timeout = 1000000000/pause;
  // While Broadcom Serial Controller status not registering DONE
  while (!(BSC_S & BSC_S_DONE) && --timeout)
  {
    // Sleep the process for 50,000 nanoseconds
    nanosleep((struct timespec[]) {{0, pause}}, NULL);  
  }
  // If timeout is 0 then the device failed to respond
  // after 1 second of wait. Register timeout to stderr
  if (!timeout)
  {
    printf("%09x\n", BSC_S);
    ERR("I2C timeout occurred.\n\n");
  }
}
