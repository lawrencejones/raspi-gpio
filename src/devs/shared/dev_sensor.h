///////////////////////////////////////////////////////////////////////////////
// Raspberry Pi GPIO Interface
// ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
// File: dev_sensor.h
// PA Consulting - Lawrence Jones
///////////////////////////////////////////////////////////////////////////////

#ifndef DEVS_SENSOR_HEADER_INC
#define DEVS_SENSOR_HEADER_INC

#include <stdint.h>
#include "i2c.h"
#include "../shared.h"

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
// Function for deallocing a sensor
typedef void          (*DeallocSensor)(Sensor **s);
// Typedef for a device configure function
typedef int           (*SensorConfigure)(Sensor*, char*);
// Typedef for a device reset function
typedef void          (*SensorReset)(Sensor *);
// Typedef for a device read burst function
typedef Axes*         (*ReadBurst)(Sensor *s);
// Typedef for a selftest function
typedef int           (*Selftest)(Sensor *s);

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
  // Model of the device
  model_t model;
  // Specifies gyro or accel
  type_t type;
  ///////////////////////////////////////////////
  // Pointer to the sensors i2c_bus
  i2c_bus *i2c;
  // Specifies an i2c address
  short i2c_addr;
  // Pointer to the controlling i2c mux
  Mux *mux;
  // Specifies the i2c channel
  short mux_channel;
  ///////////////////////////////////////////////
  // Function to reset the default config
  SensorReset reset;                                      // RESET
  // Function to enable the device
  SensorEnable enable;                                    // ENABLE
  // Function to disable the device
  SensorDisable disable;                                  // DISABLE
  // Function to read axes data from given target
  ReadAxes read;                                          // READ
  // Function to read the data burst from the sensor
  ReadBurst readBurst;                                    // READ_BURST
  // Function to configure sensor
  SensorConfigure config;                                 // CONFIG
  // Function for selftesting
  Selftest selftest;                                      // SELFTEST
  // Dealloc function
  DeallocSensor dealloc;                                  // DEALLOC
};

#endif