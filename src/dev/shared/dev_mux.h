///////////////////////////////////////////////////////////////////////////////
// Raspberry Pi GPIO Interface
// ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
// File: dev_mux.h
// PA Consulting - Lawrence Jones
///////////////////////////////////////////////////////////////////////////////

#ifndef DEVS_MUX_HEADER_INC
#define DEVS_MUX_HEADER_INC

#include "i2c.h"
#include "dev/shared.h"

///////////////////////////////////////////////////////////////////////////////
// FUNCTION TYPEDEFS
///////////////////////////////////////////////////////////////////////////////

// Function type for fetching/updating an i2c mux channel
typedef uint8_t       (*ChannelGet)(Mux *m);
// Function type for setting an i2c mux channel
typedef void          (*ChannelSet)(Mux *m, uint8_t c);
// Function for retrieving an i2c mux registry
typedef MuxNetwork*   (*MuxDeviceDetect)(Mux *m);
// Function for deallocing a mux
typedef void          (*DeallocMux)(Mux **s);

///////////////////////////////////////////////////////////////////////////////
// STRUCTS
///////////////////////////////////////////////////////////////////////////////

// Define a struct to represent an i2c multiplexer
struct Mux {
  // Model of the mux
  model_t model;
  // An i2c bus handle for read and writing
  i2c_bus *i2c;
  // i2c slave address
  short i2c_addr;
  // A field to store latest value of the selected
  // channel. This field can be updated by calling
  // the `fetch_channel` function, and is included
  // to avoid too frequent reads of the i2c muxes
  uint8_t channel;
  ///////////////////////////////////////////////
  // Function to fetch and update current channel
  ChannelGet get_channel;
  // Function to set the channel to 0..3
  ChannelSet set_channel;
  // Function to retrieve a list of all devices
  MuxDeviceDetect get_devs;
  // Function to dealloc
  DeallocMux dealloc;
};

// Define a struct to represent an i2c mux network of devices
struct MuxNetwork {
  // Field for the channel
  // -1 represents external devices
  int channel;
  // Field for a linked list of devices on this channel
  i2c_dev* dev;
  // Pointer to the next channel
  MuxNetwork* next;
};

#endif
