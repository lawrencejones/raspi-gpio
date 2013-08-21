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
typedef enum {GYRO, ACCEL} type_t;
// Define enum for sensor read target
typedef enum {HOST, AUX} target_t;
// Enum for all model numbers
typedef enum {MPU3300, ITG3050, L3G4200D} model_t;


/////////////////////////////////////////////////////////////
// Function Typedefs   //////////////////////////////////////
// Required for definitions
typedef struct Sensor Sensor;
typedef struct Axes Axes;
typedef struct KeyVal KeyVal;
// Function to return an axes struct pointer
typedef Axes* (*ReadAxes)(Sensor *s, target_t t);
// Typedef for a sensor enable function
typedef int (*SensorEnable)(Sensor *s, KeyVal *settings);
// Typedef for a sensor disable function
typedef int (*SensorDisable)(Sensor *s);
// Function to configure board to access a sensor
typedef int (*PrepBoard)(Sensor *s);


/////////////////////////////////////////////////////////////
// Struct Typedefs   ////////////////////////////////////////

// Define generic KeyVal pair for config
struct KeyVal {                                    // KeyVal
  char *key;
  char *val;
};

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
  int i2c_addr;
  // Function to enable the device
  SensorEnable enable;
  // Function to disable the device
  SensorDisable disable;
  // Function to read axes data from given target
  ReadAxes read;
  // Function to prep the board for device access
  PrepBoard prep;
};

///////////////////////////////////////////////////////////////////////////////
// FUNCTION STUBS
///////////////////////////////////////////////////////////////////////////////

int prep_board(Sensor *S);

#endif
