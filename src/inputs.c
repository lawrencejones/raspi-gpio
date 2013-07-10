///////////////////////////////////////////////////////////////////////////////
// Raspberry Pi GPIO Interface
// ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
// File: inputs.c
// PA Consulting - Lawrence Jones
///////////////////////////////////////////////////////////////////////////////

#include "inputs.h"
#include "print.c"

// Configure the memory access required to alter
// the GPIO pins. Will request memory access from the system,
// note that this requires sudo privileges!
// Based on example at http://elinux.org/RPi_Low-level_peripherals
volatile unsigned* initialiseGpios()
{
  // Open system /dev/mem location for direct mem access
  int devmem = open("/dev/mem", O_RDWR|O_SYNC);
  // Verify mem access successful
  if (devmem < 0)
  {
    // Access not successful, print error and exit
    fprintf(stderr, "Failed to access dev/mem - verify sudo?\n");
    exit(EXIT_FAILURE);
  }
  // Use mmap to map to the gpio direct memory locations
  gpioMap = mmap(
    NULL,        // specific address not required
    BLOCK_SIZE,  // the length of the mapping, to include
                 // all the gpio reserved space
    PROT_READ|PROT_WRITE,  // enable RW to memory
    MAP_SHARED,  // share this location with other processes
    devmem,      // specific file to open
    GPIO_BASE    // move to the GPIO reserved space by offset
  );
  // Close the filestream
  close(devmem);
  // Verify the integrity of the memory mapping
  if (gpioMap == MAP_FAILED)
  {
    // Mapping not successful, print error and exit
    fprintf(stderr, "Error from builtin mmap - code %d\n", (int) gpioMap);
    exit(EXIT_FAILURE);
  }
  // Use volatile pointer to allow shared access of the gpio locations
  // Slows performance but refreshing is required
  gpio = (volatile unsigned *)gpioMap;
  return gpio;
}

// Takes p, the physical pin number and translates to the
// memory address pin location
int chipIndexToMem(int p)
{
  if ((p < 27) && (p > 0)) return memIndexes[p];
  fprintf(stderr, "Not a valid physical pin number.\n");
  return NA;
}

// Given the --PHYSICAL-- chip pin index, create a
// pin struct and return the pointer
Pin* mallocPin(int p)
{
  Pin *pin = malloc(sizeof(Pin));
  pin->chipIndex = p;
  pin->memIndex = chipIndexToMem(p);
  return pin;
}

// Given a --PHYSICAL-- chip pin index, return a
// pin struct containing the current information
Pin* pinStatus(int p)
{
  // Convert the physical chip pin index to it's memory index
  int g = chipIndexToMem(p);
  // Extract pin control code (3 bits signifying current state)
  // by retrieving the control word, shifting it the appropriate
  // distance to the right, then masking for the first 3 bits.
  int ctrlCode  = ((PIN_CONTROL_WORD(g) >> PIN_SHIFT(g)) & 7),
      isHigh    = ((VAL_WORD & (1u << g)) != 0);
  // Using the ctrlCode (setting in or out) and the current state,
  // return the state_t type that represents the pins current status
  Pin *pin = mallocPin(p);
  pin->state = 1u & ctrlCode;
  pin->value = isHigh;
  // Note mem dealloc responsibility
  return pin;
}

// Given a --PHYSICAL-- chip pin index, set it's output
// value to v
void setPin(int p, int v)
{
	int g = chipIndexToMem(p);
	// Clear the pin
	GPIO_CLR(g);
	// Now set it
	GPIO_SET(g,v);
}

// Main function for testing purposes
int main()
{
	printf("Not yet implemented C interface.\n");
  return 0;
}
