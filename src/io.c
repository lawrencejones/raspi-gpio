///////////////////////////////////////////////////////////////////////////////
// Raspberry Pi GPIO Interface
// ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
// File: io.c
// PA Consulting - Lawrence Jones
///////////////////////////////////////////////////////////////////////////////

#include "io.h"
#include "print.h"
#include "pinmaps.h"
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

// Define the entry point for gpios
volatile unsigned *gpio;
// Define the entry point for I2C interface
volatile unsigned *i2c;
// Function stub
static i2c_dev* i2c_dev_malloc(short addr);

///////////////////////////////////////////////////////////////////////////////
// INITIALISATION
///////////////////////////////////////////////////////////////////////////////

// Configure the memory access required to alter
// the GPIO pins. Will request memory access from the system,
// note that this requires sudo privileges!
// Based on example at http://elinux.org/RPi_Low-level_peripherals
static inline volatile unsigned* get_mmap(int base)                 // get_mmap
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

// Initialises gpio access specifically
volatile unsigned* init_gpio_access()                       // init_gpio_access
{
  // Use the get_mmap function to pull from the devmem page
  // Initialise with the GPIO_BASE
  // Use volatile pointer to allow shared access of the gpio locations
  // Slows performance but refreshing is required
  gpio = (volatile unsigned *)get_mmap(GPIO_BASE);
  return gpio;
}



///////////////////////////////////////////////////////////////////////////////
// GPIO READ
///////////////////////////////////////////////////////////////////////////////

// Given a --PHYSICAL-- chip pin index, return a
// pin struct containing the current information
Pin* get_pin_status(int p)                                    // get_pin_status
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
Pin* update_pin_status(Pin* pin)                           // update_pin_status
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
Pin** update_all_pins()                                      // update_all_pins
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
void  set_with_word(uint32_t w)                                // set_with_word
{
  SET_WORD = w;
}

// As above, given a word, apply to the clearing
// memory address to turn multiple pins off at once
void  clr_with_word(uint32_t w)                                // clr_with_word
{
  CLR_WORD = w;
}

// Given a --PHYSICAL-- chip pin index, set it's output
// value to v
void set_pin_value(int p, int v)                               // set_pin_value
{
  int g = chip_index_to_mem(p);
  if (v == 0)
    // Clear the pin
    GPIO_CLR(g);
  else
    // Now set it
    GPIO_SET(g,v);
}

void set_pin_state(int p, int v)                               // set_pin_state
{
  int g = chip_index_to_mem(p);
  INP_GPIO(g);
  // If output
  if (v) OUT_GPIO(g);
}

///////////////////////////////////////////////////////////////////////////////
// I2C PROTOCOL
///////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////
// I2C Initialization
/////////////////////////////////////////////////////////////

// TODO - Set macros to auto fix pins for various board revisions
// Initialises pins to prepare for the I2C protocol
volatile unsigned* i2c_init(int bus)                                // i2c_init
{
  // Verify that gpio access has already been achieved
  if (gpio == NULL)
  {
    // If gpio isn't init'd, attempt an init
    init_gpio_access();
  }
  // Open the devmem and fetch from the i2c base
  volatile unsigned* i2c = get_mmap(I2C_BASE(bus));
  // Initialise clock and data ints
  int clk, data;
  // Dependent on which bus, select the correct pins
  switch (bus)
  {
    // For i2c_0, gpio pins 28 and 29
    case 0: 
      data = 28;
      clk = 29; 
      break;
      // For i2c_1, gpio pins 2 and 3
    case 1:
      data = 2;
      clk = 3;
      break;
  }
  // Rememeber to set gpio's to INP initially to reset
  // the current alt setting
  // Set gpio data to input
  INP_GPIO(data);
  // Set the gpio data pin to it's alternate state - in this case,
  // we want state 0, SDA0, the I2C data line
  SET_GPIO_ALT(data, 0);
  // Set gpio clk pin to input
  INP_GPIO(clk);
  // Set gpio clk pin to alternate state, SCL0
  SET_GPIO_ALT(clk, 0);
  // Return the i2c pointer
  return i2c;
}


/////////////////////////////////////////////////////////////
// I2C System Functions
/////////////////////////////////////////////////////////////

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

// Given an i2c_bus and an address, ping the dev at
// the addr and verify that there is a response
int i2c_bus_addr_active(i2c_bus *i2c, short addr)        // i2c_bus_addr_active
{
  // Set new slave address
  BSC_SLAVE_ADDR = addr;
  // Clear current bus status
  BSC_S = CLEAR_STATUS;
  // Only wish to read a single byte
  BSC_DATA_LEN = 1;
  // Initiate read using bus control
  BSC_C = START_READ;
  // Wait for bus to clear
  i2c_wait_done(i2c);
  return !(BSC_S & BSC_S_ERR);
}

