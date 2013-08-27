///////////////////////////////////////////////////////////////////////////////
// Raspberry Pi GPIO Interface
// ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
// File: io.c
// PA Consulting - Lawrence Jones
///////////////////////////////////////////////////////////////////////////////

#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include "io.h"
#include "../../tools/src/macros.h"

// Configure the memory access required to alter
// the GPIO pins. Will request memory access from the system,
// note that this requires sudo privileges!
// Based on example at http://elinux.org/RPi_Low-level_peripherals
volatile unsigned* get_mmap(int base)                              // get_mmap
{
  // Open system /dev/mem location for direct mem access
  int devmem = open("/dev/mem", O_RDWR|O_SYNC);
  // Verify mem access successful
  if (devmem < 0)
  {
    // Access not successful, print error and exit
    ERR("Failed to access dev/mem - verify sudo?\n\n");
    exit(EXIT_FAILURE);
  }
  // Use mmap to map to direct memory locations
  void *map = mmap(
      NULL,        // specific address not required
      BLOCK_SIZE,  // the length of the mapping, to include
      // all the peripheral reserved space
      PROT_READ|PROT_WRITE,  // enable RW to memory
      MAP_SHARED,  // share this location with other processes
      devmem,      // specific file to open
      base         // move to the given base
      );
  // Close the filestream
  close(devmem);
  // Verify the integrity of the memory mapping
  if (map == MAP_FAILED)
  {
    // Mapping not successful, print error and exit
    ERR("Error from builtin mmap - code %d\n", (int) map);
    exit(EXIT_FAILURE);
  }
  // If successful, return map
  return map;
}
