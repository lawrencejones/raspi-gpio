///////////////////////////////////////////////////////////////////////////////
// Raspberry Pi GPIO Interface
// ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
// File: itg_read.c
// PA Consulting - Lawrence Jones
///////////////////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include <string.h>
#include "itg_private.h"
#include "itg_registers.h"
#include "dev/shared/dev_mux.h"
#include "macros.h"

///////////////////////////////////////////////////////////////////////////////
// READ SENSOR DATA
///////////////////////////////////////////////////////////////////////////////
/*
   Currently possible targets are `HOST`, `BURST` or `AUX`, standing
   for the native gyro axes, fifo and aux sensor respectively.
*/

// Retrieves only the itg's gyro axes data, then returns
// the data as an Axes struct pointer.
static Axes *read_gyro(Sensor *s)
{
  // malloc new axes struct
  Axes *res = malloc(sizeof(Axes));
  // Read the results in a burst, starting from xout reg
  uint8_t *readings = i2c_read_block( s->i2c,
                                      s->i2c_addr,
                                      ITG_XOUT_H,
                                      6 );
  // Extract x y z values
  res->x = (readings[0] << 8) | readings[1];
  res->y = (readings[2] << 8) | readings[3];
  res->z = (readings[4] << 8) | readings[5];
  // Free the array
  free(readings);
  // Set type of results
  res->type = GYRO;
  // No next axes (not reading aux here)
  res->next = NULL;
  // Return the resulting readings
  return res;
}

// Retrieves only the aux sensor's axes data, then returns
// the data as an Axes struct pointer.
static Axes *read_aux(Sensor *s)
{
  return NULL;
}

///////////////////////////////////////////////////////////////////////////////
// READ SENSOR DATA IN BURST
///////////////////////////////////////////////////////////////////////////////
/*
   Once the fifo is configured, it is possible to read data from the device
   using the built in 512 byte buffer. As such, this read_burst method will
   read the data from the fifo, assuming that the fifo is set to xg yg zg,
   and then parse it into an Axes linked list.
*/

static Axes *read_burst(Sensor *s)
{
  // Get the current data count
  int fifo_count = (FETCH_REG(ITG_FIFO_COUNTH) << 8) +
                   (0x03 & FETCH_REG(ITG_FIFO_COUNTL));
  // Return null if currently empty
  if (!fifo_count)
  {
    // Return null
    return NULL;
  }
  // Adjust count to a multiple of three
  fifo_count = (fifo_count / 6) * 6;
  // Otherwise generate an array of uint8_ts
  uint8_t *block = i2c_read_block( s->i2c, 
                                   s->i2c_addr, 
                                   ITG_FIFO_R,
                                   fifo_count ),
          *data = (block + fifo_count);
  // Create pointers to Axes
  Axes *head = NULL;
  // Parse into axes data
  for (int i = fifo_count / 6; i > 0; i--)
  {
    // malloc next link with the previous head as its next
    head = axes_malloc(head);
    // Decrement data
    data -= 6;
    // Extract x y z values
    head->x = (data[0] << 8) | data[1];
    head->y = (data[2] << 8) | data[3];
    head->z = (data[4] << 8) | data[5];
  }
  // Free the byte array
  free(block);
  // Return the Axes
  return head;
}

///////////////////////////////////////////////////////////////////////////////
// FIFO CAPACITY
///////////////////////////////////////////////////////////////////////////////

// Calculates the current percentage (actual value is 0-1) of
// the itg's fifo that is filled given that the itg has a 512 byte fifo
float itg_fifo_capacity(Sensor *s)
{
  // Get the current data count
  int fifo_count = (FETCH_REG(ITG_FIFO_COUNTH) << 8) +
                    FETCH_REG(ITG_FIFO_COUNTL);
  // Return used / fifo capacity
  return ((float)fifo_count / 512);
}

///////////////////////////////////////////////////////////////////////////////
// ENTRY FOR READS
///////////////////////////////////////////////////////////////////////////////

// Entry point for all reads. Takes a Sensor pointer (the selected itg)
// and a target.
Axes *itg_read(Sensor *s, target_t t)
{
  // Check for valid sensor pointer
  if (s == NULL)
  { 
    ERR("Sensor pointer is not valid.\n\n");
    exit(EXIT_FAILURE);
  }
  // If there is a multiplexer, configure for access
  if (s->mux)
  {
    // Set the channel
    s->mux->set_channel(s->mux, s->mux_channel);
  }
  switch (t)
  {
    // If targetting the itg
    case HOST:
      // Return the gyro readings
      return read_gyro(s);
      break;
    // Else if targetting the auxiliary
    case AUX:
      // Return the auxiliary sensor readings
      return read_aux(s);
      break;
    // Else if wanting fifo burst readings
    case FIFO:
      // Return fifo burst readings
      return read_burst(s);
      break;
  }
  return NULL;
}
