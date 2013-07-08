///////////////////////////////////////////////////////////////////////////////
// Raspberry Pi GPIO Interface
// ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
// File: inputs.c
// PA Consulting - Lawrence Jones
///////////////////////////////////////////////////////////////////////////////

#include "inputs.h"

// Configure the memory access required to alter
// the GPIO pins. Will request memory access from the system,
// note that this requires sudo privileges!
// Based on example at http://elinux.org/RPi_Low-level_peripherals
int initialiseGpios()
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
}

// Given a pin number, will return 
state_t pinStatus(int g)
{
  // Extract pin control code (3 bits signifying current state)
  // by retrieving the control word, shifting it the appropriate
  // distance to the right, then masking for the first 3 bits.
  int ctrlCode  = ((PIN_CONTROL_WORD(g) >> PIN_SHIFT(g - 1)) & 7),
      isHigh    = SET_WORD & (1u << (g-1));
  // Using the ctrlCode (setting in or out) and the current state,
  // return the state_t type that represents the pins current status
  switch (1u & ctrlCode)
  {
    case INPUT:  if (isHigh) return INHIGH;  else return INLOW;
    case OUTPUT: if (isHigh) return OUTHIGH; else return OUTLOW;
  }
}

// Main function for testing purposes
int main()
{
  int g,rep; state_t state;
  // declare temp text entry
  char pintxt[5];
  // initialise the gpio access
  initialiseGpios();
start:
  printf("Enter a pin to find status...\n");
  // get user input
  gets(pintxt);
  int pin = atoi(pintxt);
  if (pintxt[0] == 'X') goto end;
  state = pinStatus(pin);
  printf("Pin %d is currently %s\n\n", pin, stateLabels[state]);
  goto start;
end:
  return 0;
}