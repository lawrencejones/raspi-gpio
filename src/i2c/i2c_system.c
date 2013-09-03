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

// Pause should be 5x10^4 nanoseconds, timeout should
// be 1x10^9/pause
static int pause = 5000;

// Wait for the i2c bus to register DONE status
// Pauses execution of the thread using nanosleep
void i2c_wait_done(i2c_bus *i2c)                               // i2c_wait_done
{
  // Make timeout a second
  int timeout = 1000000000 / pause;
  // While Broadcom Serial Controller status not registering DONE
  while (!(BSC_S & BSC_S_DONE) && --timeout)
  {
    // Sleep the process for 5000 nanoseconds
    nanosleep((struct timespec[]) {{0, pause}}, NULL);  
  }
  // If timeout is 0 then the device failed to respond
  // after 1 second of wait. Register timeout to stderr
  if (!timeout)
  {
    ERR("Bus status - %09x\n", BSC_S);
    ERR("I2C timeout occurred.\n\n");
  }
}

// Wait for the i2c bus to register that the fifo buffer
// is full, so that you may pull the data out
int i2c_wait_fifo(i2c_bus *i2c)                                // i2c_wait_fifo
{
  // Make timeout the amount of time it would take to
  // transfer fifo capacity (16 bytes) at i2c slow (100khz)
  // multiplied by 10 for clock stretching
  //   (16 * 8 / 100,000) * 10 = 12,800,000nS
  int timeout = 12800000 / pause;
  // While Broadcom Serial Controller status does not
  // register an fifo full interrupt or transfer done
  while (!(BSC_S & BSC_S_RXF) && !(BSC_S & BSC_S_DONE) && --timeout)
  {
    // Sleep the process for 5000 nanoseconds
    nanosleep((struct timespec[]) {{0, pause}}, NULL);  
  }
  // If timeout is 0 then the fifo has failed to become full
  // and hasn't finished transfer
  if (!timeout)
  {
    return FIFO_TIMEOUT;
  }
  // Else if the transfer has finished
  else if (BSC_S & BSC_S_DONE)
  {
    return FIFO_DONE;
  }
  // Else if the transfer has not finished but not timed out
  // then fifo must be full
  else if (BSC_S & BSC_S_RXF)
  {
    return FIFO_FULL;
  }
  // Else return unknown error
  else
  {
    return FIFO_ERR;
  }
}