// Detect all devices on the current bus and return a
// pointer to an i2c_dev struct linked to all discovered devices
i2c_dev *i2c_dev_detect(i2c_bus *i2c)                         // i2c_dev_detect
{
  // Create new dummy dev at addr 0
  i2c_dev *dev = i2c_dev_malloc(0);
  // For all of the available addresses
  for (short addr = 1; addr < 128; addr++)
  {
    // If the i2c addr is active
    if (i2c_bus_addr_active(i2c, addr))
    {
      // Add the current address to the chain of devs
      i2c_dev_append(dev, addr);
    }
  }
  // Return the initial i2c_dev pointer
  return dev;
}

// Add an i2c device to an existing i2c_dev list
void i2c_dev_append(i2c_dev *dev, short addr)                 // i2c_dev_append
{
  // bus->first will never be NULL due to ctrl dummy
  // so iterate till we get to a dev that has no next
  while (dev->next)
  { // Assign the next device
    dev = dev->next;
  }
  // Once final device has been found, malloc new
  dev->next = i2c_dev_malloc(addr);
}

// Given a pointer to a i2c_dev and a 
// Prints the devices with their addresses on the given bus
void i2c_print_bus(i2c_dev *dev)                               // i2c_print_bus
{
  // Set count to 0
  int count = 0;
  PRINTC(GREEN, "Printing i2c bus devices...\n\n");
  // Print the device while one exists
  do {
    printf("  Dev %d  -  Addr 0x%02x\n", count++, dev->addr);
  } while ((dev = dev->next));
  // Clear a line
  PRINTC(GREEN, "\n...done.\n\n");
}

/////////////////////////////////////////////////////////////
// I2C Read
/////////////////////////////////////////////////////////////

// Read a single byte from the given addr on the given bus
uint8_t i2c_read_byte(i2c_bus *i2c, short addr)                // i2c_read_byte
{
  // Clear the fifo
  BSC_C = BSC_C_CLEAR;
  // Set new address
  BSC_SLAVE_ADDR = addr;
  // Clear the bus status
  BSC_S = CLEAR_STATUS;
  // Start the bus read
  BSC_C = START_READ;
  // Wait for the bus to clear
  i2c_wait_done(i2c);
  // Return the result in the fifo
  return (uint8_t)BSC_FIFO;
}

// Read a single byte from the given register at the given
// addresss, on i2c_bus*
uint8_t i2c_read_reg(i2c_bus *i2c, short addr, short reg)       // i2c_read_reg
{
  // Read block of 1 byte
  uint8_t *result = i2c_read_block(i2c, addr, reg, 1),
          byte = result[0];
  // Free the result pointer
  free(result);
  // Return the literal uint8_t byte
  return byte;
}


// Same as the read byte, just allows specification of block
// size to read. Returns an array of uint32_t in the heap
// that contains all the information read out of the FIFO reg
uint8_t *i2c_read_block(i2c_bus *i2c,                         // i2c_read_block
    short addr, 
    short reg,
    short block_size)
{
  // Declare result array
  uint8_t *result;
  // Clear the fifo
  BSC_C = BSC_C_CLEAR;
  // Set new address
  BSC_SLAVE_ADDR = addr;
  // Set size of transfer
  BSC_DATA_LEN = 1;
  // Write the register to the fifo
  BSC_FIFO = reg;
  // Clear status
  BSC_S = CLEAR_STATUS;
  // Initiate the transfer
  BSC_C = START_WRITE;
  // Wait for acknowledgement
  i2c_wait_done(i2c);
  // Set length to block size
  BSC_DATA_LEN = block_size;
  // Clear the bus status
  BSC_S = CLEAR_STATUS;
  // TODO - Setup error checking for the reg transfer
  // Start the bus read
  BSC_C = START_READ;
  // Wait for the bus to clear
  i2c_wait_done(i2c);
  // Allocate the uint8_t array
  result = malloc(sizeof(uint8_t) * block_size);
  // Verify successful malloc
  if (!result)
  {
    ERR("Error allocating memory (malloc). Read failed.\n\n");
    exit(EXIT_FAILURE);
  }
  // Read result into the result array
  for (int i = 0; i < block_size; i++)
  {
    // printf("0x%20x\n", BSC_FIFO);
    result[i] = BSC_FIFO;
  }
  // Return the result
  // NOTE - Memory responsibility passed to calling function
  return result;
}

/////////////////////////////////////////////////////////////
// I2C Write
/////////////////////////////////////////////////////////////

// Write a single byte to the addr given, on the supplied
// i2c bus
uint32_t i2c_write_byte(i2c_bus *i2c,                         // i2c_write_byte
    short addr,
    uint8_t byte)
{
  // Write nothing to a register
  // Counts as a single byte write
  return i2c_write_reg(i2c, addr, byte, NULL, 0);
}

