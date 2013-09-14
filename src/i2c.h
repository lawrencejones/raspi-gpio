///////////////////////////////////////////////////////////////////////////////
// Raspberry Pi GPIO Interface
// ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
// File: i2c.h
// PA Consulting - Lawrence Jones
///////////////////////////////////////////////////////////////////////////////

#ifndef I2C_HEADER_INC
#define I2C_HEADER_INC

#include <stdint.h>

///////////////////////////////////////////////////////////////////////////////
// TYPEDEFS / STRUCTS
///////////////////////////////////////////////////////////////////////////////

// Alias for i2c buses
typedef volatile unsigned i2c_bus;

// Define an i2c device type and struct
typedef struct i2c_dev i2c_dev;
struct i2c_dev {
  uint8_t addr;
  i2c_dev *next;
}; 

///////////////////////////////////////////////////////////////////////////////
// I2C INTERFACE
///////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////
// I2C Initialization   /////////////////////////////////////
// Initialise the bus, returns an i2c_bus handle
i2c_bus             *i2c_init            (  int bus  );

/////////////////////////////////////////////////////////////
// I2C System Functions /////////////////////////////////////
// Blocking function to allow the bus to finish
void                i2c_wait_done       (  i2c_bus *i2c  );
// Verifies that the addr is active and responding
int                 i2c_bus_addr_active (  i2c_bus *i2c, 
                                           short   addr  );
// Detects all the active devices on the given i2c_bus
i2c_dev             *i2c_dev_detect     (  i2c_bus *i2c  );
// Prints to stdout the given devices
void                i2c_print_bus       (  i2c_dev *dev  );
// Appends an i2c device struct with the given address onto
// the end of the current dev linked list
void                i2c_dev_append      (  i2c_dev *dev, 
                                           short addr  );

/////////////////////////////////////////////////////////////
// I2C Read /////////////////////////////////////////////////
// Reads a byte from the given address
uint8_t             i2c_read_byte       (  i2c_bus *i2c, 
                                           short addr  );
// Reads a byte from the register at the given address
uint8_t             i2c_read_reg        (  i2c_bus *i2c, 
                                           short addr, 
                                           short reg  );
// Reads bytes from an i2c device sequentially, starting
// at the register on the device at the given address
uint8_t             *i2c_read_block     (  i2c_bus *i2c, 
                                           short addr, 
                                           short reg, 
                                           int   block_size  );

/////////////////////////////////////////////////////////////
// I2C Write ////////////////////////////////////////////////
// Writes a single byte to the given device and address
// Just wraps arround the i2c_write_reg function
uint32_t            i2c_write_byte      (  i2c_bus *i2c,
                                           short addr,
                                           uint8_t byte  );
// Writes bytes to the i2c device at the given address,
// on the supplied bus. Writes to the given register.
uint32_t            i2c_write_reg       (  i2c_bus *i2c, 
                                           short addr, 
                                           short reg, 
                                           uint8_t *bytes, 
                                           short size  );
// Writes bytes (with no register) to the given address on
// the supplied i2c bus
uint32_t            i2c_write_block     (  i2c_bus *i2c, 
                                           short addr, 
                                           short size, 
                                           uint8_t *content  );
/////////////////////////////////////////////////////////////
// I2C Memory Management ////////////////////////////////////
// Frees all of the chained devs
void                i2c_dev_dealloc     (  i2c_dev **a  );

#endif
