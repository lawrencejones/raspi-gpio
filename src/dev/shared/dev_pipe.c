///////////////////////////////////////////////////////////////////////////////
// Raspberry Pi GPIO Interface
// ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
// File: dev_pipe.c
// PA Consulting - Lawrence Jones
///////////////////////////////////////////////////////////////////////////////

#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "macros.h"
#include "dev_sensor.h"

///////////////////////////////////////////////////////////////////////////////
// IO READ LOOP
///////////////////////////////////////////////////////////////////////////////

// Starts the thread that will pipe the readings
void *dev_pipe_start(void *arg)
{
  // Cast a sensor pointer from the given args
  Sensor *s = (Sensor*)arg;
  // Maintain a parity bit
  int par = 0;
  // Verify pointer
  if (s == NULL)
  {
    // Print error
    ERR("Sensor pointer no longer valid.\n\n");
  }
  // Start loop
  while (s->pipe_running)
  {
    // If the sensor fifo buffer capacity is over 50%
    if (s->fifo_capacity(s) > 0.5)
    {
      // Read from the fifo buffer
      Axes *ax = s->read(s, FIFO),
           *crrt = ax;
      // Iterate through the axes
      while (crrt)
      {
        // Adjust parity
        par = !par;
        // Write parity
        write(s->wpipe, (void*)&par, sizeof(int));
        // Load next three values into array
        short vals[3] = {crrt->x, crrt->y, crrt->z};
        // Write the readings into the fifo
        write(s->wpipe, (void*)vals, sizeof(short) * 3);
        // Move to next reading
        crrt = crrt->next;
      }
      // Deallocate the axes struct
      axes_dealloc(&ax);
    }
    // Pause the thread
    nanosleep((struct timespec[]) {{0, 5000000}}, NULL);
  }
  return NULL;
}

///////////////////////////////////////////////////////////////////////////////
// PIPE INITIALISATION
///////////////////////////////////////////////////////////////////////////////

// Takes a Sensor struct pointer and creates a fifo access point
int dev_pipe(Sensor *s, const char* path)
{
  // If the pipe is enabled
  if (s->pipe_running)
  {
    // Cancel the pipe if currently running
    s->pipe_running = pthread_cancel(s->pipe_thread);
    // Check cancellation was a success
    if (s->pipe_running)
    {
      // Output error message
      ERR("Thread failed to cancel. Pipe may still be running.\n\n");
      // Exit from error
      exit(EXIT_FAILURE);
    }
  }
  // Make the fifo page
  if (!mkfifo(path, 0666))
  {
    // Print error
    ERR("Make fifo failed to location `%s`\n\n", path);
    // Exit with failure
    exit(EXIT_FAILURE);
  }
  // Open the new fifo for write only
  int fifo = open(path, O_WRONLY);
  // Check successful open
  if (!fifo)
  {
    // Print error message
    ERR("Unable to create fifo at `%s`\n\n", path);
    // Exit as a failure
    exit(EXIT_FAILURE);
  }
  // Assign fifo handle to the sensor
  s->wpipe = fifo;
  // Start the new thread
  int err = pthread_create(&(s->pipe_thread), NULL, &dev_pipe_start, (void*)s);
  // If the error code is non-zero
  if (err)
  {
    // Print error
    ERR("Thread creation failed with error (%d)\n\n", err);
    // Exit with failure
    exit(EXIT_FAILURE);
  }
  // Set pipe_running to true
  s->pipe_running = !err;
  // Detach and run the thread
  return pthread_detach(s->pipe_thread);
}
