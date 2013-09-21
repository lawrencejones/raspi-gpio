///////////////////////////////////////////////////////////////////////////////
// Raspberry Pi GPIO Interface
// ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
// File: i2c_err.h
// PA Consulting - Lawrence Jones
///////////////////////////////////////////////////////////////////////////////

#ifndef I2C_ERR_HEADER_INC
#define I2C_ERR_HEADER_INC

///////////////////////////////////////////////////////////////////////////////
// DEFINE ERROR CODES
///////////////////////////////////////////////////////////////////////////////

// Signifies i2c device not active
#define I2C_DEV_DEAD  0xa0

// Signifies that after the given timeout period, the i2c
// fifo buffer has not become full nor has the transfer ended
#define FIFO_TIMEOUT  0xf0
// Signifies that the transfer has completed for an fifo
#define FIFO_DONE     0xf1
// Signifies that the fifo buffer has now been filled
#define FIFO_FULL     0xf2
// Signifies that the fifo has quit with no known error
#define FIFO_ERR      0xf3

#endif
