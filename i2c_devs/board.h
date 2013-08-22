///////////////////////////////////////////////////////////////////////////////
// Raspberry Pi GPIO Interface
// ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
// File: board.h
// PA Consulting - Lawrence Jones
///////////////////////////////////////////////////////////////////////////////

#ifndef IMU_BOARD_INC
#define IMU_BOARD_INC

#include <stdint.h>
#include "keyval.h"
#include "../src/io.h"

///////////////////////////////////////////////////////////////////////////////
// TYPEDEFS
///////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////
// Enums   //////////////////////////////////////////////////
// Define enum for accel and gyro
typedef enum {GYRO, ACCEL} type_t;
// Define enum for sensor read target
typedef enum {HOST, AUX} target_t;
// Enum for all model numbers
typedef enum {MPU3300, ITG3050, L3G4200D, PCA9548A} model_t;


/////////////////////////////////////////////////////////////
// Function Typedefs   //////////////////////////////////////
// Required for definitions
typedef struct Sensor Sensor;
typedef struct Axes Axes;
typedef struct ConfigFunctionMap ConfigFunctionMap;
typedef struct Mux Mux;
typedef struct MuxNetwork MuxNetwork;
// Function to return an axes struct pointer
typedef Axes*         (*ReadAxes)(Sensor *s, target_t t);
// Typedef for a sensor enable function
typedef int           (*SensorEnable)(Sensor *s, KeyVal *settings);
// Typedef for a sensor disable function
typedef int           (*SensorDisable)(Sensor *s);
// Function to configure board to access a sensor
typedef int           (*PrepBoard)(Sensor *s);
// Function type for configuration helpers
typedef uint8_t       (*Configs)(Sensor *s, KeyVal *pairs);
// Function type for fetching/updating an i2c mux channel
typedef uint8_t       (*ChannelFetch)(Mux *m);
// Function type for setting an i2c mux channel
typedef void          (*ChannelSet)(Mux *m, uint8_t c);
// Function for retrieving an i2c mux registry
typedef MuxNetwork*   (*MuxDeviceDetect)(Mux *m);

/////////////////////////////////////////////////////////////
// Struct Typedefs   ////////////////////////////////////////
// Define a struct to represent 3-axes data
// Will form a linked list of axes readings
struct Axes {                                         // Axes
  // States what type of readings, accel or gyro
  type_t type;
  // The respective xyz readings
  uint16_t x, y, z;
  // The next Axes result struct
  Axes *next;
};

// Define a struct to represent a sensor
struct Sensor {                                     // Sensor
  // Name for the device
  char *name;
  // Model of the device
  model_t model;
  // Specifies gyro or accel
  type_t type;
  // Specifies an i2c address
  short i2c_addr;
  // Pointer to the controlling i2c mux
  Mux *mux;
  ///////////////////////////////////////////////
  // Function to enable the device
  SensorEnable enable;
  // Function to disable the device
  SensorDisable disable;
  // Function to read axes data from given target
  ReadAxes read;
  // Function to prep the board for device access
  PrepBoard prep;
};

// Define a struct to represent an i2c multiplexer
struct Mux {
  // Name for this i2c multiplexer
  char *name;
  // Model of the mux
  model_t model;
  // i2c slave address
  short i2c_addr;
  // An i2c bus handle for read and writing
  i2c_bus *i2c;
  // A field to store latest value of the selected
  // channel. This field can be updated by calling
  // the `fetch_channel` function, and is included
  // to avoid too frequent reads of the i2c muxes
  uint8_t channel;
  ///////////////////////////////////////////////
  // Function to fetch and update current channel
  ChannelFetch fetch_channel;
  // Function to set the channel to 0..3
  ChannelSet set_channel;
  // Function to retrieve a list of all devices
  MuxDeviceDetect get_devs;
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

// Define a struct to map config keys to functions
struct ConfigFunctionMap {
  // String of keys
  char *keys;
  // Function pointer to a helper
  Configs helper;
};

///////////////////////////////////////////////////////////////////////////////
// FUNCTION STUBS
///////////////////////////////////////////////////////////////////////////////

int prep_board(Sensor *S);

#endif