// Write to the register of the device specified the contents
// of the `bytes` array
uint32_t i2c_write_reg(i2c_bus *i2c,                           // i2c_write_reg
    short addr, 
    short reg, 
    uint8_t *bytes, 
    short size)
{
  // Generate content package
  uint8_t content[size + 1];
  // First byte is reg number
  content[0] = reg;
  // Copy in the content
  for (int i = 1; i <= size; i++)
  {
    content[i] = bytes[i - 1];
  }
  // Write the block with the included reg
  uint32_t status = i2c_write_block(i2c, addr, size + 1, content);
  // Return the i2c bus status
  return status;
}

uint32_t i2c_write_block(i2c_bus *i2c,                       // i2c_write_block
    short addr, 
    short size, 
    uint8_t *content)
{
  // Verify that the addressed device is currently active and registered
  // on the bus
  if (!i2c_bus_addr_active(i2c, addr))
  {
    ERR("No device found at current address (0x%02x)\n\n", addr);
    exit(EXIT_FAILURE);
  }
  // Clear the current fifo
  // TODO - Investigate if this is actually the best method
  BSC_C = BSC_C_CLEAR;
  // Set dev address
  BSC_SLAVE_ADDR = addr;
  // Clear the current status
  BSC_S = CLEAR_STATUS;
  // Set the length of the transfer + addr byte
  BSC_DATA_LEN = size;
  // Load the content into the fifo buffer
  for (int i = 0; i < size; i++)
  {
    BSC_FIFO = content[i];
  }
  // Start the write
  BSC_C = START_WRITE;
  // Wait for the i2c transfer to finish
  i2c_wait_done(i2c);
  // Return the value of the status register
  return BSC_S;
}

///////////////////////////////////////////////////////////////////////////////
// MALLOC / DEALLOC
///////////////////////////////////////////////////////////////////////////////

// Allocate memory to a chip struct that will represent the
// current state of the raspberry pi pins
Chip *init_chip()                                                  // init_chip
{
  // Allocate heap memory for the chip
  chip = malloc(sizeof(Chip));
  // Verify successful malloc of chip
  if (!chip)
  {
    ERR("Memory allocation failed (malloc) of chip.\n\n");
    exit(EXIT_FAILURE);
  }
  // For all the pins on the chip
  for (int i = 1; i <= NO_OF_PINS; i++)
  {
    // Initialise the pin struct
    chip->pins[i - 1] = malloc_pin(i);
  }
  // Return the pointer to the chip
  return chip;
}

// Given the --PHYSICAL-- chip pin index, create a
// pin struct and return the pointer
Pin* malloc_pin(int p)                                            // malloc_pin
{
  // Allocate space in heap for the Pin struct
  Pin *pin = malloc(sizeof(Pin));
  // Verify successful malloc
  if (!pin)
  {
    ERR("Memory allocation failed (malloc) of pin.\n\n");
    exit(EXIT_FAILURE);
  }
  // Set the chip index (chip pin, physical 1-26)
  pin->chipIndex = p;
  // Set the memory index
  pin->memIndex = chip_index_to_mem(p);
  // Fetch current pin data
  update_pin_status(pin);
  // Return the pin pointer
  return pin;
}

// Allocate memory for an i2c_dev struct in the heap
static i2c_dev* i2c_dev_malloc(short addr)                    // i2c_dev_malloc
{
  // malloc the dev struct
  i2c_dev *dev = malloc(sizeof(i2c_dev));
  // Verify successful malloc
  if (!dev)
  {
    ERR("Memory allocation failed (malloc) of dev.\n\n");
    exit(EXIT_FAILURE);
  }
  // Set address value
  dev->addr = addr;
  // Set the next dev to NULL
  dev->next = NULL;
  // Return dev pointer
  return dev;
}

// Doesn't take a chip parameter as it is already
// globally accessable
void dealloc_chip()                                             // dealloc_chip
{
  for (int i = 0; i < NO_OF_PINS; i++)
    free(chip->pins[i]);
  free(chip);
}

// Takes p, the physical pin number and translates to the
// memory address pin location
int chip_index_to_mem(int p)                               // chip_index_to_mem
{
  if ((p <= NO_OF_PINS) && (p > 0)) return chipPinToMem(p);
  ERR("Not a valid physical pin number.\n");
  return NA;
}

// Deallocate all the heap memory required for the
// bus, including all it's devices
void i2c_dev_dealloc(i2c_dev **a)                            // i2c_dev_dealloc
{
  // Declare placeholder
  i2c_dev *b = *a;
  // While there is a i2c_dev
  while ((*a = b))
  {
    // Advance
    b = b->next;
    // Free previous
    free(*a);
  }
  // Null the callee's pointer up the stack
  *a = NULL;
}

///////////////////////////////////////////////////////////////////////////////
// SINATRA ADDONS
///////////////////////////////////////////////////////////////////////////////

Chip* get_chip()
{ return chip; }
