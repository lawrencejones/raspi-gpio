///////////////////////////////////////////////////////////////////////////////
// Raspberry Pi GPIO Interface
// ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
// File: io.c
// PA Consulting - Lawrence Jones
///////////////////////////////////////////////////////////////////////////////

#include "io.h"
#include "print.h"
#include "pinmaps.h"
#include <pthread.h>
#include <semaphore.h>

///////////////////////////////////////////////////////////////////////////////
// INITIALISATION
///////////////////////////////////////////////////////////////////////////////

// Configure the memory access required to alter
// the GPIO pins. Will request memory access from the system,
// note that this requires sudo privileges!
// Based on example at http://elinux.org/RPi_Low-level_peripherals
volatile unsigned* init_gpio_access()
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
  void *gpioMap = mmap(
    NULL,        // specific address not required
    BLOCK_SIZE,  // the length of the mapping, to include
                 // all the gpio reserved space
    PROT_READ|PROT_WRITE,  // enable RW to memory
    MAP_SHARED,  // share this location with other processes
    devmem,      // specific file to open
    GPIO_BASE    // move to the GPIO reserved space by offset
  );
  // User mmap to map to the I2C bus
  void *i2cMap = mmap(
    NULL,        // specific address not required
    BLOCK_SIZE,  // the length of the mapping, to include
                 // all the I2C reserved space
    PROT_READ|PROT_WRITE,  // enable RW to memory
    MAP_SHARED,  // share this location with other processes
    devmem,      // specific file to open
    I2C_BASE     // move to the Broadcom Serial Bus 0
  );
  // Close the filestream
  close(devmem);
  // Verify the integrity of the memory mapping
  if ((gpioMap == MAP_FAILED) | (i2cMap == MAP_FAILED))
  {
    // Mapping not successful, print error and exit
    fprintf(stderr, "Error from builtin mmap - code %d\n", (int) gpioMap);
    exit(EXIT_FAILURE);
  }
  // Use volatile pointer to allow shared access of the gpio locations
  // Slows performance but refreshing is required
  gpio = (volatile unsigned *)gpioMap;
  free(gpioMap);
  i2c = (volatile unsigned *)i2cMap;
  free(i2cMap);
  return gpio;
}


// TODO - Set macros to auto fix pins for various board revisions
// Initialises pins to prepare for the I2C protocol
void init_i2c()
{
  // Rememeber to set gpio's to INP initially to reset
  // the current alt setting
  // Set gpio 0 to input
  INP_GPIO(0);
  // Set the gpio 0 to it's alternate state - in this case,
  // we want state 0, SDA0, the I2C data line
  SET_GPIO_ALT(0, 0);
  // Set gpio 1 to input
  INP_GPIO(1);
  // Set gpio 1 to alternate state, SCL0
  SET_GPIO_ALT(1, 0);
}

///////////////////////////////////////////////////////////////////////////////
// GPIO READ
///////////////////////////////////////////////////////////////////////////////

// Given a --PHYSICAL-- chip pin index, return a
// pin struct containing the current information
Pin* get_pin_status(int p)
{
  // Convert the physical chip pin index to it's memory index
  int g = chip_index_to_mem(p);
  Pin *pin = malloc_pin(p);
  // Update the pin with it's current values
  update_pin_status(pin);
  // Note mem dealloc responsibility
  return pin;
}

// Helper function to prevent reallocation of memory
// Given the pin struct pointer, will update it's value
Pin* update_pin_status(Pin* pin)
{
  int g = pin->memIndex;
  // Extract pin control code (3 bits signifying current state)
  // by retrieving the control word, shifting it the appropriate
  // distance to the right, then masking for the first 3 bits.
  int ctrlCode  = ((PIN_CONTROL_WORD(g) >> PIN_SHIFT(g)) & 7),
      isHigh    = ((VAL_WORD & (1u << g)) != 0);
  // Using the ctrlCode (setting in or out) and the current state,
  // return the state_t type that represents the pins current status
  pin->state = 1u & ctrlCode;
  pin->value = isHigh;
  return pin;
}

// Updates all the pin values in the chip struct
Pin** update_all_pins()
{
  for (int i = 0; i < NO_OF_PINS; i++)
    update_pin_status(chip->pins[i]);
  return chip->pins;
}

///////////////////////////////////////////////////////////////////////////////
// GPIO WRITE
///////////////////////////////////////////////////////////////////////////////

// Given a word, apply it to the set mem address to
// set multiple pins in one go
void  set_with_word(uint32_t w)
{
  SET_WORD = w;
}

// As above, given a word, apply to the clearing
// memory address to turn multiple pins off at once
void  clr_with_word(uint32_t w)
{
  CLR_WORD = w;
}

// Given a --PHYSICAL-- chip pin index, set it's output
// value to v
void set_pin_value(int p, int v)
{
	int g = chip_index_to_mem(p);
  if (v == 0)
	  // Clear the pin
	  GPIO_CLR(g);
  else
	  // Now set it
	  GPIO_SET(g,v);
}

void set_pin_state(int p, int v)
{
  int g = chip_index_to_mem(p);
  INP_GPIO(g);
  // If output
  if (v) OUT_GPIO(g);
}

///////////////////////////////////////////////////////////////////////////////
// I2C PROTOCOL
///////////////////////////////////////////////////////////////////////////////

void wait_i2c_done()
{
  // Pause should be 5x10^6 nanoseconds, timeout should
  // be 1x10^9/pause
  int pause = 5000000, timeout = 1000000000/pause;
  // While Broadcom Serial Controller status not registering DONE
  while (!(BSC_S & BSC_S_DONE) && --timeout)
  {
    // Sleep the process for 5,000,000 nanoseconds
    nanosleep((struct timespec[]) {{0, pause}}, NULL);  
  }
  // If timeout is 0 then the device failed to respond
  // after 1 second of wait. Register timeout to stderr
  if (!timeout)
  {
    fprintf(stderr, "I2C timeout occurred\n");
  }
}

///////////////////////////////////////////////////////////////////////////////
// MALLOC / DEALLOC
///////////////////////////////////////////////////////////////////////////////

// Allocate memory to a chip struct that will represent the
// current state of the raspberry pi pins
Chip *init_chip()
{
  chip = malloc(sizeof(Chip));
  for (int i = 1; i <= NO_OF_PINS; i++)
    chip->pins[i - 1] = malloc_pin(i);
  return chip;
}

// Given the --PHYSICAL-- chip pin index, create a
// pin struct and return the pointer
Pin* malloc_pin(int p)
{
  Pin *pin = malloc(sizeof(Pin));
  pin->chipIndex = p;
  pin->memIndex = chip_index_to_mem(p);
  update_pin_status(pin);
  return pin;
}

// Doesn't take a chip parameter as it is already
// globally accessable
void dealloc_chip()
{
  for (int i = 0; i < NO_OF_PINS; i++)
    free(chip->pins[i]);
  free(chip);
}

// Takes p, the physical pin number and translates to the
// memory address pin location
int chip_index_to_mem(int p)
{
  if ((p <= NO_OF_PINS) && (p > 0)) return chipPinToMem(p);
  fprintf(stderr, "Not a valid physical pin number.\n");
  return NA;
}

///////////////////////////////////////////////////////////////////////////////
// SINATRA ADDONS
///////////////////////////////////////////////////////////////////////////////

Chip* get_chip()
{ return chip; }
