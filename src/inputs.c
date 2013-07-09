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

static const char* yn[] = {"\x1b[32mINP\x1b[0m", "OUT"};
static const char* vals[] = {"0", "\x1b[91m1\x1b[0m"};

void printHeader()
{
	printf("+------------------------------------------------+\n");
  printf("| PIN  | MEM ADDR  | GPIO NAME | STATE  | VALUE  |\n"); 
	printf("+------------------------------------------------+\n");
}

void printPin(Pin *pin)
{
	int gpio = memToGpioLabel(pin->memIndex);
  printf("| %02d   |    %02d     | ", 
    pin->chipIndex, pin->memIndex);
	if (gpio > 0) printf("Gpio %2d   ", gpio);
	else printf("          ");
	printf("| %s    |  %sV    |\n",
		yn[pin->state], vals[pin->value]);
}

void printAll()
{
	printHeader();
	for (int i = 1; i < 27; i++)
		printPin(pinStatus(i));
	printf("+------------------------------------------------+\n");
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
      isHigh    = (VAL_WORD & (1u << (g-1)) != 0);
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
/*  printf("Enter a pin to find status...\n");
  // get user input
  gets(pintxt);
  int pin = atoi(pintxt);
  if (pintxt[0] == 'X') goto end;
	printHeader();
  printPin(pinStatus(pin)); 
  goto start;*/
	printAll();
end:
  return 0;
}
