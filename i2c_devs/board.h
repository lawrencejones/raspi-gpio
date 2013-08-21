///////////////////////////////////////////////////////////////////////////////
// Raspberry Pi GPIO Interface
// ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
// File: board.h
// PA Consulting - Lawrence Jones
///////////////////////////////////////////////////////////////////////////////

#ifndef IMU_BOARD_INC
#define IMU_BOARD_INC

#include <stdint.h>

///////////////////////////////////////////////////////////////////////////////
// TYPEDEFS
///////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////
// Enums   //////////////////////////////////////////////////
// Define enum for accel and gyro
enum Type {GYRO, ACCEL};
// Define enum for sensor read target
enum Target {HOST, AUX};


/////////////////////////////////////////////////////////////
// Function Typedefs   //////////////////////////////////////
// Required for definitions
typedef struct Sensor Sensor;
typedef struct Axes Axes;
// Function to return an axes struct pointer
typedef Axes* (*ReadAxes)(Sensor *s, enum Target t);
// Typedef for a sensor on/off toggle function
typedef int (*SensorToggle)(Sensor *s);
// Function to configure board to access a sensor
typedef int (*PrepBoard)(Sensor *s);


/////////////////////////////////////////////////////////////
// Struct Typedefs   ////////////////////////////////////////

// Define generic KeyVal pair for config
typedef struct {                                    // KeyVal
  char *key;
  char *val;
} KeyVal;


// Define a struct to represent 3-axes data
// Will form a linked list of axes readings
struct Axes {                                         // Axes
  // States what type of readings, accel or gyro
  enum Type type;
  // The respective xyz readings
  uint16_t x, y, z;
  // The next Axes result struct
  Axes *next;
};

// Define a struct to represent a sensor
struct Sensor {                                     // Sensor
  // Both a name and model string for the device
  char *name, *model;
  // Specifies gyro or accel
  enum Type type;
  // Specifies an i2c address
  int i2c_addr;
  // Function to enable the device
  SensorToggle enable;
  // Function to disable the device
  SensorToggle disable;
  // Function to read axes data from given target
  ReadAxes read;
  // Function to prep the board for device access
  PrepBoard prep;
};

#endif
