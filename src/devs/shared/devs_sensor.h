///////////////////////////////////////////////////////////////////////////////
// Raspberry Pi GPIO Interface
// ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
// File: devs_sensor.h
// PA Consulting - Lawrence Jones
///////////////////////////////////////////////////////////////////////////////

#ifndef DEVS_SENSOR_HEADER_INC
#define DEVS_SENSOR_HEADER_INC

#include <stdint.h>
#include "i2c.h"
#include "devs_private.h"

///////////////////////////////////////////////////////////////////////////////
// TYPEDEFS
///////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////
// Function Typedefs   //////////////////////////////////////
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
// Function for deallocing a sensor
typedef void          (*DeallocSensor)(Sensor **s);
// Typedef for a device configure function
typedef int           (*SensorConfigure)(Sensor*, char*);
// Typedef for a device reset function
typedef void          (*SensorReset)(Sensor *);

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
  // Pointer to the sensors i2c_bus
  i2c_bus *i2c;
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
  // Function to configure sensor
  SensorConfigure config;
  // Function to reset the default config
  SensorReset reset;
  // Dealloc function
  DeallocSensor dealloc;
};

// Define a struct to map config keys to functions
struct ConfigFunctionMap {
  // String of keys
  char* keys;
  // Function pointer to a helper
  Configs helper;
};

#endif
