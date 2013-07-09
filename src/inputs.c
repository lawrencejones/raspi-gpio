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
  return EXIT_SUCCESS;
}

static const int memIndexes[] = {
  /* Not applicable */                NA,
  /* Pin 1  - 3.3v Power        */    0,
  /* Pin 2  - 5v Power          */    1,
  /* Pin 3  - SDA - Gpio 8      */    2,
  /* Pin 4  - 5v Power          */    NA,
  /* Pin 5  - SCL - Gpio 9      */    3,
  /* Pin 6  - 0v Ground         */    NA,
  /* Pin 7  - GPCLK0 - Gpio 7   */    4,
  /* Pin 8  - TXD - Gpio 15     */    14,
  /* Pin 9  - 0v Ground         */    NA,
  /* Pin 10 - RXD - Gpio 16     */    15,
  /* Pin 11 - Gpio 0            */    17,
  /* Pin 12 - PCM_CLK - Gpio 1  */    18,
  /* Pin 13 - Gpio 2            */    27,
  /* Pin 14 - 0v Ground         */    NA,
  /* Pin 15 - Gpio 3            */    22,
  /* Pin 16 - Gpio 4            */    23,
  /* Pin 17 - 3.3v Power        */    NA,
  /* Pin 18 - Gpio 5            */    24,
  /* Pin 19 - MOSI - Gpio 12    */    10,
  /* Pin 20 - 0v Ground         */    NA,
  /* Pin 21 - MISO - Gpio 13    */    9,
  /* Pin 22 - Gpio 6            */    25,
  /* Pin 23 - SCLK - Gpio 14    */    11,
  /* Pin 24 - CE0 - Gpio 10     */    8,
  /* Pin 25 - 0v Ground         */    NA,
  /* Pin 26 - CE1 - Gpio 11     */    7,
};

// Takes p, the physical pin number and translates to the
// memory address pin location
int chipIndexToMem(int p)
{
  if ((p < 27) && (p > 0)) return memIndexes[p];
  fprintf(stderr, "Not a valid physical pin number.\n");
  return NA;
}

// Given the --MEMORY-- pin index, return the current input
// reading
int inputReading(int g)
{
  // Function stub
  return 0;
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

char* yn[] = {"YES", "NO"};

void printPin(Pin *pin)
{
  printf("The physical pin %d on the chip, location %d\n", 
    pin->chipIndex, pin->memIndex);
  printf("in memory, is currently...\n");
  printf("    Input? %s\n", yn[pin->state]);
  printf("    Value? %d\n", pin->value);
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
  int ctrlCode  = ((PIN_CONTROL_WORD(g) >> PIN_SHIFT(g - 1)) & 7),
      isHigh    = SET_WORD & (1u << (g-1));
  // Using the ctrlCode (setting in or out) and the current state,
  // return the state_t type that represents the pins current status
  Pin *pin = mallocPin(p);
  pin->state = 1u & ctrlCode;
  pin->value = isHigh;
  // Note mem dealloc responsibility
  return pin;
}

// Main function for testing purposes
int main()
{
  int g,rep; Pin* status;
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
  printPin(pinStatus(pin));
  goto start;
end:
  return 0;
}
