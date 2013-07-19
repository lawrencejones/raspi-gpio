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
    ERR("Failed to access dev/mem - verify sudo?\n");
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
    ERR("Error from builtin mmap - code %d\n", (int) gpioMap);
    exit(EXIT_FAILURE);
  }
  // Use volatile pointer to allow shared access of the gpio locations
  // Slows performance but refreshing is required
  gpio = (volatile unsigned *)gpioMap;
  i2c = (volatile unsigned *)i2cMap;
  return gpio;
}


// TODO - Set macros to auto fix pins for various board revisions
// Initialises pins to prepare for the I2C protocol
void init_i2c()
{
  // Rememeber to set gpio's to INP initially to reset
  // the current alt setting
  // Set gpio 2 to input
  INP_GPIO(2);
  // Set the gpio 2 to it's alternate state - in this case,
  // we want state 0, SDA0, the I2C data line
  SET_GPIO_ALT(2, 0);
  // Set gpio 3 to input
  INP_GPIO(3);
  // Set gpio 3 to alternate state, SCL0
  SET_GPIO_ALT(3, 0);
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

// Wait for the i2c bus to register DONE status
// Pauses execution of the thread using nanosleep
void wait_i2c_done()
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

// Deallocate all the heap memory required for the
// bus, including all it's devices
void dealloc_i2c_bus(i2c_bus *bus)
{
  // Assign first bus dev to d1
  // Make d2 pointer available for use
  i2c_dev *d1 = bus->first, *d2;
  do {
    // Assign the next dev to d2
    d2 = d1->next;
    // Free the current dev
    free(d1); 
  } while ((d1 = d2));
  free(bus);
}

// Detect all devices on the current bus and return a
// pointer to an i2c_bus struct containing all discovered
// i2c devices
i2c_bus* i2c_bus_detect()
{
  // malloc new bus struct
  i2c_bus *bus = malloc_i2c_bus();
  // For all of the available addresses
  for (short addr = 1; addr < 128; addr++)
  {
    // If the i2c addr is active
    if (i2c_bus_addr_active(addr))
    {
      // Add the current address to the bus
      add_i2c_dev(bus, addr);
    }
  }
  // Return the i2c bus pointer
  return bus;
}

int i2c_bus_addr_active(short addr)
{
  // Set new slave address
  BSC_SLAVE_ADDR = addr;
  // Clear current bus status
  BSC_S = CLEAR_STATUS;
  // Only wish to read a single byte
  BSC_DATA_LEN = 1;
  // Initiate read using bus control
  BSC_C = START_READ;
  wait_i2c_done();
  return !(BSC_S & BSC_S_ERR);
}

// Allocate memory in the heap for an i2c_bus struct
i2c_bus *malloc_i2c_bus()
{
  // Allocate memory space for the bus
  i2c_bus *bus = malloc(sizeof(i2c_bus));
  if (!bus)
  {
    ERR("Memory allocation failed (malloc) of bus.\n\n");
    exit(EXIT_FAILURE);
  }
  // Assign the first i2c dev, the dummy ctrl
  bus->first = malloc_i2c_dev(0);
  // Return the allocated bus
  return bus;
}

// Add an i2c device to an existing i2c_bus
void add_i2c_dev(i2c_bus *bus, short addr)
{
  // Increment the no of devices
  bus->no_of_devs++;
  // Pick up bus first device
  i2c_dev *dev = bus->first;
  // bus->first will never be NULL due to ctrl dummy
  // so iterate till we get to a dev that has no next
  while (dev->next)
  { // Assign the next device
    dev = dev->next;
  }
  // Once final device has been found, malloc new
  dev->next = malloc_i2c_dev(addr);
}

// Allocate memory for an i2c_dev struct in the heap
i2c_dev* malloc_i2c_dev(short addr)
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

// Read a single byte from an i2c device at the given register. 
// Once read, return the literal byte read from the dev
uint8_t i2c_read_byte(i2c_dev *dev, short reg)
{
  // Read block of 1 byte
  uint8_t *result = i2c_read_block(dev, reg, 1),
           byte = result[0];
  // Free the result pointer
  free(result);
  // Return the literal uint8_t byte
  return byte;
}

// Same as the read byte, just allows specification of block
// size to read. Returns an array of uint32_t in the heap
// that contains all the information read out of the FIFO reg
uint8_t *i2c_read_block(i2c_dev *dev, short reg, short block_size)
{
  // Declare result array
  uint8_t *result;
  // Clear the fifo
  BSC_C = BSC_C_CLEAR;
  // Set new address
  BSC_SLAVE_ADDR = dev->addr;
  // Set size of transfer
  BSC_DATA_LEN = 1;
  // Write the register to the fifo
  BSC_FIFO = reg;
  // Clear status
  BSC_S = CLEAR_STATUS;
  // Initiate the transfer
  BSC_C = START_WRITE;
  // Wait for acknowledgement
  wait_i2c_done();
  // Set length to block size
  BSC_DATA_LEN = block_size;
  // Clear the bus status
  BSC_S = CLEAR_STATUS;
  // TODO - Setup error checking for the reg transfer
  // Start the bus read
  BSC_C = START_READ;
  // Wait for the bus to clear
  wait_i2c_done();
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

uint32_t i2c_write_block(i2c_dev *dev, short size, uint8_t *content)
{
  // Verify that the addressed device is currently active and registered
  // on the bus
  if (!i2c_bus_addr_active(dev->addr))
  {
    ERR("No device found at current address (0x%02x)\n\n", dev->addr);
    exit(EXIT_FAILURE);
  }
  // Clear the current fifo
  // TODO - Investigate if this is actually the best method
  BSC_C = BSC_C_CLEAR;
  // Set dev address
  BSC_SLAVE_ADDR = dev->addr;
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
  wait_i2c_done();
  // Return the value of the status register
  return BSC_S;
}

uint32_t i2c_write_reg(i2c_dev *dev, short reg, uint8_t *bytes, short size)
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
  uint32_t status = i2c_write_block(dev, size + 1, content);
  // Return the i2c bus status
  return status;
}

// Given a pointer to a i2c_dev and a 
// Prints the devices with their addresses on the given bus
void print_i2c_bus(i2c_bus *bus)
{
  // Set count to 0
  int count = 0;
  // Initialise first device in chain
  i2c_dev *dev = bus->first;
  PRINTC("Printing i2c bus devices...\n\n", GREEN);
  // Print the device while one exists
  do {
    printf("  Dev %d  -  Addr 0x%02x\n", count++, dev->addr);
  } while ((dev = dev->next));
  // Clear a line
  PRINTC("\n...done.\n\n", GREEN);
}


///////////////////////////////////////////////////////////////////////////////
// MALLOC / DEALLOC
///////////////////////////////////////////////////////////////////////////////

// Allocate memory to a chip struct that will represent the
// current state of the raspberry pi pins
Chip *init_chip()
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
Pin* malloc_pin(int p)
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
  ERR("Not a valid physical pin number.\n");
  return NA;
}

///////////////////////////////////////////////////////////////////////////////
// SINATRA ADDONS
///////////////////////////////////////////////////////////////////////////////

Chip* get_chip()
{ return chip; }
