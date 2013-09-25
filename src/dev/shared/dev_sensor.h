///////////////////////////////////////////////////////////////////////////////
// Raspberry Pi GPIO Interface
// ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
// File: dev_sensor.h
// PA Consulting - Lawrence Jones
///////////////////////////////////////////////////////////////////////////////

#ifndef DEVS_SENSOR_HEADER_INC
#define DEVS_SENSOR_HEADER_INC

#include <stdint.h>
#include <pthread.h>
#include "i2c.h"
#include "dev/shared.h"

///////////////////////////////////////////////////////////////////////////////
// TYPEDEFS
///////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////
// Function Typedefs   //////////////////////////////////////
// Function to return an axes struct pointer
typedef Axes*         (*ReadAxes)(Sensor*, target_t);
// Typedef for a sensor enable function
typedef int           (*SensorEnable)(Sensor*, KeyVal*);
// Typedef for a sensor i2c bypass enable function
typedef void          (*SensorBypass)(Sensor*, int);
// Typedef for a sensor disable function
typedef int           (*SensorDisable)(Sensor*);
// Function for deallocing a sensor
typedef void          (*DeallocSensor)(Sensor**);
// Typedef for a device configure function
typedef int           (*SensorConfigure)(Sensor*, char*);
// Typedef for a device reset function
typedef void          (*SensorReset)(Sensor*);
// Typedef for a device read burst function
typedef Axes*         (*ReadBurst)(Sensor*);
// Typedef for a selftest function
typedef int           (*Selftest)(Sensor*, int);
// Sensor pipe
typedef int           (*SensorPipe)(Sensor*, const char*);
// Get's current fifo capacity
typedef float         (*SensorFifoCapcity)(Sensor*);

/////////////////////////////////////////////////////////////
// Struct Typedefs   ////////////////////////////////////////
// Define a struct to represent 3-axes data
// Will form a linked list of axes readings
struct Axes {                                             // Axes
  // States what type of readings, accel or gyro
  type_t type;
  // The respective xyz readings
  short x, y, z;
  // The next Axes result struct
  Axes *next;
};

// Define a struct to represent a sensor
struct Sensor {                                           // Sensor
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
  // The handle to write to the pipe file
  int wpipe;
  // A status int for whether the thread is active
  int pipe_running;
  // Keeps a pointer to the pipe thread
  pthread_t *pipe_thread;
  ///////////////////////////////////////////////
  // Function to reset the default config
  SensorReset reset;                                      // RESET
  // Function to turn the i2c bypass on
  SensorBypass i2c_bypass;                                // I2C_BYPASS
  // Function to read axes data from given target
  ReadAxes read;                                          // READ
  // Function to configure sensor
  SensorConfigure config;                                 // CONFIG
  // Function for selftesting
  Selftest selftest;                                      // SELFTEST
  // Function to initiate piping of results
  SensorPipe pipe;                                        // PIPE
  // Function to determine current fifo capacity
  SensorFifoCapcity fifo_capacity;                        // FIFO_CAPACITY
  // Dealloc function
  DeallocSensor dealloc;                                  // DEALLOC
};

#endif
